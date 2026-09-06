#pragma once

/// @file
/// @brief Streamproof external overlay: a separate, capture-excluded top-level window that carries
/// the ESP + menu, so screen/window capture (OBS, Game Bar, Discord) sees the game but not the
/// overlay.
///
/// Why a whole separate window. SetWindowDisplayAffinity(WDA_EXCLUDEFROMCAPTURE) on the *game*
/// window hides the whole game from capture — useless for streaming. To hide only the overlay it
/// must live in its own window with that affinity set on it.
///
/// Why its own thread. The menu must be interactive on this window (click-through is dropped while
/// it's open), which needs a Win32 message pump — and the game's render thread (where Present runs)
/// has none for our window. So this owns a dedicated thread that creates the window, its own D3D11
/// device + DirectComposition swap chain (for true per-pixel alpha), and a *second* ImGui context,
/// then runs a pump + render loop. GUI drives Start()/Stop() as RendererMode::External is entered /
/// left; while it runs, the game-window overlay skips the menu + ESP flush so nothing draws twice.
///
/// Per-pixel transparency uses a composition swap chain (DXGI_ALPHA_MODE_PREMULTIPLIED) presented
/// through DirectComposition; ImGui's DX11 blend writes premultiplied-correct alpha, so clearing the
/// target transparent and drawing ImGui on top composites cleanly over the desktop/game.

#include <Windows.h>

#include <d3d11.h>
#include <dxgi1_2.h>
#include <dcomp.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dcomp.lib")

#include <imgui.h>
#include "imgui_Impl_dx11.h"
#include "imgui_Impl_Win32.h"

#include "Window.h" // Window::WindowHandle (the game window) + the WDA_EXCLUDEFROMCAPTURE fallback
#include "Styles.h" // GUI::Styles::Init — same theme as the game-window overlay
#include "../Menu.h"
#include "../../render/Render.h"
#include "../../settings/Settings.h"
#include "../../utils/Logger.h"

// Declared by the ImGui Win32 backend (also declared in Window.h, which we include).

/// @brief The streamproof external overlay window, its D3D11/DirectComposition pipeline, its own
/// ImGui context, and the dedicated thread that pumps + renders it.
namespace ExternalWindow
{
	inline HWND Hwnd = nullptr;					   ///< the overlay window (separate from the game window)
	inline ID3D11Device* Device = nullptr;		   ///< overlay's own D3D11 device (independent of the game's)
	inline ID3D11DeviceContext* Context = nullptr; ///< immediate context for @ref Device
	inline IDXGISwapChain1* SwapChain = nullptr;   ///< composition swap chain (per-pixel alpha)
	inline ID3D11RenderTargetView* Rtv = nullptr;  ///< RTV over the swap chain's back buffer
	inline IDCompositionDevice* DcompDevice = nullptr;
	inline IDCompositionTarget* DcompTarget = nullptr;
	inline IDCompositionVisual* DcompVisual = nullptr;
	inline ImGuiContext* Ctx = nullptr; ///< the overlay's ImGui context (distinct from the game's)

	inline HANDLE Thread = nullptr;			 ///< the pump + render thread
	inline volatile bool Running = false;	 ///< thread loop guard; Stop() clears it
	inline bool Interactive = false;		 ///< true while click-through is dropped (menu open)
	inline int Width = 0, Height = 0;		 ///< current back-buffer size (game client size)
	inline int PosX = 0, PosY = 0;			 ///< current window top-left (game client, in screen coords)
	static constexpr wchar_t ClassName[] = L"SplitgateOverlay";

	/// @brief RAII helper: make @p ctx current for the scope, then restore the previous context.
	/// The ImGui DX11/Win32 backends key their state off the current context, so every backend/ImGui
	/// call on the overlay must run under its context — and must not leak it back to the game overlay.
	struct ScopedContext
	{
		ImGuiContext* prev;
		explicit ScopedContext(ImGuiContext* ctx) : prev(ImGui::GetCurrentContext()) { ImGui::SetCurrentContext(ctx); }
		~ScopedContext() { ImGui::SetCurrentContext(prev); }
	};

	/// @brief The game window whose client rect the overlay tracks. Prefer the handle the game
	/// overlay already found; fall back to a fresh FindWindow if it isn't set yet.
	inline HWND GameWindow()
	{
		if (Window::WindowHandle) return Window::WindowHandle;
		return FindWindowW(L"UnrealWindow", L"PortalWars  ");
	}

	/// @brief Whether the overlay should be visible this frame: true when the game window is in the
	/// foreground, or when the overlay window itself is (it pulls focus while the menu is open). Alt-
	/// tabbing to any other app makes this false, so the ESP/menu doesn't float over the desktop.
	inline bool GameFocused()
	{
		const HWND fg = GetForegroundWindow();
		return fg && (fg == GameWindow() || fg == Hwnd);
	}

	/// @brief (Re)create the render target view from the swap chain's back buffer. Flip-model buffer 0
	/// always aliases the current back buffer, so one RTV stays valid across Presents.
	inline void CreateRtv()
	{
		if (!SwapChain) return;
		ID3D11Texture2D* backBuffer = nullptr;
		if (FAILED(SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))) || !backBuffer) return;
		Device->CreateRenderTargetView(backBuffer, nullptr, &Rtv);
		backBuffer->Release();
	}

	inline void ReleaseRtv()
	{
		if (!Rtv) return;
		Rtv->Release();
		Rtv = nullptr;
	}

	/// @brief Window procedure: while the menu is open, feed input to the overlay's ImGui context and
	/// swallow it; otherwise let messages fall through. The window is click-through (WS_EX_TRANSPARENT)
	/// when the menu is closed, so it receives no mouse input then anyway.
	inline LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (Settings.MENU.ShowMenu && Ctx)
		{
			ScopedContext scoped(Ctx);
			ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
			switch (msg)
			{
			case WM_MOUSEMOVE:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_MOUSEWHEEL:
			case WM_MOUSEHWHEEL:
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_CHAR:
			case WM_SETCURSOR:
				return true;
			default:
				break;
			}
		}
		return DefWindowProcW(hWnd, msg, wParam, lParam);
	}

	/// @brief Apply the click-through / interactivity state for the current @ref Settings.MENU.ShowMenu.
	/// Closed menu: WS_EX_TRANSPARENT | WS_EX_NOACTIVATE so clicks pass to the game. Open menu: drop
	/// those and pull focus so the menu is usable. Only touches styles on an actual state change.
	inline void SyncInteractivity()
	{
		const bool wantInteractive = Settings.MENU.ShowMenu;
		if (wantInteractive == Interactive) return;
		Interactive = wantInteractive;

		LONG_PTR ex = GetWindowLongPtrW(Hwnd, GWL_EXSTYLE);
		if (wantInteractive)
			ex &= ~(WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
		else
			ex |= (WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
		SetWindowLongPtrW(Hwnd, GWL_EXSTYLE, ex);

		// Opening the menu pulls foreground onto the overlay so it takes input; closing it must hand
		// foreground back to the game, otherwise the overlay stays the activated window (now just click-
		// through) and the game never regains keyboard focus.
		SetForegroundWindow(wantInteractive ? Hwnd : GameWindow());
	}

	/// @brief Keep the overlay positioned over the game's client area and sized to it; resize the swap
	/// chain when the size changes. No-op when nothing moved.
	inline void SyncRect()
	{
		HWND game = GameWindow();
		if (!game) return;

		RECT client{};
		if (!GetClientRect(game, &client)) return;
		POINT topLeft{client.left, client.top};
		ClientToScreen(game, &topLeft);
		const int w = client.right - client.left;
		const int h = client.bottom - client.top;
		if (w <= 0 || h <= 0) return;

		if (topLeft.x != PosX || topLeft.y != PosY || w != Width || h != Height)
		{
			const bool sizeChanged = (w != Width || h != Height);
			PosX = topLeft.x;
			PosY = topLeft.y;
			Width = w;
			Height = h;
			SetWindowPos(Hwnd, HWND_TOPMOST, PosX, PosY, Width, Height, SWP_NOACTIVATE);

			if (sizeChanged && SwapChain)
			{
				ReleaseRtv();
				SwapChain->ResizeBuffers(0, static_cast<UINT>(Width), static_cast<UINT>(Height), DXGI_FORMAT_UNKNOWN, 0);
				CreateRtv();
			}
		}
	}

	/// @brief Render one overlay frame: clear transparent, replay this frame's recorded ESP commands
	/// into the overlay context's draw list, draw the menu, and present through DirectComposition.
	/// While the game (and overlay) are not the foreground window the overlay paints nothing — it still
	/// runs a frame to drain the recorded ESP command buffer and presents a fully transparent frame, so
	/// alt-tabbing away hides the ESP/menu instead of leaving it over the desktop or another app.
	inline void RenderFrame()
	{
		if (!Rtv) return;

		const bool focused = GameFocused();

		ScopedContext scoped(Ctx);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImFont* font = ImGui::GetFont();
		// Drain the recorded ESP commands every frame (Flush swaps the buffer out under its lock even
		// when the draw list is null), but only actually draw them while focused.
		Render::Flush(focused ? ImGui::GetBackgroundDrawList() : nullptr, font, ImGui::GetFontSize());

		const bool showMenu = focused && Settings.MENU.ShowMenu;
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDrawCursor = showMenu;
		io.WantCaptureMouse = showMenu;
		io.WantTextInput = showMenu;
		io.WantCaptureKeyboard = showMenu;

		// Call Menu::Draw whenever focused (not just when the menu is visible): it also polls the toggle
		// hotkey and draws the watermark, both of which must run while the menu is closed so it can be
		// reopened. Menu::Draw self-guards the window with its own ShowMenu check.
		if (focused) Menu::Draw();

		ImGui::EndFrame();
		ImGui::Render();

		const float transparent[4] = {0.f, 0.f, 0.f, 0.f};
		Context->OMSetRenderTargets(1, &Rtv, nullptr);
		Context->ClearRenderTargetView(Rtv, transparent);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		SwapChain->Present(1, 0);
	}

	/// @brief Create the layered/click-through/topmost/capture-excluded window (no redirection bitmap,
	/// so DirectComposition owns the pixels). @return true on success.
	inline bool CreateOverlayWindow()
	{
		WNDCLASSEXW wc{};
		wc.cbSize = sizeof(wc);
		wc.lpfnWndProc = WndProc;
		wc.hInstance = GetModuleHandleW(nullptr);
		wc.lpszClassName = ClassName;
		RegisterClassExW(&wc);

		HWND game = GameWindow();
		RECT client{0, 0, 100, 100};
		POINT topLeft{0, 0};
		if (game && GetClientRect(game, &client))
		{
			topLeft = {client.left, client.top};
			ClientToScreen(game, &topLeft);
		}
		Width = client.right - client.left;
		Height = client.bottom - client.top;
		PosX = topLeft.x;
		PosY = topLeft.y;

		// NOREDIRECTIONBITMAP: DirectComposition presents the pixels, so we don't want a GDI
		// redirection surface. TRANSPARENT | NOACTIVATE start it click-through (menu closed).
		const DWORD exStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_NOREDIRECTIONBITMAP;
		Hwnd = CreateWindowExW(exStyle, ClassName, L"", WS_POPUP, PosX, PosY, Width, Height, nullptr, nullptr, wc.hInstance, nullptr);
		if (!Hwnd) return false;

		SetWindowDisplayAffinity(Hwnd, WDA_EXCLUDEFROMCAPTURE);
		ShowWindow(Hwnd, SW_SHOWNOACTIVATE);
		Interactive = false;
		return true;
	}

	/// @brief Build the D3D11 device, the DirectComposition swap chain/visual, and the RTV.
	/// @return true on success.
	inline bool CreatePipeline()
	{
		const UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // required for DirectComposition
		D3D_FEATURE_LEVEL featureLevel;
		if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, &Device, &featureLevel, &Context)))
			return false;

		IDXGIDevice* dxgiDevice = nullptr;
		if (FAILED(Device->QueryInterface(IID_PPV_ARGS(&dxgiDevice)))) return false;

		IDXGIAdapter* adapter = nullptr;
		IDXGIFactory2* factory = nullptr;
		bool ok = SUCCEEDED(dxgiDevice->GetAdapter(&adapter)) && SUCCEEDED(adapter->GetParent(IID_PPV_ARGS(&factory)));

		if (ok)
		{
			DXGI_SWAP_CHAIN_DESC1 desc{};
			desc.Width = static_cast<UINT>(Width > 0 ? Width : 1);
			desc.Height = static_cast<UINT>(Height > 0 ? Height : 1);
			desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc.BufferCount = 2;
			desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED; // per-pixel alpha over the desktop
			ok = SUCCEEDED(factory->CreateSwapChainForComposition(Device, &desc, nullptr, &SwapChain));
		}

		if (ok) ok = SUCCEEDED(DCompositionCreateDevice(dxgiDevice, IID_PPV_ARGS(&DcompDevice)));
		if (ok) ok = SUCCEEDED(DcompDevice->CreateTargetForHwnd(Hwnd, TRUE, &DcompTarget));
		if (ok) ok = SUCCEEDED(DcompDevice->CreateVisual(&DcompVisual));
		if (ok) ok = SUCCEEDED(DcompVisual->SetContent(SwapChain));
		if (ok) ok = SUCCEEDED(DcompTarget->SetRoot(DcompVisual));
		if (ok) ok = SUCCEEDED(DcompDevice->Commit());

		if (factory) factory->Release();
		if (adapter) adapter->Release();
		if (dxgiDevice) dxgiDevice->Release();

		if (!ok) return false;
		CreateRtv();
		return Rtv != nullptr;
	}

	/// @brief Create the overlay's own ImGui context and its DX11/Win32 backends (bound to @ref Hwnd
	/// and @ref Device). Applies the same theme as the game-window overlay so the two menus match.
	/// @return true on success.
	inline bool CreateImGui()
	{
		IMGUI_CHECKVERSION();
		Ctx = ImGui::CreateContext();
		if (!Ctx) return false;

		ScopedContext scoped(Ctx);
		GUI::Styles::Init(); // same cream/red palette + metrics as the internal overlay
		if (!ImGui_ImplWin32_Init(Hwnd)) return false;
		if (!ImGui_ImplDX11_Init(Device, Context)) return false;
		ImGui_ImplDX11_CreateDeviceObjects();
		return true;
	}

	/// @brief Release everything the overlay owns, in reverse creation order. Safe to call partially
	/// initialized (each step is guarded).
	inline void Teardown()
	{
		if (Ctx)
		{
			ScopedContext scoped(Ctx);
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
		}

		ReleaseRtv();
		if (DcompVisual) { DcompVisual->Release(); DcompVisual = nullptr; }
		if (DcompTarget) { DcompTarget->Release(); DcompTarget = nullptr; }
		if (DcompDevice) { DcompDevice->Release(); DcompDevice = nullptr; }
		if (SwapChain) { SwapChain->Release(); SwapChain = nullptr; }
		if (Context) { Context->Release(); Context = nullptr; }
		if (Device) { Device->Release(); Device = nullptr; }

		if (Ctx) { ImGui::DestroyContext(Ctx); Ctx = nullptr; }

		if (Hwnd)
		{
			DestroyWindow(Hwnd);
			Hwnd = nullptr;
		}
		UnregisterClassW(ClassName, GetModuleHandleW(nullptr));
	}

	/// @brief Thread body: build the window + pipeline + ImGui, then pump messages and render until
	/// Stop() clears @ref Running, then tear everything down. All D3D/ImGui state is created and
	/// destroyed on this one thread.
	inline DWORD WINAPI ThreadProc(LPVOID)
	{
		if (!CreateOverlayWindow() || !CreatePipeline() || !CreateImGui())
		{
			Logger::Log("ERROR", "[Overlay] external overlay init failed");
			Teardown();
			Running = false;
			return 1;
		}
		Logger::Log("SUCCESS", "[Overlay] external streamproof overlay started");

		while (Running)
		{
			MSG msg;
			while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}

			SyncRect();
			SyncInteractivity();
			RenderFrame(); // Present(1, ...) paces the loop to vsync
		}

		Teardown();
		Logger::Log("INFO", "[Overlay] external streamproof overlay stopped");
		return 0;
	}

	/// @brief Spin up the overlay thread if it isn't already running. Idempotent.
	inline void Start()
	{
		if (Running || Thread) return;
		Running = true;
		Thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
		if (!Thread) Running = false;
	}

	/// @brief Stop the overlay thread and wait for it to finish tearing down. Idempotent.
	inline void Stop()
	{
		if (!Thread) return;
		Running = false;
		WaitForSingleObject(Thread, 2000);
		CloseHandle(Thread);
		Thread = nullptr;
	}

	/// @return true while the overlay thread is active.
	inline bool Active() { return Thread != nullptr; }
} // namespace ExternalWindow

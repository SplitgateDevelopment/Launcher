#pragma once

/// @file
/// @brief Streamproof external overlay renderer: a separate, capture-excluded top-level window that
/// carries everything the drawing backend renders — the ESP, bullet traces, radar, glow, snaplines,
/// watermark, etc. (all replayed from the recorded Render::* command buffer) — so screen / window
/// capture (OBS, Game Bar, Discord) sees the game but none of the overlay's drawing. The menu is
/// deliberately NOT drawn here — it renders on the game-window overlay, which owns the input path;
/// this window is display-only and never touches input or focus.
///
/// Why a whole separate window. SetWindowDisplayAffinity(WDA_EXCLUDEFROMCAPTURE) on the *game*
/// window hides the whole game from capture — useless for streaming. To hide only the overlay's
/// drawing it must live in its own window with that affinity set on it.
///
/// Same device, same thread. This window reuses the *game's* D3D11 device (Window::Device) for its
/// DirectComposition swap chain, and is created, rendered and presented entirely from the game thread
/// (GUI::Overlay, inside the Present hook) — GUI calls Start()/Render()/Stop(). An earlier version ran
/// a second D3D device on a second thread, presenting concurrently with the game; that contended on
/// the GPU/compositor and could hang the display driver (a TDR), and the cross-thread ImGui lock then
/// froze the game with it. One device + one thread removes both hazards: nothing runs on the GPU
/// concurrently, and the two ImGui contexts (game menu + this overlay) are switched serially with no
/// lock. Present uses no vsync so it never blocks the game thread.
///
/// Why display-only / click-through. Making this window interactive needs input, which means either
/// stealing the game's focus (can't be handed back reliably) or global low-level hooks (can lock
/// input system-wide). Both proved unshippable, so this window never takes focus or input; it is
/// permanently click-through (WM_NCHITTEST -> HTTRANSPARENT, since WS_EX_TRANSPARENT alone doesn't
/// pass the mouse through a no-redirection-bitmap window).
///
/// Per-pixel transparency uses a composition swap chain (DXGI_ALPHA_MODE_PREMULTIPLIED) presented
/// through DirectComposition; ImGui's DX11 blend writes premultiplied-correct alpha, so clearing the
/// target transparent and drawing on top composites cleanly over the desktop/game.

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

#include "Window.h" // Window::Device / Window::DeviceContext (the game's) + WindowHandle + WDA fallback
#include "../../render/Render.h"
#include "../../settings/Settings.h"
#include "../../utils/Logger.h"

/// @brief The streamproof external overlay window, its DirectComposition swap chain (on the game's
/// D3D11 device), its own ImGui context (draw list + font only), and the Start/Render/Stop surface
/// GUI::Overlay drives from the game thread.
namespace ExternalWindow
{
	inline HWND Hwnd = nullptr;					  ///< the overlay window (separate from the game window)
	inline IDXGISwapChain1* SwapChain = nullptr;  ///< composition swap chain on the game's device (per-pixel alpha)
	inline ID3D11RenderTargetView* Rtv = nullptr; ///< RTV over the swap chain's back buffer
	inline IDCompositionDevice* DcompDevice = nullptr;
	inline IDCompositionTarget* DcompTarget = nullptr;
	inline IDCompositionVisual* DcompVisual = nullptr;
	inline ImGuiContext* Ctx = nullptr; ///< the overlay's ImGui context (distinct from the game menu's)

	inline bool Started = false;	  ///< true between Start() and Stop()
	inline int Width = 0, Height = 0; ///< current back-buffer size (game client size)
	inline int PosX = 0, PosY = 0;	  ///< current window top-left (game client, in screen coords)

	static constexpr wchar_t ClassName[] = L"SplitgateOverlay";

	/// @brief The game window whose client rect the overlay tracks.
	inline HWND GameWindow()
	{
		if (Window::WindowHandle) return Window::WindowHandle;
		return FindWindowW(L"UnrealWindow", L"PortalWars  ");
	}

	/// @brief Whether the game window is the foreground window — used to hide the overlay when you
	/// alt-tab away so it doesn't float over the desktop or another app.
	inline bool GameFocused()
	{
		const HWND fg = GetForegroundWindow();
		return fg && fg == GameWindow();
	}

	/// @brief (Re)create the RTV from the swap chain's back buffer, on the game's device.
	inline void CreateRtv()
	{
		if (!SwapChain || !Window::Device) return;
		ID3D11Texture2D* backBuffer = nullptr;
		if (FAILED(SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))) || !backBuffer) return;
		Window::Device->CreateRenderTargetView(backBuffer, nullptr, &Rtv);
		backBuffer->Release();
	}

	inline void ReleaseRtv()
	{
		if (!Rtv) return;
		Rtv->Release();
		Rtv = nullptr;
	}

	/// @brief Window procedure. Force click-through at the hit-test level: WS_EX_TRANSPARENT alone
	/// doesn't reliably pass the mouse through a DirectComposition (WS_EX_NOREDIRECTIONBITMAP) window,
	/// so HTTRANSPARENT routes the mouse to whatever is underneath.
	inline LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_NCHITTEST) return HTTRANSPARENT;
		return DefWindowProcW(hWnd, msg, wParam, lParam);
	}

	/// @brief Keep the overlay positioned over the game's client area and sized to it; resize the swap
	/// chain when the size changes. No-op when nothing moved. Runs on the game thread.
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

	/// @brief Create the click-through/topmost/capture-excluded window (no redirection bitmap, so
	/// DirectComposition owns the pixels). @return true on success.
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

		const DWORD exStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_NOREDIRECTIONBITMAP;
		Hwnd = CreateWindowExW(exStyle, ClassName, L"", WS_POPUP, PosX, PosY, Width, Height, nullptr, nullptr, wc.hInstance, nullptr);
		if (!Hwnd) return false;

		SetWindowDisplayAffinity(Hwnd, WDA_EXCLUDEFROMCAPTURE);
		ShowWindow(Hwnd, SW_SHOWNOACTIVATE);
		return true;
	}

	/// @brief Build the DirectComposition swap chain/visual on the *game's* D3D11 device and the RTV.
	/// @return true on success.
	inline bool CreatePipeline()
	{
		if (!Window::Device) return false;

		IDXGIDevice* dxgiDevice = nullptr;
		if (FAILED(Window::Device->QueryInterface(IID_PPV_ARGS(&dxgiDevice)))) return false;

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
			ok = SUCCEEDED(factory->CreateSwapChainForComposition(Window::Device, &desc, nullptr, &SwapChain));
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

	/// @brief Create the overlay's own ImGui context and its DX11/Win32 backends, bound to @ref Hwnd
	/// and the *game's* device. A distinct context from the menu's, switched to serially on the game
	/// thread (no lock needed). @return true on success.
	inline bool CreateImGui()
	{
		IMGUI_CHECKVERSION();
		Ctx = ImGui::CreateContext();
		if (!Ctx) return false;

		ImGui::SetCurrentContext(Ctx);
		if (!ImGui_ImplWin32_Init(Hwnd)) return false;
		if (!ImGui_ImplDX11_Init(Window::Device, Window::DeviceContext)) return false;
		ImGui_ImplDX11_CreateDeviceObjects();
		return true;
	}

	/// @brief Release everything the overlay owns, in reverse creation order. Does NOT release the
	/// game's device/context (owned by the game-window overlay). Safe to call partially initialized.
	inline void Teardown()
	{
		if (Ctx)
		{
			ImGui::SetCurrentContext(Ctx);
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext(Ctx);
			Ctx = nullptr;
		}

		ReleaseRtv();
		if (DcompVisual) { DcompVisual->Release(); DcompVisual = nullptr; }
		if (DcompTarget) { DcompTarget->Release(); DcompTarget = nullptr; }
		if (DcompDevice) { DcompDevice->Release(); DcompDevice = nullptr; }
		if (SwapChain) { SwapChain->Release(); SwapChain = nullptr; }

		if (Hwnd)
		{
			DestroyWindow(Hwnd);
			Hwnd = nullptr;
		}
		UnregisterClassW(ClassName, GetModuleHandleW(nullptr));
	}

	/// @brief Create the window + pipeline + ImGui (all on the game thread). Idempotent. On any
	/// failure it tears down and stays inactive.
	inline void Start()
	{
		if (Started) return;
		if (!CreateOverlayWindow() || !CreatePipeline() || !CreateImGui())
		{
			Logger::Log("ERROR", "[Overlay] external overlay init failed");
			Teardown();
			return;
		}
		Started = true;
		Logger::Log("SUCCESS", "[Overlay] external streamproof overlay started");
	}

	/// @brief Tear the overlay down (game thread). Idempotent.
	inline void Stop()
	{
		if (!Started) return;
		Teardown();
		Started = false;
		Logger::Log("INFO", "[Overlay] external streamproof overlay stopped");
	}

	/// @return true while the overlay is active.
	inline bool Active() { return Started; }

	/// @brief Render one overlay frame. Called from GUI::Overlay (game thread) each Present while the
	/// External renderer is selected: pump this window's messages, track the game rect, then replay the
	/// recorded draw commands into this context and present. Leaves this context current — the caller
	/// re-selects the menu context afterwards. Present uses no vsync so the game thread never blocks.
	inline void Render()
	{
		if (!Started) return;

		// Pump this window's own messages (WM_NCHITTEST for click-through) on its creating thread.
		MSG msg;
		while (PeekMessageW(&msg, Hwnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		SyncRect();
		if (!Rtv) return; // mid-resize; skip this frame

		const bool focused = GameFocused();

		ImGui::SetCurrentContext(Ctx);
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImFont* font = ImGui::GetFont();
		// Drain the recorded draw commands every frame (Flush swaps the buffer out under its own lock
		// even when the draw list is null), but only actually draw them while the game is focused.
		Render::Flush(focused ? ImGui::GetBackgroundDrawList() : nullptr, font, ImGui::GetFontSize());

		ImGui::EndFrame();
		ImGui::Render();

		const float transparent[4] = {0.f, 0.f, 0.f, 0.f};
		Window::DeviceContext->OMSetRenderTargets(1, &Rtv, nullptr);
		Window::DeviceContext->ClearRenderTargetView(Rtv, transparent);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		SwapChain->Present(0, 0); // no vsync: we're on the game thread and must not block on it
	}
} // namespace ExternalWindow

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
/// Why its own thread. The window needs a Win32 message pump, and the game's render thread (where
/// Present runs) has none for our window. So this owns a dedicated thread that creates the window,
/// its own D3D11 device + DirectComposition swap chain (for true per-pixel alpha), and a *second*
/// ImGui context, then runs a pump + render loop. GUI drives Start()/Stop() as RendererMode::External
/// is entered / left; while it runs, the game-window overlay skips the menu + ESP flush so nothing
/// draws twice.
///
/// Input WITHOUT stealing focus. The overlay window is permanently click-through
/// (WS_EX_TRANSPARENT | WS_EX_NOACTIVATE) and never takes the foreground from the game — stealing
/// focus and handing it back proved impossible to do reliably (the game's viewport wouldn't re-grab
/// the mouse, and the handoff froze input). Instead, while the menu is open, low-level input hooks
/// (WH_MOUSE_LL / WH_KEYBOARD_LL) feed this ImGui context and swallow the events so they don't reach
/// the game — but only while the game is the foreground window, so other applications are untouched.
/// The game keeps focus the entire time, so its input can never break.
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

	inline HANDLE Thread = nullptr;		  ///< the pump + render thread
	inline volatile bool Running = false; ///< thread loop guard; Stop() clears it
	inline int Width = 0, Height = 0;	  ///< current back-buffer size (game client size)
	inline int PosX = 0, PosY = 0;		  ///< current window top-left (game client, in screen coords)

	inline HHOOK MouseHook = nullptr;	 ///< WH_MOUSE_LL, installed only while the menu is open
	inline HHOOK KeyboardHook = nullptr; ///< WH_KEYBOARD_LL, installed only while the menu is open
	inline bool HooksInstalled = false;	 ///< tracks the install/remove edge for SyncInputHooks

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

	/// @brief Whether the game window is the foreground window. Drives both what the overlay draws
	/// (hidden when you alt-tab away) and whether the input hooks act (so they never swallow input in
	/// another application). The overlay never takes the foreground itself, so it's never the match.
	inline bool GameFocused()
	{
		const HWND fg = GetForegroundWindow();
		return fg && fg == GameWindow();
	}

	/// @brief Map a Win32 virtual-key code to an ImGuiKey so the low-level keyboard hook can feed key
	/// events into the overlay's ImGui context (the Win32 backend's own mapping is internal to it).
	inline ImGuiKey ImGuiKeyFromVk(int vk)
	{
		switch (vk)
		{
		case VK_TAB: return ImGuiKey_Tab;
		case VK_LEFT: return ImGuiKey_LeftArrow;
		case VK_RIGHT: return ImGuiKey_RightArrow;
		case VK_UP: return ImGuiKey_UpArrow;
		case VK_DOWN: return ImGuiKey_DownArrow;
		case VK_PRIOR: return ImGuiKey_PageUp;
		case VK_NEXT: return ImGuiKey_PageDown;
		case VK_HOME: return ImGuiKey_Home;
		case VK_END: return ImGuiKey_End;
		case VK_INSERT: return ImGuiKey_Insert;
		case VK_DELETE: return ImGuiKey_Delete;
		case VK_BACK: return ImGuiKey_Backspace;
		case VK_SPACE: return ImGuiKey_Space;
		case VK_RETURN: return ImGuiKey_Enter;
		case VK_ESCAPE: return ImGuiKey_Escape;
		case VK_OEM_7: return ImGuiKey_Apostrophe;
		case VK_OEM_COMMA: return ImGuiKey_Comma;
		case VK_OEM_MINUS: return ImGuiKey_Minus;
		case VK_OEM_PERIOD: return ImGuiKey_Period;
		case VK_OEM_2: return ImGuiKey_Slash;
		case VK_OEM_1: return ImGuiKey_Semicolon;
		case VK_OEM_PLUS: return ImGuiKey_Equal;
		case VK_OEM_4: return ImGuiKey_LeftBracket;
		case VK_OEM_5: return ImGuiKey_Backslash;
		case VK_OEM_6: return ImGuiKey_RightBracket;
		case VK_OEM_3: return ImGuiKey_GraveAccent;
		case VK_LCONTROL: return ImGuiKey_LeftCtrl;
		case VK_RCONTROL: return ImGuiKey_RightCtrl;
		case VK_LSHIFT: return ImGuiKey_LeftShift;
		case VK_RSHIFT: return ImGuiKey_RightShift;
		case VK_LMENU: return ImGuiKey_LeftAlt;
		case VK_RMENU: return ImGuiKey_RightAlt;
		default: break;
		}
		if (vk >= 'A' && vk <= 'Z') return static_cast<ImGuiKey>(ImGuiKey_A + (vk - 'A'));
		if (vk >= '0' && vk <= '9') return static_cast<ImGuiKey>(ImGuiKey_0 + (vk - '0'));
		if (vk >= VK_F1 && vk <= VK_F12) return static_cast<ImGuiKey>(ImGuiKey_F1 + (vk - VK_F1));
		if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) return static_cast<ImGuiKey>(ImGuiKey_Keypad0 + (vk - VK_NUMPAD0));
		return ImGuiKey_None;
	}

	/// @brief WH_MOUSE_LL proc: while the menu is open and the game is focused, feed mouse motion /
	/// buttons / wheel into the overlay's ImGui context and swallow buttons + wheel so the game doesn't
	/// also act on them. Motion passes through (raw-input mouse-look is a separate path this can't
	/// block anyway). Passes everything through when the menu is closed or another app is focused.
	inline LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam)
	{
		if (code == HC_ACTION && Settings.MENU.ShowMenu && Ctx && GameFocused())
		{
			auto* ms = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
			{
				ScopedContext scoped(Ctx);
				ImGuiIO& io = ImGui::GetIO();
				io.AddMousePosEvent(static_cast<float>(ms->pt.x - PosX), static_cast<float>(ms->pt.y - PosY));
				switch (wParam)
				{
				case WM_LBUTTONDOWN: io.AddMouseButtonEvent(0, true); break;
				case WM_LBUTTONUP: io.AddMouseButtonEvent(0, false); break;
				case WM_RBUTTONDOWN: io.AddMouseButtonEvent(1, true); break;
				case WM_RBUTTONUP: io.AddMouseButtonEvent(1, false); break;
				case WM_MBUTTONDOWN: io.AddMouseButtonEvent(2, true); break;
				case WM_MBUTTONUP: io.AddMouseButtonEvent(2, false); break;
				case WM_MOUSEWHEEL: io.AddMouseWheelEvent(0.f, static_cast<short>(HIWORD(ms->mouseData)) / static_cast<float>(WHEEL_DELTA)); break;
				case WM_MOUSEHWHEEL: io.AddMouseWheelEvent(static_cast<short>(HIWORD(ms->mouseData)) / static_cast<float>(WHEEL_DELTA), 0.f); break;
				default: break;
				}
			}
			if (wParam != WM_MOUSEMOVE) return 1; // swallow clicks + wheel so the game doesn't act on them
		}
		return CallNextHookEx(nullptr, code, wParam, lParam);
	}

	/// @brief WH_KEYBOARD_LL proc: while the menu is open and the game is focused, feed keys + typed
	/// characters into the overlay's ImGui context and swallow them so the game doesn't act on them
	/// (WASD, etc.). Alt/Win combos pass through so the user can still alt-tab / use the shell.
	inline LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam)
	{
		if (code == HC_ACTION && Settings.MENU.ShowMenu && Ctx && GameFocused())
		{
			auto* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
			const int vk = static_cast<int>(kb->vkCode);
			const bool altDown = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
			if (altDown || vk == VK_LWIN || vk == VK_RWIN)
				return CallNextHookEx(nullptr, code, wParam, lParam); // let system combos through

			const bool down = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
			{
				ScopedContext scoped(Ctx);
				ImGuiIO& io = ImGui::GetIO();

				// Modifiers from the physical key state — reliable off the overlay thread's own queue.
				io.AddKeyEvent(ImGuiMod_Ctrl, (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
				io.AddKeyEvent(ImGuiMod_Shift, (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);
				io.AddKeyEvent(ImGuiMod_Alt, (GetAsyncKeyState(VK_MENU) & 0x8000) != 0);

				const ImGuiKey key = ImGuiKeyFromVk(vk);
				if (key != ImGuiKey_None) io.AddKeyEvent(key, down);

				if (down)
				{
					// Translate the key to its character(s) for text inputs. Build the key state from the
					// physical modifier state so the current keypress is reflected (the thread queue isn't).
					BYTE keyState[256] = {};
					keyState[VK_SHIFT] = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? 0x80 : 0;
					keyState[VK_CONTROL] = (GetAsyncKeyState(VK_CONTROL) & 0x8000) ? 0x80 : 0;
					keyState[VK_MENU] = (GetAsyncKeyState(VK_MENU) & 0x8000) ? 0x80 : 0;
					keyState[VK_CAPITAL] = static_cast<BYTE>(GetKeyState(VK_CAPITAL) & 0x0001);

					WCHAR buf[8] = {};
					const int n = ToUnicode(static_cast<UINT>(vk), kb->scanCode, keyState, buf, 7, 0);
					for (int i = 0; i < n; i++)
						if (buf[i] >= 0x20 && buf[i] != 0x7f) io.AddInputCharacterUTF16(static_cast<ImWchar16>(buf[i]));
				}
			}
			return 1; // swallow so keystrokes don't leak into the game while typing in the menu
		}
		return CallNextHookEx(nullptr, code, wParam, lParam);
	}

	/// @brief Install the low-level input hooks (idempotent). Called on the overlay thread, which has
	/// the message pump WH_*_LL hooks require. hMod is this DLL's handle (resolved from the proc's
	/// address), which SetWindowsHookExW wants for a global (thread id 0) low-level hook.
	inline void InstallInputHooks()
	{
		if (MouseHook || KeyboardHook) return;
		HMODULE self = nullptr;
		GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
						   reinterpret_cast<LPCWSTR>(&LowLevelMouseProc), &self);
		MouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, self, 0);
		KeyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, self, 0);
	}

	/// @brief Remove the input hooks and release any buttons ImGui still thinks are held, so a click
	/// that was down when the menu closed doesn't stick. Idempotent.
	inline void RemoveInputHooks()
	{
		if (MouseHook) { UnhookWindowsHookEx(MouseHook); MouseHook = nullptr; }
		if (KeyboardHook) { UnhookWindowsHookEx(KeyboardHook); KeyboardHook = nullptr; }
		if (!Ctx) return;
		ScopedContext scoped(Ctx);
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseButtonEvent(0, false);
		io.AddMouseButtonEvent(1, false);
		io.AddMouseButtonEvent(2, false);
	}

	/// @brief Install the input hooks while the menu is open, remove them when it closes. Only touches
	/// the hooks on an actual state change. The procs themselves also gate on GameFocused, so leaving
	/// the hooks installed across an alt-tab never swallows input in another application.
	inline void SyncInputHooks()
	{
		const bool want = Settings.MENU.ShowMenu;
		if (want == HooksInstalled) return;
		HooksInstalled = want;
		if (want)
			InstallInputHooks();
		else
			RemoveInputHooks();
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

	/// @brief Window procedure. The overlay is permanently click-through and never focused, so it gets
	/// essentially no input messages — all menu input arrives through the low-level hooks. Nothing to
	/// handle here.
	inline LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProcW(hWnd, msg, wParam, lParam);
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
	/// Draws only while the game is the foreground window: when unfocused it still runs a frame to
	/// drain the recorded ESP command buffer and presents a fully transparent frame, so alt-tabbing
	/// away hides the ESP/menu.
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

		// Draw the ImGui software cursor while the menu is open (the OS cursor stays hidden by the
		// focused game); input reaches ImGui through the low-level hooks, not window messages.
		ImGui::GetIO().MouseDrawCursor = focused && Settings.MENU.ShowMenu;

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
	/// so DirectComposition owns the pixels). The window stays click-through for its whole life — it
	/// never takes the foreground — so these ex-styles are never toggled. @return true on success.
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
		// redirection surface. TRANSPARENT | NOACTIVATE keep it click-through and non-activating for
		// its whole life — the menu is driven by the low-level input hooks, not window focus.
		const DWORD exStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_NOREDIRECTIONBITMAP;
		Hwnd = CreateWindowExW(exStyle, ClassName, L"", WS_POPUP, PosX, PosY, Width, Height, nullptr, nullptr, wc.hInstance, nullptr);
		if (!Hwnd) return false;

		SetWindowDisplayAffinity(Hwnd, WDA_EXCLUDEFROMCAPTURE);
		ShowWindow(Hwnd, SW_SHOWNOACTIVATE);
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
		RemoveInputHooks();

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
		HooksInstalled = false;
	}

	/// @brief Thread body: build the window + pipeline + ImGui, then pump messages and render until
	/// Stop() clears @ref Running, then tear everything down. All D3D/ImGui state is created and
	/// destroyed on this one thread, which also owns the low-level input hooks' message pump.
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
			SyncInputHooks();
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

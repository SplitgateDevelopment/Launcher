#pragma once

/// @file
/// @brief DirectX 11 overlay bootstrap: hooks the swap chain, initializes ImGui, and renders the menu each Present.

#include "Window.h"
#include "Config.h"
#include "Styles.h"
#include "Custom.h"
#include "ExternalWindow.h"
#include "../Menu.h"
#include "../../render/Render.h"

#include "imgui_Impl_dx11.h"
#include "imgui_Impl_Win32.h"

/// @brief DirectX 11 overlay: swap-chain hooks, ImGui setup, and per-frame rendering.
namespace GUI
{
	bool initialized = false;				 ///< True once ImGui and the D3D11 render target have been set up.
	ImGuiContext* internalContext = nullptr; ///< The game-window overlay's ImGui context. Selected explicitly
											 ///< each frame because the external overlay renders with its own
											 ///< context on this same thread (both share ImGui's global current
											 ///< context, so each pass picks its own before NewFrame).

	typedef HRESULT(APIENTRY* IDXGISwapChainPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	IDXGISwapChainPresent oIDXGISwapChainPresent = NULL; ///< Trampoline to the original IDXGISwapChain::Present.

	typedef void(APIENTRY* ID3D11DrawIndexed)(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
	ID3D11DrawIndexed oID3D11DrawIndexed = NULL; ///< Trampoline to the original ID3D11DeviceContext::DrawIndexed.

	typedef HRESULT(APIENTRY* IDXGISwapChainResizeBuffers)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	IDXGISwapChainResizeBuffers oIDXGISwapChainResizeBuffers = NULL; ///< Trampoline to the original IDXGISwapChain::ResizeBuffers.

	/// @brief Lazily initializes ImGui against the game's swap chain: finds the game window, grabs the D3D11
	/// device/context and back buffer, creates the render target, wires the Win32/DX11 backends, and subclasses
	/// the window procedure. @return True on success; sets @ref initialized accordingly.
	bool InitializeImGui(IDXGISwapChain* swapChain)
	{
		Window::WindowHandle = FindWindow((L"UnrealWindow"), (L"PortalWars  "));
		// Keep the real game swap chain so CreateRenderTarget can rebuild the RTV on resize. AddRef
		// so the matching Release in Window::Destroy() is balanced (we don't own the game's chain).
		Window::SwapChain = swapChain;
		swapChain->AddRef();

		if (!SUCCEEDED(swapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Window::Device)))
		{
			initialized = false;
			return false;
		}

		IMGUI_CHECKVERSION();
		internalContext = ImGui::CreateContext();

		Config::Init();
		Styles::Init();

		Window::Device->GetImmediateContext(&Window::DeviceContext);

		ID3D11Texture2D* BackBuffer = nullptr;
		HRESULT hResult = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);

		if (FAILED(hResult) || BackBuffer == nullptr)
		{
			initialized = false;
			return false;
		}

		hResult = Window::Device->CreateRenderTargetView(BackBuffer, NULL, &Window::RenderTargetView);
		BackBuffer->Release();

		if (FAILED(hResult))
		{
			initialized = false;
			return false;
		}

		ImGui_ImplWin32_Init(Window::WindowHandle);
		ImGui_ImplDX11_Init(Window::Device, Window::DeviceContext);
		ImGui_ImplDX11_CreateDeviceObjects();

		// Cache this context's default font for Render::Measure, so features measure text without ever
		// reading ImGui's global current-context (which the external overlay switches on another thread).
		// Fonts[0] is valid now that the atlas has been built above.
		if (ImFontAtlas* fonts = ImGui::GetIO().Fonts; fonts && fonts->Fonts.Size > 0)
			Render::imgui.measureFont = fonts->Fonts[0];
		ImGui::GetMainViewport()->PlatformHandleRaw = Window::WindowHandle;
		Window::OldWindowProcess = (WNDPROC)SetWindowLongPtr(Window::WindowHandle, GWLP_WNDPROC, (__int3264)(LONG_PTR)Window::WndProc);

		initialized = true;
		return true;
	}

	/// @brief Per-Present entry point: initializes ImGui on first call, services pending swap-chain resizes,
	/// begins a new frame, syncs input capture to menu visibility, and draws the menu.
	void Overlay(IDXGISwapChain* pSwapChain = nullptr)
	{
		if (!pSwapChain) return;

		if (!initialized)
			InitializeImGui(pSwapChain);
		if (!initialized) return;

		// Streamproof (External) mode: a separate, capture-excluded window renders everything the
		// drawing backend produces (ESP / radar / bullet traces / ...) plus the watermark, so capture
		// can't see them. Keep that thread running; the game window below still draws the MENU (it owns
		// the working input path), but skips the Render flush + watermark so those are drawn only on the
		// streamproof window - which is also where the command buffer is drained, not here.
		const bool external = (Settings.MENU.Renderer == RendererMode::External);
		if (external)
			ExternalWindow::Start();
		else if (ExternalWindow::Active())
			ExternalWindow::Stop();

		// Skip rendering into a degenerate target: minimized, or a zero-sized client area (which is
		// also true during the minimize/restore transition, before IsIconic flips). Running ImGui at
		// a 0x0 display size corrupts D3D state (freeze then crash) - and it's worse with the menu
		// open, since that draws far more geometry. Just let Present forward until the window is back.
		RECT clientRect{};
		if (IsIconic(Window::WindowHandle) || !GetClientRect(Window::WindowHandle, &clientRect) ||
			clientRect.right - clientRect.left <= 0 || clientRect.bottom - clientRect.top <= 0)
			return;

		// A window resize releases our render target in HookResizeBuffers (so the game's own
		// ResizeBuffers can succeed); recreate it here once the swap chain has the new back buffer.
		if (!Window::RenderTargetView)
			Window::CreateRenderTarget();
		if (!Window::RenderTargetView) return; // RTV not ready yet (mid-resize) - skip this frame

		// External renderer: draw the ESP/radar/traces/watermark on the streamproof window first (its own
		// ImGui context, but the same device and this same thread, so no lock is needed). It drains the
		// recorded command buffer, so the game-window pass below must not also flush it.
		if (external)
			ExternalWindow::Render();

		// The two overlays share ImGui's single global current-context pointer but run serially on this
		// thread, so just select the menu's context explicitly before rendering it.
		ImGui::SetCurrentContext(internalContext);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// Replay the recorded draw commands (ESP/radar/traces/watermark/..., no-op in canvas mode) on
		// the game window only in the non-streamproof modes; External already drew + drained them above.
		if (!external)
			Render::Flush();

		// Game-input capture is keyed on ShowMenu (backend-agnostic) so input is blocked whenever the
		// menu is open, regardless of which backend draws it. The menu cursor is a backend concern:
		// default it off; the ImGui backend re-enables ImGui's software cursor itself when it draws.
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.MouseDrawCursor = false;
		io.WantCaptureMouse = Settings.MENU.ShowMenu;
		io.WantTextInput = Settings.MENU.ShowMenu;
		io.WantCaptureKeyboard = Settings.MENU.ShowMenu;

		// Draw the menu for the Present-driven backend (the ImGui backend); a no-op if Canvas is active.
		Menu::Frame(Menu::Phase::Present);

		ImGui::EndFrame();
		ImGui::Render();

		Window::DeviceContext->OMSetRenderTargets(1, &Window::RenderTargetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	/// @brief Hooked IDXGISwapChain::Present: renders the overlay, then forwards to the original Present.
	HRESULT APIENTRY HookPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		// The hook sits on the shared IDXGISwapChain::Present, so our own external-overlay swap chain's
		// Present lands here too. Drive the overlay ONLY for the game's swap chain: presenting the
		// external chain from inside Overlay would otherwise re-enter Overlay -> ExternalWindow::Render
		// -> Present -> HookPresent -> ... and overflow the stack. Forward the external chain untouched.
		if (pSwapChain != static_cast<IDXGISwapChain*>(ExternalWindow::SwapChain))
			Overlay(pSwapChain);
		return oIDXGISwapChainPresent(pSwapChain, SyncInterval, Flags);
	}

	/// @brief Hooked IDXGISwapChain::ResizeBuffers: release our render target so the game's resize
	/// can proceed (an outstanding back-buffer reference would make ResizeBuffers fail), forward to
	/// the original, then let Overlay recreate the RTV from the new back buffer next frame.
	HRESULT APIENTRY HookResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
	{
		Window::CleanupRenderTarget();
		return oIDXGISwapChainResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
	}

	/// @brief Hooked ID3D11DeviceContext::DrawIndexed; currently a no-op stub.
	void APIENTRY MJDrawIndexed(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
	{
		return;
	}

	/// @brief Sets up the dummy device and installs the Present and DrawIndexed vtable hooks. @return True on success.
	bool Init()
	{
		if (!Window::Init()) return FALSE;

		Window::CreateHook(8, (void**)&oIDXGISwapChainPresent, HookPresent);
		Window::CreateHook(13, (void**)&oIDXGISwapChainResizeBuffers, HookResizeBuffers);
		Window::CreateHook(12, (void**)&oID3D11DrawIndexed, MJDrawIndexed);

		return TRUE;
	};

	/// @brief Tears down the ImGui backends and context and releases the window/D3D resources.
	void Destroy()
	{
		ExternalWindow::Stop(); // tear down the external overlay (window + swap chain + its ImGui context)
		// Select our context explicitly (the external teardown may have left the global elsewhere) and
		// destroy it by handle rather than "the current context".
		ImGui::SetCurrentContext(internalContext);
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext(internalContext);
		internalContext = nullptr;
		Window::Destroy();
		initialized = false; // so a re-inject re-runs InitializeImGui instead of assuming it's set up
	};
} // namespace GUI
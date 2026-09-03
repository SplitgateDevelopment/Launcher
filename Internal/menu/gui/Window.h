#pragma once

/// @file
/// @brief Windowing and D3D11 plumbing for the overlay: a throwaway device used to harvest the DXGI/D3D
/// vtables for hooking, render-target management, the subclassed window procedure, and MinHook helpers.

#include <Windows.h>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <MinHook.h>
#include <imgui.h>

#pragma comment(lib, "MinHook.x64.lib")

#include "../../settings/Settings.h"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Defined on Windows 10 2004+ SDKs; provide a fallback so older SDKs still build (the call just
// no-ops at runtime on pre-2004 Windows).
#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

/// @brief Overlay windowing/D3D11 state and the hook plumbing that backs the GUI.
namespace Window
{
	HWND WindowHandle{};						   ///< Target game window (or the temporary dummy window during init).
	static UINT ResizeWidth = 0, ResizeHeight = 0; ///< Pending swap-chain resize queued from WM_SIZE (0 = none).

	static ID3D11Device* Device{};
	static ID3D11DeviceContext* DeviceContext{};
	static IDXGISwapChain* SwapChain = nullptr;
	static ID3D11RenderTargetView* RenderTargetView{};

	WNDPROC OldWindowProcess{}; ///< Original game window procedure, saved when subclassing.

	namespace
	{
		WNDCLASSEX WindowClass;
		static uint64_t* MethodsTable = NULL; ///< Copied DXGI/D3D vtable entries, indexed by CreateHook.

		/// @brief Destroys and unregisters the temporary window. @return True once the handle is cleared.
		bool DeleteWindow()
		{
			DestroyWindow(WindowHandle);
			UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);

			return (WindowHandle == 0);
		};

		/// @brief Registers the window class and creates the temporary window used to build a swap chain. @return True on success.
		bool InitWindow()
		{
			WindowClass.cbSize = sizeof(WNDCLASSEX);
			WindowClass.style = CS_HREDRAW | CS_VREDRAW;
			WindowClass.lpfnWndProc = DefWindowProc;
			WindowClass.cbClsExtra = 0;
			WindowClass.cbWndExtra = 0;
			WindowClass.hInstance = GetModuleHandle(NULL);
			WindowClass.hIcon = NULL;
			WindowClass.hCursor = NULL;
			WindowClass.hbrBackground = NULL;
			WindowClass.lpszMenuName = NULL;
			WindowClass.lpszClassName = L"MJ";
			WindowClass.hIconSm = NULL;

			RegisterClassEx(&WindowClass);
			WindowHandle = CreateWindow(WindowClass.lpszClassName, L"DX11 Window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, WindowClass.hInstance, NULL);

			return (WindowHandle != NULL);
		}
	} // namespace

	/// @brief Subclassed game window procedure: while the menu is open it feeds input to ImGui and swallows the
	/// message; otherwise it queues resizes, blocks the ALT menu, and forwards to the original procedure.
	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (Settings.MENU.ShowMenu)
		{
			ImGui_ImplWin32_WndProcHandler((HWND)OldWindowProcess, msg, wParam, lParam);
			return true;
		}

		switch (msg)
		{
		case WM_SIZE:
			if (wParam == SIZE_MINIMIZED)
				return 0;

			// Queue resize
			ResizeWidth = (UINT)LOWORD(lParam);
			ResizeHeight = (UINT)HIWORD(lParam);

			return 0;

		case WM_SYSCOMMAND:
			// Disable ALT application menu
			if ((wParam & 0xfff0) == SC_KEYMENU)
				return 0;

			break;

		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		}

		return CallWindowProc((WNDPROC)OldWindowProcess, hWnd, msg, wParam, lParam);
	}

	/// @brief Creates the render target view from the swap chain's back buffer.
	void CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer = nullptr;

		if (!SUCCEEDED(SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer))))
		{
			return;
		}

		Device->CreateRenderTargetView(pBackBuffer, nullptr, &RenderTargetView);
		pBackBuffer->Release();
	}

	/// @brief Toggles whether the overlay window is excluded from screen capture (OBS, Discord,
	/// Game Bar). WDA_EXCLUDEFROMCAPTURE needs Windows 10 2004+; no-op until WindowHandle is set.
	/// @param enabled true to hide from capture, false to show normally.
	void SetStreamproof(bool enabled)
	{
		if (WindowHandle) SetWindowDisplayAffinity(WindowHandle, enabled ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
	}

	/// @brief Releases the render target view if present.
	void CleanupRenderTarget()
	{
		if (!RenderTargetView) return;

		RenderTargetView->Release();
		RenderTargetView = nullptr;
	}

	/**
	 * @brief Installs a MinHook detour on the vtable entry at @p Index of the harvested methods table.
	 * @param Index Slot in @ref MethodsTable to hook.
	 * @param Original Out: receives the trampoline to the original function.
	 * @param Function The detour to install.
	 * @return True if the hook was created and enabled.
	 */
	bool CreateHook(uint16_t Index, void** Original, void* Function)
	{
		assert(Index >= 0 && Original != NULL && Function != NULL);
		void* target = (void*)MethodsTable[Index];
		if (MH_CreateHook(target, Function, Original) != MH_OK || MH_EnableHook(target) != MH_OK)
		{
			return FALSE;
		}
		return TRUE;
	}

	/// @brief Builds a throwaway device + swap chain to copy the DXGI/D3D11 vtables into @ref MethodsTable,
	/// then releases all temporaries. Populates the table so CreateHook can target the game's real objects.
	/// @return True on success.
	bool Init()
	{
		if (!InitWindow())
			return FALSE;

		HMODULE D3D11Module = GetModuleHandleA("d3d11.dll");

		D3D_FEATURE_LEVEL FeatureLevel;
		const D3D_FEATURE_LEVEL FeatureLevels[] = {D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0};

		DXGI_RATIONAL RefreshRate;
		RefreshRate.Numerator = 60;
		RefreshRate.Denominator = 1;

		DXGI_MODE_DESC BufferDesc;
		BufferDesc.Width = 0;
		BufferDesc.Height = 0;
		BufferDesc.RefreshRate = RefreshRate;
		BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		DXGI_SAMPLE_DESC SampleDesc;
		SampleDesc.Count = 1;
		SampleDesc.Quality = 0;

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));
		SwapChainDesc.BufferDesc = BufferDesc;
		SwapChainDesc.SampleDesc = SampleDesc;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = 2;
		SwapChainDesc.OutputWindow = WindowHandle;
		SwapChainDesc.Windowed = TRUE;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		UINT createDeviceFlags = 0;
		// createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

		HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, FeatureLevels, 2, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain, &Device, &FeatureLevel, &DeviceContext);
		if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
			res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, FeatureLevels, 2, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain, &Device, &FeatureLevel, &DeviceContext);
		if (res != S_OK)
		{
			DeleteWindow();
			return FALSE;
		}

		MethodsTable = (uint64_t*)::calloc(205, sizeof(uint64_t));
		if (!MethodsTable)
		{
			DeleteWindow();
			return FALSE;
		}

		memcpy(MethodsTable, *(uint64_t**)SwapChain, 18 * sizeof(uint64_t));
		memcpy(MethodsTable + 18, *(uint64_t**)Device, 43 * sizeof(uint64_t));
		memcpy(MethodsTable + 18 + 43, *(uint64_t**)DeviceContext, 144 * sizeof(uint64_t));

		SwapChain->Release();
		SwapChain = NULL;

		Device->Release();
		Device = NULL;

		DeviceContext->Release();
		DeviceContext = NULL;

		DeleteWindow();
		return TRUE;
	}

	/// @brief Releases the render target and the swap chain/device/context held by the overlay.
	void Destroy()
	{
		CleanupRenderTarget();

		SwapChain->Release();
		SwapChain = NULL;
		Device->Release();
		Device = NULL;
		DeviceContext->Release();
		DeviceContext = NULL;
	}
} // namespace Window
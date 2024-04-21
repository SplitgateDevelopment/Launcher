#pragma once

#include "Window.h"
#include "Config.h"
#include "Styles.h"
#include "Custom.h"
#include "../Menu.h"

#include "imgui_Impl_dx11.h"
#include "imgui_Impl_Win32.h"

namespace GUI {
	bool initialized = false;

	typedef HRESULT(APIENTRY* IDXGISwapChainPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	IDXGISwapChainPresent oIDXGISwapChainPresent = NULL;

	typedef void(APIENTRY* ID3D11DrawIndexed)(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
	ID3D11DrawIndexed oID3D11DrawIndexed = NULL;

	bool InitializeImGui(IDXGISwapChain* swapChain)
	{
		Window::WindowHandle = FindWindow((L"UnrealWindow"), (L"PortalWars  "));

		if (SUCCEEDED(swapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Window::Device))) {
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			Config::Init();
			Styles::Init();

			Window::Device->GetImmediateContext(&Window::DeviceContext);

			ID3D11Texture2D* BackBuffer;
			swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);
			Window::Device->CreateRenderTargetView(BackBuffer, NULL, &Window::RenderTargetView);
			BackBuffer->Release();

			ImGui_ImplWin32_Init(Window::WindowHandle);
			ImGui_ImplDX11_Init(Window::Device, Window::DeviceContext);
			ImGui_ImplDX11_CreateDeviceObjects();
			ImGui::GetMainViewport()->PlatformHandleRaw = Window::WindowHandle;
			Window::OldWindowProcess = (WNDPROC)SetWindowLongPtr(Window::WindowHandle, GWLP_WNDPROC, (__int3264)(LONG_PTR)Window::WndProc);
			initialized = true;
			return true;
		}

		initialized = false;
		return false;
	}

	void Overlay(IDXGISwapChain* pSwapChain)
	{
		if (!initialized)
			InitializeImGui(pSwapChain);

		/*if (Window::ResizeWidth != 0 && Window::ResizeHeight != 0)
		{
			Window::CleanupRenderTarget();
			Window::SwapChain->ResizeBuffers(0, Window::ResizeWidth, Window::ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			Window::ResizeWidth = Window::ResizeHeight = 0;
			Window::CreateRenderTarget();
		}*/

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		const ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(mainViewport->WorkPos.x + 550, mainViewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 350), ImGuiCond_FirstUseEver);

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.MouseDrawCursor = Settings.MENU.ShowMenu;
		io.WantCaptureMouse = Settings.MENU.ShowMenu;
		io.WantTextInput = Settings.MENU.ShowMenu;
		io.WantCaptureKeyboard = Settings.MENU.ShowMenu;
		
		Menu::Draw();

		ImGui::EndFrame();
		ImGui::Render();

		Window::DeviceContext->OMSetRenderTargets(1, &Window::RenderTargetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	HRESULT APIENTRY HookPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		Overlay(pSwapChain);
		return oIDXGISwapChainPresent(pSwapChain, SyncInterval, Flags);
	}

	void APIENTRY MJDrawIndexed(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation) { return; }

	bool Init()
	{
		if (!Window::Init()) return FALSE;

		Window::CreateHook(8, (void**)&oIDXGISwapChainPresent, HookPresent);
		Window::CreateHook(12, (void**)&oID3D11DrawIndexed, MJDrawIndexed);

		return TRUE;
	};

	void Destroy()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		Window::Destroy();
	};
}
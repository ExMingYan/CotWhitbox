#include "entry.hpp"
#include "mapper/mapper.h"

#include "service/runable.h"

#include "context/configuration.h"

#include "controller/renderer.h"
#include "controller/dominator.h"

#include "utils/logger.hpp"

#include "font/MainFont.h"
#include "font/FontAewsome6.h"
#include "font/IconsFontAwesome6.inl"
#include "font/FontAwesome6Brands.h"
#include "font/IconsFontAwesome6Brands.inl"

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HRESULT hkPresent(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags);

HRESULT hkPresent(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags) {
	if (!ImGui_Initialised) {
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&DirectX12Interface::Device))) {
			ImGui::CreateContext();

			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

			DXGI_SWAP_CHAIN_DESC Desc;
			pSwapChain->GetDesc(&Desc);
			Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			Desc.OutputWindow = Process::Hwnd;
			Desc.Windowed = ((GetWindowLongPtr(Process::Hwnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

			DirectX12Interface::BuffersCounts = Desc.BufferCount;
			if (DirectX12Interface::BuffersCounts == 0) {
				ImGui::DestroyContext();
				return oPresent(pSwapChain, SyncInterval, Flags);
			}

			DirectX12Interface::FrameContext = new DirectX12Interface::_FrameContext[DirectX12Interface::BuffersCounts];

			D3D12_DESCRIPTOR_HEAP_DESC DescriptorImGuiRender = {};
			DescriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			DescriptorImGuiRender.NumDescriptors = (UINT)DirectX12Interface::BuffersCounts;
			DescriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

			if (DirectX12Interface::Device->CreateDescriptorHeap(&DescriptorImGuiRender, IID_PPV_ARGS(&DirectX12Interface::DescriptorHeapImGuiRender)) != S_OK)
				return oPresent(pSwapChain, SyncInterval, Flags);

			ID3D12CommandAllocator* Allocator;
			if (DirectX12Interface::Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&Allocator)) != S_OK)
				return oPresent(pSwapChain, SyncInterval, Flags);

			for (size_t i = 0; i < DirectX12Interface::BuffersCounts; i++) {
				DirectX12Interface::FrameContext[i].CommandAllocator = Allocator;
			}

			if (DirectX12Interface::Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, Allocator, NULL, IID_PPV_ARGS(&DirectX12Interface::CommandList)) != S_OK ||
				DirectX12Interface::CommandList->Close() != S_OK)
				return oPresent(pSwapChain, SyncInterval, Flags);

			D3D12_DESCRIPTOR_HEAP_DESC DescriptorBackBuffers;
			DescriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			DescriptorBackBuffers.NumDescriptors = (UINT)DirectX12Interface::BuffersCounts;
			DescriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			DescriptorBackBuffers.NodeMask = 1;

			if (DirectX12Interface::Device->CreateDescriptorHeap(&DescriptorBackBuffers, IID_PPV_ARGS(&DirectX12Interface::DescriptorHeapBackBuffers)) != S_OK)
				return oPresent(pSwapChain, SyncInterval, Flags);

			const auto RTVDescriptorSize = DirectX12Interface::Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = DirectX12Interface::DescriptorHeapBackBuffers->GetCPUDescriptorHandleForHeapStart();

			for (size_t i = 0; i < DirectX12Interface::BuffersCounts; i++) {
				ID3D12Resource* pBackBuffer = nullptr;
				DirectX12Interface::FrameContext[i].DescriptorHandle = RTVHandle;
				pSwapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&pBackBuffer));
				DirectX12Interface::Device->CreateRenderTargetView(pBackBuffer, nullptr, RTVHandle);
				DirectX12Interface::FrameContext[i].Resource = pBackBuffer;
				RTVHandle.ptr += RTVDescriptorSize;
			}

			float font_size = 16.0f;
			float icon_size = font_size * 2.0f / 3.0f;

			ImFontConfig ifc;
			ifc.FontDataOwnedByAtlas = false;
			configurs::mainfont = io.Fonts->AddFontFromMemoryTTF((void*)MainFont, MainFont_len, 20.0f, &ifc, io.Fonts->GetGlyphRangesChineseFull());

			static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA,0 };
			ImFontConfig icons_config;
			icons_config.MergeMode = true;
			icons_config.PixelSnapH = true;
			icons_config.GlyphMinAdvanceX = icon_size;
			configurs::icon_awesmoe = io.Fonts->AddFontFromMemoryTTF(FontAewsome6, sizeof(FontAewsome6), icon_size, &icons_config, icons_ranges);
			configurs::icon_brands = io.Fonts->AddFontFromMemoryTTF(FontAwesome6Brands, sizeof(FontAwesome6Brands), 30.0f, &icons_config, icons_ranges);

			ImGuiStyle& style = imgui::GetStyle();
			style.Colors[ImGuiCol_WindowBg] = ImColor(36, 40, 49, 255).Value;//背景色
			style.FrameRounding = 4.0f;//圆角

			ImGui::StyleColorsDark();
			ImGui_ImplWin32_Init(Process::Hwnd);
			ImGui_ImplDX12_Init(DirectX12Interface::Device, (UINT)DirectX12Interface::BuffersCounts,
				DXGI_FORMAT_R8G8B8A8_UNORM, DirectX12Interface::DescriptorHeapImGuiRender,
				DirectX12Interface::DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart(),
				DirectX12Interface::DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart());
			ImGui_ImplDX12_CreateDeviceObjects();
			ImGui::GetIO().ImeWindowHandle = Process::Hwnd;
		}
		else
		{
			return oPresent(pSwapChain, SyncInterval, Flags);
		}
		ImGui_Initialised = true;
	}

	if (DirectX12Interface::CommandQueue == nullptr || ImGui::GetCurrentContext() == nullptr || DirectX12Interface::CommandList == nullptr || DirectX12Interface::DescriptorHeapImGuiRender == nullptr)
		return oPresent(pSwapChain, SyncInterval, Flags);

	if (GetAsyncKeyState(VK_INSERT) & 1)
		ShowMouseCursor = !ShowMouseCursor;
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::GetIO().MouseDrawCursor = ShowMouseCursor;
	static bool open = true;
	static bool pressed = false;
	short state = GetAsyncKeyState(VK_F3) & 0x8000;
	if (state && !pressed)
		open = !open;
	pressed = state;

	if (open) {
		dominator dom;
		dom.window();
	}

	renderer render;
	render.render();

	ImGui::Render();

	ImGui::EndFrame();
	DirectX12Interface::_FrameContext& CurrentFrameContext = DirectX12Interface::FrameContext[pSwapChain->GetCurrentBackBufferIndex()];
	CurrentFrameContext.CommandAllocator->Reset();

	D3D12_RESOURCE_BARRIER Barrier;
	Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	Barrier.Transition.pResource = CurrentFrameContext.Resource;
	Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	DirectX12Interface::CommandList->Reset(CurrentFrameContext.CommandAllocator, nullptr);
	DirectX12Interface::CommandList->ResourceBarrier(1, &Barrier);
	DirectX12Interface::CommandList->OMSetRenderTargets(1, &CurrentFrameContext.DescriptorHandle, FALSE, nullptr);
	DirectX12Interface::CommandList->SetDescriptorHeaps(1, &DirectX12Interface::DescriptorHeapImGuiRender);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectX12Interface::CommandList);
	Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	DirectX12Interface::CommandList->ResourceBarrier(1, &Barrier);
	DirectX12Interface::CommandList->Close();
	DirectX12Interface::CommandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&DirectX12Interface::CommandList));
	return oPresent(pSwapChain, SyncInterval, Flags);
}

void hkExecuteCommandLists(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists) {
	if (!DirectX12Interface::CommandQueue)
		DirectX12Interface::CommandQueue = queue;
	oExecuteCommandLists(queue, NumCommandLists, ppCommandLists);
}

void APIENTRY hkDrawInstanced(ID3D12GraphicsCommandList* dCommandList, UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation) {

	return oDrawInstanced(dCommandList, VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

void APIENTRY hkDrawIndexedInstanced(ID3D12GraphicsCommandList* dCommandList, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation) {

	return oDrawIndexedInstanced(dCommandList, IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	return ::CallWindowProcA(Process::WndProc, hWnd, msg, wParam, lParam);
}

bool entry(HMODULE hmodule)
{
	mapper* mapping = mapper::instance();
	if (mapping->initliaze() == false) {
		return false;
	}

	runable* invoke = runable::instance();
	if (invoke->initialize(mapping->uworld, mapping->WorldToScreen) == false) {
		return false;
	}

	Process::Title = "FATAL FURY: City of the Wolves  ";
	Process::ClassName = "UnrealWindow";

	do {
		Process::Hwnd = FindWindowA(Process::ClassName, Process::Title);
		if (Process::Hwnd) {
			Process::WndProc = (WNDPROC)SetWindowLongPtr(Process::Hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
		}

		if (Process::WndProc == nullptr) {
			Sleep(10);
		}
	} while (Process::WndProc == nullptr);

	Process::Module = hmodule;
	Process::ID = GetCurrentProcessId();
	Process::Handle = GetCurrentProcess();
	SetIniPath();
	bool InitHook = false;
	while (InitHook == false) {
		if (DirectX12::Init() == true) {
			CreateHook(54, (void**)&oExecuteCommandLists, hkExecuteCommandLists);
			CreateHook(140, (void**)&oPresent, hkPresent);
			CreateHook(84, (void**)&oDrawInstanced, hkDrawInstanced);
			CreateHook(85, (void**)&oDrawIndexedInstanced, hkDrawIndexedInstanced);
			InitHook = true;
		}
	}
	return true;
}

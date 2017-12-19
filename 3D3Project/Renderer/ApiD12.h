#pragma once
#ifndef APID12_H
#define APID12_H

#include <DXGI.h>
#include <DXGI1_5.h>
#include <D3DCommon.h>
#include <d3d12.h>
#include <D3d12sdklayers.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <wrl.h>

#include <vector>
#include <DTK12\DirectXHelpers.h>
#include <DTK12\d3dx12.h>
#include <DTK12\PlatformHelpers.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace Microsoft::WRL;
#define NUM_SWAP_BUFFERS 2

#pragma comment(lib,"DXGI.lib")
#pragma comment(lib,"D3d12.lib")
namespace D3DPROJECT
{
	namespace RENDERER
	{
		class ApiD12
		{
		public:
			ApiD12();
			~ApiD12();
			void CreateDirectXContent(HWND wndHandle, int width, int height);
			ID3D12Device* GetDevice() { return m_device; }
			IDXGISwapChain1* GetSwapChain() { return m_swapChain; }
			void SwapChain();
			void Clean(ID3D12PipelineState** pip, ID3D12RootSignature** root);
			void SetRenderTarget();
			void WaitForGpu();
			void D3DPROJECT::RENDERER::ApiD12::SetResourceTransitionBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource,
				D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);

			ID3D12GraphicsCommandList* GetComList() { return m_CommandList; }
			ID3D12CommandQueue* GetComQueue() { return m_CommandQueue; }
			ID3D12DescriptorHeap* GetRenderTHeap() { return m_RenderTargetsHeap; }
			ID3D12DescriptorHeap* GetDepthHeap() { return m_dsDescriptorHeap; }
			ID3D12CommandAllocator* GetComAlloc() { return m_CommandAllocator; }
			ID3D12DescriptorHeap* GetDescHeap(int index) { return m_DescriptorHeap[index]; }
			ID3D12Resource* GetRendTarget(int index) { return m_RenderTargets[index]; }
			ID3D12Resource* GetDeapthTarget(int index) { return m_depthStencilBuffer[index]; }
			D3D12_VIEWPORT GetViewPort() { return m_Viewport; }
			D3D12_RECT GetScissorRect() { return m_ScissorRect; }
			UINT64 GetREndTDescSize() { return m_RenderTargetsDescriptorSize; }
			UINT64 GetDeapthDescSize() { return m_DeapthTargetsDescriptorSize; }
		private:
			int SCREEN_WIDTH = 0;
			int SCREEN_HEIGHT = 0;
			HANDLE WINAPI m_EventHandle;
			UINT64 m_FenceValue;
			UINT64 m_RenderTargetsDescriptorSize;
			UINT64 m_DeapthTargetsDescriptorSize;
			ID3D12Device* m_device;
			ID3D12CommandQueue* m_CommandQueue;
			ID3D12CommandAllocator* m_CommandAllocator;
			ID3D12GraphicsCommandList* m_CommandList;
			ID3D12Fence* m_Fence;
			IDXGISwapChain1* m_swapChain;
			ID3D12DescriptorHeap* m_RenderTargetsHeap;
			ID3D12DescriptorHeap* m_DescriptorHeap[NUM_SWAP_BUFFERS] = {};
			ID3D12DescriptorHeap* m_dsDescriptorHeap;
			ID3D12Resource* m_RenderTargets[NUM_SWAP_BUFFERS];
			D3D12_VIEWPORT m_Viewport;
			D3D12_RECT m_ScissorRect;
			ID3D12Resource* m_depthStencilBuffer[NUM_SWAP_BUFFERS];

			std::vector<D3D12_VERTEX_BUFFER_VIEW> gVertexBufferView;
			std::vector<ComPtr<ID3D12Resource>>  gVertexBufferResource;

			int m_FrameIndex = 0;
		};

	}
}

#endif
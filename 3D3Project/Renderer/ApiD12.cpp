#include "ApiD12.h"
#include <stdio.h>
#include <iostream>
#include <comdef.h>

D3DPROJECT::RENDERER::ApiD12::ApiD12()
{
}

D3DPROJECT::RENDERER::ApiD12::~ApiD12()
{
	WaitForGpu();
	m_device->Release();
	m_CommandQueue->Release();
	m_CommandAllocator->Release();
	m_CommandList->Release();
	m_Fence->Release();
	m_swapChain->Release();
	m_RenderTargetsHeap->Release();
	for (auto& heap : m_DescriptorHeap)
	{
		heap->Release();
	}
	for (auto& depthStencil : m_depthStencilBuffer)
	{
		depthStencil->Release();
	}
	for (auto& RenderTarget : m_RenderTargets)
	{
		RenderTarget->Release();
	}
	m_dsDescriptorHeap->Release();
}

void D3DPROJECT::RENDERER::ApiD12::CreateDirectXContent(HWND wndHandle, int width, int height)
{
	UINT dxgiFactoryFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	//enable the d3d12 debug layer
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();

		// Enable additional debug layers.
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	//dxgi1_5 is only needed for the initialization process using the adepter
	IDXGIFactory4* factory = nullptr;
	IDXGIAdapter1* adapter = nullptr;
	//First a factory is created to iterate through the adapters available
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

	for (UINT adapterIndex = 0;; ++adapterIndex)
	{
		adapter = nullptr;
		if (DXGI_ERROR_NOT_FOUND == factory->EnumAdapters1(adapterIndex, &adapter))
		{
			break; //No more adapters to enumerate
		}

		//check to see if the adapter supports Direct3D 12, but don't create the actual device yet
		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr)))
		{
			break;
		}

		adapter->Release();
	}

	if (adapter)
	{
		//Create the actual device
		ThrowIfFailed(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device)));

		adapter->Release();
	}
	else
	{
		//Create warp device if no adapter was found
		factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
		ThrowIfFailed(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
	}

	//Describe and create the command queue
	D3D12_COMMAND_QUEUE_DESC cqd = {};
	ThrowIfFailed(m_device->CreateCommandQueue(&cqd, IID_PPV_ARGS(&m_CommandQueue)));


	//Create command allocator. The command allocator object corresponds
	//to the underlying allocations in which GPU commands are stored
	ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)));

	//Create command list
	ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator, nullptr, IID_PPV_ARGS(&m_CommandList)));


	//Command list are created in the recording state. Since there is nothing to
	//record right now and the main loop expects it to be closed, we close it
	m_CommandList->Close();



	//Vi är här nu 4. Create Fence
	ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

	m_FenceValue = 1;
	//Create an event handler to use for GPU synchronization
	m_EventHandle = CreateEvent(0, false, false, 0);

	//5. Create swap chain
	IDXGIFactory2* factory1 = nullptr;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory1)));


	//IDXGISwapChain1* swapChain1 = nullptr;

	//Create swap chain
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = SCREEN_WIDTH;
	scDesc.Height = SCREEN_HEIGHT;
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.Stereo = FALSE;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = NUM_SWAP_BUFFERS;
	scDesc.Scaling = DXGI_SCALING_NONE;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.Flags = 0;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	ThrowIfFailed(factory1->CreateSwapChainForHwnd(
		m_CommandQueue,
		wndHandle,
		&scDesc,
		nullptr,
		nullptr,
		&m_swapChain
	));

	//Create descriptor heap for render target views.
	D3D12_DESCRIPTOR_HEAP_DESC dhd = {};
	dhd.NumDescriptors = NUM_SWAP_BUFFERS;
	dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	ThrowIfFailed(m_device->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&m_RenderTargetsHeap)));


	//Create resource for the render targets
	m_RenderTargetsDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = m_RenderTargetsHeap->GetCPUDescriptorHandleForHeapStart();

	//One RTV for each frame
	for (UINT n = 0; n < NUM_SWAP_BUFFERS; n++)
	{
		ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_RenderTargets[n])));
		m_device->CreateRenderTargetView(m_RenderTargets[n], nullptr, cdh);
		cdh.ptr += m_RenderTargetsDescriptorSize;
	}

	//7. Define viewport and scissor rect
	m_Viewport.TopLeftX = 0.0f;
	m_Viewport.TopLeftY = 0.0f;
	m_Viewport.Width = (float)SCREEN_WIDTH;
	m_Viewport.Height = (float)SCREEN_HEIGHT;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	m_ScissorRect.left = (long)0;
	m_ScissorRect.right = (long)SCREEN_WIDTH;
	m_ScissorRect.top = (long)0;
	m_ScissorRect.bottom = (long)SCREEN_HEIGHT;

	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 4;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		// This flag indicates that this descriptor heap can be bound to the pipeline and that descriptors contained in it can be referenced by a root table.
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_DescriptorHeap[i])));

	}

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = NUM_SWAP_BUFFERS;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsDescriptorHeap));


	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	m_DeapthTargetsDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	D3D12_CPU_DESCRIPTOR_HANDLE dsh = m_dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//One RTV for each frame
	for (UINT i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(&m_depthStencilBuffer[i])
		);

		m_device->CreateDepthStencilView(m_depthStencilBuffer[i], &depthStencilDesc, dsh);
		dsh.ptr += m_DeapthTargetsDescriptorSize;
	}
}

void D3DPROJECT::RENDERER::ApiD12::SwapChain()
{
	//present the frame
	DXGI_PRESENT_PARAMETERS pp = {};
	m_swapChain->Present1(0, 0, &pp);
	//WaitForGpu();	//wait for gpu to finnish
					//NOT BEST PRACTICE, only used as such for simplicity
}

void D3DPROJECT::RENDERER::ApiD12::Clean(ID3D12PipelineState** pip, ID3D12RootSignature** root)
{
	//WaitForGpu();
	//Get the handle for the current render target used as back buffer
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = m_RenderTargetsHeap->GetCPUDescriptorHandleForHeapStart();
	cdh.ptr += m_RenderTargetsDescriptorSize * m_FrameIndex;

	D3D12_CPU_DESCRIPTOR_HANDLE dsh = m_dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	dsh.ptr += m_DeapthTargetsDescriptorSize * m_FrameIndex;

	m_CommandAllocator->Reset();
	m_CommandList->Reset(m_CommandAllocator, *pip);

	//Set constant buffer descriptor heap
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_DescriptorHeap[m_FrameIndex] };
	m_CommandList->SetDescriptorHeaps(ARRAYSIZE(descriptorHeaps), descriptorHeaps);


	//Set root signature
	m_CommandList->SetGraphicsRootSignature(*root);

	//Set root descriptor table to index 0 in previously set root signature
	m_CommandList->SetGraphicsRootDescriptorTable(0, m_DescriptorHeap[m_FrameIndex]->GetGPUDescriptorHandleForHeapStart());

	//Set necessary states
	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	//Draw cont
	//Indicate that the back buffer will be used as render target
	SetResourceTransitionBarrier(
		m_CommandList,
		m_RenderTargets[m_FrameIndex],
		D3D12_RESOURCE_STATE_PRESENT,	//state before
		D3D12_RESOURCE_STATE_RENDER_TARGET	//state after
	);
	float _clearColor[] = { 1, 1, 0, 1 };
	m_CommandList->OMSetRenderTargets(1, &cdh, FALSE, nullptr);
	m_CommandList->ClearRenderTargetView(cdh, _clearColor, 0, nullptr);
	m_CommandList->ClearDepthStencilView(dsh, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	SetResourceTransitionBarrier(
		m_CommandList,
		m_RenderTargets[m_FrameIndex],
		D3D12_RESOURCE_STATE_RENDER_TARGET,	//state before
		D3D12_RESOURCE_STATE_PRESENT	//state after
	);

	//Close the list to prepare it for execution
	m_CommandList->Close();

	//Execute the command list
	ID3D12CommandList* listsToExecute[] = { m_CommandList };
	m_CommandQueue->ExecuteCommandLists(ARRAYSIZE(listsToExecute), listsToExecute);
	
	m_FrameIndex = (m_FrameIndex + 1) % NUM_SWAP_BUFFERS;
}

void D3DPROJECT::RENDERER::ApiD12::SetRenderTarget()
{

}

void D3DPROJECT::RENDERER::ApiD12::WaitForGpu()
{
	//WATING FOR EACH FRAME TO COMPLEATE BEFORE CONTINUING IS NOT BEST PRACTICE.
	//This is code implemented as such for simplicity. the cpu could for example be used
	//for other tasks to prepare the next frame while the current one is being rendered.

	//Signal and incriment the fence value.
	const UINT64 fence = m_FenceValue;
	m_CommandQueue->Signal(m_Fence, fence);
	m_FenceValue++;

	//Wait until command queue is done.
	if (m_Fence->GetCompletedValue() < fence)
	{
		m_Fence->SetEventOnCompletion(fence, m_EventHandle);
		WaitForSingleObject(m_EventHandle, INFINITE);
	}
}

void D3DPROJECT::RENDERER::ApiD12::SetResourceTransitionBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource,
	D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter)
{
	D3D12_RESOURCE_BARRIER barrierDesc = {};

	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierDesc.Transition.pResource = resource;
	barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrierDesc.Transition.StateBefore = StateBefore;
	barrierDesc.Transition.StateAfter = StateAfter;

	commandList->ResourceBarrier(1, &barrierDesc);
}
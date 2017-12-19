#include "ApiDirectX.h"

D3DPROJECT::RENDERER::ApiDirectX::ApiDirectX()
{

}

D3DPROJECT::RENDERER::ApiDirectX::~ApiDirectX()
{
	if (m_sampleState != nullptr)
	{
		m_sampleState->Release();
	}
	if (m_swapChain != nullptr)
	{
		m_swapChain->Release();
	}
	if (m_device != nullptr)
	{
		m_device->Release();
	}
	if (m_deviceContext != nullptr)
	{
		m_deviceContext->Release();
	}
	if (m_backbufferRTV != nullptr)
	{
		m_backbufferRTV->Release();
	}
	if (m_depthStencil != nullptr)
	{
		m_depthStencil->Release();
	}
	if (m_depthStencilView != nullptr)
	{
		m_depthStencilView->Release();
	}
	if (m_rasterState != nullptr)
	{
		m_rasterState->Release();
	}
}

HRESULT D3DPROJECT::RENDERER::ApiDirectX::CreateDirectXContent(HWND wndHandle, int width, int height)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC _swapChainDesc;

	// clear out the struct for use
	ZeroMemory(&_swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	_swapChainDesc.BufferCount = 1;                                    // one back buffer
	_swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	_swapChainDesc.OutputWindow = wndHandle;                           // the window to be used
	_swapChainDesc.SampleDesc.Count = 1;                               // how many multisamples
	_swapChainDesc.Windowed = TRUE;                                    // windowed/full-screen mode

																	   // create a device, device context and swap chain using the information in the scd struct
	HRESULT _hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,//D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&_swapChainDesc,
		&m_swapChain,
		&m_device,
		NULL,
		&m_deviceContext);



	if (SUCCEEDED(_hr))
	{
		_hr = DepthStencilInitialicer(width, height); //skapar depthstencil/desc
		if (_hr != S_OK)
		{
			return _hr;
		}

										 // get the address of the back buffer
		ID3D11Texture2D* _backBuffer = nullptr;
		m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&_backBuffer);

		// use the back buffer address to create the render target
		_hr = m_device->CreateRenderTargetView(_backBuffer, NULL, &m_backbufferRTV);
		if (_hr != S_OK)
		{
			return _hr;
		}
		_backBuffer->Release();

		// set the render target as the back buffer
		m_deviceContext->OMSetRenderTargets(1, &m_backbufferRTV, m_depthStencilView);

		// description for sampler
		D3D11_SAMPLER_DESC samplDesc;
		ZeroMemory(&samplDesc, sizeof(D3D11_SAMPLER_DESC));

		samplDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplDesc.MinLOD = 0;
		samplDesc.MaxLOD = D3D11_FLOAT32_MAX;

		// create the sampler
		_hr = m_device->CreateSamplerState(&samplDesc, &m_sampleState);
		//Set the sampler state
		m_deviceContext->PSSetSamplers(0, 1, &m_sampleState);

		D3D11_VIEWPORT _vp;
		_vp.Width = width;
		_vp.Height = height;
		_vp.MinDepth = 0.0f;
		_vp.MaxDepth = 1.0f;
		_vp.TopLeftX = 0;
		_vp.TopLeftY = 0;
		m_deviceContext->RSSetViewports(1, &_vp);
	}


	return _hr;
}

HRESULT D3DPROJECT::RENDERER::ApiDirectX::DepthStencilInitialicer(int width, int height)
{
	HRESULT _hr;

	//create the depth stencil
	D3D11_TEXTURE2D_DESC _descDepth;
	_descDepth.Width = width;
	_descDepth.Height = height;
	_descDepth.MipLevels = 1;
	_descDepth.ArraySize = 1;
	_descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	_descDepth.SampleDesc.Count = 1;
	_descDepth.SampleDesc.Quality = 0;
	_descDepth.Usage = D3D11_USAGE_DEFAULT;
	_descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	_descDepth.CPUAccessFlags = 0;
	_descDepth.MiscFlags = 0;
	_hr = m_device->CreateTexture2D(&_descDepth, NULL, &m_depthStencil);
	if (SUCCEEDED(_hr))
	{
		_hr = m_device->CreateDepthStencilView(m_depthStencil, NULL, &m_depthStencilView);
	}

	D3D11_RASTERIZER_DESC rasterState;
	rasterState.FillMode = D3D11_FILL_WIREFRAME;
	rasterState.CullMode = D3D11_CULL_NONE;
	rasterState.FrontCounterClockwise = false;
	rasterState.DepthBias = false;
	rasterState.DepthBiasClamp = 0;
	rasterState.SlopeScaledDepthBias = 0;
	rasterState.DepthClipEnable = true;
	rasterState.ScissorEnable = true;
	rasterState.MultisampleEnable = false;
	rasterState.AntialiasedLineEnable = false;
	_hr = m_device->CreateRasterizerState(&rasterState, &m_rasterState);
	return _hr;
}

void D3DPROJECT::RENDERER::ApiDirectX::SwapChain()
{
	m_swapChain->Present(0, 0);
}

void D3DPROJECT::RENDERER::ApiDirectX::Clean()
{
	float _clearColor[] = { 1, 1, 0, 1 };
	m_deviceContext->ClearRenderTargetView(m_backbufferRTV, _clearColor);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_deviceContext->OMSetRenderTargets(1, &m_backbufferRTV, m_depthStencilView);
}

void D3DPROJECT::RENDERER::ApiDirectX::SetRenderTarget()
{
	m_deviceContext->OMSetRenderTargets(1, &m_backbufferRTV, m_depthStencilView);
}
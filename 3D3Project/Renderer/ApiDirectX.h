#ifndef APIDIRECTX_H
#define APIDIRECTX_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

using namespace DirectX;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#ifdef _DEBUG
#pragma comment(lib, "DirectXTKD.lib")
#else
#pragma comment(lib, "DirectXTK.lib")
#endif
namespace D3DPROJECT
{
	namespace RENDERER
	{
		class ApiDirectX
		{
		public:
			ApiDirectX();
			~ApiDirectX();
			HRESULT CreateDirectXContent(HWND wndHandle, int width, int height);
			ID3D11Device* GetDevice() { return m_device; }
			ID3D11DeviceContext* GetDeviceContext() { return m_deviceContext; }
			IDXGISwapChain* GetSwapChain() { return m_swapChain; }
			ID3D11RenderTargetView* GetBackBuffer() { return m_backbufferRTV; }
			ID3D11Texture2D* GetDepthStencil() { return m_depthStencil; }
			ID3D11DepthStencilView* GetDepthStencilView() { return m_depthStencilView; }
			ID3D11RasterizerState* GetRasterizerState() { return m_rasterState; }
			void SwapChain();
			void Clean();
			void SetRenderTarget();

		private:
			HRESULT DepthStencilInitialicer(int width, int height);

			ID3D11SamplerState* m_sampleState = nullptr;
			IDXGISwapChain* m_swapChain = nullptr;
			ID3D11Device* m_device = nullptr;
			ID3D11DeviceContext* m_deviceContext = nullptr;
			ID3D11RenderTargetView* m_backbufferRTV = nullptr;
			ID3D11Texture2D* m_depthStencil = nullptr;
			ID3D11DepthStencilView* m_depthStencilView = nullptr;
			ID3D11RasterizerState* m_rasterState = nullptr;
		};

	}
}

#endif
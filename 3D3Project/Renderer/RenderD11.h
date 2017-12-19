#pragma once
#ifndef RENDERD11_H
#define RENDERD11_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <Renderer\IRenderer.h>
#include <vector>
#include "ApiDirectX.h"

using namespace DirectX;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

namespace D3DPROJECT
{
	namespace RENDERER
	{
		class RenderD11 : public IRenderer
		{
		public:
			RenderD11();
			~RenderD11();
			int Initialize(HWND wndHandle, int width, int height) override;
			int CreateTransformBuffer() override;
			int CreateMaterialBuffer(void* matData) override;
			int CreateVertexBuffer(void* vertexData, size_t nrOfFaces) override;
			int CreateTextureResource(std::wstring filename) override;
			int CreateCameraBuffer() override;
			void SetCameraBuffer(void* cameraDat, int cameraIndex) override;
			void SetTransformBuffer(void* worldRotation, int index) override;
			void Render(int transIndex, int matIndex, int vertexIndex, int texIndex, int cameraIndex, int faceCount, int entCount) override;
			void Present() override;
			void Clean() override;
		private:
			struct CameraType
			{
				XMFLOAT4 CameraPos;
				XMFLOAT4X4 ViewProjection;
			};
			typedef struct VertexInputType
			{
				XMFLOAT3 position;
				XMFLOAT2 uv;
				XMFLOAT3 normal;
			};
			struct OBJColourType
			{
				XMFLOAT3 Deffuse;
				XMFLOAT3 Albi;
				XMFLOAT3 Tf;
				XMFLOAT3 Ni;
			};
			struct worldPosition
			{
				XMFLOAT4X4 World[256];
				XMFLOAT4X4 Rotation[256];
			};
			HRESULT CreateSimpleShaders();

			std::vector<ID3D11Buffer *> transformBuffers;
			std::vector<ID3D11Buffer *> vertexBuffers;
			std::vector<ID3D11Buffer *> matBuffers;
			std::vector<ID3D11ShaderResourceView*> textureResource;
			std::vector<ID3D11Buffer *> cameraBuffer;
			ApiDirectX api;

			ID3D11InputLayout* vertexLayout = nullptr;
			ID3D11VertexShader* vertexShader = nullptr;
			ID3D11GeometryShader* geometryShader = nullptr;
			ID3D11PixelShader* pixelShader = nullptr;
		};
	}
}


#endif
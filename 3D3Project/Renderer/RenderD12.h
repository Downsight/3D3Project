#pragma once
#pragma once
#ifndef RENDERD12_H
#define RENDERD12_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <Renderer\IRenderer.h>
#include <vector>
#include "ApiD12.h"

using namespace DirectX;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

namespace D3DPROJECT
{
	namespace RENDERER
	{
		enum class ShaderType { VS = 0, PS = 1, GS = 2, CS = 3 };
		class RenderD12 : public IRenderer
		{
		public:
			RenderD12();
			~RenderD12();
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
			HRESULT compileShader(D3DPROJECT::RENDERER::ShaderType type);

			ID3DBlob* vertexBlob = nullptr;
			ID3DBlob* pixelBlob = nullptr;
			ID3DBlob* geomtryBlob = nullptr;
			ID3DBlob* computeBlob = nullptr;
			ID3D12RootSignature* m_RootSignature = nullptr;
			ID3D12PipelineState* PipelineState = nullptr;

			int m_FrameIndex = 0;

			std::vector<D3D12_VERTEX_BUFFER_VIEW> gVertexBufferView;
			std::vector<ComPtr<ID3D12Resource>>  gVertexBufferResource;
			ID3D12Resource* m_ConstantBufferResource[NUM_SWAP_BUFFERS] = {};
			

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

			struct texResource
			{
				ID3D12Resource* textureBuffer;
				ID3D12Resource* textureBufferUploadHeap;
			};

			HRESULT CreateSimpleShaders();

			std::vector<texResource> texBuffer;
			std::vector<void*> vertexBuffers;
			std::vector<void*> matBuffers;
			std::vector<void*> transformBuffer;
			std::vector<void*> cameraBuffer;
			D3D12_RESOURCE_DESC resourceDescription;
			std::vector<ID3D11ShaderResourceView*> textureResource;
			void** m_MappedBuffer = nullptr;
			ApiD12 api;


			bool transBuffer = false;
			bool camBuffer = false;
			bool matBuffer = false;
			D3D12_CPU_DESCRIPTOR_HANDLE cdh;
			D3D12_CPU_DESCRIPTOR_HANDLE dsh;
		};
	}
}


#endif
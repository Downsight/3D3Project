#pragma once
#ifndef IRENDERER_H
#define IRENDERER_H

#include <Windows.h>
#include <DirectXMath.h>
#include <vector>

#if defined DLL_EXPORT_R
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

namespace D3DPROJECT
{
	namespace RENDERER
	{

		class IRenderer
		{
		public:
			IRenderer();
			virtual ~IRenderer();
			virtual int Initialize(HWND wndHandle, int width, int height) = 0;
			virtual int CreateTransformBuffer() = 0;
			virtual int CreateMaterialBuffer(void* matData) = 0;
			virtual int CreateVertexBuffer(void* vertexData, size_t nrOfFaces) = 0;
			virtual int CreateTextureResource(std::wstring filename) = 0;
			virtual int CreateCameraBuffer() = 0;
			virtual void SetCameraBuffer(void* cameraDat, int cameraIndex) = 0;
			virtual void SetTransformBuffer(void* worldRotation, int index) = 0;
			virtual void Render(int transIndex, int matIndex, int vertexIndex, int texIndex, int cameraIndex, int faceCount, int entCount) = 0;
			virtual void Present() = 0;
			virtual void Clean() = 0;

		private:



		};
		DECLDIR IRenderer* CreateNewRenderer(int api);
	}
}


#endif
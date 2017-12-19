#include "RenderD12.h"
#include <DTK\WICTextureLoader.h>
#include <wrl.h>
#include <wincodec.h>


D3DPROJECT::RENDERER::RenderD12::RenderD12()
{

}


D3DPROJECT::RENDERER::RenderD12::~RenderD12()
{

}

int D3DPROJECT::RENDERER::RenderD12::Initialize(HWND wndHandle, int width, int height)
{
	api.CreateDirectXContent(wndHandle, width, height);
	CreateSimpleShaders();
	api.SetRenderTarget();
	m_MappedBuffer = new void*[NUM_SWAP_BUFFERS];
	return 1;
}

int D3DPROJECT::RENDERER::RenderD12::CreateTransformBuffer()
{
	if (transBuffer == false)
	{
		D3D12_HEAP_PROPERTIES hp = {};
		hp.Type = D3D12_HEAP_TYPE_UPLOAD;
		hp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		hp.CreationNodeMask = 1;
		hp.VisibleNodeMask = 1;
		hp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC rd = {};
		rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		rd.Width = ((sizeof(worldPosition) + 255) & ~255) + ((sizeof(XMFLOAT3) * 4 + 255) & ~255) + ((sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT4X4) + 255) & ~255);
		rd.Height = 1;
		rd.DepthOrArraySize = 1;
		rd.MipLevels = 1;
		rd.SampleDesc.Count = 1;
		rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		{
			// Create the constant buffer.
			ThrowIfFailed(api.GetDevice()->CreateCommittedResource(
				&hp,
				D3D12_HEAP_FLAG_NONE,
				&rd,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_ConstantBufferResource[i])));

			m_ConstantBufferResource[i]->SetName(L"CB");

			// Describe and create a constant buffer view.
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;// = {};
			cbvDesc.BufferLocation = m_ConstantBufferResource[i]->GetGPUVirtualAddress();
			cbvDesc.SizeInBytes = (sizeof(worldPosition) + 255) & ~255;

			CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle(api.GetDescHeap(i)->GetCPUDescriptorHandleForHeapStart(), 0, api.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
			api.GetDevice()->CreateConstantBufferView(&cbvDesc, cbvHandle);

		}
		transBuffer = true;
	}
	void* temp;
	transformBuffer.push_back(temp);

	return transformBuffer.size() - 1;
}

int D3DPROJECT::RENDERER::RenderD12::CreateMaterialBuffer(void* matData)
{
	if (matBuffer == false)
	{
		for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;// = {};
			cbvDesc.BufferLocation = m_ConstantBufferResource[i]->GetGPUVirtualAddress() + ((sizeof(worldPosition) + 255) & ~255) + ((sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT4X4) + 255) & ~255);
			cbvDesc.SizeInBytes = (sizeof(XMFLOAT3) * 4 + 255) & ~255;
			CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle1(api.GetDescHeap(i)->GetCPUDescriptorHandleForHeapStart(), 2, api.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
			api.GetDevice()->CreateConstantBufferView(&cbvDesc, cbvHandle1);
		}
		matBuffer = true;
	}
	void* tempBuffer = matData;
	D3D12_RANGE range = { 0, 0 };
	matBuffers.push_back(tempBuffer);
	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_ConstantBufferResource[i]->Map(0, &range, &m_MappedBuffer[i]);
		memcpy(static_cast<void*>(static_cast<char*>(m_MappedBuffer[i]) + ((sizeof(worldPosition) + 255) & ~255) + ((sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT4X4) + 255) & ~255)), matBuffers[matBuffers.size() - 1], (sizeof(XMFLOAT3) * 4 + 255) & ~255);
	}
	return matBuffers.size() - 1;
}

int D3DPROJECT::RENDERER::RenderD12::CreateVertexBuffer(void* vertexData, size_t nrOfFaces)
{
	VertexInputType* vertexInData = (VertexInputType*)vertexData;
	ID3D12Resource *_resource1;
	D3D12_HEAP_PROPERTIES hp1 = {};
	hp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	hp1.CreationNodeMask = 1;
	hp1.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC rd1 = {};
	rd1.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	rd1.Width = sizeof(VertexInputType) * nrOfFaces * 3;
	rd1.Height = 1;
	rd1.DepthOrArraySize = 1;
	rd1.MipLevels = 1;
	rd1.SampleDesc.Count = 1;
	rd1.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//Creates both a resource and an implicit heap, such that the heap is big enough
	//to contain the entire resource and the resource is mapped to the heap.
	ThrowIfFailed(api.GetDevice()->CreateCommittedResource(
		&hp1,
		D3D12_HEAP_FLAG_NONE,
		&rd1,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_resource1)
	));

	//_resource1->SetName(L"vb heap1");

	//copy the triangle data to the vertex buffer
	const void* _texInData = vertexInData;
	void* dataBegin1 = nullptr;
	D3D12_RANGE range = { 0, 0 }; //Wedo not intend to read this resource on the cpu
	_resource1->Map(0, &range, &dataBegin1);
	memcpy(dataBegin1, _texInData, sizeof(VertexInputType) * nrOfFaces * 3);
	_resource1->Unmap(0, nullptr);

	D3D12_VERTEX_BUFFER_VIEW _gVertexBufferView1;
	//Initialize vertex buffer view, used in the render call
	_gVertexBufferView1.BufferLocation = _resource1->GetGPUVirtualAddress();
	_gVertexBufferView1.StrideInBytes = sizeof(float) * 8;
	_gVertexBufferView1.SizeInBytes = sizeof(VertexInputType) * nrOfFaces * 3;

	gVertexBufferResource.push_back(_resource1);
	gVertexBufferView.push_back(_gVertexBufferView1);

	return gVertexBufferView.size() - 1;
}

// get the dxgi format equivilent of a wic format
DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID)
{
	if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;

	else return DXGI_FORMAT_UNKNOWN;
}

// get a dxgi compatible wic format from another wic format
WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID)
{
	if (wicFormatGUID == GUID_WICPixelFormatBlackWhite) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat1bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat2bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat4bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat2bppGray) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat4bppGray) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayFixedPoint) return GUID_WICPixelFormat16bppGrayHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFixedPoint) return GUID_WICPixelFormat32bppGrayFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR555) return GUID_WICPixelFormat16bppBGRA5551;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR101010) return GUID_WICPixelFormat32bppRGBA1010102;
	else if (wicFormatGUID == GUID_WICPixelFormat24bppBGR) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat24bppRGB) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppPBGRA) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppPRGBA) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGB) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppBGR) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPBGRA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppBGRFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppPRGBAFloat) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFloat) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBE) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppCMYK) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppCMYK) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat40bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat80bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGB) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGB) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBAHalf) return GUID_WICPixelFormat64bppRGBAHalf;
#endif

	else return GUID_WICPixelFormatDontCare;
}

// get the number of bits per pixel for a dxgi format
int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat)
{
	if (dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT) return 128;
	else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
	else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
	else if (dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;

	else if (dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R32_FLOAT) return 32;
	else if (dxgiFormat == DXGI_FORMAT_R16_FLOAT) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R16_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R8_UNORM) return 8;
	else if (dxgiFormat == DXGI_FORMAT_A8_UNORM) return 8;
}

// load and decode image from file
int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, LPCWSTR filename, int &bytesPerRow)
{
	HRESULT hr;

	// we only need one instance of the imaging factory to create decoders and frames
	static IWICImagingFactory *wicFactory;

	// reset decoder, frame and converter since these will be different for each image we load
	IWICBitmapDecoder *wicDecoder = NULL;
	IWICBitmapFrameDecode *wicFrame = NULL;
	IWICFormatConverter *wicConverter = NULL;

	bool imageConverted = false;

	//if no WicFactory we create one
	if (wicFactory == NULL)
	{
		// Initialize the COM library
		CoInitialize(NULL);

		// create the WIC factory
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&wicFactory)
		);
		if (FAILED(hr)) return 0;
	}

	// load a decoder for the image
	hr = wicFactory->CreateDecoderFromFilename(
		filename,                        // Image we want to load in
		NULL,                            // This is a vendor ID, we do not prefer a specific one so set to null
		GENERIC_READ,                    // We want to read from this file
		WICDecodeMetadataCacheOnLoad,    // We will cache the metadata right away, rather than when needed, which might be unknown
		&wicDecoder                      // the wic decoder to be created
	);
	if (FAILED(hr)) return 0;

	// get image from decoder (this will decode the "frame")
	hr = wicDecoder->GetFrame(0, &wicFrame);
	if (FAILED(hr)) return 0;

	// get wic pixel format of image
	WICPixelFormatGUID pixelFormat;
	hr = wicFrame->GetPixelFormat(&pixelFormat);
	if (FAILED(hr)) return 0;

	// get size of image
	UINT textureWidth, textureHeight;
	hr = wicFrame->GetSize(&textureWidth, &textureHeight);
	if (FAILED(hr)) return 0;

	// we are not handling sRGB types in this tutorial, so if you need that support, you'll have to figure
	// out how to implement the support yourself

	// convert wic pixel format to dxgi pixel format
	DXGI_FORMAT dxgiFormat = GetDXGIFormatFromWICFormat(pixelFormat);

	int bitsPerPixel = GetDXGIFormatBitsPerPixel(dxgiFormat); // number of bits per pixel
	bytesPerRow = (textureWidth * bitsPerPixel) / 8; // number of bytes in each row of the image data
	int imageSize = bytesPerRow * textureHeight; // total image size in bytes

												 // allocate enough memory for the raw image data, and set imageData to point to that memory
	*imageData = (BYTE*)malloc(imageSize);

	// copy (decoded) raw image data into the newly allocated memory (imageData)
	if (imageConverted)
	{
		// if image format needed to be converted, the wic converter will contain the converted image
		hr = wicConverter->CopyPixels(0, bytesPerRow, imageSize, *imageData);
		if (FAILED(hr)) return 0;
	}
	else
	{
		// no need to convert, just copy data from the wic frame
		hr = wicFrame->CopyPixels(0, bytesPerRow, imageSize, *imageData);
		if (FAILED(hr)) return 0;
	}

	// now describe the texture with the information we have obtained from the image
	resourceDescription = {};
	resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDescription.Alignment = 0; // may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
	resourceDescription.Width = textureWidth; // width of the texture
	resourceDescription.Height = textureHeight; // height of the texture
	resourceDescription.DepthOrArraySize = 1; // if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
	resourceDescription.MipLevels = 1; // Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
	resourceDescription.Format = dxgiFormat; // This is the dxgi format of the image (format of the pixels)
	resourceDescription.SampleDesc.Count = 1; // This is the number of samples per pixel, we just want 1 sample
	resourceDescription.SampleDesc.Quality = 0; // The quality level of the samples. Higher is better quality, but worse performance
	resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
	resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE; // no flags

														  // return the size of the image. remember to delete the image once your done with it (in this tutorial once its uploaded to the gpu)
	return imageSize;
}

int D3DPROJECT::RENDERER::RenderD12::CreateTextureResource(std::wstring filename)
{
	// Load the image from file
	D3D12_RESOURCE_DESC textureDesc;
	int imageBytesPerRow;
	BYTE* imageData;
	api.GetComList()->Reset(api.GetComAlloc(), PipelineState);
	LPCWSTR result = filename.c_str();
	std::string shaderText;

	//Most of the texture load functiona are taken from a guide on how to load a texture in direcx 12. Link: https://www.braynzarsoft.net/viewtutorial/q16390-directx-12-textures-from-file 
	int imageSize = LoadImageDataFromFile(&imageData, textureDesc, result, imageBytesPerRow);

	// make sure we have data
	if (imageSize <= 0)
	{
		return -1;
	}
	texResource textureResource;
	// create a default heap where the upload heap will copy its contents into (contents being the texture)
	HRESULT hr = api.GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&textureDesc, // the description of our texture
		D3D12_RESOURCE_STATE_COPY_DEST, // We will copy the texture from the upload heap to here, so we start it out in a copy dest state
		nullptr, // used for render targets and depth/stencil buffers
		IID_PPV_ARGS(&textureResource.textureBuffer));
	if (FAILED(hr))
	{
		return -1;
	}
	textureResource.textureBuffer->SetName(L"Texture Buffer Resource Heap");

	UINT64 textureUploadBufferSize;
	api.GetDevice()->GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);

	// now we create an upload heap to upload our texture to the GPU
	hr = api.GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize), // resource description for a buffer (storing the image data in this heap just to copy to the default heap)
		D3D12_RESOURCE_STATE_GENERIC_READ, // We will copy the contents from this heap to the default heap above
		nullptr,
		IID_PPV_ARGS(&textureResource.textureBufferUploadHeap));
	if (FAILED(hr))
	{
		return -1;
	}
	textureResource.textureBufferUploadHeap->SetName(L"Texture Buffer Upload Resource Heap");

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = &imageData[0]; // pointer to our image data
	textureData.RowPitch = imageBytesPerRow; // size of all our triangle vertex data
	textureData.SlicePitch = imageBytesPerRow * textureDesc.Height; // also the size of our triangle vertex data

																	// Now we copy the upload buffer contents to the default heap
	UpdateSubresources(api.GetComList(), textureResource.textureBuffer, textureResource.textureBufferUploadHeap, 0, 0, 1, &textureData);

	// transition the texture default heap to a pixel shader resource (we will be sampling from this heap in the pixel shader to get the color of pixels)
	api.GetComList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textureResource.textureBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	for (int i = 0; i < 2; i++)
	{
		// now we create a shader resource view (descriptor that points to the texture and describes it)
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle1(api.GetDescHeap(i)->GetCPUDescriptorHandleForHeapStart(), 3,  api.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		api.GetDevice()->CreateShaderResourceView(textureResource.textureBuffer, &srvDesc, cbvHandle1);// gmainDescriptorHeap[i]->GetCPUDescriptorHandleForHeapStart());
	}
	// Now we execute the command list to upload the initial assets (triangle data)
	api.GetComList()->Close();
	ID3D12CommandList* ppCommandLists[] = { api.GetComList() };
	api.GetComQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	//api.WaitForGpu();

	return 0;
}

int D3DPROJECT::RENDERER::RenderD12::CreateCameraBuffer()
{
	if (camBuffer == false)
	{
		for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;// = {};
			cbvDesc.BufferLocation = m_ConstantBufferResource[i]->GetGPUVirtualAddress() + (sizeof(worldPosition) + 255) & ~255;
			cbvDesc.SizeInBytes = (sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT4X4) + 255) & ~255;
			CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle1(api.GetDescHeap(i)->GetCPUDescriptorHandleForHeapStart(), 1, api.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
			api.GetDevice()->CreateConstantBufferView(&cbvDesc, cbvHandle1);
		}
		camBuffer = true;
	}
	void* tempBuffer;
	cameraBuffer.push_back(tempBuffer);
	return cameraBuffer.size() - 1;
}

void D3DPROJECT::RENDERER::RenderD12::SetCameraBuffer(void* cameraData, int cameraIndex)
{
	cameraBuffer[cameraIndex] = cameraData;
	D3D12_RANGE range = { 0, 0 };
	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_ConstantBufferResource[i]->Map(0, &range, &m_MappedBuffer[i]);
		memcpy(static_cast<void*>(static_cast<char*>(m_MappedBuffer[i]) + ((sizeof(worldPosition) + 255) & ~255)), cameraBuffer[cameraIndex], ((sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT4X4) + 255) & ~255));
	}
}

void D3DPROJECT::RENDERER::RenderD12::SetTransformBuffer(void* worldRotation, int index)
{
	transformBuffer[index] = worldRotation;
	D3D12_RANGE range = { 0, 0 };
	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_ConstantBufferResource[i]->Map(0, &range, &m_MappedBuffer[i]);
		memcpy(m_MappedBuffer[i], transformBuffer[index], sizeof(worldPosition));
	}
}

void D3DPROJECT::RENDERER::RenderD12::Render(int transIndex, int matIndex, int vertexIndex, int texIndex, int cameraIndex, int faceCount, int entCount)
{
	//api.WaitForGpu();

	cdh = api.GetRenderTHeap()->GetCPUDescriptorHandleForHeapStart();
	cdh.ptr += api.GetREndTDescSize() * m_FrameIndex;


	api.GetComAlloc()->Reset();
	api.GetComList()->Reset(api.GetComAlloc(), PipelineState);

	//Set constant buffer descriptor heap
	ID3D12DescriptorHeap* gdescriptorHeaps[] = { api.GetDescHeap(m_FrameIndex) };
	api.GetComList()->SetDescriptorHeaps(ARRAYSIZE(gdescriptorHeaps), gdescriptorHeaps);

	//Set root signature
	api.GetComList()->SetGraphicsRootSignature(m_RootSignature);

	//Offset the CBV  we want to use for this draw call
	CD3DX12_GPU_DESCRIPTOR_HANDLE cbv(api.GetDescHeap(m_FrameIndex)->GetGPUDescriptorHandleForHeapStart());
	cbv.Offset(0, api.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	CD3DX12_GPU_DESCRIPTOR_HANDLE cbv1(api.GetDescHeap(m_FrameIndex)->GetGPUDescriptorHandleForHeapStart());
	cbv1.Offset(1, api.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	CD3DX12_GPU_DESCRIPTOR_HANDLE cbv2(api.GetDescHeap(m_FrameIndex)->GetGPUDescriptorHandleForHeapStart());
	cbv2.Offset(2, api.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	CD3DX12_GPU_DESCRIPTOR_HANDLE cbv3(api.GetDescHeap(m_FrameIndex)->GetGPUDescriptorHandleForHeapStart());
	cbv3.Offset(3, api.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	//(sizeof(float) * (4 + 255 & ~255)));
	//Set root descriptor table to index 0 in previously set root signature
	api.GetComList()->SetGraphicsRootDescriptorTable(0, cbv);
	api.GetComList()->SetGraphicsRootDescriptorTable(1, cbv1);
	api.GetComList()->SetGraphicsRootDescriptorTable(2, cbv2);
	api.GetComList()->SetGraphicsRootDescriptorTable(3, cbv3);

	//Set necessary states
	api.GetComList()->RSSetViewports(1, &api.GetViewPort());
	api.GetComList()->RSSetScissorRects(1, &api.GetScissorRect());

	//Draw cont
	//Indicate that the back buffer will be used as render target
	api.SetResourceTransitionBarrier(
		api.GetComList(),
		api.GetRendTarget(m_FrameIndex),
		D3D12_RESOURCE_STATE_PRESENT,	//state before
		D3D12_RESOURCE_STATE_RENDER_TARGET	//state after
	);


	dsh = api.GetDepthHeap()->GetCPUDescriptorHandleForHeapStart();
	dsh.ptr += api.GetDeapthDescSize() * m_FrameIndex;


	//Record commands
	api.GetComList()->OMSetRenderTargets(1, &cdh, FALSE, &dsh);
	api.GetComList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	api.GetComList()->IASetVertexBuffers(0, 1, &gVertexBufferView.at(vertexIndex));




	api.GetComList()->DrawInstanced(faceCount * 3, entCount, 0, 0);


	api.SetResourceTransitionBarrier(
		api.GetComList(),
		api.GetRendTarget(m_FrameIndex),
		D3D12_RESOURCE_STATE_RENDER_TARGET,	//state before
		D3D12_RESOURCE_STATE_PRESENT	//state after
	);

	//Close the list to prepare it for execution
	api.GetComList()->Close();

	//Execute the command list
	ID3D12CommandList* listsToExecute[] = { api.GetComList() };
	api.GetComQueue()->ExecuteCommandLists(ARRAYSIZE(listsToExecute), listsToExecute);
}

void D3DPROJECT::RENDERER::RenderD12::Present()
{
	m_FrameIndex = (m_FrameIndex + 1) % NUM_SWAP_BUFFERS;
	api.SwapChain();
}

void D3DPROJECT::RENDERER::RenderD12::Clean()
{
	api.Clean(&PipelineState, &m_RootSignature);
}

HRESULT D3DPROJECT::RENDERER::RenderD12::compileShader(D3DPROJECT::RENDERER::ShaderType type)
{
	std::string shaderText;
	//shader compile
	HRESULT hr;
	if (type == ShaderType::VS)
	{
		hr = D3DCompileFromFile(
			L"Shaders/SimpleVS.hlsl",	//filename
			nullptr,	//optional macros
			nullptr,	//optional include files
			"VS_main",	//entry point
			"vs_5_0",	//shader model (target)
			0,			//shader compile options	//here DEBUGGING OPTIONS
			0,			//effectcompile options
			&vertexBlob,	//double pointer to ID3DBlob
			nullptr		//pointer for Error Blob messages
		);
	}
	else if (type == ShaderType::PS)
	{
		hr = D3DCompileFromFile(
			L"Shaders/SimplePS.hlsl",	//filename
			nullptr,	//optional macros
			nullptr,	//optional include files
			"PS_main",	//entry point
			"ps_5_0",	//shader model (target)
			0,			//shader compile options	//here DEBUGGING OPTIONS
			0,			//effectcompile options
			&pixelBlob,	//double pointer to ID3DBlob
			nullptr		//pointer for Error Blob messages
		);
	}
	else if (type == ShaderType::GS)
	{
		hr = D3DCompileFromFile(
			L"Shaders/SimpleGS.hlsl",	//filename
			nullptr,	//optional macros
			nullptr,	//optional include files
			"GS_main",	//entry point
			"gs_5_0",	//shader model (target)
			0,			//shader compile options	//here DEBUGGING OPTIONS
			0,			//effectcompile options
			&geomtryBlob,	//double pointer to ID3DBlob
			nullptr		//pointer for Error Blob messages
		);
	}
	else if (type == ShaderType::CS)
	{
		hr = D3DCompileFromFile(
			L"",	//filename
			nullptr,	//optional macros
			nullptr,	//optional include files
			"cS_main",	//entry point
			"cs_5_0",	//shader model (target)
			0,			//shader compile options	//here DEBUGGING OPTIONS
			0,			//effectcompile options
			&computeBlob,	//double pointer to ID3DBlob
			nullptr		//pointer for Error Blob messages
		);
	}

	return hr;
}

HRESULT D3DPROJECT::RENDERER::RenderD12::CreateSimpleShaders()
{
	// compile shaders
	std::string err;
	HRESULT hr = compileShader(ShaderType::VS);
	if (hr != S_OK) {
		return hr;
	};
	hr = compileShader(ShaderType::PS);
	if (hr != S_OK) {
		return hr;
	};
	hr = compileShader(ShaderType::GS);
	if (hr != S_OK) {
		return hr;
	};

	//Root parameter can be a table, root descriptor or root constants
	CD3DX12_ROOT_PARAMETER slotRootParameter[4];

	CD3DX12_DESCRIPTOR_RANGE dtRanges[4];
	dtRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	dtRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	dtRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);
	dtRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	//create root parameter
	slotRootParameter[0].InitAsDescriptorTable(1, &dtRanges[0]);
	slotRootParameter[1].InitAsDescriptorTable(1, &dtRanges[1]);
	slotRootParameter[2].InitAsDescriptorTable(1, &dtRanges[2]);
	slotRootParameter[3].InitAsDescriptorTable(1, &dtRanges[3]);

	// create a static sampler
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	CD3DX12_ROOT_SIGNATURE_DESC rsDesc(4, slotRootParameter, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS);

	ID3DBlob* sBlob;
	ThrowIfFailed(D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&sBlob,
		nullptr
	));

	ThrowIfFailed(api.GetDevice()->CreateRootSignature(
		0,
		sBlob->GetBufferPointer(),
		sBlob->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)
	));

	////// Input Layout //////
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD"	, 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		
		
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = ARRAYSIZE(inputElementDesc);

	////// Pipline State //////
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsd = {};

	//Specify pipeline stages:
	gpsd.pRootSignature = m_RootSignature;
	gpsd.InputLayout = inputLayoutDesc;
	gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsd.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpsd.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpsd.VS.pShaderBytecode = reinterpret_cast<void*>(vertexBlob->GetBufferPointer());
	gpsd.VS.BytecodeLength = vertexBlob->GetBufferSize();
	gpsd.GS.pShaderBytecode = reinterpret_cast<void*>(geomtryBlob->GetBufferPointer());
	gpsd.GS.BytecodeLength = geomtryBlob->GetBufferSize();
	gpsd.PS.pShaderBytecode = reinterpret_cast<void*>(pixelBlob->GetBufferPointer());
	gpsd.PS.BytecodeLength = pixelBlob->GetBufferSize();

	//Specify render target and depthstencil usage.
	gpsd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsd.NumRenderTargets = 1;

	gpsd.SampleDesc.Count = 1;
	gpsd.SampleMask = UINT_MAX;

	//Specify rasterizer behaviour.
	gpsd.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsd.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

	//Specify blend descriptions.
	D3D12_RENDER_TARGET_BLEND_DESC defaultRTdesc = {
		false, false,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL };

	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsd.BlendState.RenderTarget[i] = defaultRTdesc;

	ThrowIfFailed(api.GetDevice()->CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(&PipelineState)));
	return hr;
}

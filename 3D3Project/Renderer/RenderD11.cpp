#include "RenderD11.h"
#include <DTK\WICTextureLoader.h>

D3DPROJECT::RENDERER::RenderD11::RenderD11()
{
	
}


D3DPROJECT::RENDERER::RenderD11::~RenderD11()
{
	for (auto& buffer : transformBuffers)
	{
		buffer->Release();
	}
	for (auto& buffer : vertexBuffers)
	{
		buffer->Release();
	}
}

int D3DPROJECT::RENDERER::RenderD11::Initialize(HWND wndHandle, int width, int height)
{
	HRESULT hr = api.CreateDirectXContent(wndHandle, width, height);
	if (hr != S_OK)
	{
		return -1;
	}
	 hr = CreateSimpleShaders();
	 if (hr != S_OK)
	 {
		 return -1;
	 }
	 api.SetRenderTarget();
	return 1;
}

int D3DPROJECT::RENDERER::RenderD11::CreateTransformBuffer()
{
	D3D11_BUFFER_DESC _worldBufferInstanceDesc;
	//Create world constant buffer desc
	ZeroMemory(&_worldBufferInstanceDesc, sizeof(D3D11_BUFFER_DESC));

	_worldBufferInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	_worldBufferInstanceDesc.ByteWidth = sizeof(worldPosition);
	_worldBufferInstanceDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	_worldBufferInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	_worldBufferInstanceDesc.MiscFlags = 0;

	//Create the world constant buffer
	ID3D11Buffer* tempBuffer;
	HRESULT hr = api.GetDevice()->CreateBuffer(&_worldBufferInstanceDesc, NULL, &tempBuffer);
	if (hr != S_OK)
	{
		return -1;
	}
	transformBuffers.push_back(tempBuffer);
	return transformBuffers.size() - 1;
}

int D3DPROJECT::RENDERER::RenderD11::CreateMaterialBuffer(void* matData)
{
	D3D11_BUFFER_DESC  _colourDesc;
	D3D11_SUBRESOURCE_DATA _matData;
	ZeroMemory(&_colourDesc, sizeof(D3D11_BUFFER_DESC));
	
		_colourDesc.Usage = D3D11_USAGE_DYNAMIC;
		_colourDesc.ByteWidth = sizeof(OBJColourType);
		_colourDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		_colourDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		_colourDesc.MiscFlags = 0;
	
		_matData.pSysMem = matData;
		_matData.SysMemPitch = 0;
		_matData.SysMemSlicePitch = 0;

		//skapar constant buffer
		ID3D11Buffer* tempBuffer;
		HRESULT hr = api.GetDevice()->CreateBuffer(&_colourDesc, &_matData, &tempBuffer);
		if (hr != S_OK)
		{
			return -1;
		}
		matBuffers.push_back(tempBuffer);
	return matBuffers.size() - 1;
}

int D3DPROJECT::RENDERER::RenderD11::CreateVertexBuffer(void* vertexData, size_t nrOfFaces)
{
	VertexInputType* vertexInData = (VertexInputType*)vertexData;
	D3D11_BUFFER_DESC _vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA _vertexData;

	// description for vertexbuffer
	_vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	_vertexBufferDesc.ByteWidth = sizeof(float) * 8 * (nrOfFaces * 3);
	_vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	_vertexBufferDesc.CPUAccessFlags = 0;
	_vertexBufferDesc.MiscFlags = 0;
	_vertexBufferDesc.StructureByteStride = 0;


	_vertexData.pSysMem = vertexInData;
	_vertexData.SysMemPitch = 0;
	_vertexData.SysMemSlicePitch = 0;

	// Creates vertex buffern
	ID3D11Buffer* tempBuffer;
	HRESULT hr =  api.GetDevice()->CreateBuffer(&_vertexBufferDesc, &_vertexData, &tempBuffer);

	if (hr != S_OK)
	{
		return -1;
	}
	vertexBuffers.push_back(tempBuffer);
	return vertexBuffers.size() - 1;
}

int D3DPROJECT::RENDERER::RenderD11::CreateTextureResource(std::wstring filename)
{
	ID3D11ShaderResourceView* tempResource;
	HRESULT hr = CreateWICTextureFromFile(api.GetDevice(), api.GetDeviceContext(), filename.c_str(), nullptr, &tempResource);
	if (hr != S_OK)
	{
		return -1;
	}
	textureResource.push_back(tempResource);
	return textureResource.size() - 1;
}

int D3DPROJECT::RENDERER::RenderD11::CreateCameraBuffer()
{
	HRESULT hr;

	D3D11_BUFFER_DESC cameraBufferDesc;
	ZeroMemory(&cameraBufferDesc, sizeof(D3D11_BUFFER_DESC));

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;

	//skapar constant buffer
	ID3D11Buffer* tempBuffer;
	hr = api.GetDevice()->CreateBuffer(&cameraBufferDesc, NULL, &tempBuffer);

	if (hr != S_OK)
	{
		return -1;
	}
	cameraBuffer.push_back(tempBuffer);
	return cameraBuffer.size() - 1;
}

void D3DPROJECT::RENDERER::RenderD11::SetCameraBuffer(void* cameraData, int cameraIndex)
{
	D3D11_MAPPED_SUBRESOURCE _mappedResource;
	CameraType* camera = (CameraType*)cameraData;
	HRESULT hr = api.GetDeviceContext()->Map(cameraBuffer[cameraIndex], 0, D3D11_MAP_WRITE_DISCARD, 0, &_mappedResource);
	memcpy(_mappedResource.pData, camera, sizeof(CameraType));

	api.GetDeviceContext()->Unmap(cameraBuffer[cameraIndex], 0);
}

void D3DPROJECT::RENDERER::RenderD11::SetTransformBuffer(void* worldRotation, int index)
{
	XMFLOAT4X4 _worldInstance;
	XMFLOAT4X4 _worldRotation;
	XMMATRIX Translation;
	XMMATRIX Scale;
	worldPosition* transData = (worldPosition*)worldRotation;
	D3D11_MAPPED_SUBRESOURCE _mappedResource;

	HRESULT hr = api.GetDeviceContext()->Map(transformBuffers[index], 0, D3D11_MAP_WRITE_DISCARD, 0, &_mappedResource);

	memcpy(_mappedResource.pData, transData, sizeof(worldPosition));

	api.GetDeviceContext()->Unmap(transformBuffers[index], 0);

	api.GetDeviceContext()->VSSetConstantBuffers(0, 1, &transformBuffers[index]);
}

void D3DPROJECT::RENDERER::RenderD11::Render(int transIndex, int matIndex, int vertexIndex, int texIndex, int cameraIndex, int faceCount, int entCount)
{

		api.GetDeviceContext()->GSSetConstantBuffers(1, 1, &cameraBuffer[cameraIndex]);

		UINT32 _vertexSize = sizeof(float) * 8;
		UINT32 _offset = 0;
		api.GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffers[vertexIndex], &_vertexSize, &_offset);

		api.GetDeviceContext()->VSSetConstantBuffers(0, 1, &transformBuffers[transIndex]);

		api.GetDeviceContext()->PSSetConstantBuffers(2, 1, &matBuffers[matIndex]);

		api.GetDeviceContext()->PSSetShaderResources(0, 1, &textureResource[texIndex]);

	api.GetDeviceContext()->VSSetShader(vertexShader, nullptr, 0);
	api.GetDeviceContext()->HSSetShader(nullptr, nullptr, 0);
	api.GetDeviceContext()->DSSetShader(nullptr, nullptr, 0);
	api.GetDeviceContext()->GSSetShader(geometryShader, nullptr, 0);
	api.GetDeviceContext()->PSSetShader(pixelShader, nullptr, 0);

	api.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	api.GetDeviceContext()->IASetInputLayout(vertexLayout);

	api.GetDeviceContext()->DrawInstanced(faceCount * 3, entCount, 0, 0);
}

void D3DPROJECT::RENDERER::RenderD11::Present()
{
	api.SwapChain();
}

void D3DPROJECT::RENDERER::RenderD11::Clean()
{
	api.Clean();
}

HRESULT D3DPROJECT::RENDERER::RenderD11::CreateSimpleShaders()
{
	//create vertex shader
	ID3DBlob* _vs = nullptr;
	HRESULT _hr = D3DCompileFromFile(L"Shaders/SimpleVS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", 0, NULL, &_vs, nullptr);

	if (_hr != S_OK)
	{
		_vs->Release();
		return _hr;
	}

	LPVOID testbuffer = _vs->GetBufferPointer();
	int testsize = _vs->GetBufferSize();

	_hr = api.GetDevice()->CreateVertexShader(_vs->GetBufferPointer(), _vs->GetBufferSize(), nullptr, &vertexShader);

	if (_hr != S_OK)
	{
		_vs->Release();
		return _hr;
	}

	//create input layout (verified with vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	_hr = api.GetDevice()->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), _vs->GetBufferPointer(), _vs->GetBufferSize(), &vertexLayout);
	_vs->Release();
	if (_hr != S_OK)
	{
		return _hr;
	}

	////create pixel shader
	ID3DBlob* _gs = nullptr;
	D3DCompileFromFile(L"Shaders/SimpleGS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS_main", "gs_5_0", 0, NULL, &_gs, nullptr);
	_hr = api.GetDevice()->CreateGeometryShader(_gs->GetBufferPointer(), _gs->GetBufferSize(), nullptr, &geometryShader);
	_gs->Release();
	if (_hr != S_OK)
	{
		return _hr;
	}

	//create pixel shader
	ID3DBlob* _ps = nullptr;
	D3DCompileFromFile(L"Shaders/SimplePS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_5_0", 0, NULL, &_ps, nullptr);
	_hr = api.GetDevice()->CreatePixelShader(_ps->GetBufferPointer(), _ps->GetBufferSize(), nullptr, &pixelShader);
	_ps->Release();
	return _hr;
}

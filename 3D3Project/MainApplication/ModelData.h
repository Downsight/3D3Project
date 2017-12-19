#ifndef MODELDATA_H
#define MODELDATA_H

#include "OBJloader.h"

class ModelData
{
private:
	struct OBJColourType
	{
		XMFLOAT3 Deffuse;
		XMFLOAT3 Albi;
		XMFLOAT3 Tf;
		XMFLOAT3 Ni;
	};
public:
	ModelData();
	~ModelData();
	bool LoadObjData(string OBJFileName, string colourFileName);
	int GetFaceCount() { return m_faceCount; }
	void* GetVertexData() { return (void*)m_vertexInput; }
	void* GetMatData() { return (void*)&m_colour; }

	int GetVBufferIndex() { return vBufferIndex; }
	int GetMBufferIndex() { return mBufferIndex; }
	int GetTexIndex() { return tIndex; }
	void SetVBufferIndex(int index) { vBufferIndex = index; }
	void SetMBufferIndex(int index) { mBufferIndex = index; }
	void SetTIndex(int index) { tIndex = index; }
	std::wstring GetTexName() { return texName; }

private:
	XMFLOAT3* m_vertices, *m_normals, *m_RGBDeffuse, *m_RGBAL, *m_Tf, *m_Ni;
	XMFLOAT2* m_texcoords;
	int m_vertexCount, m_textureCount, m_normalCount, m_faceCount, m_kdCount, m_kaCount, m_tfCount, m_niCount;
	FaceType* m_faces;
	std::wstring texName;
	VertexInputType* m_vertexInput;
	OBJColourType m_colour;
	int vBufferIndex = -1;
	int mBufferIndex = -1;
	int tIndex = -1;
};



#endif
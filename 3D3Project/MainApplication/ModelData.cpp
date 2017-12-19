#include "ModelData.h"

ModelData::ModelData()
{

}

ModelData::~ModelData()
{
	if (m_RGBDeffuse)
	{
		delete m_RGBDeffuse;
		delete m_RGBAL;
		delete m_Tf;
		delete m_Ni;
		delete m_vertices;
		delete m_vertexInput;
		delete m_texcoords;
		delete m_faces;
	}
}

bool ModelData::LoadObjData(string OBJFileName, string colourFileName)
{
	bool _ifOK;
	OBJLoader m_OBJLoad;

	m_vertexInput = m_OBJLoad.LoadObj(m_vertexCount, m_textureCount, m_normalCount, m_faceCount, OBJFileName);

	//Sets amount of kd, ka, tf and ni counts for the file
	_ifOK = m_OBJLoad.ReadColourCounts(m_kdCount, m_kaCount, m_tfCount, m_niCount, colourFileName);
	if (_ifOK == false)
	{
		return false;
	}

	// Initialize the four data structures.
	m_RGBDeffuse = new XMFLOAT3[m_kdCount];
	if (!m_RGBDeffuse)
	{
		return false;
	}

	m_RGBAL = new XMFLOAT3[m_kaCount];
	if (!m_RGBAL)
	{
		return false;
	}

	m_Tf = new XMFLOAT3[m_tfCount];
	if (!m_Tf)
	{
		return false;
	}

	m_Ni = new XMFLOAT3[m_niCount];
	if (!m_Ni)
	{
		return false;
	}

	//Loads the colour data from file
	m_OBJLoad.LoadColour(colourFileName, m_RGBDeffuse, m_RGBAL, m_Tf, m_Ni, texName);
	return true;
}

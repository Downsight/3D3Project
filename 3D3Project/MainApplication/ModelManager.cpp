#include "ModelManager.h"

ModelManager::ModelManager()
{
	
}

ModelManager::ModelManager(D3DPROJECT::RENDERER::IRenderer * rend)
{
	renderer = rend;
}

ModelManager::~ModelManager()
{
	for (int i = 0; m_models.size() > i; i++)
	{
		delete m_models[i];
	}
	m_models.clear();
}

int ModelManager::NewModel(string OBJFileName, string colourFileName)
{
	ModelData *TempData = new ModelData;
	TempData->LoadObjData(OBJFileName, colourFileName);
	TempData->SetVBufferIndex(renderer->CreateVertexBuffer(TempData->GetVertexData(), TempData->GetFaceCount()));
	TempData->SetMBufferIndex(renderer->CreateMaterialBuffer(TempData->GetMatData()));
	TempData->SetTIndex(renderer->CreateTextureResource(TempData->GetTexName()));
	if (TempData->GetMBufferIndex() < 0 || TempData->GetVertexData() < 0 || TempData->GetTexIndex() < 0)
	{
		return -1;
	}

	m_models.push_back(TempData);
	return m_models.size() - 1;
}

int ModelManager::GetFaceCount(int nrModel)
{ 
	if (nrModel < m_models.size())
	{
		return m_models.at(nrModel)->GetFaceCount();
	}
	return 0;
}

ModelData* ModelManager::GetModel(int nrModel)
{
	return m_models.at(nrModel);
}
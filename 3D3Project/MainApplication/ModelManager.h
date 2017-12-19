#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "ModelData.h"
#include <vector>
#include <Renderer\IRenderer.h>

class ModelManager
{
public:
	ModelManager();
	ModelManager(D3DPROJECT::RENDERER::IRenderer* rend);
	~ModelManager();
	int GetFaceCount(int nrModel);
	ModelData* GetModel(int nrModel);
	int NewModel(string OBJFileName, string colourFileName);
	
	

private:
	std::vector<ModelData*> m_models;
	D3DPROJECT::RENDERER::IRenderer* renderer = nullptr;
};



#endif
#pragma once
#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "Entity.h"
#include <vector>
#include <Renderer\IRenderer.h>

class EntityManager
{
public:
	EntityManager();
	EntityManager(D3DPROJECT::RENDERER::IRenderer* rend);
	~EntityManager();
	int NewEntityType(float posx, float posy, float posz, float scalex, float scaley, float scalez);
	void NewEntity(float posx, float posy, float posz, float scalex, float scaley, float scalez, int nrEntityType);
	void UpdateRot(XMFLOAT4 rotationaxis, float rotation, int entity, int entityType);
	int GetEntityAmount(int nrEntityType);
	void UpdateBuffers(int nrEntityType);
	int GetEntityTypeAmount() { return m_entityType.size(); }
	void SetRot(XMFLOAT4 rotationaxis, float rotation, int entity, int entityType);
	void SetPos(XMFLOAT4 newPos, int entity, int entityType);
	XMFLOAT4 GetPos(int entity, int entityType);
	void UpdatePos(XMFLOAT4 newPos, int entity, int entityType);
	void Frame();
	int GetTransBufferIndex(int entityType) { return m_entityType[entityType].GetTransBufferIndex(); }

private:
	std::vector<Entity> m_entityType;
	D3DPROJECT::RENDERER::IRenderer* renderer = nullptr;

};



#endif
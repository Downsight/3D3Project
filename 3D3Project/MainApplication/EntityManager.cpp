#include "EntityManager.h"

EntityManager::EntityManager()
{

}

EntityManager::EntityManager(D3DPROJECT::RENDERER::IRenderer * rend)
{
	renderer = rend;
}

EntityManager::~EntityManager()
{
	m_entityType.clear();
}

int EntityManager::NewEntityType(float posx, float posy, float posz, float scalex, float scaley, float scalez)
{
	Entity _temp;
	int index = renderer->CreateTransformBuffer();
	if (index == -1)
	{
		return index;
	}
	_temp.NewEntity(posx, posy, posz, scalex, scaley, scalez);
	m_entityType.push_back(_temp);
	return m_entityType.size() - 1;
}

void EntityManager::NewEntity(float posx, float posy, float posz, float scalex, float scaley, float scalez, int nrEntityType)
{
	if (nrEntityType < m_entityType.size())
	{
		m_entityType.at(nrEntityType).NewEntity(posx, posy, posz, scalex, scaley, scalez);
	}
}

void EntityManager::UpdateRot(XMFLOAT4 rotationaxis, float rotation, int entity, int entityType)
{
	m_entityType.at(entityType).UpdateRot(rotationaxis, rotation, entity);
}

void EntityManager::UpdateBuffers(int nrEntityType)
{
	if ( nrEntityType < m_entityType.size() && m_entityType[nrEntityType].GetChange() == true)
	{
		renderer->SetTransformBuffer(m_entityType[nrEntityType].GetTransform(), m_entityType[nrEntityType].GetTransBufferIndex());
		m_entityType[nrEntityType].SetChanged(false);
	}
}

int EntityManager::GetEntityAmount(int nrEntityType)
{
	return m_entityType.at(nrEntityType).GetAmount();
}

void EntityManager::SetRot(XMFLOAT4 rotationaxis, float rotation, int entity, int entityType)
{
	m_entityType.at(entityType).SetRot(rotationaxis, rotation, entity);
}

void EntityManager::SetPos(XMFLOAT4 newPos, int entity, int entityType)
{
	m_entityType.at(entityType).SetPos(newPos, entity);
}

XMFLOAT4 EntityManager::GetPos(int entity, int entityType)
{
	return m_entityType.at(entityType).GetPos(entity);
}

void EntityManager::UpdatePos(XMFLOAT4 newPos, int entity, int entityType)
{
	m_entityType.at(entityType).UpdatePos(newPos, entity);
}

void EntityManager::Frame()
{
	for (auto& entType : m_entityType)
	{
		entType.Frame();
	}
}

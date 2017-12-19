#include "Entity.h"

Entity::Entity()
{

}

Entity::~Entity()
{
	m_entities.clear();
}

void Entity::NewEntity(float posx, float posy, float posz, float scalex, float scaley, float scalez)
{
	EntityArray temp;
	temp.m_x = posx;
	temp.m_y = posy;
	temp.m_z = posz;
	temp.m_rotation = XMMatrixIdentity();
	temp.m_scalex = scalex;
	temp.m_scaley = scaley;
	temp.m_scalez = scalez;
	dirtyIndex.push(m_entities.size());
	m_entities.push_back(temp);
}

void Entity::UpdateRot(XMFLOAT4 rotationaxis, float rotation, int entity)
{
	XMVECTOR _axis = XMVectorSet(rotationaxis.x, rotationaxis.y, rotationaxis.z, rotationaxis.w);
	m_entities[entity].m_rotation = m_entities[entity].m_rotation * XMMatrixRotationAxis(_axis, XM_PI * rotation);
	dirtyIndex.push(entity);
	changed = true;
}

int Entity::GetAmount()
{
	return m_entities.size();
}

void Entity::SetRot(XMFLOAT4 rotationaxis, float rotation, int entity)
{
	XMVECTOR _axis = XMVectorSet(rotationaxis.x, rotationaxis.y, rotationaxis.z, rotationaxis.w);
	m_entities[entity].m_rotation = XMMatrixRotationAxis(_axis, XM_PI * rotation);
	dirtyIndex.push(entity);
	changed = true;
}

void Entity::SetPos(XMFLOAT4 newPos, int entity)
{
	m_entities.at(entity).m_x = newPos.x;
	m_entities.at(entity).m_y = newPos.y;
	m_entities.at(entity).m_z = newPos.z;
	dirtyIndex.push(entity);
	changed = true;
}

void Entity::SetTransBuffer(int index)
{
	this->m_transformIndex = index;
}

XMFLOAT4 Entity::GetPos(int entity)
{
	return XMFLOAT4(m_entities.at(entity).m_x, m_entities.at(entity).m_y, m_entities.at(entity).m_z, 0.0);
}

void Entity::UpdatePos(XMFLOAT4 newPos, int entity)
{
	m_entities.at(entity).m_x += newPos.x;
	m_entities.at(entity).m_y += newPos.y;
	m_entities.at(entity).m_z += newPos.z;
	dirtyIndex.push(entity);
	changed = true;
}

void Entity::Frame()
{
	XMFLOAT4X4 _worldInstance;
	XMFLOAT4X4 _worldRotation;
	XMMATRIX Translation;
	XMMATRIX Scale;
	
	int hej = 0;
	while (dirtyIndex.size() > 0)
	{
		Translation = XMMatrixTranslation(m_entities.at(dirtyIndex.top()).m_x, m_entities.at(dirtyIndex.top()).m_y, m_entities.at(dirtyIndex.top()).m_z);
		Scale = XMMatrixScaling(m_entities.at(dirtyIndex.top()).m_scalex, m_entities.at(dirtyIndex.top()).m_scaley, m_entities.at(dirtyIndex.top()).m_scalez);
		XMStoreFloat4x4(&_worldInstance, XMMatrixTranspose(Scale * m_entities.at(dirtyIndex.top()).m_rotation * Translation));
		XMStoreFloat4x4(&_worldRotation, XMMatrixTranspose(m_entities.at(dirtyIndex.top()).m_rotation));
		transforms.World[dirtyIndex.top()] = _worldInstance;
		transforms.Rotation[dirtyIndex.top()] = _worldRotation;
		dirtyIndex.pop();
	}
}

void* Entity::GetTransform()
{
	return (void*)&transforms;
}

size_t Entity::GetTransSize()
{
	return m_entities.size();
}

int Entity::GetTransBufferIndex()
{
	return m_transformIndex;
}

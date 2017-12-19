#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <stack>

using namespace DirectX;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

struct EntityArray
{
	float m_x, m_y, m_z;
	XMMATRIX m_rotation;
	float m_scalex, m_scaley, m_scalez;
};

struct worldPosition
{
	XMFLOAT4X4 World[256];
	XMFLOAT4X4 Rotation[256];
};

class Entity
{
public:
	Entity();
	~Entity();
	void NewEntity(float posx, float posy, float posz, float scalex, float scaley, float scalez);
	void UpdateRot(XMFLOAT4 rotationaxis, float rotation, int entity);
	int GetAmount();
	void SetRot(XMFLOAT4 rotationaxis, float rotation, int entity);
	void SetPos(XMFLOAT4 newPos, int entity);
	void SetTransBuffer(int index);
	XMFLOAT4 GetPos(int entity);
	void UpdatePos(XMFLOAT4 newPos, int entity);
	void Frame();
	void* GetTransform();
	size_t GetTransSize();
	int GetTransBufferIndex();

	bool GetChange() { return changed; }
	void SetChanged(bool newChange) { changed = newChange; }

private:
	std::vector<EntityArray> m_entities;
	worldPosition transforms;
	int m_transformIndex;

	std::stack<int> dirtyIndex;
	bool changed = false;
};



#endif
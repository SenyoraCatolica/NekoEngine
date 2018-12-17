#pragma once

#include <list>
#include "MathGeoLib\include\MathGeoLib.h"

class btRigidBody;
class Module;
class BoxColliderComponent;
class RigidBodyComponent;

class PhysicBody3D
{
public:
	PhysicBody3D(btRigidBody* body);
	~PhysicBody3D();

	void Push(float x, float y, float z);
	void Push(math::float3 dir);

	math::float4x4 GetTransform();
	math::float3 GetPos();

	void SetTransform(math::float4x4 matrix);
	void SetPos(float x, float y, float z);
	void SetPos(math::float3 pos);


private:
	math::float3 position = math::float3::zero;

public:
	std::list<Module*> collision_listeners;
	btRigidBody* body = nullptr;
	bool is_sensor = false;

};
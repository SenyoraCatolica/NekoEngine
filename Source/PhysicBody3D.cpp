
#include "PhysicBody3D.h"
#include "Bullet\include\btBulletDynamicsCommon.h"

PhysicBody3D::PhysicBody3D(btRigidBody* body) : body(body) {}

PhysicBody3D::~PhysicBody3D()
{
	delete body;
}

void PhysicBody3D::Push(float x, float y, float z)
{
	body->applyCentralImpulse(btVector3(x, y, z));
}

void PhysicBody3D::Push(math::float3 dir)
{
	body->applyCentralImpulse(btVector3(dir.x, dir.y, dir.z));
}

math::float4x4 PhysicBody3D::GetTransform()
{
	math::float4x4 ret = math::float4x4::identity;

	if (body != nullptr)
		body->getWorldTransform().getOpenGLMatrix(*ret.v);

	return ret;
}

void PhysicBody3D::SetTransform(math::float4x4 matrix)
{
	if (body != nullptr)
	{
		btTransform t;
		t.setFromOpenGLMatrix(*matrix.v);
		body->setWorldTransform(t);
	}
}

math::float3 PhysicBody3D::GetPos()
{
	return position;
}

void PhysicBody3D::SetPos(float x, float y, float z)
{
	btTransform t = body->getWorldTransform();
	t.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(t);
	position = math::float3(x, y, z);
}

void PhysicBody3D::SetPos(math::float3 pos)
{
	SetPos(pos.x, pos.y, pos.z);
}



#pragma once

#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"
#include "PrimitiveShape.h"

class PhysicBody3D;

class SphereColliderComponent : public Component
{
public:

	SphereColliderComponent(GameObject* parent);
	~SphereColliderComponent();

	void GenerateSphereCollider();
	void UpdateSphereCollider(bool render);

	void OnUniqueEditor();

	PrimitiveShapeSphere* GetSphereCollider();

	virtual void OnInternalSave(JSON_Object* file);
	virtual void OnLoad(JSON_Object* file);

public:

	math::float3 offset = math::float3::zero;
	float radius = 1.0;

private:
	PrimitiveShapeSphere * sphere;
};
	
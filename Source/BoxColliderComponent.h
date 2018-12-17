#pragma once

#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"
#include "PrimitiveShape.h"


class PhysicBody3D;
class RigidBody3DComponent;

class BoxColliderComponent : public Component
{

public:

	BoxColliderComponent(GameObject* embedded_game_object);
	~BoxColliderComponent();

	void UpdateBoxCollider(bool render);

	void GenerateBoxCollider();

	void OnUniqueEditor();

	PrimitiveShapeCube* GetBoxCollider();

	virtual void OnInternalSave(JSON_Object* file);
	virtual void OnLoad(JSON_Object* file);

public:

	math::float3 offset = math::float3::zero;
	math::float3 size = math::float3::one;
	bool is_trigger = false;

private:
	PrimitiveShapeCube* box = nullptr;

};
#pragma once

#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"


class PhysicBody3D;
class RigidBody3DComponent;

class BoxColliderComponent : public Component
{

public:

	BoxColliderComponent(GameObject* embedded_game_object);
	~BoxColliderComponent();

	void GenerateBoxCollider();

	void SetRigidBody(RigidBody3DComponent* rb);

	void OnUniqueEditor();

	bool Save(JSON_Object* component_obj)const;
	bool Load(const JSON_Object* component_obj);

public:

	math::float3 offset;
	math::float3 size;
	bool is_trigger = false;
	math::OBB box;

private:
	RigidBody3DComponent * rb;
};
#pragma once

#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"


class BoxColliderComponent;
class ComponentTransform;
class PhysicBody3D;

class RigidBody3DComponent : public Component
{

public:
	RigidBody3DComponent(GameObject* embedded_game_object);
	~RigidBody3DComponent();

	void GeneratePhysicBbody();

	void SetBox(BoxColliderComponent* col);

	void OnUniqueEditor();

	bool Save(JSON_Object* component_obj)const;
	bool Load(const JSON_Object* component_obj);



public:

	PhysicBody3D * body = nullptr;
	BoxColliderComponent* box = nullptr;
	ComponentTransform* transform = nullptr;

	bool is_kinematic = false;
	float mass = 1.0f;

};

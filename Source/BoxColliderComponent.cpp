
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "ModulePhysics.h"
#include "RigidBody3DComponent.h"

BoxColliderComponent::BoxColliderComponent(GameObject* embedded_game_object) :
	Component(embedded_game_object, ComponentType::COMPONENT_BOX)
{
	GenerateBoxCollider();
	box.SetNegativeInfinity();
}

BoxColliderComponent::~BoxColliderComponent()
{
	rb = nullptr;
}

void BoxColliderComponent::GenerateBoxCollider()
{
	if (parent)
	{
		rb = (RigidBody3DComponent*)parent->GetComponent(COMPONENT_RB);
	}

	box.r = math::float3::one;
	box.pos = math::float3::zero;
	box.axis[0] = math::float3::unitX;
	box.axis[1] = math::float3::unitY;
	box.axis[2] = math::float3::unitZ;


	if (rb != nullptr)
		rb->SetBox(this);
}

void BoxColliderComponent::SetRigidBody(RigidBody3DComponent* rb)
{
	this->rb = rb;
}


bool BoxColliderComponent::Save(JSON_Object* component_obj)const
{
	return true;
}

bool BoxColliderComponent::Load(const JSON_Object* component_obj)
{
	return true;
}
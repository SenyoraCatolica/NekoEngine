#include "RigidBody3DComponent.h"
#include "BoxColliderComponent.h"
#include "PhysicBody3D.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "ModulePhysics.h"


RigidBody3DComponent::RigidBody3DComponent(GameObject* embedded_game_object) :
	Component(embedded_game_object, ComponentType::COMPONENT_BOX)
{
	GeneratePhysicBbody();
}

RigidBody3DComponent::~RigidBody3DComponent()
{

}

void RigidBody3DComponent::GeneratePhysicBbody()
{
	if (parent != nullptr)
	{
		transform = parent->transform;
		box = (BoxColliderComponent*)parent->GetComponent(COMPONENT_BOX);

		if (box != nullptr)
			box->SetRigidBody(this);
	}
}

void RigidBody3DComponent::SetBox(BoxColliderComponent* col)
{
	box = col;
}



bool RigidBody3DComponent::Save(JSON_Object* component_obj)const
{
	return true;
}

bool RigidBody3DComponent::Load(const JSON_Object* component_obj)
{
	return true;
}
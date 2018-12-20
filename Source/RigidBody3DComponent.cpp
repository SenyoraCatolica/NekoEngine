#include "RigidBody3DComponent.h"
#include "BoxColliderComponent.h"
#include "PhysicBody3D.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "ModulePhysics.h"
#include "imgui\imgui.h"
#include "imgui\imgui_internal.h"



RigidBody3DComponent::RigidBody3DComponent(GameObject* embedded_game_object) :
	Component(embedded_game_object, ComponentType::COMPONENT_RB)
{
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
	}

	is_kinematic = false;
	mass = 1.0f;
}

void RigidBody3DComponent::SetBox(BoxColliderComponent* col)
{
	box = col;
}

void RigidBody3DComponent::OnUniqueEditor()
{
	ImGui::TextColored(ImVec4(0, 0, 1.0, 1), "Component Rigidbody 3D");

	if (IsActive())
	{
		ImGui::NewLine();
		//const double f64_lo_a = -1000000000000000.0, f64_hi_a = +1000000000000000.0;

		//ImGui::Text("Mass");
		//ImGui::PushItemWidth(TRANSFORMINPUTSWIDTH);
		//ImGui::DragScalar("##Mass", ImGuiDataType_Float, (void*)&mass, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f);
		ImGui::DragFloat("Mass: ", &mass, 0.1f, 0.0f, 100000.0f);

		ImGui::NewLine();

		ImGui::Checkbox("Is Kinematic", &is_kinematic);
	}	
}


void RigidBody3DComponent::OnInternalSave(JSON_Object* file)
{
	json_object_set_number(file, "Mass", (double)mass);
	json_object_set_boolean(file, "IsKinematic", is_kinematic);
}

void RigidBody3DComponent::OnLoad(JSON_Object* file)
{
	mass = json_object_get_number(file, "Mass");
	is_kinematic = json_object_get_boolean(file, "IsKinematic");
}
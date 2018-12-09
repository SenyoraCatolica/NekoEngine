
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "ModulePhysics.h"
#include "RigidBody3DComponent.h"

#include "imgui\imgui.h"
#include "imgui\imgui_internal.h"

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

void BoxColliderComponent::OnUniqueEditor()
{
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);


	const double f64_lo_a = -1000000000000000.0, f64_hi_a = +1000000000000000.0;

	ImGui::Text("Position");

	ImGui::PushItemWidth(50);
	ImGui::DragScalar("##PosX", ImGuiDataType_Float, (void*)&offset.x, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f); ImGui::SameLine();
	ImGui::PushItemWidth(50);
	ImGui::DragScalar("##PosY", ImGuiDataType_Float, (void*)&offset.y, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f); ImGui::SameLine();
	ImGui::PushItemWidth(50);
	ImGui::DragScalar("##PosZ", ImGuiDataType_Float, (void*)&offset.z, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f); ImGui::SameLine();

	ImGui::Text("Size");

	ImGui::PushItemWidth(50);
	ImGui::DragScalar("##SizeX", ImGuiDataType_Float, (void*)&size.x, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f); ImGui::SameLine();
	ImGui::PushItemWidth(50);
	ImGui::DragScalar("##SizeY", ImGuiDataType_Float, (void*)&size.y, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f); ImGui::SameLine();
	ImGui::PushItemWidth(50);
	ImGui::DragScalar("##SixeZ", ImGuiDataType_Float, (void*)&size.z, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f); ImGui::SameLine();

	ImGui::Checkbox("Is Trigger", &is_trigger);
}


bool BoxColliderComponent::Save(JSON_Object* component_obj)const
{
	return true;
}

bool BoxColliderComponent::Load(const JSON_Object* component_obj)
{
	return true;
}
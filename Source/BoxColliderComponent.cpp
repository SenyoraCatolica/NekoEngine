
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "ComponentTransform.h"


#include "imgui\imgui.h"
#include "imgui\imgui_internal.h"

BoxColliderComponent::BoxColliderComponent(GameObject* embedded_game_object) :
	Component(embedded_game_object, ComponentType::COMPONENT_BOX)
{
	GenerateBoxCollider();
}

BoxColliderComponent::~BoxColliderComponent()
{
}

void BoxColliderComponent::UpdateBoxCollider(bool render)
{
	if (box != nullptr)
	{
		if (App->IsEditor())
		{
			//update box on editor
			if (box != nullptr)
			{
				//Get the transform data from go transform
				math::float3 trans; math::Quat rot; math::float3 scale;
				parent->transform->GetGlobalMatrix().Decompose(trans, rot, scale);

				//Add offset
				math::float3 real_offset = rot.Transform(offset);
				trans -= real_offset;

				//Set the new transform
				box->SetPos(trans.x, trans.y, trans.z);
				box->SetRotation(rot.Inverted());

				if(render)
					box->Render();
			}
		}
	}
}


void BoxColliderComponent::GenerateBoxCollider()
{	
	if (parent->boundingBox.IsFinite())
		box = new PrimitiveCube(parent->boundingBox.Size());
	else
		box = new PrimitiveCube(math::float3::one);
}

void BoxColliderComponent::SetRigidBody(RigidBody3DComponent* rb)
{

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

PrimitiveCube* BoxColliderComponent::GetBoxCollider()
{
	return box;
}



bool BoxColliderComponent::Save(JSON_Object* component_obj)const
{
	return true;
}

bool BoxColliderComponent::Load(const JSON_Object* component_obj)
{
	return true;
}
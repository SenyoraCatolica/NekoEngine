
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
			if (box != nullptr && parent != nullptr)
			{
				//Get the transform data from go transform
				math::float3 trans; math::Quat rot; math::float3 scale;
				math::float4x4 matrix = parent->transform->GetGlobalMatrix();
				matrix.Decompose(trans, rot, scale);

				//Add offset
				math::float3 real_offset = rot.Transform(offset);
				trans -= real_offset;

				//Set the new transform
				box->SetPos(trans.x, trans.y, trans.z);
				box->SetRotation(rot.Inverted());

				if (render)
					box->Render();
			}
		}
	}
}


void BoxColliderComponent::GenerateBoxCollider()
{
	if (parent->boundingBox.IsFinite())
		box = new PrimitiveShapeCube(parent->boundingBox.Size().x, parent->boundingBox.Size().y, parent->boundingBox.Size().z);
	else
		box = new PrimitiveShapeCube(1, 1, 1);
}

void BoxColliderComponent::SetRigidBody(RigidBody3DComponent* rb)
{

}

void BoxColliderComponent::OnUniqueEditor()
{
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);


	const double f64_lo_a = -1000000000000000.0, f64_hi_a = +1000000000000000.0;

	ImGui::DragFloat3("Collider offset: ", offset.ptr(), 0.1f, -1000.0f, 1000.0f);

	ImGui::DragFloat3("Size: ", box->size.ptr(), 0.1f, -1000.0f, 1000.0f);

	ImGui::Checkbox("Is Trigger", &is_trigger);
}

PrimitiveShapeCube* BoxColliderComponent::GetBoxCollider()
{
	if(box) return box;
}



void BoxColliderComponent::OnInternalSave(JSON_Object* file)
{
	json_object_set_number(file, "OffsetX", offset.x);
	json_object_set_number(file, "OffsetY", offset.y);
	json_object_set_number(file, "OffsetZ", offset.z);
	json_object_set_number(file, "SizeX", size.x);
	json_object_set_number(file, "SizeY", size.y);
	json_object_set_number(file, "SizeZ", size.z);

	json_object_set_boolean(file, "IsTrigger", is_trigger);
}

void BoxColliderComponent::OnLoad(JSON_Object* file)
{
	offset.x = json_object_get_number(file, "OffsetX");
	offset.y = json_object_get_number(file, "OffsetY");
	offset.z = json_object_get_number(file, "OffsetZ");
	size.x = json_object_get_number(file, "SizeX");
	size.y = json_object_get_number(file, "SizeY");
	size.z = json_object_get_number(file, "SizeZ");

	is_trigger = json_object_get_boolean(file, "IsTrigger");
}
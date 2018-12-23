
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
			if (parent != nullptr)
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
	if (parent->localBoundingBox.IsFinite())
	{
		box = new PrimitiveShapeCube(parent->localBoundingBox.Size().x, parent->localBoundingBox.Size().y, parent->localBoundingBox.Size().z);
		size = box->size;
	}
	else
		box = new PrimitiveShapeCube(1, 1, 1);

	box->wire = true;
}

void BoxColliderComponent::OnUniqueEditor()
{
	ImGui::TextColored(ImVec4(0, 0, 1.0, 1), "Component Box Collider");
	if (IsActive())
	{
		ImGui::NewLine();

		const double f64_lo_a = -1000000000000000.0, f64_hi_a = +1000000000000000.0;

		ImGui::Text("Offset");

		ImGui::PushItemWidth(TRANSFORMINPUTSWIDTH);
		ImGui::DragScalar("##OffX", ImGuiDataType_Float, (void*)&offset.x, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f); ImGui::SameLine();
		ImGui::PushItemWidth(TRANSFORMINPUTSWIDTH);
		ImGui::DragScalar("##OffY", ImGuiDataType_Float, (void*)&offset.y, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f); ImGui::SameLine();
		ImGui::PushItemWidth(TRANSFORMINPUTSWIDTH);
		ImGui::DragScalar("##OffZ", ImGuiDataType_Float, (void*)&offset.z, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f);



		ImGui::Text("Box Scale");

		bool modify = false;

		ImGui::PushItemWidth(TRANSFORMINPUTSWIDTH);
		if (ImGui::DragScalar("##BSX", ImGuiDataType_Float, (void*)&size.x, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f))  modify = true; ImGui::SameLine();
		ImGui::PushItemWidth(TRANSFORMINPUTSWIDTH);
		if (ImGui::DragScalar("##BSY", ImGuiDataType_Float, (void*)&size.y, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f))  modify = true; ImGui::SameLine();
		ImGui::PushItemWidth(TRANSFORMINPUTSWIDTH);
		if(ImGui::DragScalar("##BSZ", ImGuiDataType_Float, (void*)&size.z, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f))  modify = true;

		if(modify)
			box->size = size;
	}	
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
}

void BoxColliderComponent::OnLoad(JSON_Object* file)
{
	offset.x = json_object_get_number(file, "OffsetX");
	offset.y = json_object_get_number(file, "OffsetY");
	offset.z = json_object_get_number(file, "OffsetZ");
	size.x = json_object_get_number(file, "SizeX");
	size.y = json_object_get_number(file, "SizeY");
	size.z = json_object_get_number(file, "SizeZ");

	if (size.x != 0 && size.y != 0 && size.z != 0)
		box->size = size;
	else
		size = box->size;
}
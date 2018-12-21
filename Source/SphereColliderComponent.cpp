#include "SphereColliderComponent.h"
#include "GameObject.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "ComponentTransform.h"

#include "imgui\imgui.h"
#include "imgui\imgui_internal.h"


SphereColliderComponent::SphereColliderComponent(GameObject* parent) :
	Component(parent, ComponentType::COMPONENT_SPHERE)
{
	GenerateSphereCollider();
}

SphereColliderComponent::~SphereColliderComponent()
{

}

void SphereColliderComponent::GenerateSphereCollider()
{
	sphere = new PrimitiveShapeSphere(1);
	sphere->wire = true;
}

void SphereColliderComponent::UpdateSphereCollider(bool render)
{
	if (sphere != nullptr)
	{
		if (App->IsEditor())
		{
			if (parent != nullptr)
			{
				//Get the transform data from go transform
				math::float3 trans; math::Quat rot; math::float3 scale;
				math::float4x4 matrix = parent->transform->GetGlobalMatrix();
				matrix.Decompose(trans, rot, scale);

				//Add offset
				math::float3 real_offset = rot.Transform(offset);
				trans -= real_offset;

				sphere->SetPos(trans.x, trans.y, trans.z);
				sphere->SetRotation(rot.Inverted());

				if (render)
					sphere->Render();
			}
		}
	}
}

void SphereColliderComponent::OnUniqueEditor()
{
	ImGui::TextColored(ImVec4(0, 0, 1.0, 1), "Component Sphere Collider");
	if (IsActive())
	{
		ImGui::NewLine();

		ImGui::DragFloat3("Collider sphere offset: ", offset.ptr(), 0.1f, -1000.0f, 1000.0f);

		ImGui::NewLine();

		if (ImGui::DragFloat("Radius: ", &radius, 0.1f, 0.0f, 1000.0f))
			sphere->radius = radius;
	}
}

PrimitiveShapeSphere* SphereColliderComponent::GetSphereCollider()
{
	return sphere;
}

void SphereColliderComponent::OnInternalSave(JSON_Object* file)
{
	json_object_set_number(file, "OffsetSX", offset.x);
	json_object_set_number(file, "OffsetSY", offset.y);
	json_object_set_number(file, "OffsetSZ", offset.z);
	json_object_set_number(file, "Radius", radius);
}

void SphereColliderComponent::OnLoad(JSON_Object* file)
{
	offset.x = json_object_get_number(file, "OffsetSX");
	offset.y = json_object_get_number(file, "OffsetSY");
	offset.z = json_object_get_number(file, "OffsetSZ");
	radius = json_object_get_number(file, "Radius");
}
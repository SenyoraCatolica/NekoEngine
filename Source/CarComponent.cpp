#include "CarComponent.h"
#include "GameObject.h"
#include "PhysicVehicle3D.h"

#include "imgui\imgui.h"
#include "imgui\imgui_internal.h"


CarComponent::CarComponent(GameObject* parent) :
	Component(parent, ComponentType::COMPONENT_BOX) {}

CarComponent::~CarComponent() 
{
	car = nullptr;
}

void CarComponent::SetCarData()
{
	car->info.mass = mass;
	car->info.suspensionStiffness = suspensionStiffness;
	car->info.suspensionCompression = suspensionCompression;
	car->info.suspensionDamping = suspensionDamping;
	car->info.maxSuspensionTravelCm = maxSuspensionTravelCm;
	car->info.frictionSlip = frictionSlip;
	car->info.maxSuspensionForce = maxSuspensionForce;

}

void CarComponent::OnUniqueEditor()
{
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);

	ImGui::SliderFloat("Mass", &mass, 0.0f, 1000.0f);
	ImGui::SliderFloat("Suspension Stiffness", &suspensionStiffness, 0.0f, 200.0f);
	ImGui::SliderFloat("Suspension Compression", &suspensionCompression, 0.0f, 1.0f);
	ImGui::SliderFloat("Suspension Damping", &suspensionDamping, 0.0f, 1.0f);
	ImGui::SliderFloat("Max Suspension Travel", &maxSuspensionTravelCm, 0.0f, 1000.0f);
	ImGui::SliderFloat("Friction Slip", &frictionSlip, 0.0f, 1000.0f);
	ImGui::SliderFloat("Max Suspension Force", &maxSuspensionForce, 0.0f, 100000.0f);
}

PhysicVehicle3D* CarComponent::GetVehicle()
{
	return car;
}

void CarComponent::SetVehicle(PhysicVehicle3D* vehicle)
{
	car = vehicle;
}

void CarComponent::OnInternalSave(JSON_Object* file)
{
	json_object_set_number(file, "Mass", mass);
	json_object_set_number(file, "SuspensionStifness", suspensionStiffness);
	json_object_set_number(file, "SuspensionCompresion", suspensionCompression);
	json_object_set_number(file, "SuspensionDamping", suspensionDamping);
	json_object_set_number(file, "MaxSuspensionTravvelCm", maxSuspensionTravelCm);
	json_object_set_number(file, "FrictionSlip", frictionSlip);
	json_object_set_number(file, "MaxsuspensionForce", maxSuspensionForce);

}

void CarComponent::OnLoad(JSON_Object* file)
{
	mass = json_object_get_number(file, "Mass");
	suspensionStiffness = json_object_get_number(file, "SuspensionStifness");
	suspensionCompression = json_object_get_number(file, "SuspensionCompresion");
	suspensionDamping = json_object_get_number(file, "SuspensionDamping");
	maxSuspensionTravelCm = json_object_get_number(file, "MaxSuspensionTravvelCm");
	frictionSlip = json_object_get_number(file, "FrictionSlip");
	maxSuspensionForce = json_object_get_number(file, "MaxsuspensionForce");
}
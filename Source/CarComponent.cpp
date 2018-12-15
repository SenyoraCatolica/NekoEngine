#include "CarComponent.h"
#include "GameObject.h"
#include "PhysicVehicle3D.h"

#include "imgui\imgui.h"
#include "imgui\imgui_internal.h"


CarComponent::CarComponent(GameObject* parent) :
	Component(parent, ComponentType::COMPONENT_BOX) {}

CarComponent::~CarComponent() 
{

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
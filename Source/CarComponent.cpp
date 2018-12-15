#include "CarComponent.h"
#include "GameObject.h"
#include "PhysicVehicle3D.h"
#include  "PrimitiveShape.h"
#include "ComponentTransform.h"

#include "imgui\imgui.h"
#include "imgui\imgui_internal.h"


CarComponent::CarComponent(GameObject* parent) :
	Component(parent, ComponentType::COMPONENT_BOX) 
{
	CreateCar();
}

CarComponent::~CarComponent() 
{
	car = nullptr;
}

void CarComponent::SetCarData()
{
	car->info.mass = car_info->mass;
	car->info.suspensionStiffness = car_info->suspensionStiffness;
	car->info.suspensionCompression = car_info->suspensionCompression;
	car->info.suspensionDamping = car_info->suspensionDamping;
	car->info.maxSuspensionTravelCm = car_info->maxSuspensionTravelCm;
	car->info.frictionSlip = car_info->frictionSlip;
	car->info.maxSuspensionForce = car_info->maxSuspensionForce;

	car->SetPos(parent->transform->position);
}

void CarComponent::CreateCar()
{
	car_info = new VehicleInfo();
	// Car properties ----------------------------------------
	car_info->chassis_size.Set(1.6, 0.2, 4);
	car_info->chassis_offset.Set(0, 1.2, 0);
	car_info->cabin_size.Set(1.5, 0.8, 1);
	car_info->cabin_offset.Set(0, 2, -1.5);
	car_info->front_size.Set(0.5, 0.3, 4.2);
	car_info->front_offset.Set(0, 1.6, 0);
	car_info->bar_size.Set(2.4, 0.3, 0.2);
	car_info->bar_offset.Set(0, 1.5, 2);

	car_info->mass = 500.0f;
	car_info->suspensionStiffness = 15.88f;
	car_info->suspensionCompression = 0.83f;
	car_info->suspensionDamping = 0.88f;
	car_info->maxSuspensionTravelCm = 1000.0f;
	car_info->frictionSlip = 10.5;
	car_info->maxSuspensionForce = 9800.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.5f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 0.2f;

	// Don't change anything below this line ------------------

	float half_width = car_info->chassis_size.x*0.5f;
	float half_length = car_info->chassis_size.z*0.5f;

	math::float3 direction(0, -1, 0);
	math:float3 axis(-1, 0, 0);

	car_info->num_wheels = 4;
	car_info->wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car_info->wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car_info->wheels[0].direction = direction;
	car_info->wheels[0].axis = axis;
	car_info->wheels[0].suspensionRestLength = suspensionRestLength;
	car_info->wheels[0].radius = wheel_radius;
	car_info->wheels[0].width = wheel_width;
	car_info->wheels[0].front = true;
	car_info->wheels[0].drive = true;
	car_info->wheels[0].brake = false;
	car_info->wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car_info->wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car_info->wheels[1].direction = direction;
	car_info->wheels[1].axis = axis;
	car_info->wheels[1].suspensionRestLength = suspensionRestLength;
	car_info->wheels[1].radius = wheel_radius;
	car_info->wheels[1].width = wheel_width;
	car_info->wheels[1].front = true;
	car_info->wheels[1].drive = true;
	car_info->wheels[1].brake = false;
	car_info->wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car_info->wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car_info->wheels[2].direction = direction;
	car_info->wheels[2].axis = axis;
	car_info->wheels[2].suspensionRestLength = suspensionRestLength;
	car_info->wheels[2].radius = wheel_radius;
	car_info->wheels[2].width = wheel_width;
	car_info->wheels[2].front = false;
	car_info->wheels[2].drive = false;
	car_info->wheels[2].brake = true;
	car_info->wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car_info->wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car_info->wheels[3].direction = direction;
	car_info->wheels[3].axis = axis;
	car_info->wheels[3].suspensionRestLength = suspensionRestLength;
	car_info->wheels[3].radius = wheel_radius;
	car_info->wheels[3].width = wheel_width;
	car_info->wheels[3].front = false;
	car_info->wheels[3].drive = false;
	car_info->wheels[3].brake = true;
	car_info->wheels[3].steering = false;
}


void CarComponent::OnUniqueEditor()
{
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);

	ImGui::SliderFloat("Mass", &car_info->mass, 0.0f, 1000.0f);
	ImGui::SliderFloat("Suspension Stiffness", &car_info->suspensionStiffness, 0.0f, 200.0f);
	ImGui::SliderFloat("Suspension Compression", &car_info->suspensionCompression, 0.0f, 1.0f);
	ImGui::SliderFloat("Suspension Damping", &car_info->suspensionDamping, 0.0f, 1.0f);
	ImGui::SliderFloat("Max Suspension Travel", &car_info->maxSuspensionTravelCm, 0.0f, 1000.0f);
	ImGui::SliderFloat("Friction Slip", &car_info->frictionSlip, 0.0f, 1000.0f);
	ImGui::SliderFloat("Max Suspension Force", &car_info->maxSuspensionForce, 0.0f, 100000.0f);
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
	json_object_set_number(file, "Mass", car_info->mass);
	json_object_set_number(file, "SuspensionStifness", car_info->suspensionStiffness);
	json_object_set_number(file, "SuspensionCompresion", car_info->suspensionCompression);
	json_object_set_number(file, "SuspensionDamping", car_info->suspensionDamping);
	json_object_set_number(file, "MaxSuspensionTravvelCm", car_info->maxSuspensionTravelCm);
	json_object_set_number(file, "FrictionSlip", car_info->frictionSlip);
	json_object_set_number(file, "MaxsuspensionForce", car_info->maxSuspensionForce);

}

void CarComponent::OnLoad(JSON_Object* file)
{
	car_info->mass = json_object_get_number(file, "Mass");
	car_info->suspensionStiffness = json_object_get_number(file, "SuspensionStifness");
	car_info->suspensionCompression = json_object_get_number(file, "SuspensionCompresion");
	car_info->suspensionDamping = json_object_get_number(file, "SuspensionDamping");
	car_info->maxSuspensionTravelCm = json_object_get_number(file, "MaxSuspensionTravvelCm");
	car_info->frictionSlip = json_object_get_number(file, "FrictionSlip");
	car_info->maxSuspensionForce = json_object_get_number(file, "MaxsuspensionForce");
}
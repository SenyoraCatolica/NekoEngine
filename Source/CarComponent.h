#pragma once

#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"

class PhysicVehicle3D;
struct VehicleInfo;

class CarComponent : public Component
{
public:

	CarComponent(GameObject* parent);
	~CarComponent();

	void SetCarData();
	void CreateCar();

	void OnUniqueEditor();

	PhysicVehicle3D* GetVehicle();
	void SetVehicle(PhysicVehicle3D* vehicle);

	virtual void OnInternalSave(JSON_Object* file);
	virtual void OnLoad(JSON_Object* file);

	VehicleInfo* car_info;

private:
	PhysicVehicle3D* car = nullptr;
};
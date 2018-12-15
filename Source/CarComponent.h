#pragma once

#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"

class PhysicVehicle3D;

class CarComponent : public Component
{
public:

	CarComponent(GameObject* parent);
	~CarComponent();

	void SetCarData();

	void OnUniqueEditor();

	PhysicVehicle3D* GetVehicle();
	void SetVehicle(PhysicVehicle3D* vehicle);

	virtual void OnInternalSave(JSON_Object* file);
	virtual void OnLoad(JSON_Object* file);

public:

	float mass;
	float suspensionStiffness; // default to 5.88 / 10.0 offroad / 50.0 sports car / 200.0 F1 car
	float suspensionCompression; // default to 0.83
	float suspensionDamping; // default to 0.88 / 0..1 0 bounces / 1 rigid / recommended 0.1...0.3
	float maxSuspensionTravelCm; // default to 500 cm suspension can be compressed
	float frictionSlip; // defaults to 10.5 / friction with the ground. 0.8 should be good but high values feels better (kart 1000.0)
	float maxSuspensionForce; // defaults to 6000 / max force to the chassis

private:
	PhysicVehicle3D* car = nullptr;
};
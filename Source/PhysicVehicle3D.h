#pragma once

#include "PhysicBody3D.h"
#include "PrimitiveShape.h"
#include "MathGeoLib/include/MathGeoLib.h"


class btRaycastVehicle;
class PhysBody3D;

struct Wheel
{
	math::float3 connection; // origin of the ray. Must come from within the chassis
	math::float3 direction;
	math::float3 axis;
	float suspensionRestLength; // max length for suspension in meters
	float radius;
	float width;
	bool front; // is front wheel ?
	bool drive; // does this wheel received engine power ?
	bool brake; // does breakes affect this wheel ?
	bool steering; // does this wheel turns ?
};

struct VehicleInfo
{
	~VehicleInfo();

	math::float3 chassis_size;
	math::float3 chassis_offset;

	math::float3 cabin_size;
	math::float3 cabin_offset;

	math::float3 front_size;
	math::float3 front_offset;

	math::float3 bar_size;
	math::float3 bar_offset;


	float mass;
	float suspensionStiffness; // default to 5.88 / 10.0 offroad / 50.0 sports car / 200.0 F1 car
	float suspensionCompression; // default to 0.83
	float suspensionDamping; // default to 0.88 / 0..1 0 bounces / 1 rigid / recommended 0.1...0.3
	float maxSuspensionTravelCm; // default to 500 cm suspension can be compressed
	float frictionSlip; // defaults to 10.5 / friction with the ground. 0.8 should be good but high values feels better (kart 1000.0)
	float maxSuspensionForce; // defaults to 6000 / max force to the chassis

	Wheel* wheels;
	int num_wheels;
};

class PhysicVehicle3D : public PhysicBody3D
{
public:
	PhysicVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info);
	~PhysicVehicle3D();

	void Render();
	void ApplyEngineForce(float force);
	void Brake(float force);
	void Turn(float degrees);
	float GetKmh() const;
	math::float3 GetBottomVector() const;

public:

	VehicleInfo info;
	btRaycastVehicle* vehicle;
};
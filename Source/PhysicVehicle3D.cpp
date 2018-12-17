#include "PhysicVehicle3D.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo(){}

// ----------------------------------------------------------------------------
PhysicVehicle3D::PhysicVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) 
	: PhysicBody3D(body), vehicle(vehicle), info(info){}

// ----------------------------------------------------------------------------
PhysicVehicle3D::~PhysicVehicle3D()
{
	delete vehicle;
}


// ----------------------------------------------------------------------------
void PhysicVehicle3D::Render()
{
	PrimitiveShapeCylinder wheel;

	wheel.color = Red;

	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		wheel.radius = info.wheels[0].radius;
		wheel.height = info.wheels[0].width;

		vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(*wheel.transform.v);

		wheel.wire = true;
		wheel.Render();
	}

	PrimitiveShapeCube chassis(info.chassis_size.x, info.chassis_size.y, info.chassis_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(*chassis.transform.v);
	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	chassis.transform.At(0, 3) += offset.getX();
	chassis.transform.At(1, 3) += offset.getY();
	chassis.transform.At(2, 3) += offset.getZ();

	chassis.wire = true;
	chassis.Render();


	PrimitiveShapeCube cabin(info.cabin_size.x, info.cabin_size.y, info.cabin_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(*cabin.transform.v);
	btQuaternion q2 = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset2(info.cabin_offset.x, info.cabin_offset.y, info.cabin_offset.z);
	offset2 = offset2.rotate(q2.getAxis(), q2.getAngle());

	cabin.transform.At(0, 3) += offset2.getX();
	cabin.transform.At(1, 3) += offset2.getY();
	cabin.transform.At(2, 3) += offset2.getZ();

	cabin.wire = true;
	cabin.Render();


	PrimitiveShapeCube front(info.front_size.x, info.front_size.y, info.front_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(*front.transform.v);
	btQuaternion q3 = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset3(info.front_offset.x, info.front_offset.y, info.front_offset.z);
	offset3 = offset3.rotate(q3.getAxis(), q3.getAngle());

	front.transform.At(0, 3) += offset3.getX();
	front.transform.At(1, 3) += offset3.getY();
	front.transform.At(2, 3) += offset3.getZ();

	front.wire = true;
	front.Render();


	PrimitiveShapeCube bar(info.bar_size.x, info.bar_size.y, info.bar_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(*bar.transform.v);
	btQuaternion q4 = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset4(info.bar_offset.x, info.bar_offset.y, info.bar_offset.z);
	offset4 = offset4.rotate(q4.getAxis(), q4.getAngle());

	bar.transform.At(0, 3) += offset4.getX();
	bar.transform.At(1, 3) += offset4.getY();
	bar.transform.At(2, 3) += offset4.getZ();

	bar.wire = true;
	bar.Render();
}

// ----------------------------------------------------------------------------
void PhysicVehicle3D::ApplyEngineForce(float force)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysicVehicle3D::Brake(float force)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysicVehicle3D::Turn(float degrees)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysicVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}

math::float3 PhysicVehicle3D::GetBottomVector() const
{
	btVector3 h = vehicle->getForwardVector();
	math::float3 ret;
	ret.Set(h.getX(), h.getY(), h.getZ());
	return ret;
}

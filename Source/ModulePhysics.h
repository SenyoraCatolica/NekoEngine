#pragma once

#include "Module.h"
#include "Globals.h"
#include "PrimitiveShape.h"
#include "Bullet\include\btBulletDynamicsCommon.h"
#include <list>
#include <map>

#define GRAVITY btVector3(0.0f, -10.0f, 0.0f) 

class GameObject;
class PhysicBody3D;
class RigidBody3DComponent;
class BoxColliderComponent;
class DebugPhysicsDrawer;

class ComponentCamera;

class ModulePhysics : public Module
{
public:
	ModulePhysics();
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();

	PhysicBody3D* AddBody(const PrimitiveShapeSphere& sphere, float mass = 1.0f);
	PhysicBody3D* AddBody(const PrimitiveShapeCube& cube, float mass = 1.0f);
	PhysicBody3D* AddBody(RigidBody3DComponent* rb, BoxColliderComponent* col, GameObject* go);

	void UpdateBodies();
	void ClearBodies();

	void SetMainCamera(ComponentCamera* cam);

private:

	btDefaultCollisionConfiguration * collision_conf = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broad_phase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld* world = nullptr;
	DebugPhysicsDrawer* debug_draw = nullptr;


	std::list<btCollisionShape*> shapes;
	std::list<PhysicBody3D*> bodies;
	std::list<btDefaultMotionState*> motions;
	std::list<btTypedConstraint*> constraints;
	std::map<GameObject*, PhysicBody3D*> body_gos;

	ComponentCamera* gameCamera = nullptr;

};


//=========================================================================================================
//small class to handle the debug drawing of the bullet physics

class DebugPhysicsDrawer : public btIDebugDraw
{
public:
	DebugPhysicsDrawer() : line(0, 0, 0)
	{}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);

	int	 getDebugMode() const;
	void setDebugMode(int debugMode);

	DebugDrawModes mode;
	PrimitiveShapeLine line;
	PrimitiveShape point;
};
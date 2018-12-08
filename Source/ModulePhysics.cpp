#include "ModulePhysics.h"
#include "PhysicBody3D.h"
#include "RigidBody3DComponent.h"
#include "BoxColliderComponent.h"

#ifdef _DEBUG
#pragma comment (lib, "Bullet/libx86/BulletDynamics_debug.lib")
#pragma comment (lib, "Bullet/libx86/BulletCollision_debug.lib")
#pragma comment (lib, "Bullet/libx86/LinearMath_debug.lib")
#else
#pragma comment (lib, "Bullet/libx86/BulletDynamics.lib")
#pragma comment (lib, "Bullet/libx86/BulletCollision.lib")
#pragma comment (lib, "Bullet/libx86/LinearMath.lib")
#endif


ModulePhysics::ModulePhysics()
{
	name = "Physics";

	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	debug_draw = new DebugPhysicsDrawer();
}

ModulePhysics::~ModulePhysics()
{
	delete solver;
	delete broad_phase;
	delete dispatcher;
	delete collision_conf;
}

bool ModulePhysics::Start()
{
	CONSOLE_LOG("Creating Physics environment");

	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
	world->setDebugDrawer(debug_draw);
	world->setGravity(GRAVITY);

	// Big plane as ground
	{
		btCollisionShape* colShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

		btDefaultMotionState* myMotionState = new btDefaultMotionState();
		btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, colShape);

		btRigidBody* body = new btRigidBody(rbInfo);
		world->addRigidBody(body);
	}

	debug_draw->setDebugMode(1); //TODO this should be set in a window

	return true;
}

bool ModulePhysics::PreUpdate(float dt)
{
	return true;
}

bool ModulePhysics::Update(float dt)
{
	return true;
}

bool ModulePhysics::PostUpdate(float dt)
{
	return true;
}

bool ModulePhysics::CleanUp()
{
	CONSOLE_LOG("Destroying 3D Physics simulation");

	// Remove from the world all collision bodies
	for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}

	for (std::list<btTypedConstraint*>::iterator item = constraints.begin(); item != constraints.end(); item++)
	{
		world->removeConstraint((*item));
		delete *item;
	}

	constraints.clear();

	for (std::list<btDefaultMotionState*>::iterator item = motions.begin(); item != motions.end(); item++)
		delete *item;

	motions.clear();

	for (std::list<btCollisionShape*>::iterator item = shapes.begin(); item != shapes.end(); item++)
		delete *item;

	shapes.clear();

	for (std::list<PhysicBody3D*>::iterator item = bodies.begin(); item != bodies.end(); item++)
		delete *item;

	bodies.clear();
	delete world;

	return true;
}

PhysicBody3D* ModulePhysics::AddBody(const PrimitiveSphere& sphere, float mass)
{
	btCollisionShape* colShape = new btSphereShape(sphere.radius);
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(*sphere.localTransform.v);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysicBody3D* pbody = new PhysicBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.push_back(pbody);

	return pbody;
}
/*
PhysicBody3D* ModulePhysics::Addbody(RigidBody3DComponent* rb, Sphere& sphere)
{
btCollisionShape* colShape = new btSphereShape(sphere.r);
shapes.push_back(colShape);

btTransform startTransform;
startTransform.setFromOpenGLMatrix(*sphere.Transform.);

btVector3 localInertia(0, 0, 0);

if (rb->is_kinematic)
rb->mass = 0.0f;

colShape->calculateLocalInertia(rb->mass, localInertia);


btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
motions.push_back(myMotionState);
btRigidBody::btRigidBodyConstructionInfo rbInfo(rb->mass, myMotionState, colShape, localInertia);

btRigidBody* body = new btRigidBody(rbInfo);
PhysicBody3D* pbody = new PhysicBody3D(body);

body->setUserPointer(pbody);
world->addRigidBody(body);
bodies.push_back(pbody);

return pbody;
}
*/

PhysicBody3D* ModulePhysics::AddBody(const PrimitiveCube& cube, float mass)
{
	btCollisionShape* colShape = new btBoxShape(btVector3(cube.size.x*0.5f, cube.size.y*0.5f, cube.size.z*0.5f));
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(*cube.localTransform.v);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysicBody3D* pbody = new PhysicBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.push_back(pbody);

	return pbody;
}

/*PhysicBody3D* ModulePhysics::Addbody(RigidBody3DComponent* rb, OBB& box)
{
btCollisionShape* colShape = new btBoxShape(btVector3(box.Size().x * 0.5, box.Size().y * 0.5, box.Size().x * 0.5));
shapes.push_back(colShape);

btTransform startTransform;
startTransform.setFromOpenGLMatrix(*box.Transform.v);

btVector3 localInertia(0, 0, 0);

if (rb->is_kinematic)
rb->mass = 0.0f;

colShape->calculateLocalInertia(rb->mass, localInertia);


btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
motions.push_back(myMotionState);
btRigidBody::btRigidBodyConstructionInfo rbInfo(rb->mass, myMotionState, colShape, localInertia);

btRigidBody* body = new btRigidBody(rbInfo);
PhysicBody3D* pbody = new PhysicBody3D(body);

body->setUserPointer(pbody);
world->addRigidBody(body);
bodies.push_back(pbody);

return pbody;
}*/

PhysicBody3D* ModulePhysics::AddBody(RigidBody3DComponent* rb, BoxColliderComponent* col)
{
	if (rb == nullptr && col == nullptr)
		return nullptr;

	math::OBB box;

	if (col != nullptr)
		box = col->box;

	else
		box.SetNegativeInfinity();

	btCollisionShape* colShape = new btBoxShape(btVector3(0.0f, 0.0f, 0.0f));
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(*math::float4x4::identity.v);


	btVector3 localInertia(0, 0, 0);

	float mass = 0.0f;

	if (rb != nullptr)
		if (rb->is_kinematic == false)
			mass = rb->mass;

	if (mass > 0)
		colShape->calculateLocalInertia(mass, localInertia);


	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysicBody3D* pbody = new PhysicBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.push_back(pbody);

	return pbody;
}



//Small class to handle the debug draw of the physics
// =============================================
void DebugPhysicsDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	line.origin.Set(from.getX(), from.getY(), from.getZ());
	line.destination.Set(to.getX(), to.getY(), to.getZ());
	line.SetColor(Color(color.getX(), color.getY(), color.getZ()));
	line.Render();
}

void DebugPhysicsDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	//point.(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
	line.SetColor(Color(color.getX(), color.getY(), color.getZ()));
	point.Render();
}

void DebugPhysicsDrawer::reportErrorWarning(const char* warningString)
{
	CONSOLE_LOG("Bullet warning: %s", warningString);
}

void DebugPhysicsDrawer::draw3dText(const btVector3& location, const char* textString)
{
	CONSOLE_LOG("Bullet draw text: %s", textString);
}

void DebugPhysicsDrawer::setDebugMode(int debugMode)
{
	mode = (DebugDrawModes)debugMode;
}

int	 DebugPhysicsDrawer::getDebugMode() const
{
	return mode;
}

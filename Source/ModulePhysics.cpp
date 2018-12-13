#include "ModulePhysics.h"
#include "PhysicBody3D.h"
#include "RigidBody3DComponent.h"
#include "BoxColliderComponent.h"
#include "Primitive.h"

#include "Application.h"
#include "ModuleTimeManager.h"
#include "ModuleInput.h"
#include "Globals.h"
#include "GameObject.h"
#include "ComponentTransform.h"


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

update_status ModulePhysics::PreUpdate()
{
	if (App->GetEngineState() != engine_states::ENGINE_PLAY)
		return UPDATE_CONTINUE;

	float dt = App->timeManager->GetDt();
	world->stepSimulation( dt, 15);

	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		int numContacts = contactManifold->getNumContacts();
		if (numContacts > 0)
		{
			PhysicBody3D* pbodyA = (PhysicBody3D*)obA->getUserPointer();
			PhysicBody3D* pbodyB = (PhysicBody3D*)obB->getUserPointer();

			if (pbodyA && pbodyB)
			{
				std::list<Module*>::iterator it = pbodyA->collision_listeners.begin();
				while (it != pbodyA->collision_listeners.end())
				{
					(*it)->OnCollision(pbodyA, pbodyB);
					it++;
				}

				it = pbodyB->collision_listeners.begin();
				while (it != pbodyB->collision_listeners.end())
				{
					(*it)->OnCollision(pbodyB, pbodyA);
					it++;
				}
			}
		}
	}

	return UPDATE_CONTINUE;
}

update_status ModulePhysics::Update()
{
	if (App->IsPlay() == false)
		return UPDATE_CONTINUE;

	//2do render vehicle
	world->debugDrawWorld();

	//Update PhysicBodies with their gos
	UpdateBodies();

	//2do implement throwing balls
	/*if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		PrimitiveSphere s(1);
		s.SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
		float force = 30.0f;
		AddBody(s)->Push(-(App->camera->Z.x * force), -(App->camera->Z.y * force), -(App->camera->Z.z * force));
	}*/

	return UPDATE_CONTINUE;
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

PhysicBody3D* ModulePhysics::AddBody(const PrimitiveShapeSphere& sphere, float mass)
{
	btCollisionShape* colShape = new btSphereShape(sphere.radius);
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(*sphere.transform.v);

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

PhysicBody3D* ModulePhysics::AddBody(const PrimitiveShapeCube& cube, float mass)
{
	btCollisionShape* colShape = new btBoxShape(btVector3(cube.size.x*0.5f, cube.size.y*0.5f, cube.size.z*0.5f));
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(*cube.transform.v);

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

PhysicBody3D* ModulePhysics::AddBody(RigidBody3DComponent* rb, BoxColliderComponent* col, GameObject* go)
{
	PrimitiveShapeCube* box;
	GameObject* parent;

	if (col != nullptr)
	{
		box = col->GetBoxCollider();
		parent = col->GetParent();
	}

	else
		box = new PrimitiveShapeCube(1, 1, 1);

	btCollisionShape* colShape = new btBoxShape(btVector3(box->size.x*0.5f, box->size.y*0.5f, box->size.z*0.5f));
	shapes.push_back(colShape);

	if (parent == nullptr)
		parent = rb->GetParent();

	math::float4x4 matrix;
	if (col != nullptr)
		matrix = col->GetBoxCollider()->transform;
	else
		matrix = parent->transform->GetGlobalMatrix();

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(*matrix.v);


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

	body_gos.insert(std::pair<GameObject*, PhysicBody3D*>(go, pbody));

	return pbody;
}


void ModulePhysics::UpdateBodies()
{
	std::map<GameObject*, PhysicBody3D*>::iterator it = body_gos.begin();
	while (it != body_gos.end())
	{
		BoxColliderComponent* box = (BoxColliderComponent*)(*it).first->GetComponent(ComponentType::COMPONENT_BOX);

		if (App->IsPlay())
		{
			//update box on play
			if (box->GetBoxCollider())
			{
				//Get the transform data from go transform
				math::float3 trans; math::Quat rot; math::float3 scale;
				(*it).second->GetTransform().Transposed().Decompose(trans, rot, scale);

				//Add offset
				math::float3 real_offset = rot.Transform(box->offset);

				//Set the new transform
				box->GetBoxCollider()->SetPos(trans.x, trans.y, trans.z);
				box->GetBoxCollider()->SetRotation(rot.Inverted());

				math::float4x4 matrix = math::float4x4::FromTRS(trans + real_offset, rot, (*it).first->transform->scale);
				(*it).first->transform->SetTransform(matrix);

				(*it).first->transform->UpdateOnTransformChanged();

				box->GetBoxCollider()->Render();
			}
		}

		it++;
	}
}

void ModulePhysics::ClearBodies()
{
	//erase body + go map
	std::map<GameObject*, PhysicBody3D*>::iterator it = body_gos.begin();
	body_gos.erase(it, body_gos.end());
	body_gos.clear();

	//erase body list
	std::list<PhysicBody3D*>::iterator itl = bodies.begin();
	while (itl != bodies.end())
	{
		world->removeRigidBody((*itl)->body);
		itl++;
	}
	bodies.clear();

	//erase shapes list
	/*std::list<btCollisionShape*>::iterator it_shapes = shapes.begin();
	while (it_shapes != shapes.end())
	{
		shapes.remove(*it_shapes);
		it_shapes++;
	}*/
	shapes.clear();

	//erase motions list
	std::list<btDefaultMotionState*>::iterator it_mot = motions.begin();
	/*while (it_mot != motions.end())
	{
		motions.remove(*it_mot);
		it_mot++;
	}*/
	motions.clear();

	//erase constraints list
	std::list<btTypedConstraint*>::iterator it_cons = constraints.begin();
	while (it_cons != constraints.end())
	{
		world->removeConstraint(*it_cons);
		it_cons++;
	}
	constraints.clear();

	//2DO remove vehicles
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

#include "ModulePhysics.h"
#include "PhysicBody3D.h"
#include "PhysicVehicle3D.h"
#include "Primitive.h"
#include "SDL\include\SDL.h"


#include "Application.h"
#include "ModuleTimeManager.h"
#include "ModuleInput.h"
#include "Globals.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "RigidBody3DComponent.h"
#include "BoxColliderComponent.h"
#include "JointP2PComponent.h"
#include "CarComponent.h"


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
	delete vehicle_raycaster;
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
	vehicle_raycaster = new btDefaultVehicleRaycaster(world);

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

	if(draw_physics)
		world->debugDrawWorld();

	turn = acceleration = brake = 0.0f;
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		acceleration = MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (turn < TURN_DEGREES)
			turn += TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
	}


	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		acceleration = -MAX_ACCELERATION;
	}


	//Update PhysicBodies with their gos
	UpdateBodies();

	//throwing balls and camera sphere
	if (gameCamera != nullptr)
	{
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			PrimitiveShapeSphere s(1);
			math::float3 pos = gameCamera->GetParent()->transform->position;
			s.SetPos(pos.x, pos.y, pos.z + 5);
			s.wire = true;
			s.Render();

			float force = 30.0f;
			math::float3 direction = gameCamera->GetParent()->transform->GetGlobalMatrix().WorldZ();
			PhysicBody3D* sphereBody = AddBody(s); 
			sphereBody->Push((direction.x * force), (direction.y * force), (direction.z * force));
		}

		//update camera shere
		cameraBody->SetPos(gameCamera->GetParent()->transform->position);
	}

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

PhysicBody3D* ModulePhysics::AddBody(RigidBody3DComponent* rb, BoxColliderComponent* col, GameObject* go, bool is_constraint)
{
	PrimitiveShapeCube* box;

	if (col != nullptr && col->IsActive())
	{
		box = col->GetBoxCollider();
	}

	else
		box = new PrimitiveShapeCube(1, 1, 1);

	btCollisionShape* colShape = new btBoxShape(btVector3(box->size.x*0.5f, box->size.y*0.5f, box->size.z*0.5f));
	shapes.push_back(colShape);



	math::float4x4 matrix;
	if (col != nullptr && col->IsActive())
		matrix = col->GetBoxCollider()->transform;
	else
		matrix = go->transform->GetGlobalMatrix();

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(*matrix.v);


	btVector3 localInertia(0, 0, 0);

	float mass = 0.0f;

	if (rb != nullptr)
		if (rb->is_kinematic == false && go->IsStatic() == false)
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

	if(is_constraint == false)
		body_gos.insert(std::pair<GameObject*, PhysicBody3D*>(go, pbody));

	return pbody;
}

PhysicVehicle3D* ModulePhysics::AddVehicle(CarComponent* car)
{
	btCompoundShape* comShape = new btCompoundShape();
	shapes.push_back(comShape);

	const VehicleInfo info = *car->car_info;

	btCollisionShape* colShape1 = new btBoxShape(btVector3(info.chassis_size.x*0.5f, info.chassis_size.y*0.5f, info.chassis_size.z*0.5f));
	shapes.push_back(colShape1);

	btCollisionShape* colShape2 = new btBoxShape(btVector3(info.cabin_size.x*0.5f, info.cabin_size.y*0.5f, info.cabin_size.z*0.5f));
	shapes.push_back(colShape2);

	btCollisionShape* colShape3 = new btBoxShape(btVector3(info.front_size.x*0.5f, info.front_size.y*0.5f, info.front_size.z*0.5f));
	shapes.push_back(colShape3);

	btCollisionShape* colShape4 = new btBoxShape(btVector3(info.bar_size.x*0.5f, info.bar_size.y*0.5f, info.bar_size.z*0.5f));
	shapes.push_back(colShape4);

	btTransform trans1;
	trans1.setIdentity();
	trans1.setOrigin(btVector3(info.chassis_offset.x, info.chassis_offset.y - 0, info.chassis_offset.z));

	btTransform trans2;
	trans2.setIdentity();
	trans2.setOrigin(btVector3(info.cabin_offset.x, info.cabin_offset.y, info.cabin_offset.z));

	btTransform trans3;
	trans3.setIdentity();
	trans3.setOrigin(btVector3(info.front_offset.x, info.front_offset.y, info.front_offset.z));

	btTransform trans4;
	trans4.setIdentity();
	trans4.setOrigin(btVector3(info.bar_offset.x, info.bar_offset.y, info.bar_offset.z));

	comShape->addChildShape(trans1, colShape1);
	comShape->addChildShape(trans2, colShape2);
	comShape->addChildShape(trans3, colShape3);
	comShape->addChildShape(trans4, colShape4);

	btTransform startTransform;
	startTransform.setIdentity();

	btVector3 localInertia(0, 0, 0);
	comShape->calculateLocalInertia(info.mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(info.mass, myMotionState, comShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	body->setContactProcessingThreshold(BT_LARGE_FLOAT);
	body->setActivationState(DISABLE_DEACTIVATION);

	world->addRigidBody(body);

	btRaycastVehicle::btVehicleTuning tuning;
	tuning.m_frictionSlip = info.frictionSlip;
	tuning.m_maxSuspensionForce = info.maxSuspensionForce;
	tuning.m_maxSuspensionTravelCm = info.maxSuspensionTravelCm;
	tuning.m_suspensionCompression = info.suspensionCompression;
	tuning.m_suspensionDamping = info.suspensionDamping;
	tuning.m_suspensionStiffness = info.suspensionStiffness;

	btRaycastVehicle* vehicle = new btRaycastVehicle(tuning, body, vehicle_raycaster);

	vehicle->setCoordinateSystem(0, 1, 2);

	for (int i = 0; i < info.num_wheels; ++i)
	{
		btVector3 conn(info.wheels[i].connection.x, info.wheels[i].connection.y, info.wheels[i].connection.z);
		btVector3 dir(info.wheels[i].direction.x, info.wheels[i].direction.y, info.wheels[i].direction.z);
		btVector3 axis(info.wheels[i].axis.x, info.wheels[i].axis.y, info.wheels[i].axis.z);

		vehicle->addWheel(conn, dir, axis, info.wheels[i].suspensionRestLength, info.wheels[i].radius, tuning, info.wheels[i].front);
	}
	// ---------------------

	PhysicVehicle3D* pvehicle = new PhysicVehicle3D(body, vehicle, info);
	world->addVehicle(vehicle);
	vehicles.push_back(pvehicle);

	return pvehicle;
}


void ModulePhysics::AddConstraintP2P(JointP2PComponent* jointA, JointP2PComponent* jointB)
{
	if (jointA == nullptr || jointB == nullptr)
		return;

	btTypedConstraint* p2p = new btPoint2PointConstraint(
		*(jointA->body->body),
		*(jointB->body->body),
		btVector3(jointA->anchor.x, jointA->anchor.y, jointA->anchor.z),
		btVector3(jointB->anchor.x, jointB->anchor.y, jointB->anchor.z));

	world->addConstraint(p2p);
	constraints.push_back(p2p);
	body_gos.insert(std::pair<GameObject*, PhysicBody3D*>(jointA->GetParent(), jointA->body));
	body_gos.insert(std::pair<GameObject*, PhysicBody3D*>(jointB->GetParent(), jointB->body));
	p2p->setDbgDrawSize(2.0f);
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

	std::list<PhysicVehicle3D*>::iterator itv = vehicles.begin();
	while (itv != vehicles.end())
	{
		(*itv)->ApplyEngineForce(acceleration);
		(*itv)->Turn(turn);
		(*itv)->Brake(brake);

		(*itv)->Render();
		itv++;
	}
}

void ModulePhysics::ClearBodies()
{

	/*for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}*/

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
	std::list<btCollisionShape*>::iterator it_shapes = shapes.begin();
	while (it_shapes != shapes.end())
	{
		delete (*it_shapes);
		it_shapes++;
	}
	shapes.clear();

	//erase motions list
	std::list<btDefaultMotionState*>::iterator it_mot = motions.begin();
	while (it_mot != motions.end())
	{
		delete (*it_mot);
		it_mot++;
	}
	motions.clear();

	//erase constraints list
	std::list<btTypedConstraint*>::iterator it_cons = constraints.begin();
	while (it_cons != constraints.end())
	{
		world->removeConstraint(*it_cons);
		delete (*it_cons);
		it_cons++;
	}
	constraints.clear();

	//erase constraints map
	std::map<JointP2PComponent*, JointP2PComponent*>::iterator it_consp = constraints_pair.begin();
	while (it_consp != constraints_pair.end())
	{
		it_consp->first->is_paired = false;
		it_consp->second->is_paired = false;
		it_consp++;
	}
	constraints_pair.clear();

	//2DO remove vehicles
	std::list<PhysicVehicle3D*>::iterator it_v = vehicles.begin();
	while (it_v != vehicles.end())
	{
		world->removeRigidBody((*it_v)->body);
		world->removeVehicle((*it_v)->vehicle);
		delete (*it_v);
		it_v++;
	}
	vehicles.clear();

	gameCamera = nullptr;
	cameraBody = nullptr;
}
void ModulePhysics::AddConstraint(JointP2PComponent* jointA, JointP2PComponent* jointB)
{
	if (jointA != nullptr && jointB != nullptr)
	{
		constraints_pair.insert(std::pair<JointP2PComponent*, JointP2PComponent*>(jointA, jointB));
	}
}

void ModulePhysics::AddBodiestoConstraints()
{
	std::map<JointP2PComponent*, JointP2PComponent*>::iterator it = constraints_pair.begin();
	while (it != constraints_pair.end())
	{
		(*it).first->body = AddBody((*it).first->GetParent()->rb, (*it).first->GetParent()->box_collider, (*it).first->GetParent(), true);
		(*it).second->body = AddBody((*it).second->GetParent()->rb, (*it).second->GetParent()->box_collider, (*it).second->GetParent(), true);

		AddConstraintP2P((*it).first, (*it).second);
		it++;
	}
}


void ModulePhysics::SetMainCamera(ComponentCamera* cam)
{
	gameCamera = cam;
}

void ModulePhysics::CreateCameraSphare()
{
	if (gameCamera != nullptr)
	{
		PrimitiveShapeSphere s(1);
		math::float3 pos = gameCamera->frustum.pos;
		s.SetPos(pos.x, pos.y, pos.z);
		cameraBody = AddBody(s, 500);
	}
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

#include "GameObject.h"

#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "BoxColliderComponent.h"
#include "RigidBody3DComponent.h"
#include "ResourceMesh.h"
#include "ModuleResourceManager.h"

#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleGOs.h"
#include "ModuleScene.h"

#include "MathGeoLib\include\Geometry\OBB.h"

#include <algorithm>

GameObject::GameObject(const char* name, GameObject* parent, bool disableTransform) : parent(parent)
{
	this->name = new char[DEFAULT_BUF_SIZE];
	strcpy_s(this->name, DEFAULT_BUF_SIZE, name);

	if (parent != nullptr)
	{
		parent->AddChild(this);

		if (!disableTransform)
			AddComponent(ComponentType::Transform_Component);
	}

	boundingBox.SetNegativeInfinity();

	UUID = App->GenerateRandomNumber();
}

GameObject::GameObject(const GameObject& gameObject)
{
	name = new char[DEFAULT_BUF_SIZE];
	strcpy_s(name, DEFAULT_BUF_SIZE, gameObject.name);

	transform = new ComponentTransform(*gameObject.transform);
	transform->SetParent(this);
	components.push_back(transform);

	if (gameObject.materialRenderer != nullptr)
	{
		materialRenderer = new ComponentMaterial(*gameObject.materialRenderer);
		materialRenderer->SetParent(this);
		components.push_back(materialRenderer);
	}

	if (gameObject.meshRenderer != nullptr)
	{
		meshRenderer = new ComponentMesh(*gameObject.meshRenderer);
		meshRenderer->SetParent(this);
		components.push_back(meshRenderer);
	}

	if (gameObject.camera != nullptr)
	{
		camera = new ComponentCamera(*gameObject.camera);
		camera->SetParent(this);
		components.push_back(camera);
	}

	boundingBox = gameObject.boundingBox;

	isActive = gameObject.isActive;
	isStatic = gameObject.isStatic;
	seenLastFrame = gameObject.seenLastFrame;

	UUID = gameObject.UUID;
	parentUUID = gameObject.parent->UUID;
}

void GameObject::Activate() const
{
	if (meshRenderer != nullptr)
		App->renderer3D->AddMeshComponent(meshRenderer);

	if (camera != nullptr)
		App->renderer3D->AddCameraComponent(camera);

	if (box_collider != nullptr)
		App->renderer3D->AddBoxColliderComponent(box_collider);
}

GameObject::~GameObject()
{
	RELEASE_ARRAY(name);
	InternallyDeleteComponents();

	if (isStatic)
	{
		// Static game object deleted: recreate quadtree
		System_Event newEvent;
		newEvent.type = System_Event_Type::RecreateQuadtree;
		App->PushSystemEvent(newEvent);
	}
}

void GameObject::Update() {}

void GameObject::OnSystemEvent(System_Event event)
{
	switch (event.type)
	{
	case System_Event_Type::RecalculateBBoxes:
		RecursiveRecalculateBoundingBoxes();
		break;
	}
}

void GameObject::SetParent(GameObject* parent)
{
	this->parent = parent;
}

GameObject* GameObject::GetParent() const
{
	return parent;
}

uint GameObject::GetParentUUID() const
{
	return parentUUID;
}

void GameObject::DestroyChildren()
{
	for (uint i = 0; i < children.size(); ++i)
	{
		children[i]->DestroyChildren();
		RELEASE(children[i]);
	}
}

void GameObject::DeleteMe()
{
	App->GOs->SetToDelete(this);

	DeleteChildren();
}

void GameObject::AddChild(GameObject* children)
{
	this->children.push_back(children);
}

void GameObject::EraseChild(GameObject* child)
{
	children.erase(std::remove(children.begin(), children.end(), child), children.end());
}

void GameObject::DeleteChild(uint index)
{
	children[index]->DeleteMe();
	children.erase(children.begin() + index);
}

void GameObject::DeleteChildren()
{
	for (uint i = 0; i < children.size(); ++i)
		children[i]->DeleteMe();

	children.clear();
}

bool GameObject::HasChildren() const
{
	return children.size() > 0;
}

uint GameObject::GetChildrenLength() const
{
	return children.size();
}

GameObject* GameObject::GetChild(uint index) const
{
	return children[index];
}

Component* GameObject::AddComponent(ComponentType type)
{
	Component* newComponent;

	switch (type)
	{
	case No_type:
		break;
	case Transform_Component:
		newComponent = transform = new ComponentTransform(this);
		break;
	case Mesh_Component:
		newComponent = meshRenderer = App->renderer3D->CreateMeshComponent(this);
		break;
	case Material_Component:
		newComponent = materialRenderer = new ComponentMaterial(this);
		break;
	case Camera_Component:
		newComponent = camera = App->renderer3D->CreateCameraComponent(this);
		break;
	case COMPONENT_BOX:
		newComponent = box_collider = App->renderer3D->CreateBoxColliderComponent(this);
		break;

	case COMPONENT_RB:
		newComponent = rb = new RigidBody3DComponent(this);;
		break;
	default:
		break;
	}
	
	components.push_back(newComponent);

	return newComponent;
}

void GameObject::MarkToDeleteComponent(uint index)
{
	App->GOs->SetComponentToDelete(components[index]);
}

void GameObject::MarkToDeleteComponentByValue(Component* component)
{
	App->GOs->SetComponentToDelete(component);
}

void GameObject::MarkToDeleteAllComponents()
{
	for (uint i = 0; i < components.size(); ++i)
		App->GOs->SetComponentToDelete(components[i]);
}

void GameObject::InternallyDeleteComponent(Component* toDelete)
{
	switch (toDelete->GetType())
	{
	case ComponentType::Mesh_Component:
		App->renderer3D->EraseMeshComponent((ComponentMesh*)toDelete);
		meshRenderer = nullptr;
		break;
	case ComponentType::Camera_Component:
		App->renderer3D->EraseCameraComponent((ComponentCamera*)toDelete);
		materialRenderer = nullptr;
		break;
	}

	components.erase(std::remove(components.begin(), components.end(), toDelete), components.end());
	RELEASE(toDelete);
}

void GameObject::InternallyDeleteComponents()
{
	for (int i = components.size() - 1; i >= 0; --i)
	{   
		switch (components[i]->GetType())
		{
		case ComponentType::Mesh_Component:
			App->renderer3D->EraseMeshComponent((ComponentMesh*)components[i]);
			break;
		case ComponentType::Camera_Component:
			App->renderer3D->EraseCameraComponent((ComponentCamera*)components[i]);
			break;
		}		

		RELEASE(components[i]);
	}

	components.clear();
}

bool GameObject::HasComponents() const
{
	return components.size() > 0;
}

uint GameObject::GetComponenetsLength() const
{
	return components.size();
}

Component* GameObject::GetComponent(uint index) const
{
	return components[index];
}

Component* GameObject::GetComponent(ComponentType type)
{
	std::vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if ((*it)->GetType() == type)
			return (*it);
		it++;
	}

	return nullptr;
}

// Get the index of the component from the gameobject's components vector. If the component cannot be found, returns -1
int GameObject::GetComponentIndexOnComponents(Component* component) const
{
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i] == component)
			return i;
	}

	return -1;
}

void GameObject::SwapComponents(Component* firstComponent, Component* secondComponent)
{
	std::swap(components[GetComponentIndexOnComponents(firstComponent)], components[GetComponentIndexOnComponents(secondComponent)]);
}

void GameObject::ReorderComponents(Component* source, Component* target)
{
	int index = GetComponentIndexOnComponents(target);
	components.erase(std::remove(components.begin(), components.end(), source), components.end());
	components.insert(components.begin() + index, source);
}

bool GameObject::EqualsToChildrenOrMe(const void* isEqual) const
{
	bool ret = false;

	if (isEqual == this)
		ret = true;

	for (int i = 0; i < children.size() && !ret; ++i)
		ret = children[i]->EqualsToChildrenOrMe(isEqual);

	return ret;
}

void GameObject::SetName(const char* name)
{
	strcpy_s(this->name, DEFAULT_BUF_SIZE, name);
}

const char* GameObject::GetName() const
{
	return name;
}

uint GameObject::GetUUID() const
{
	return UUID;
}

void GameObject::ForceUUID(uint uuid)
{
	UUID = uuid;
}

void GameObject::ToggleIsActive()
{
	isActive = !isActive;
}

bool GameObject::IsActive() const
{
	return isActive;
}

void GameObject::ToggleIsStatic()
{
	isStatic = !isStatic;

	if (isStatic)
		App->scene->quadtree.Insert(this);
	else
	{
		// Game object changed from static to dynamic: recreate quadtree
		System_Event newEvent;
		newEvent.type = System_Event_Type::RecreateQuadtree;
		App->PushSystemEvent(newEvent);
	}
}

bool GameObject::IsStatic() const
{
	return isStatic;
}

void GameObject::SetSeenLastFrame(bool seenLastFrame)
{
	this->seenLastFrame = seenLastFrame;
}

bool GameObject::GetSeenLastFrame() const
{
	return seenLastFrame;
}

void GameObject::RecursiveRecalculateBoundingBoxes()
{
	boundingBox.SetNegativeInfinity();

	// Grow bounding box
	if (meshRenderer != nullptr && meshRenderer->res != 0) 
	{
		const ResourceMesh* meshRes = (const ResourceMesh*)App->res->GetResource(meshRenderer->res);
		boundingBox.Enclose((const math::float3*)meshRes->vertices, meshRes->verticesSize);
	}

	// Transform bounding box (calculate OBB)
	math::OBB obb;
	obb.SetFrom(boundingBox);
	math::float4x4 transformMatrix = transform->GetGlobalMatrix();
	obb.Transform(transformMatrix);

	// Calculate AABB
	if (obb.IsFinite())
		boundingBox = obb.MinimalEnclosingAABB();

	for (uint i = 0; i < children.size(); ++i)
		children[i]->RecursiveRecalculateBoundingBoxes();
}

void GameObject::OnSave(JSON_Object* file) const
{
	json_object_set_string(file, "name", name);
	json_object_set_number(file, "UUID", UUID);
	json_object_set_number(file, "Parent UUID", parent->UUID);

	JSON_Value* arrayValue = json_value_init_array();
	JSON_Array* jsonComponents = json_value_get_array(arrayValue);
	for (int i = 0; i < components.size(); ++i)
	{
		JSON_Value* newValue = json_value_init_object();
		JSON_Object* objToSerialize = json_value_get_object(newValue);

		components[i]->OnSave(objToSerialize);
		json_array_append_value(jsonComponents, newValue);	
	}
	json_object_set_value(file, "jsonComponents", arrayValue);
}

void GameObject::OnLoad(JSON_Object* file)
{
	UUID = json_object_get_number(file, "UUID");
	JSON_Array* jsonComponents = json_object_get_array(file, "jsonComponents");
	JSON_Object* cObject;

	MarkToDeleteAllComponents();
	
	for (int i = 0; i < json_array_get_count(jsonComponents); i++) {
		cObject = json_array_get_object(jsonComponents, i);
		Component* newComponent = AddComponent((ComponentType)(int)json_object_get_number(cObject, "Type"));
		newComponent->OnLoad(cObject);
	}
}

void GameObject::RecursiveSerialitzation(JSON_Array* goArray) const
{
	if (parent != nullptr) {
		JSON_Value* newValue = json_value_init_object();
		JSON_Object* objToSerialize = json_value_get_object(newValue);

		OnSave(objToSerialize);

		json_array_append_value(goArray, newValue);
	}

	for (uint i = 0; i < children.size(); ++i)
		children[i]->RecursiveSerialitzation(goArray);	
}

void GameObject::RecursiveForceAllResources(uint forceRes) const
{
	if (materialRenderer != nullptr)
	{
		for (int i = 0; i < materialRenderer->res.size(); ++i)
			materialRenderer->res[i].res = forceRes;
	}

	if (meshRenderer != nullptr)
		meshRenderer->res = forceRes;

	for (int i = 0; i < children.size(); ++i)
		children[i]->RecursiveForceAllResources(forceRes);
}
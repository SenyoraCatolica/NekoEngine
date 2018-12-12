#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Globals.h"
#include "ComponentTypes.h"
#include "EventSystem.h"

#include "MathGeoLib\include\Geometry\AABB.h"
#include "parson\parson.h"

#include <vector>

class Component;
class ComponentTransform;
class ComponentMaterial;
class ComponentMesh;
class ComponentCamera;
class BoxColliderComponent;
class RigidBody3DComponent;

class GameObject
{
public:

	GameObject(const char* name, GameObject* parent, bool disableTransform = false);
	GameObject(const GameObject& gameObject);
	void Activate() const;
	virtual ~GameObject();

	void Update();

	void OnSystemEvent(System_Event event);

	void SetParent(GameObject* parent);
	GameObject* GetParent() const;
	uint GetParentUUID() const;

	void DestroyChildren();
	void DeleteMe();
	void AddChild(GameObject* children);
	void EraseChild(GameObject* child);
	void DeleteChild(uint index);
	void DeleteChildren();
	bool HasChildren() const;
	uint GetChildrenLength() const;
	GameObject* GetChild(uint index) const;

	Component* AddComponent(ComponentType type);
	void MarkToDeleteComponent(uint index);
	void MarkToDeleteComponentByValue(Component* component);
	void MarkToDeleteAllComponents();
	void InternallyDeleteComponent(Component* index);
	void InternallyDeleteComponents();
	bool HasComponents() const;
	uint GetComponenetsLength() const;
	Component* GetComponent(uint index) const;
	Component* GetComponent(ComponentType type);
	int GetComponentIndexOnComponents(Component* component) const;
	void SwapComponents(Component* firstComponent, Component* secondComponent);
	void ReorderComponents(Component* source, Component* target);

	bool EqualsToChildrenOrMe(const void* isEqual) const;

	void SetName(const char* name);
	const char* GetName() const;

	uint GetUUID() const;
	void ForceUUID(uint uuid);

	void ToggleIsActive();
	bool IsActive() const;

	void ToggleIsStatic();
	bool IsStatic() const;

	void SetSeenLastFrame(bool seenLastFrame);
	bool GetSeenLastFrame() const;

	void RecursiveRecalculateBoundingBoxes();

	void OnSave(JSON_Object* file) const;
	void OnLoad(JSON_Object* file);
	void RecursiveSerialitzation(JSON_Array* goArray) const;

	void RecursiveForceAllResources(uint forceRes) const;

public:

	ComponentTransform* transform = nullptr;
	ComponentMaterial* materialRenderer = nullptr;
	ComponentMesh* meshRenderer = nullptr;
	ComponentCamera* camera = nullptr;
	BoxColliderComponent* box_collider = nullptr;
	RigidBody3DComponent* rb = nullptr;

	math::AABB boundingBox;

private:

	char* name = nullptr;
	uint UUID = 0;

	std::vector<Component*> components;

	GameObject* parent = nullptr;
	uint parentUUID = 0;

	std::vector<GameObject*> children;

	bool isActive = true;
	bool isStatic = true;
	bool seenLastFrame = false;
};

#endif
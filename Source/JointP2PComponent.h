#pragma once

#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"

class PhysicBody3D;
class GameObject;

class JointP2PComponent : public Component
{
public:

	JointP2PComponent(GameObject* parent);
	~JointP2PComponent();

	void SetJoinToPair(JointP2PComponent* joint);
	void UnpairJoint();
	void ClearJoint();

	void OnUniqueEditor();

	bool MergeJoints(char* name);

	virtual void OnInternalSave(JSON_Object* file);
	virtual void OnLoad(JSON_Object* file);

public:

	PhysicBody3D* body;
	math::float3 anchor = math::float3::zero;

	JointP2PComponent* jointTo = nullptr;
	GameObject* jointToGO = nullptr;

private:
	std::string jointToName = "";
};
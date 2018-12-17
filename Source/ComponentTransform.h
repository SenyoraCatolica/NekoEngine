#ifndef __COMPONENT_TRANSFORM_H__
#define __COMPONENT_TRANSFORM_H__

#include "Component.h"

#include "Globals.h"

#include "MathGeoLib\include\Math\float3.h"
#include "MathGeoLib\include\Math\Quat.h"
#include "MathGeoLib\include\Math\float4x4.h"

#define TRANSFORMINPUTSWIDTH 50.0f

class ComponentTransform : public Component
{
public:

	ComponentTransform(GameObject* parent);
	ComponentTransform(const ComponentTransform& componentTransform);
	~ComponentTransform();

	void Update();

	void OnEditor();

	void OnUniqueEditor();

	math::float4x4& GetMatrix() const;
	math::float4x4& GetGlobalMatrix() const;
	void SetMatrixFromGlobal(math::float4x4& globalMatrix);

	void SetTransform(math::float4x4& matrix);
	void SetRotation(math::float3 rot);

	void UpdateOnTransformChanged();

	virtual void OnInternalSave(JSON_Object* file);
	virtual void OnLoad(JSON_Object* file);

public:

	math::float3 position = math::float3::zero;
	math::Quat rotation = math::Quat::identity;
	math::float3 scale = math::float3::one;
};

#endif
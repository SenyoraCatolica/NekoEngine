#ifndef __COMPONENT_TRANSFORM_H__
#define __COMPONENT_TRANSFORM_H__

#include "Component.h"

#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "MathGeoLib/include/Math/float4x4.h"

#define TRANSFORMINPUTSWIDTH 50.0f

class ComponentTransform : public Component
{
public:

	ComponentTransform(GameObject* parent);
	~ComponentTransform();

	void Update();

	void OnUniqueEditor();

	math::float4x4 GetMatrix();
	math::float4x4 GetGlobalMatrix();

public:

	math::float3 position = math::float3::zero;
	math::Quat rotation = math::Quat::identity;
	math::float3 scale = math::float3::one;
};

#endif
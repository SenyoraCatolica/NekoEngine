#pragma once

#include "MathGeoLib\include\MathGeoLib.h"
#include "Color.h"

using namespace math;

enum ShapeTypes
{
	Shape_Line,
	Shape_Plane,
	Shape_Cube,
	Shape_Sphere,
	Shape_Cylinder
};

class  PrimitiveShape
{
public:

	PrimitiveShape();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(Quat rot);
	void			Scale(float x, float y, float z);
	ShapeTypes		GetType() const;
	math::float3	GetPos()const;
	void			SetColor(Color color);


public:
	
	Color color;
	float4x4 transform;
	bool axis, wire;

protected:
	ShapeTypes type;
};

// ============================================
class PrimitiveShapeLine : public PrimitiveShape
{
public:
	PrimitiveShapeLine();
	PrimitiveShapeLine(float x, float y, float z);
	void InnerRender() const;
public:
	math::float3 origin;
	math::float3 destination;
};

// ============================================
class PrimitiveShapePlane : public PrimitiveShape
{
public:
	PrimitiveShapePlane();
	PrimitiveShapePlane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	math::float3 normal;
	float constant;
};

// ============================================
class PrimitiveShapeCube : public PrimitiveShape
{
public:
	PrimitiveShapeCube();
	PrimitiveShapeCube(float x, float y, float z);
	void InnerRender() const;
public:
	math::float3 size;
};

// ============================================
class PrimitiveShapeSphere : public PrimitiveShape
{
public:
	PrimitiveShapeSphere();
	PrimitiveShapeSphere(float radius);
	void InnerRender() const;
public:
	float radius;
};

// ============================================
class PrimitiveShapeCylinder : public PrimitiveShape
{
public:
	PrimitiveShapeCylinder();
	PrimitiveShapeCylinder(float radius, float height);
	void InnerRender() const;
public:
	float radius;
	float height;
};
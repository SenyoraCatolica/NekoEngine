#include "PrimitiveShape.h"
#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "glut/glut.h"

#pragma comment (lib, "glut/glut32.lib")


// ------------------------------------------------------------
PrimitiveShape::PrimitiveShape() : transform(transform.identity), color(White), wire(false), axis(false), type(ShapeTypes::Shape_Cube)
{}

// ------------------------------------------------------------
ShapeTypes PrimitiveShape::GetType() const
{
	return type;
}

// ------------------------------------------------------------
void PrimitiveShape::Render() const
{
	glPushMatrix();
	glMultMatrixf(*transform.v);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);


	glColor3f(color.r, color.g, color.b);

	if (wire)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_CULL_FACE);
	}

	InnerRender();



	

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	glPopMatrix();

}

// ------------------------------------------------------------
void PrimitiveShape::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

// ------------------------------------------------------------
void PrimitiveShape::SetPos(float x, float y, float z)
{
	transform.v[3][0] = x;
	transform.v[3][1] = y;
	transform.v[3][2] = z;
}

// ------------------------------------------------------------
void PrimitiveShape::SetRotation(Quat rot)
{
	transform.SetRotatePart(rot);
}

// ------------------------------------------------------------
void PrimitiveShape::SetScale(float x, float y, float z)
{
	transform.Scale(x, y, z);
}

// ------------------------------------------------------------
void PrimitiveShape::Scale(float x, float y, float z)
{
	//transform.RemoveScale();
	transform.Set3x3Part(transform.Float3x3Part() * transform.Scale(x, y, z).ToFloat3x3());
}

// ------------------------------------------------------------
math::float3 PrimitiveShape::GetPos()const
{
	math::float3 ret;
	ret.x = transform.v[3][0];
	ret.y = transform.v[3][1];
	ret.z = transform.v[3][2];
	return ret;
}

void PrimitiveShape::SetColor(Color color)
{
	this->color = color;
}


// LINE ==================================================
PrimitiveShapeLine::PrimitiveShapeLine() : PrimitiveShape(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = ShapeTypes::Shape_Line;
}

PrimitiveShapeLine::PrimitiveShapeLine(float x, float y, float z) : PrimitiveShape(), origin(0, 0, 0), destination(x, y, z)
{
	type = ShapeTypes::Shape_Line;
}

void PrimitiveShapeLine::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
}

// PLANE ==================================================
PrimitiveShapePlane::PrimitiveShapePlane() : PrimitiveShape(), normal(0, 1, 0), constant(1)
{
	type = ShapeTypes::Shape_Plane;
}

PrimitiveShapePlane::PrimitiveShapePlane(float x, float y, float z, float d) : PrimitiveShape(), normal(x, y, z), constant(d)
{
	type = ShapeTypes::Shape_Plane;
}

void PrimitiveShapePlane::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 20.0f;

	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}

// CUBE ============================================
PrimitiveShapeCube::PrimitiveShapeCube() : PrimitiveShape(), size(1.0f, 1.0f, 1.0f)
{
	type = ShapeTypes::Shape_Cube;
}

PrimitiveShapeCube::PrimitiveShapeCube(float sizeX, float sizeY, float sizeZ) : PrimitiveShape(), size(sizeX, sizeY, sizeZ)
{
	type = ShapeTypes::Shape_Cube;
}

void PrimitiveShapeCube::InnerRender() const
{
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glBegin(GL_QUADS);


	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glVertex3f(sx, -sy, sz);
	glVertex3f(sx, sy, sz);
	glVertex3f(-sx, sy, sz);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, sy, -sz);
	glVertex3f(sx, sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(sx, -sy, sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx, sy, -sz);
	glVertex3f(sx, sy, sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy, sz);
	glVertex3f(-sx, sy, sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-sx, sy, sz);
	glVertex3f(sx, sy, sz);
	glVertex3f(sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx, -sy, sz);
	glVertex3f(-sx, -sy, sz);

	glEnd();
}

// SPHERE ============================================
PrimitiveShapeSphere::PrimitiveShapeSphere() : PrimitiveShape(), radius(1.0f)
{
	type = ShapeTypes::Shape_Sphere;
}

PrimitiveShapeSphere::PrimitiveShapeSphere(float radius) : PrimitiveShape(), radius(radius)
{
	type = ShapeTypes::Shape_Sphere;
}

void PrimitiveShapeSphere::InnerRender() const
{
	glutSolidSphere(radius, 10, 10);
}

// CYLINDER ============================================
PrimitiveShapeCylinder::PrimitiveShapeCylinder() : PrimitiveShape(), radius(1.0f), height(1.0f)
{
	type = ShapeTypes::Shape_Cylinder;
}

PrimitiveShapeCylinder::PrimitiveShapeCylinder(float radius, float height) : PrimitiveShape(), radius(radius), height(height)
{
	type = ShapeTypes::Shape_Cylinder;
}

void PrimitiveShapeCylinder::InnerRender() const
{
	int n = 30;

	// Cylinder Bottom
	glBegin(GL_POLYGON);

	for (int i = 360; i >= 0; i -= (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(-height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder Top
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for (int i = 0; i <= 360; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder "Cover"
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i < 480; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians

		glVertex3f(height*0.5f, radius * cos(a), radius * sin(a));
		glVertex3f(-height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();
}
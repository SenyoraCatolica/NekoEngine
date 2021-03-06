#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

#include "Component.h"

#include "Globals.h"

#include <vector>
#include <list>
#include "MathGeoLib\include\Math\Quat.h"
#include "MathGeoLib\include\Math\float4x4.h"

#include "glew/include/GL/glew.h"

struct MaterialResource
{
	uint res = 0;
	math::float4x4 matrix = math::float4x4::identity;

	bool operator==(const MaterialResource rhs)
	{
		return this->res == rhs.res && matrix.Equals(rhs.matrix);
	}
};

class ComponentMaterial : public Component
{
public:

	ComponentMaterial(GameObject* parent);
	ComponentMaterial(const ComponentMaterial& componentMaterial);
	~ComponentMaterial();

	void Update();

	void SetResource(uint res_uuid, uint position);

	void OnUniqueEditor();

	virtual void OnInternalSave(JSON_Object* file);
	virtual void OnLoad(JSON_Object* file);

	void AttachShaderObject(GLuint shaderObject);
	void DetachShaderObject(GLuint shaderObject);
	void ClearShaderObjects();
	bool LinkShaderProgram();

private:

	void EditCurrentResMatrixByIndex(int i);

public:

	std::vector<MaterialResource> res;
	float color[4] = { 1.0f,1.0f,1.0f,255.0f };

	GLuint shaderProgram;
	std::list<GLuint> shObj;
};

#endif
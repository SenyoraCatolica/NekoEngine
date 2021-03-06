#include "ComponentMaterial.h"

#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "MaterialImporter.h"
#include "Resource.h"
#include "ResourceTexture.h"
#include "GameObject.h"
#include "Panel.h"

#include "imgui\imgui.h"

ComponentMaterial::ComponentMaterial(GameObject* parent) : Component(parent, ComponentType::Material_Component) 
{
	res.reserve(App->renderer3D->GetMaxTextureUnits());

	// Default texture (0)
	MaterialResource texture;
	texture.res = App->materialImporter->GetDefaultTexture();
	res.push_back(texture);
}

ComponentMaterial::ComponentMaterial(const ComponentMaterial& componentMaterial) : Component(componentMaterial.parent, ComponentType::Material_Component)
{
	res = componentMaterial.res;

	for (uint i = 0; i < res.size(); ++i)
	{
		res[i].res = 0;
		SetResource(componentMaterial.res[i].res, i);
	}

	for (uint i = 0; i < 4; ++i)
		color[i] = componentMaterial.color[i];
}

ComponentMaterial::~ComponentMaterial()
{
	parent->materialRenderer = nullptr;

	for (uint i = 0; i < res.size(); ++i)
		SetResource(0, i);

	if (glIsProgram(shaderProgram))
		glDeleteProgram(shaderProgram);
}

void ComponentMaterial::Update() {}

void ComponentMaterial::SetResource(uint res_uuid, uint position)
{
	if (res[position].res != 0)
		App->res->SetAsUnused(res[position].res);

	if (res_uuid != 0) {
		if (App->res->SetAsUsed(res_uuid) == -1)
			return;
	}

	res[position].res = res_uuid;
}

void ComponentMaterial::OnUniqueEditor()
{
#ifndef GAMEMODE
	ImGui::Text("Material Renderer");
	ImGui::Spacing();
	ImGui::ColorEdit4("Color", (float*)&color, ImGuiColorEditFlags_NoInputs);
	for (uint i = 0; i < res.size(); ++i)
	{
		ImGui::Text("Texture %i", i + 1);
		ImGui::SameLine();

		std::string fileName;
		const Resource* resource = App->res->GetResource(res[i].res);
		if (resource != nullptr)
			App->fs->GetFileName(resource->GetFile(), fileName);

		char itemName[DEFAULT_BUF_SIZE];
		sprintf_s(itemName, DEFAULT_BUF_SIZE, "%s##%i", fileName.data(), i);
		ImGui::Button(itemName, ImVec2(100.0f, 0.0f));

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("%u", (res.size() > 0) ? res.front().res : 0);
			ImGui::EndTooltip();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIAL_INSPECTOR_SELECTOR"))
			{
				uint payload_n = *(uint*)payload->Data;
				SetResource(payload_n, i);
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();

		sprintf_s(itemName, DEFAULT_BUF_SIZE, "-##%i", i);
		bool minusClicked = false;
		if (ImGui::Button(itemName))
			minusClicked = true;

		if (minusClicked)
		{
			SetResource(0, i);
			res.erase(std::remove(res.begin(), res.end(), res[i]));
		}

		const ResourceTexture* texture = (const ResourceTexture*)App->res->GetResource(res[i].res);

		uint id = 0;
		uint width = 0;
		uint height = 0;
		if (texture != nullptr)
		{
			id = texture->id;
			width = texture->width;
			height = texture->height;
		}

		ImGui::Image((void*)(intptr_t)id, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SameLine();

		sprintf_s(itemName, DEFAULT_BUF_SIZE, "Matrix##%i", i);
		ImGui::Button(itemName);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Edit Texture Matrix");

		sprintf_s(itemName, DEFAULT_BUF_SIZE, "Edit Matrix##%i", i);
		if (ImGui::BeginPopupContextItem(itemName, 0)) {

			EditCurrentResMatrixByIndex(i);

			ImGui::EndPopup();
		}

		ImGui::TextColored(BLUE, "%u x %u", width, height);
		ImGui::Spacing();
	}

	if (res.size() < App->renderer3D->GetMaxTextureUnits())
	{
		if (ImGui::Button("+")) {
			MaterialResource newRes;
			res.push_back(newRes);
		}
	}
#endif
}

void ComponentMaterial::OnInternalSave(JSON_Object* file)
{
	JSON_Value* arrayValue = json_value_init_array();
	JSON_Array* jsonArray = json_value_get_array(arrayValue);
	json_array_append_number(jsonArray, color[0]);
	json_array_append_number(jsonArray, color[1]);
	json_array_append_number(jsonArray, color[2]);
	json_array_append_number(jsonArray, color[3]);
	json_object_set_value(file, "VertexColor", arrayValue);

	json_array_append_value(jsonArray, arrayValue);

	arrayValue = json_value_init_array();
	jsonArray = json_value_get_array(arrayValue);

	for (int i = 0; i < res.size(); ++i)
	{
		JSON_Value* newValue = json_value_init_object();
		JSON_Object* resource = json_value_get_object(newValue);

		json_object_set_number(resource, "res", res[i].res);

		math::float3 pos; math::Quat rot; math::float3 scale;
		res[i].matrix.Decompose(pos, rot, scale);
		json_object_set_number(resource, "posX", pos.x);
		json_object_set_number(resource, "posY", pos.y);
		json_object_set_number(resource, "posZ", pos.z);

		json_object_set_number(resource, "rotX",rot.x);
		json_object_set_number(resource, "rotY",rot.y);
		json_object_set_number(resource, "rotZ",rot.z);
		json_object_set_number(resource, "rotW",rot.w);

		json_object_set_number(resource, "scaleX", scale.x);
		json_object_set_number(resource, "scaleY", scale.y);
		json_object_set_number(resource, "scaleZ", scale.z);

		json_array_append_value(jsonArray, newValue);
	}
	json_object_set_value(file, "TexturesResources", arrayValue);
}

void ComponentMaterial::OnLoad(JSON_Object* file)
{
	JSON_Array* jsonArray = json_object_get_array(file, "VertexColor");

	uint arraySize = json_array_get_count(jsonArray);

	for (int i = 0; i < arraySize; i++)
		color[i] = json_array_get_number(jsonArray, i);

	jsonArray = json_object_get_array(file, "TexturesResources");

	arraySize = json_array_get_count(jsonArray);

	while (res.size() < arraySize)
	{
		MaterialResource newRes;
		res.push_back(newRes);
	}

	for (int i = 0; i < arraySize; i++) {
		JSON_Object* rObject = json_array_get_object(jsonArray, i);

		uint newRes = json_object_get_number(rObject, "res");
		if (newRes == 0 || App->res->GetResource(newRes) != nullptr)
			SetResource(newRes, i);

		math::float3 pos; math::Quat rot; math::float3 scale;

		pos.x = json_object_get_number(rObject, "posX");
		pos.y = json_object_get_number(rObject, "posY");
		pos.z = json_object_get_number(rObject, "posZ");

		rot.x = json_object_get_number(rObject, "rotX");
		rot.y = json_object_get_number(rObject, "rotY");
		rot.z = json_object_get_number(rObject, "rotZ");
		rot.w = json_object_get_number(rObject, "rotW");

		scale.x = json_object_get_number(rObject, "scaleX");
		scale.y = json_object_get_number(rObject, "scaleY");
		scale.z = json_object_get_number(rObject, "scaleZ");

		res[i].matrix = math::float4x4::FromTRS(pos, rot, scale);
	}
}

void ComponentMaterial::AttachShaderObject(GLuint shaderObject)
{
	if (std::find(shObj.begin(), shObj.end(), shaderObject) != shObj.end())
		shObj.push_back(shaderObject);
}

void ComponentMaterial::DetachShaderObject(GLuint shaderObject)
{
	shObj.remove(shaderObject);
}

void ComponentMaterial::ClearShaderObjects()
{
	shObj.clear();
}

bool ComponentMaterial::LinkShaderProgram()
{
	// 2. LINKING

	// Create a Shader Program
	shaderProgram = glCreateProgram();

	for (auto it = shObj.begin(); it != shObj.end(); ++it)
		glAttachShader(shaderProgram, *it);

	glLinkProgram(shaderProgram);

	for (auto it = shObj.begin(); it != shObj.end(); ++it)
		glDetachShader(shaderProgram, *it);

	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) 
	{
		GLint logSize = 0;
		glGetShaderiv(shaderProgram, GL_INFO_LOG_LENGTH, &logSize);

		GLchar* infoLog = new GLchar[logSize];
		glGetProgramInfoLog(shaderProgram, logSize, NULL, infoLog);

		CONSOLE_LOG("Shader Program could not be linked. ERROR: %s", infoLog);
	
		glDeleteProgram(shaderProgram);
	}
	else
		CONSOLE_LOG("Successfully linked Shader Program");

	return success;
}

void ComponentMaterial::EditCurrentResMatrixByIndex(int i)
{
#ifndef GAMEMODE
	char itemName[DEFAULT_BUF_SIZE];
	sprintf_s(itemName, DEFAULT_BUF_SIZE, "Reset##%i", i);
	if (ImGui::Button(itemName))
		res[i].matrix = math::float4x4::identity;

	math::float3 pos = math::float3::zero;
	math::Quat rot = math::Quat::identity;
	math::float3 scale = math::float3::one;
	res[i].matrix.Decompose(pos, rot, scale);

	////////////////////////////
	const double f64_lo_a = -1000000000000000.0, f64_hi_a = +1000000000000000.0;
	ImGui::Text("Position");

	sprintf_s(itemName, DEFAULT_BUF_SIZE, "##PosX%i", i);
	ImGui::PushItemWidth(50);
	ImGui::DragScalar(itemName, ImGuiDataType_Float, (void*)&pos.x, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f); ImGui::SameLine();
	sprintf_s(itemName, DEFAULT_BUF_SIZE, "##PosY%i", i);
	ImGui::PushItemWidth(50);
	ImGui::DragScalar(itemName, ImGuiDataType_Float, (void*)&pos.y, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f); ImGui::SameLine();
	sprintf_s(itemName, DEFAULT_BUF_SIZE, "##PosZ%i", i);
	ImGui::PushItemWidth(50);
	ImGui::DragScalar(itemName, ImGuiDataType_Float, (void*)&pos.z, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f);

	ImGui::Text("Rotation");
	math::float3 axis;
	float angle;
	rot.ToAxisAngle(axis, angle);
	axis *= angle;
	axis *= RADTODEG;
	sprintf_s(itemName, DEFAULT_BUF_SIZE, "##AxisAngleX%i", i);
	ImGui::PushItemWidth(50);
	ImGui::DragScalar(itemName, ImGuiDataType_Float, (void*)&axis.x, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f); ImGui::SameLine();
	sprintf_s(itemName, DEFAULT_BUF_SIZE, "##AxisAngleY%i", i);
	ImGui::PushItemWidth(50);
	ImGui::DragScalar(itemName, ImGuiDataType_Float, (void*)&axis.y, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f); ImGui::SameLine();
	sprintf_s(itemName, DEFAULT_BUF_SIZE, "##AxisAngleZ%i", i);
	ImGui::PushItemWidth(50);
	ImGui::DragScalar(itemName, ImGuiDataType_Float, (void*)&axis.z, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f);
	axis *= DEGTORAD;
	rot.SetFromAxisAngle(axis.Normalized(), axis.Length());

	ImGui::Text("Scale");

	sprintf_s(itemName, DEFAULT_BUF_SIZE, "##ScaleX%i", i);
	ImGui::PushItemWidth(50);
	ImGui::DragScalar(itemName, ImGuiDataType_Float, (void*)&scale.x, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f); ImGui::SameLine();
	sprintf_s(itemName, DEFAULT_BUF_SIZE, "##ScaleY%i", i);
	ImGui::PushItemWidth(50);
	ImGui::DragScalar(itemName, ImGuiDataType_Float, (void*)&scale.y, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f); ImGui::SameLine();
	sprintf_s(itemName, DEFAULT_BUF_SIZE, "##ScaleZ%i", i);
	ImGui::PushItemWidth(50);
	ImGui::DragScalar(itemName, ImGuiDataType_Float, (void*)&scale.z, 0.1f, &f64_lo_a, &f64_hi_a, "%f", 1.0f);

	res[i].matrix = math::float4x4::FromTRS(pos, rot, scale);
#endif
}

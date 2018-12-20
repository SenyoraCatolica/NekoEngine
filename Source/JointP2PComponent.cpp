#include "JointP2PComponent.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleGOs.h"
#include "ModulePhysics.h"
#include "Globals.h"

#include "imgui\imgui.h"
#include "imgui\imgui_internal.h"


JointP2PComponent::JointP2PComponent(GameObject* parent) :
	Component(parent, ComponentType::COMPONENT_JP2P)
{

}

JointP2PComponent::~JointP2PComponent()
{
	ClearJoint();
}

void JointP2PComponent::SetJoinToPair(JointP2PComponent* joint)
{
	jointTo = joint;
	jointToGO = joint->parent;
	jointToName = jointTo->parent->GetName();
}

void JointP2PComponent::UnpairJoint()
{
	jointTo = nullptr;
	jointToGO = nullptr;
	jointToName = "";
}


void JointP2PComponent::ClearJoint()
{
	body = nullptr;
	jointTo = nullptr;
	jointToGO = nullptr;
}

void JointP2PComponent::OnUniqueEditor()
{
	ImGui::TextColored(ImVec4(0, 0, 1.0, 1), "Component Joint Point to Point");
	if (IsActive())
	{
		ImGui::NewLine();

		ImGui::PushItemWidth(100.0f);
		ImGuiInputTextFlags inputFlag = ImGuiInputTextFlags_EnterReturnsTrue;
		ImGui::Text("Object name to joint to:");
		ImGui::TextColored(ImVec4(1.0, 0, 0, 1.0), "Warning! - the object must have a Component Joint too");
		static char goName[INPUT_BUF_SIZE];

		if (jointToName.empty() == false)
			strcpy(goName, jointToName.data());

		if (ImGui::InputText("##goName", goName, IM_ARRAYSIZE(goName), inputFlag))
		{
			if(MergeJoints(goName))
				jointToName = goName;
		}
	}
}

bool JointP2PComponent::MergeJoints(char* goName)
{
	bool ret = false;

	GameObject* go = App->GOs->GetGameObjectByName(goName);

	if (go != nullptr)
	{
		//separate joint components
		if (jointTo != nullptr)
			jointTo->UnpairJoint();
		UnpairJoint();

		//pair with the new joint if exists
		jointTo = go->jp2p;

		if (jointTo != nullptr)
		{
			SetJoinToPair(jointTo);
			jointTo->SetJoinToPair(this);
			App->physics->AddConstraint(this, jointTo);
			ret = true;
		}

		else
		{
			CONSOLE_LOG("Can't find a Joint P2P Component int the object: %s in the scene", go->GetName());
			std::strcpy(goName, "");
		}
	}

	else
	{
		CONSOLE_LOG("Can't find an object with the name: %s in the scene", goName);
		std::strcpy(goName, jointToName.data());
	}

	return ret;
}



void JointP2PComponent::OnInternalSave(JSON_Object* file)
{
	json_object_set_number(file, "AnchorX", anchor.x);
	json_object_set_number(file, "AnchorY", anchor.y);
	json_object_set_number(file, "AnchorZ", anchor.z);
	json_object_set_number(file, "JointToUUID", jointToGO->GetUUID());
}

void JointP2PComponent::OnLoad(JSON_Object* file)
{
	anchor.x = json_object_get_number(file, "AnchorX");
	anchor.y = json_object_get_number(file, "AnchorY");
	anchor.z = json_object_get_number(file, "AnchorZ");
	uint uuid = json_object_get_number(file, "JointToUUID");
}
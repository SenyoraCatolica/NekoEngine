#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "Primitive.h"
#include "SceneImporter.h"
#include "ModuleGOs.h"
#include "GameObject.h"
#include "ComponentTransform.h"

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{
	name = "Scene";
}

ModuleScene::~ModuleScene()
{}

bool ModuleScene::Init(JSON_Object* jObject)
{
	CreateQuadtree();

	return true;
}

bool ModuleScene::Start()
{
	bool ret = true;

	grid = new PrimitiveGrid();
	grid->ShowAxis(true);
	root = App->GOs->CreateGameObject("Root", nullptr);
	child = App->GOs->CreateGameObject("Api", root);
	GameObject* fillGuillem = App->GOs->CreateGameObject("fill de Api", child);
	App->GOs->CreateGameObject("net de Api1", fillGuillem);
	App->GOs->CreateGameObject("net de Api2", fillGuillem);
	child = App->GOs->CreateGameObject("Patata", root);
	fillGuillem = App->GOs->CreateGameObject("fill de Patata", child);

	App->GOs->CreateGameObject("net de Patata", fillGuillem);
	// Load Baker House last mesh
	std::string outputFile;
	App->sceneImporter->Import("cube.FBX", "Assets/Meshes/", outputFile);

	//Mesh* mesh = new Mesh();
	//App->sceneImporter->Load(outputFile.data(), mesh);

	// Load Baker House texture
	//App->materialImporter->Import("Baker_house.png", "Assets/", outputFile);
	//Texture* texture = new Texture();
	//App->materialImporter->Load(outputFile.data(), texture);
	return ret;
}

update_status ModuleScene::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	bool ret = true;

	RELEASE(grid);

	return ret;
}

void ModuleScene::Draw() const
{
	grid->Render();

	bool cullFace = App->renderer3D->GetCapabilityState(GL_CULL_FACE);
	App->renderer3D->SetCapabilityState(GL_CULL_FACE, false);
	RecursiveDrawQuadtree(quadtree.root);
	App->renderer3D->SetCapabilityState(GL_CULL_FACE, cullFace);
}

bool ModuleScene::GetShowGrid() const
{
	return showGrid;
}

void ModuleScene::SetShowGrid(bool showGrid)
{
	this->showGrid = showGrid;
}

void ModuleScene::CreateQuadtree()
{
	const math::float3 center(0.0f, 5.0f, 0.0f);
	const math::float3 size(100.0f, 10.0f, 100.0f);
	math::AABB boundary;
	boundary.SetFromCenterAndSize(center, size);

	quadtree.SetBoundary(boundary);
}

void ModuleScene::RecursiveDrawQuadtree(QuadtreeNode* node) const
{
	PrimitiveCube nodeBoundingBox(node->boundingBox.Size());
	nodeBoundingBox.SetColor(Yellow);
	nodeBoundingBox.SetWireframeMode(true);
	math::float3 position = node->boundingBox.CenterPoint();
	nodeBoundingBox.Render(math::float4x4::FromTRS(position, math::Quat::identity, math::float3(1.0f, 1.0f, 1.0f)));

	for (std::list<GameObject*>::const_iterator it = node->objects.begin(); it != node->objects.end(); ++it)
	{
		PrimitiveCube objectBoundingBox((*it)->boundingBox.Size());
		objectBoundingBox.SetColor(Red);
		objectBoundingBox.SetWireframeMode(true);
		position = (*it)->boundingBox.CenterPoint();
		objectBoundingBox.Render(math::float4x4::FromTRS(position, math::Quat::identity, math::float3(1.0f, 1.0f, 1.0f)));
	}

	if (!node->IsLeaf())
	{
		for (uint i = 0; i < 4; ++i)
			RecursiveDrawQuadtree(node->children[i]);
	}
}
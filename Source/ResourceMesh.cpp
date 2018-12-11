#include "ResourceMesh.h"

#include "Application.h"
#include "ModuleGOs.h"
#include "SceneImporter.h"

#include <assert.h>

#include "glew\include\GL\glew.h"

ResourceMesh::ResourceMesh(ResourceType type, uint uuid) : Resource(type, uuid) 
{
	name = new char[DEFAULT_BUF_SIZE];
}

ResourceMesh::~ResourceMesh() 
{
	RELEASE_ARRAY(name);
	App->GOs->InvalidateResource(this);
}

bool ResourceMesh::LoadInMemory()
{
	bool ret = App->sceneImporter->Load(exportedFile.data(), this);

	if (!ret)
		return ret;

	GenerateVBO();
	GenerateVAO();
	GenerateIBO();

	return ret;
}

bool ResourceMesh::UnloadFromMemory()
{
	glDeleteBuffers(1, (GLuint*)&VBO);
	glDeleteBuffers(1, (GLuint*)IBO);

	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(indices);

	return true;
}

void ResourceMesh::GenerateVBO()
{
	assert(vertices != nullptr);

	// Vertex Buffer Object

	// Generate a VBO
	glGenBuffers(1, &VBO);
	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verticesSize, vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ResourceMesh::GenerateIBO()
{
	assert(indices != nullptr);

	// Index Buffer Object

	// Generate a IBO
	glGenBuffers(1, &IBO);
	// Bind the IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indicesSize, indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ResourceMesh::GenerateVAO()
{
	// Vertex Array Object

	// Generate a VAO
	glGenVertexArrays(1, &VAO);
	// Bind the VAO
	glBindVertexArray(VAO);

	// Bind the VBO 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Set the vertex attributes pointers
	// 1. Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glEnableVertexAttribArray(0);

	// 2. Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(1);

	// 3. Color
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, color)));
	glEnableVertexAttribArray(2);

	// 4. Tex coords
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoord)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
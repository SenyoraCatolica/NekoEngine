#ifndef __MATERIAL_IMPORTER_H__
#define __MATERIAL_IMPORTER_H__

#include "Importer.h"

#include <string>

struct Texture
{
	uint id = 0;
	uint width = 0;
	uint height = 0;
};

class MaterialImporter : public Importer
{
public:

	MaterialImporter();
	~MaterialImporter();

	bool Import(const char* importFile, const char* importPath, const char* outputFile);
	bool Import(const void* buffer, uint size, const char* outputFile);
	
	bool Load(const char* exportedFile, Texture* outputTexture);
	bool Load(const void* buffer, uint size, Texture* outputTexture);
	
	bool LoadCheckers(Texture* textureResource);

	uint GetDevILVersion() const;
};

#endif
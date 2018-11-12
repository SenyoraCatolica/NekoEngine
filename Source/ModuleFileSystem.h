#ifndef __MODULE_FILE_SYSTEM_H__
#define __MODULE_FILE_SYSTEM_H__

#include "Module.h"

#include <string>

#define DIR_LIBRARY "Library"
#define DIR_LIBRARY_MESHES "Library/Meshes"
#define DIR_LIBRARY_MATERIALS "Library/Materials"
#define DIR_ASSETS "Assets"
#define DIR_ASSETS_SCENES "Assets/Scenes"

enum FileType
{
	MeshFile,
	TextureFile,
	SceneFile
};

class ModuleFileSystem : public Module
{
public:

	ModuleFileSystem(bool start_enabled = true);
	~ModuleFileSystem();

	bool CleanUp();

	bool CreateDir(const char* dirName) const;
	bool AddPath(const char* newDir, const char* mountPoint = nullptr);

	const char* GetBasePath() const;
	const char* GetReadPaths() const;
	const char* GetWritePath() const;
	const char** GetFilesFromDir(const char* dir) const;

	uint Copy(const char* file, const char* dir, std::string& outputFile) const;

	uint SaveInLibrary(char* buffer, uint size, FileType fileType, std::string& outputFile) const;
	uint Save(const char* file, char* buffer, uint size, bool append = false) const;

	uint Load(const char* file, char** buffer) const;

	bool IsDirectory(const char* file) const;
	bool Exists(const char* file) const;

	void GetFileName(const char* file, std::string& fileName, bool extension = false) const;
	void GetExtension(const char* file, std::string& extension) const;
	void GetPath(const char* file, std::string& path) const;
};

#endif
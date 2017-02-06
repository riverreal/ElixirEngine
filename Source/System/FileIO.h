#pragma once

#include "../Helper/TypeHelper.h"
//For File open/Save Dialog
#include <Windows.h>
#include <commdlg.h>
#include <tchar.h>

namespace Elixir
{
	class FileIO
	{
	public:
		FileIO();
		~FileIO();

		void SaveMaterial(Elixir::Material& mat);

		std::vector<Material> LoadMaterials();

		//Reads every file name in the directory and subdirectories.
		//Returns a vector of file paths.
		std::vector<std::string> GetFilesPathInDirectory(std::string directory);

		ProjectData ReadProjFile(std::string filename);
		void CreateProjFile();
		void SaveProjFile(ProjectData proj);

		SceneData ReadSceneFile(std::string filename);
		void SaveSceneFile(SceneData scene);
		Vec3f GetXYZFromString(std::string source);

		enum FILE_TYPE
		{
			FILE_MATERIAL,
			FILE_SCENE,
			FILE_SAVEDATA
		};

		const std::string FileTypeExtension[3] =
		{
			".emat",
			".escene",
			".esave"
		};

		std::string GetLastOpenedFile();
		std::string OpenFile(TCHAR* extension);
		std::wstring OpenFileW(TCHAR* extension);

		std::wstring GetExePathW();
		std::string GetExePath();

		void CurrentSceneChanged();

	private:


	private:
		OPENFILENAME m_openedFileName;

		std::string m_currentScenePath;
	};
}
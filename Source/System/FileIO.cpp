#include "FileIO.h"
#include <iostream>
#include <string>
#include <fstream>
#include "../Helper/GeneralHelper.h"
#include "../../External Soruce/dirent.h"
#include <filesystem>

using namespace Elixir;
using namespace std::tr2::sys;
FileIO::FileIO()
	:m_currentScenePath("")
{

}

FileIO::~FileIO()
{

}

void FileIO::SaveMaterial(Material& mat)
{
	std::fstream file;
	std::string path = "Resource/Materials/";
	file.open(path + mat.name + FileTypeExtension[FILE_MATERIAL], std::fstream::out | std::fstream::trunc);

	file << "<ID>" << mat.materialID << "</ID>\n";
	file << "<albedo>" + std::to_string(mat.albedo) + "</albedo>\n";
	file << "<normal>" + std::to_string(mat.normal) + "</normal>\n";
	file << "<roughness>" + std::to_string(mat.roughness) + "</roughness>\n";
	file << "<metallic>" + std::to_string(mat.metallic) + "</metallic>\n";

	file.close();
}

std::vector<Material> Elixir::FileIO::LoadMaterials()
{
	std::vector<std::string> pathList;
	std::vector<Material> matList;

	pathList = GetFilesPathInDirectory("Resource/");

	//Check if material file is available
	for (auto &filename : pathList)
	{
		auto extension = filename.substr(filename.find_last_of(".") + 1);

		//if there is a material file
		if (extension == "emat")
		{
			std::ifstream file;
			file.open(filename, std::fstream::in);

			Material mat;
			mat.name = filename;

			//read content
			std::string lineContent;
			while (std::getline(file, lineContent))
			{
				auto idPos = lineContent.find("<ID>");
				if (idPos != std::string::npos)
				{
					auto idEnd = lineContent.find("</ID>");
					
					mat.materialID = std::stoi(lineContent.substr(idPos + 4, idEnd));
				}

				auto albedoPos = lineContent.find("<albedo>");
				if (albedoPos != std::string::npos)
				{
					auto albedoEnd = lineContent.find("</albedo>");
					mat.albedo = std::stoi(lineContent.substr(albedoPos + 8, albedoEnd));
				}

				auto normalPos = lineContent.find("<normal>");
				if (normalPos != std::string::npos)
				{
					auto normalEnd = lineContent.find("</normal>");
					mat.normal = std::stoi(lineContent.substr(normalPos + 8, normalEnd));
				}

				auto roughnessPos = lineContent.find("<roughness>");
				if (roughnessPos != std::string::npos)
				{
					auto roughnessEnd = lineContent.find("</roughness>");
					mat.roughness = std::stoi(lineContent.substr(roughnessPos + 11, roughnessEnd));
				}

				auto metallicPos = lineContent.find("<metallic>");
				if (metallicPos != std::string::npos)
				{
					auto metallicEnd = lineContent.find("</metallic>");
					mat.metallic = std::stoi(lineContent.substr(metallicPos + 10, metallicEnd));
				}
			}

			matList.push_back(mat);

			file.close();
		}
	}

	return matList;
}

std::vector<std::string> Elixir::FileIO::GetFilesPathInDirectory(std::string directory)
{
	std::vector<std::string> paths;
	for (recursive_directory_iterator i(directory), end; i != end; ++i)
		if (!is_directory(i->path()))
			paths.push_back(i->path().string());

	return paths;
}

ProjectData Elixir::FileIO::ReadProjFile(std::string filename)
{
	ProjectData project;

	std::ifstream file;
	file.open(filename, std::ios::in);

	std::string lineContent;
	while (std::getline(file, lineContent))
	{
		if (lineContent.find("<t>") != std::string::npos)
		{
			StringID textureData("", 1);

			auto namePos = lineContent.find("<name>");
			if (namePos != std::string::npos)
			{
				if (namePos == 0)
				{

				}
				else
				{
					auto endNamePos = lineContent.find("</name>");
					textureData.name = lineContent.substr(namePos + 6, endNamePos - 9);
				}
				
			}

			auto IDPos = lineContent.find("<ID>");
			if (IDPos != std::string::npos)
			{
				if (IDPos == 0)
				{

				}
				else
				{
					auto endIDPos = lineContent.find("</ID>");
					textureData.ID = std::stoi(lineContent.substr(IDPos + 4, endIDPos));
				}
				
			}

			project.TextureData.push_back(textureData);
		}
	}

	file.close();

	return project;
}

void Elixir::FileIO::CreateProjFile()
{
	std::fstream file;

	file.open("NewProject.elixir", std::fstream::out);

	file.close();
}

void Elixir::FileIO::SaveProjFile(ProjectData proj)
{
	auto projFilePath = GetFilesPathInDirectory(".");
	bool foundProject = false;

	std::string projFile;

	//Check if project file is available
	for (auto &filename : projFilePath)
	{
		auto extension = filename.substr(filename.find_last_of(".") + 1);
		if (extension == "elixir")
		{
			projFile = filename;
			foundProject = true;
		}
	}

	if (foundProject)
	{
		std::fstream file;
		file.open(projFile, std::fstream::in | std::fstream::out | std::fstream::trunc);

		for (auto &texData : proj.TextureData)
		{
			file << "<t><name>" << texData.name << "</name><ID>" << texData.ID << "</ID></t>\n";
		}

		file.close();
	}
	else
	{
		CreateProjFile();
		SaveProjFile(proj);
	}
}

SceneData Elixir::FileIO::ReadSceneFile(std::string filename)
{
	SceneData sData;
	std::ifstream file;
	file.open(filename, std::ifstream::in);

	std::string lineContent;

	while (std::getline(file, lineContent))
	{
		//check what kind of line it is

		//object line
		auto objPos = lineContent.find("<obj>");
		if (objPos != std::string::npos)
		{
			ObjectID objID;
			//get it's components----------

			//transform
			auto transPos = lineContent.find("<trans>");
			if (transPos != std::string::npos)
			{
				objID.transform.exists = true;
				int parseBool = std::stoi(lineContent.substr(transPos + 7, 1));
				bool dynamic = parseBool == 1 ? true : false;
				objID.transform.dynamic = dynamic;

				auto posPos = lineContent.find("<pos>");
				auto posEnd = lineContent.find("</pos>");
				auto positionLine = lineContent.substr(posPos + 5, posEnd);
				objID.transform.pos = GetXYZFromString(positionLine);

				auto rotPos = lineContent.find("<rot>");
				auto rotEnd = lineContent.find("</rot>");
				auto rotationLine = lineContent.substr(rotPos + 5, rotEnd);
				objID.transform.rot = GetXYZFromString(rotationLine);

				auto scaPos = lineContent.find("<sca>");
				auto scaEnd = lineContent.find("</sca>");
				auto scaleLine = lineContent.substr(scaPos + 5, scaEnd);
				objID.transform.sca = GetXYZFromString(scaleLine);

				auto tposPos = lineContent.find("<tpos>");
				auto tposEnd = lineContent.find("</tpos>");
				auto tposLine = lineContent.substr(tposPos + 6, tposEnd);
				objID.transform.tpos = GetXYZFromString(tposLine);

				auto tscaPos = lineContent.find("<tsca>");
				auto tscaEnd = lineContent.find("</tsca>");
				auto tscaleLine = lineContent.substr(tscaPos + 6, tscaEnd);
				objID.transform.tsca = GetXYZFromString(tscaleLine);
			}

			//renderer
			auto renderPos = lineContent.find("<render>");
			if (renderPos != std::string::npos)
			{
				auto renderEnd = lineContent.find("</render>");
				auto renderLine = lineContent.substr(renderPos + 7, renderEnd);
				objID.renderer.exists = true;

				int boolParser = std::stoi(lineContent.substr(renderPos + 8, 1));
				objID.renderer.enabled = boolParser == 1 ? true : false;
				boolParser = std::stoi(lineContent.substr(renderPos + 9, 1));
				objID.renderer.backface = boolParser == 1 ? true : false;
				boolParser = std::stoi(lineContent.substr(renderPos + 10, 1));
				objID.renderer.castShadow = boolParser == 1 ? true : false;
				boolParser = std::stoi(lineContent.substr(renderPos + 11, 1));
				objID.renderer.modelTypePrimitive = boolParser == 1 ? true : false;

				objID.renderer.primitiveType = std::stoi(lineContent.substr(renderPos + 12, 1));

				auto modelPos = lineContent.find("<model>");
				auto subModel = lineContent.substr(modelPos+7);
				auto modelEnd = subModel.find_first_of("<");
				Log() << modelEnd << "\n";
				objID.renderer.modelPath = subModel.substr(0, modelEnd);

				auto albPos = lineContent.find("<alb>");
				auto albEnd = lineContent.find("</alb>");
				objID.renderer.alb = std::stoi(lineContent.substr(albPos + 5, albEnd));

				auto normPos = lineContent.find("<norm>");
				auto normEnd = lineContent.find("</norm>");
				objID.renderer.norm = std::stoi(lineContent.substr(normPos + 6, normEnd));

				auto rouPos = lineContent.find("<rou>");
				auto rouEnd = lineContent.find("</rou>");
				objID.renderer.rou = std::stoi(lineContent.substr(rouPos + 5, rouEnd));

				auto metPos = lineContent.find("<met>");
				auto metEnd = lineContent.find("</met>");
				objID.renderer.met = std::stoi(lineContent.substr(metPos + 5, metEnd));
			}

			sData.GameObjects.push_back(objID);
		}

	}

	file.close();

	return sData;
}

void Elixir::FileIO::SaveSceneFile(SceneData scene)
{
	std::string scenePath = "";
	bool found = false;

	if (m_currentScenePath == "")
	{
		//search for the scene path
		auto fileList = GetFilesPathInDirectory(".");

		for (auto &filename : fileList)
		{
			auto sceneFile = filename.substr(filename.find_last_of("/") + 3);
			if (sceneFile == (scene.name + ".escene"))
			{
				scenePath = filename;
				found = true;
				m_currentScenePath = scenePath;
			}
		}
	}
	else
	{
		scenePath = m_currentScenePath;
		found = true;
	}
	

	if (found)
	{
		std::fstream file;
		file.open(scenePath, std::fstream::out);

		for (auto &obj : scene.GameObjects)
		{
			file << "<obj>";
			if (obj.transform.exists)
			{
				file << "<trans>";
				//0 true | 1 false
				file << obj.transform.dynamic ? 0 : 1;
				
				//transform
				file << "<pos>" << "<x>" << obj.transform.pos.x << "</x>" << "<y>" << obj.transform.pos.y << "</y>" << "<z>"  << obj.transform.pos.z << "</z>" << "</pos>";
				file << "<rot>" << "<x>" << obj.transform.rot.x << "</x>" << "<y>" << obj.transform.rot.y << "</y>" << "<z>" << obj.transform.rot.z << "</z>" << "</rot>";
				file << "<sca>" << "<x>" << obj.transform.sca.x << "</x>" << "<y>" << obj.transform.sca.y << "</y>" << "<z>" << obj.transform.sca.z << "</z>" << "</sca>";

				//texture transform
				file << "<tpos>" << "<x>" << obj.transform.tpos.x << "</x>" << "<y>" << obj.transform.tpos.y << "</y>" << "<z>" << obj.transform.tpos.z << "</z>" << "</tpos>";
				file << "<tsca>" << "<x>" << obj.transform.tsca.x << "</x>" << "<y>" << obj.transform.tsca.y << "</y>" << "<z>" << obj.transform.tsca.z << "</z>" << "</tsca>";

				file << "</trans>";
			}

			if (obj.renderer.exists)
			{
				file << "<render>";

				file << obj.renderer.enabled ? 0 : 1;
				file << obj.renderer.backface ? 0 : 1;
				file << obj.renderer.castShadow ? 0 : 1;

				file << obj.renderer.modelTypePrimitive ? 0 : 1;
				file << obj.renderer.primitiveType;

				file << "<model>" << obj.renderer.modelPath << "</end>";

				file << "<alb>" << obj.renderer.alb << "</alb>";
				file << "<norm>" << obj.renderer.norm << "</norm>";
				file << "<rou>" << obj.renderer.rou << "</rou>";
				file << "<met>" << obj.renderer.met << "</met>";

				file << "</render>";
			}

			file << "</obj>\n";
		}

		file.close();
	}
	else
	{
		std::fstream file;

		file.open((scene.name + ".escene"), std::fstream::out);

		file.close();

		SaveSceneFile(scene);
	}
}

Vec3f Elixir::FileIO::GetXYZFromString(std::string source)
{
	Vec3f xyz;
	auto xPos = source.find("<x>");
	auto xEnd = source.find("</x>");
	xyz.x = std::stof(source.substr(xPos + 3, xEnd));

	auto yPos = source.find("<y>");
	auto yEnd = source.find("</y>");
	xyz.y = std::stof(source.substr(yPos + 3, yEnd));

	auto zPos = source.find("<z>");
	auto zEnd = source.find("</z>");
	xyz.z = std::stof(source.substr(zPos + 3, zEnd));

	return xyz;
}

std::string Elixir::FileIO::OpenFile(TCHAR * extension)
{
	std::wstring path = OpenFileW(extension);
	return ws2s(path);
}

std::wstring Elixir::FileIO::OpenFileW(TCHAR * extension)
{
	TCHAR fileName[MAX_PATH];
	int flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// open a file name
	ZeroMemory(&m_openedFileName, sizeof(m_openedFileName));
	m_openedFileName.lStructSize = sizeof(m_openedFileName);
	m_openedFileName.hwndOwner = NULL;
	m_openedFileName.lpstrFile = fileName;
	m_openedFileName.lpstrFile[0] = '\0';
	m_openedFileName.nMaxFile = sizeof(fileName);
	m_openedFileName.lpstrFilter = extension;
	m_openedFileName.nFilterIndex = 1;
	m_openedFileName.lpstrFileTitle = NULL;
	m_openedFileName.nMaxFileTitle = 0;
	m_openedFileName.lpstrTitle = L"Open File";
	m_openedFileName.lpstrInitialDir = NULL;
	m_openedFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	GetOpenFileName(&m_openedFileName);

	if (_tcslen(fileName) == 0)
	{
		return L"";
	}

	ZeroMemory(&m_openedFileName, sizeof(m_openedFileName));

	return fileName;
}

std::wstring Elixir::FileIO::GetExePathW()
{
	TCHAR result[MAX_PATH];

	return std::wstring(result, GetModuleFileName(NULL, result, MAX_PATH));
}

std::string Elixir::FileIO::GetExePath()
{
	auto result = GetExePathW();
	
	return ws2s(result);
}

void Elixir::FileIO::CurrentSceneChanged()
{
	m_currentScenePath = "";
}

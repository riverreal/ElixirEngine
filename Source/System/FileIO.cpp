#include "FileIO.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace Elixir;

FileIO::FileIO()
{

}

FileIO::~FileIO()
{

}

void FileIO::SaveMaterial(Material& mat)
{
	std::fstream file;
	std::string path = "Resources/Materials/";
	file.open(path + mat.name + FileTypeExtension[FILE_MATERIAL], std::fstream::in | std::fstream::out | std::fstream::app | std::fstream::trunc);

	file << "<name>" + mat.name + "</name>\n";
	file << "<albedo>" + std::to_string(mat.albedo) + "</albedo>\n";
	file << "<normal>" + std::to_string(mat.normal) + "</normal>\n";
	file << "<roughness>" + std::to_string(mat.roughness) + "</roughness>\n";
	file << "<metallic>" + std::to_string(mat.metallic) + "</metallic>\n";

	file.close();
}
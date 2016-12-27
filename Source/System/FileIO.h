#pragma once

#include "../Helper/TypeHelper.h"

namespace Elixir
{
	class FileIO
	{
	public:
		FileIO();
		~FileIO();

		void SaveMaterial(Elixir::Material& mat);

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

	private:

	private:

	};
}
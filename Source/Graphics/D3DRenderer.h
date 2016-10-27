#pragma once
#include "../Interface/Interfaces.h"

namespace radix
{
	namespace graphics
	{
		class D3DRenderer : public IRenderer
		{
		public:
			bool Initialize();
			void Draw();
			void Shutdown();
		};
	}
}



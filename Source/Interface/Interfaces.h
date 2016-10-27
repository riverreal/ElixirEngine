#pragma once

//---------------------------------
//Experimental Interface
//Not in use

namespace radix
{
	namespace graphics
	{
		class IRenderer
		{
		public:
			virtual bool Initialize() = 0;
			virtual void Draw() = 0;
			virtual void Shutdown() = 0;

			virtual ~IRenderer() {};

		};
	}
}
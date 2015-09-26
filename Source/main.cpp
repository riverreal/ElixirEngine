#include "System\BaseApp.h"

const int SCREEN_WIDTH = 800;
const int SCRENN_HEIGHT = 600;

class SimpleApp : public BaseApp
{
public:
	SimpleApp(HINSTANCE instance, int width, int height);
	~SimpleApp();

	bool Init() override;
private:

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SimpleApp myGame(hInstance, SCREEN_WIDTH, SCRENN_HEIGHT);

	if (!myGame.Init())
	{
		return 0;
	}

	myGame.Run();

	return 0;
}


SimpleApp::SimpleApp(HINSTANCE instance, int width, int height)
	:BaseApp(instance, width, height)
{
}

SimpleApp::~SimpleApp()
{
}

bool SimpleApp::Init()
{
	if (!BaseApp::Init())
	{
		return false;
	}
	//additional init

	return true;
}
#include "Includes/LESystem.h"
#include "Includes/LEGraphics.h"

//720p (1280 x 720)
//900p (1600 x 900)
//1080p (1920 x 1080)
const int SCREEN_WIDTH = 1280;
const int SCRENN_HEIGHT = 720;

class SimpleApp : public BaseApp
{
public:
	SimpleApp(HINSTANCE instance, int width, int height);
	~SimpleApp();

	bool Init() override;
	void Update(float dt) override;
	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;
	bool SceneInit();

private:
	Model* m_shapes;
	TextureManager m_texureManager;

	//Shader for sky
	SkyDome m_rendererSky;

	Scene* m_scene;

	//Objects
	Object* m_plane[3];
	Object* m_sky;
	Object* m_radixBox;
	Object* m_pumba;
	Object* m_ship;
	Object* m_statue;

	DirectX::XMMATRIX m_baseViewMatrix;

	POINT m_lastMousePos;
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
	m_rendererSky.Shutdown();
	m_shapes->Shutdown();

	SafeRelease(m_scene);
	SafeRelease(m_shapes);

	BlendState::Shutdown();
}

bool SimpleApp::SceneInit()
{
	m_shapes = new Model();
	m_scene = new Scene(m_shapes, m_projectionMatrix);

	//Object Init
	m_sky = new Object;
	m_radixBox = new Object;
	m_scene->AddObject(m_radixBox);
	m_pumba = new Object;
	m_scene->AddObject(m_pumba);
	m_ship = new Object;
	m_scene->AddObject(m_ship);
	m_statue = new Object;
	m_scene->AddObject(m_statue);
	//-----------------------------------------------------------------------------------------------------
	//        Texture Init
	//-----------------------------------------------------------------------------------------------------
	
	auto fullValueTexture = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/full-value.jpg");

	m_scene->SetIrradiance(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/Irradiance/Irradiance.dds"));
	m_scene->SetEnvMap(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/grasscube1024.dds"));

	for (UINT i = 0; i < 3; ++i)
	{
		m_plane[i] = new Object;
		m_scene->AddObject(m_plane[i]);
		m_plane[i]->SetTexTransformScale(1.0f, 1.0f, 1.0f);
		m_plane[i]->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/carbonfiber.jpg"), 0); //albedo
		m_plane[i]->SetTexture(fullValueTexture, 3); //roughness
		m_plane[i]->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/no-value2.jpg"), 4); //metallic
	}

	m_plane[0]->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/yellow.jpg"), 0);
	m_plane[0]->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/yellow2.jpg"), 3);
	m_plane[0]->SetTexture(fullValueTexture, 4);

	m_plane[2]->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/stone.jpg"), 0);
	m_plane[1]->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/low-value.jpg"), 3);

	m_radixBox->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/grass.dds"), 0);
	m_radixBox->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/semi-value.jpg"), 3);
	m_radixBox->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/no-value.jpg"), 4);
	//m_radixBox->SetDisabled(true);

	m_pumba->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/pumba.BMP"), 0);
	m_pumba->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/semi-value.jpg"), 3);
	m_pumba->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/no-value2.jpg"), 4);
	m_pumba->SetDisabled(true);

	m_statue->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/Couch_diff.jpg"), 0);
	m_statue->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/semi-value.jpg"), 3);
	m_statue->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/no-value2.jpg"), 4);
	m_statue->SetDisabled(true);

	m_ship->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/alduin.jpg"), 0);
	m_ship->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/high-value.jpg"), 3);
	m_ship->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/no-value2.jpg"), 4);
	m_ship->SetDisabled(true);

	m_sky->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/grasscube1024.dds"), 0);
	//-----------------------------------------------------------------------------------------------------
	//        Light Init
	//-----------------------------------------------------------------------------------------------------

	m_scene->GetLight()->CreatePointLight(1.0f, 3.0f, 5.0f, 1.0f, 0.3f, 0.3f, 1.0f);
	
	//-----------------------------------------------------------------------------------------------------
	//        Object Geometry Init
	//-----------------------------------------------------------------------------------------------------
	
	for (UINT i = 0; i < 3; ++i)
	{
		m_plane[i]->SetOffset(m_shapes->AddGeometry(MODEL_TYPE_SPHERE));
	}
	m_sky->SetOffset(m_shapes->AddGeometry(MODEL_TYPE_SPHERE));
	m_radixBox->SetOffset(m_shapes->AddGeometry(MODEL_TYPE_PLAIN));
	m_pumba->SetOffset(m_shapes->AddModelFromFile("Resources/Models/pumba.obj"));
	m_statue->SetOffset(m_shapes->AddModelFromFile("Resources/Models/couch.obj"));
	m_ship->SetOffset(m_shapes->AddModelFromFile("Resources/Models/alduin.obj"));

	//-----------------------------------------------------------------------------------------------------
	//        Object World Init
	//-----------------------------------------------------------------------------------------------------
	m_plane[0]->SetScale(1.0f, 1.0f, 1.0f);
	m_plane[1]->SetScale(1.0f, 1.0f, 1.0f);
	m_plane[2]->SetScale(1.0f, 1.0f, 1.0f);
	m_plane[0]->SetPosition(0.0f, 3.0f, 5.0f);
	m_plane[1]->SetPosition(3.0f, 3.0f, 5.0f);
	m_plane[2]->SetPosition(-3.0f, 3.0f, 5.0f);
	m_radixBox->SetPosition(0.0f, 1.0f, 5.0f);
	m_radixBox->SetScale(20.0f, 20.0f, 20.0f);
	//m_radixBox->SetTexTransformScale(30.0f, 30.0f, 3.0f);
	m_pumba->SetPosition(5.0f, 3.0f, 18.0f);
	m_pumba->SetScale(1.05f, 1.05f, 1.05f);
	m_statue->SetPosition(8.0f, 1.0f, 5.0f);
	m_statue->SetScale(1.0f, 1.0f, 1.0f);
	m_ship->SetPosition(1.0f, 3.0f, 18.0f);
	m_ship->SetScale(0.01f, 0.01f, 0.01f);
	m_ship->SetRotationY(90);
	m_sky->SetPosition(0.0f, 0.0f, 0.0f);
	m_sky->SetScale(30.0f, 30.0f, 30.0f);
	//-----------------------------------------------------------------------------------------------------
	//        Renderer Init
	//-----------------------------------------------------------------------------------------------------

	if (!m_shapes->Initialize(m_d3dDevice))
	{
		return false;
	}

	if (!m_rendererSky.Initialize(m_d3dDevice, m_hWnd))
	{
		return false;
	}

	m_scene->SceneReady();
	SetScene(m_scene);
	return true;
}

bool SimpleApp::Init()
{
	//Engine Init
	if (!BaseApp::Init())
	{
		return false;
	}

	//Enable blend state
	if (!BlendState::Init(m_d3dDevice))
	{
		return false;
	}

	//Additional init
	if (!SceneInit())
	{
		return false;
	}

	return true;
}

void SimpleApp::Update(float dt)
{
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_scene->GetCamera()->Walk(-10.0f*dt);
	}
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_scene->GetCamera()->Walk(10.0f*dt);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_scene->GetCamera()->Strafe(-10.0f*dt);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_scene->GetCamera()->Strafe(10.0f*dt);
	}
	if (GetAsyncKeyState('R') & 0x8000)
	{
		m_scene->GetCamera()->SetPosition(0.0f, 10.0f, 0.0f);
	}
	if (GetAsyncKeyState('C') & 0x8000)
	{
	}
	if (GetAsyncKeyState('J') & 0x8000)
	{	
	}
	if (GetAsyncKeyState('1') & 0x8000)
	{
		//m_fog.Enabled = true;
	}
	if (GetAsyncKeyState('2') & 0x8000)
	{
		//m_fog.Enabled = false;
	}


	m_scene->UpdateScene();
	
}

void SimpleApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_lastMousePos.x = x;
	m_lastMousePos.y = y;

	SetCapture(m_hWnd);
}

void SimpleApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void SimpleApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.15f*static_cast<float>(x - m_lastMousePos.x));
		float dy = XMConvertToRadians(0.15f*static_cast<float>(y - m_lastMousePos.y));

		m_scene->GetCamera()->Pitch(dy);
		m_scene->GetCamera()->RotateY(dx);
	}

	m_lastMousePos.x = x;
	m_lastMousePos.y = y;
}
#include "Includes/LESystem.h"
#include "Includes/LEGraphics.h"
#include <stdlib.h>
#include <time.h>  
//720p (1280 x 720)
//900p (1600 x 900)
//1080p (1920 x 1080)
const int SCREEN_WIDTH = 1280;
const int SCRENN_HEIGHT = 720;

using namespace radix;

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
	//Object* m_plane[3];
	//Object* m_sky;
	Object* m_radixBox;
	//Object* m_pumba;
	//Object* m_ship;
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

	//PBR Test Balls
	Object* metalBalls[11];
	Object* dielectricBalls[11];

	ID3D11ShaderResourceView* roughnessArray[11];
	roughnessArray[0] = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/balls/0.png");
	roughnessArray[1] = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/balls/25.png");
	roughnessArray[2] = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/balls/50.png");
	roughnessArray[3] = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/balls/75.png");
	roughnessArray[4] = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/balls/100.png");
	roughnessArray[5] = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/balls/125.png");
	roughnessArray[6] = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/balls/150.png");
	roughnessArray[7] = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/balls/175.png");
	roughnessArray[8] = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/balls/200.png");
	roughnessArray[9] = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/balls/225.png");
	roughnessArray[10] = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/balls/255.png");

	auto ballColor = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/yellow.jpg");
	auto metallic = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/full-value.jpg");
	auto dielectric = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/no-value.jpg");

	for (int i = 1; i < 11; ++i)
	{
		//metals
		metalBalls[i] = new Object();
		m_scene->AddObject(metalBalls[i]);
		metalBalls[i]->SetTexture(ballColor, TEXTURE_TYPE::ALBEDO);
		metalBalls[i]->SetTexture(roughnessArray[i], TEXTURE_TYPE::ROUGHNESS);
		metalBalls[i]->SetTexture(metallic, TEXTURE_TYPE::METALLIC);
		metalBalls[i]->SetDynamic(false);
		metalBalls[i]->SetOffset(m_shapes->AddGeometry(MODEL_TYPE_SPHERE));
		metalBalls[i]->SetPositionZ(i * 1.5f);
		metalBalls[i]->SetPositionY(3.0f);
		metalBalls[i]->SetTag(1 + i);
		metalBalls[i]->SetScale(1.0f, 1.0f, 1.0f);

		//dielectrics
		dielectricBalls[i] = new Object();
		m_scene->AddObject(dielectricBalls[i]);
		dielectricBalls[i]->SetTexture(ballColor, TEXTURE_TYPE::ALBEDO);
		dielectricBalls[i]->SetTexture(roughnessArray[i], TEXTURE_TYPE::ROUGHNESS);
		dielectricBalls[i]->SetTexture(dielectric, TEXTURE_TYPE::METALLIC);
		dielectricBalls[i]->SetDynamic(false);
		dielectricBalls[i]->SetOffset(m_shapes->AddGeometry(MODEL_TYPE_SPHERE));
		dielectricBalls[i]->SetPositionX(1.5f);
		dielectricBalls[i]->SetPositionZ(i * 1.5f);
		dielectricBalls[i]->SetTag(20 + i);
		dielectricBalls[i]->SetPositionY(3.0f);
		dielectricBalls[i]->SetScale(1.0f, 1.0f, 1.0f);
	}

	//Object Init
	m_radixBox = new Object;
	m_scene->AddObject(m_radixBox);
	m_statue = new Object;
	m_scene->AddObject(m_statue);

	//walls
	Object* walls[5];
	for (U32 i = 0; i < 5; ++i)
	{
		walls[i] = new Object;
		m_scene->AddObject(walls[i]);
		walls[i]->SetName("wall" + std::to_string(i));
		walls[i]->SetTexture(roughnessArray[10], TEXTURE_TYPE::ALBEDO);
		walls[i]->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
		walls[i]->SetTexture(roughnessArray[8], TEXTURE_TYPE::ROUGHNESS);
		walls[i]->SetDynamic(false);
		walls[i]->SetOffset(m_shapes->AddGeometry(MODEL_TYPE_CUBE));
	}

	walls[4]->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/wood.jpg"), TEXTURE_TYPE::ALBEDO);
	walls[4]->SetTexTransformScale(5.0f, 2.0f, 1.0f);
	walls[4]->SetTexture(roughnessArray[3], TEXTURE_TYPE::ROUGHNESS);

	//left wall
	walls[0]->SetScale(0.4f, 8.0f, 8.0f);
	//right wall
	walls[1]->SetScale(0.4f, 8.0f, 8.0f);
	//large wall
	walls[2]->SetScale(15.0f, 8.0f, 0.4f);
	//ceil
	walls[3]->SetScale(15.0f, 0.4f, 8.0f);
	//floor
	walls[4]->SetScale(15.0f, 0.4f, 8.0f);

	float offsetX = 1.0f;
	float offsetY = 8.0f;
	float offsetZ = 30.0f;

	walls[0]->SetPosition(offsetX - (walls[2]->GetScaleX() * 0.5f + (walls[2]->GetScaleZ() * 0.5f)), offsetY, offsetZ);
	walls[1]->SetPosition(offsetX + (walls[2]->GetScaleX() * 0.5f + (walls[2]->GetScaleZ() * 0.5f)), offsetY, offsetZ);
	walls[2]->SetPosition(offsetX, offsetY, offsetZ + (walls[0]->GetScaleX() * 0.5f + (walls[0]->GetScaleZ() * 0.5f)));
	walls[3]->SetPosition(offsetX, offsetY + (walls[0]->GetScaleX() * 0.5f + (walls[0]->GetScaleZ() * 0.5f)), offsetZ);
	walls[4]->SetPosition(offsetX, offsetY - (walls[0]->GetScaleX() * 0.5f + (walls[0]->GetScaleZ() * 0.5f)), offsetZ);

	//center table
	Object* centerTable = new Object;
	m_scene->AddObject(centerTable);
	centerTable->SetOffset(m_shapes->AddModelFromFile("Resources/Models/table.obj"));
	centerTable->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/afrormosia.jpg"), TEXTURE_TYPE::ALBEDO);
	centerTable->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	centerTable->SetTexture(roughnessArray[4], TEXTURE_TYPE::ROUGHNESS);
	centerTable->SetPosition(offsetX, offsetY - 4.0f, offsetZ);
	centerTable->SetName("centerTable");
	centerTable->SetScale(2.5f, 2.5f, 2.5f);
	centerTable->SetRotationY(37.0f);

	//flower vase without flower
	Object* vase = new Object;
	m_scene->AddObject(vase);
	vase->SetOffset(m_shapes->AddModelFromFile("Resources/Models/vase.obj"));
	vase->SetTexture(roughnessArray[6], TEXTURE_TYPE::ALBEDO);
	vase->SetTexture(roughnessArray[10], TEXTURE_TYPE::METALLIC);
	vase->SetTexture(roughnessArray[5], TEXTURE_TYPE::ROUGHNESS);
	vase->SetPosition(offsetX, offsetY - 3.02f, offsetZ - 0.3f);
	vase->SetName("vase");
	vase->SetScale(2.0f, 2.0f, 2.0f);

	auto brushedAlum = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/brushed_alum.jpg");
	//spot light models
	Object* spots[3];
	for (U32 i = 0; i < 3; ++i)
	{
		spots[i] = new Object();
		m_scene->AddObject(spots[i]);
		spots[i]->SetOffset(m_shapes->AddModelFromFile("Resources/Models/spot.obj"));
		spots[i]->SetTexture(brushedAlum, TEXTURE_TYPE::ALBEDO);
		spots[i]->SetTexture(roughnessArray[10], TEXTURE_TYPE::METALLIC);
		spots[i]->SetTexture(roughnessArray[5], TEXTURE_TYPE::ROUGHNESS);
		spots[i]->SetPosition(offsetX, offsetY + walls[2]->GetScaleY() * 0.5f, offsetZ + walls[4]->GetScaleZ() * 0.48f);
		spots[i]->SetScale(0.1f, 0.1f, 0.1f);
		spots[i]->SetRotationY(-90);
	}

	spots[1]->SetPositionX(offsetX - 3.0f);
	spots[2]->SetPositionX(offsetX + 3.0f);

	auto noicedIron = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/noiced_iron.jpg");
	//Point light models
	Object* points[2];
	for (U32 i = 0; i < 2; ++i)
	{
		points[i] = new Object();
		m_scene->AddObject(points[i]);
		points[i]->SetOffset(m_shapes->AddModelFromFile("Resources/Models/point.obj"));
		points[i]->SetTexture(roughnessArray[10], TEXTURE_TYPE::ALBEDO);
		points[i]->SetTexture(roughnessArray[10], TEXTURE_TYPE::METALLIC);
		points[i]->SetTexture(roughnessArray[5], TEXTURE_TYPE::ROUGHNESS);
		int side = (i * 2) - 1;
		points[i]->SetPosition(offsetX + 3.0f * side, offsetY + walls[2]->GetScaleY() * 0.5f, offsetZ);
		points[i]->SetScale(1.5f, 1.5f, 1.5f);
	}

	Object* frame;
	frame = new Object();
	m_scene->AddObject(frame);
	frame->SetOffset(m_shapes->AddModelFromFile("Resources/Models/frame.obj"));
	frame->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/afrormosia.jpg"), TEXTURE_TYPE::ALBEDO);
	frame->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	frame->SetTexture(roughnessArray[3], TEXTURE_TYPE::ROUGHNESS);
	frame->SetPosition(offsetX, offsetY + 0.5f, offsetZ + walls[4]->GetScaleZ() * 0.49f);
	frame->SetScale(0.4f, 0.4f, 0.4f);
	frame->SetRotationZ(90);
	frame->SetRotationY(180);


	Object* drawing = new Object();
	m_scene->AddObject(drawing);
	drawing->SetOffset(m_shapes->AddGeometry(MODEL_TYPE_PLAIN));
	drawing->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/drawing.jpg"), TEXTURE_TYPE::ALBEDO);
	drawing->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	drawing->SetTexture(roughnessArray[9], TEXTURE_TYPE::ROUGHNESS);
	drawing->SetPosition(frame->GetPosition());
	drawing->SetRotationX(-90);
	drawing->SetScale(2.0f, 1.0f, 1.4f);

	Object* carpet = new Object();
	m_scene->AddObject(carpet);
	carpet->SetOffset(m_shapes->AddGeometry(MODEL_TYPE_PLAIN));
	carpet->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/carpet.jpg"), TEXTURE_TYPE::ALBEDO);
	carpet->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	carpet->SetTexture(roughnessArray[8], TEXTURE_TYPE::ROUGHNESS);
	carpet->SetPosition(offsetX, offsetY - (walls[2]->GetScaleY() * 0.4999f), offsetZ + 1.0f);
	carpet->SetScale(6.0f, 1.0f, 7.0f);
	carpet->SetRotationY(90);

	//-----------------------------------------------------------------------------------------------------
	//        Texture Init
	//-----------------------------------------------------------------------------------------------------
	
	auto fullValueTexture = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/full-value.jpg");

	m_scene->SetIrradiance(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/Irradiance/Irradiance.dds"));
	m_scene->SetEnvMap(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/desertcube1024.dds"));

	m_radixBox->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/desert_ground.jpg"), 0);
	m_radixBox->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/full-value.jpg"), 3);
	m_radixBox->SetTexture(dielectric, 4);
	m_radixBox->SetDisabled(true);

	m_statue->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/Couch_diff.jpg"), 0);
	m_statue->SetTexture(roughnessArray[8], 3);
	m_statue->SetTexture(dielectric, 4);
	m_statue->SetDisabled(false);

	//-----------------------------------------------------------------------------------------------------
	//        Light Init
	//-----------------------------------------------------------------------------------------------------

	//m_scene->GetLight()->CreatePointLight(1.0f, 3.0f, 5.0f, 1.0f, 0.3f, 0.3f, 1.0f);
	auto dirLight = m_scene->GetLight()->GetDirectionalLight();
	dirLight.LightIntensity[0] = 0.0f;
	dirLight.LightIntensity[1] = 0.001f;
	dirLight.LightColor[0] = 1.0;
	dirLight.LightColor[1] = 0.976;
	dirLight.LightColor[2] = 0.89;
	m_scene->GetLight()->SetDirectionalLight(dirLight);

	srand(time(NULL));
	
	auto lighting = m_scene->GetLight();
	
	for (U32 i = 0; i < 0; ++i)
	{
		int x = rand() % 10;
		int y = rand() % 10;
		int z = rand() % 4;
		float r = static_cast<float>(rand() % 255) / 255.0f;
		float g = static_cast<float>(rand() % 255) / 255.0f;
		float b = static_cast<float>(rand() % 255) / 255.0f;
		int range = rand() % 50 + 1;

		lighting->CreatePointLight(x, y, z, r, g, b, range);
	}
	
	lighting->CreatePointLight(offsetX - 3.0f, offsetY + walls[2]->GetScaleY() * 0.47f, offsetZ, 1.0f, 1.0f, 1.0f, 20.0f);
	lighting->CreatePointLight(offsetX + 3.0f, offsetY + walls[2]->GetScaleY() * 0.47f, offsetZ, 1.0f, 1.0f, 1.0f, 20.0f);

	PBRSpotLight spotLight;
	spotLight.Position = XMFLOAT3(offsetX, offsetY + walls[2]->GetScaleY() * 0.5f, offsetZ + walls[4]->GetScaleZ() * 0.48f);
	spotLight.LightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 2.0f);
	spotLight.Range = 40.0f;
	spotLight.Spot = 15.0f;
	spotLight.Direction = XMFLOAT3(0.0f, -1.0f, 0.2f);
	spotLight.Att = XMFLOAT3(1.0f, 0.01f, 0.001f);
	lighting->CreateSpotLight(spotLight);

	spotLight.Position = XMFLOAT3(offsetX - 3.0f, offsetY + walls[2]->GetScaleY() * 0.5f, offsetZ + walls[4]->GetScaleZ() * 0.48f);
	lighting->CreateSpotLight(spotLight);

	spotLight.Position = XMFLOAT3(offsetX + 3.0f, offsetY + walls[2]->GetScaleY() * 0.5f, offsetZ + walls[4]->GetScaleZ() * 0.48f);
	lighting->CreateSpotLight(spotLight);

	for (U32 i = 0; i < 0; ++i)
	{
		PBRSpotLight spotLight;
		int x = rand() % 10 - 10;
		int y = rand() % 10 + 1;
		int z = rand() % 10 - 5;
		spotLight.Position = XMFLOAT3(x, y, z);
		float r = static_cast<float>(rand() % 255) / 255.0f;
		float g = static_cast<float>(rand() % 255) / 255.0f;
		float b = static_cast<float>(rand() % 255) / 255.0f;
		spotLight.LightColor = XMFLOAT4(r, g, b, 1.0f);
		spotLight.Range = rand() % 50 + 1000;
		spotLight.Spot = rand() % 40 + 245;
		spotLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
		spotLight.Att = XMFLOAT3(1.0f, 0.01f, 0.001f);

		lighting->CreateSpotLight(spotLight);
	}

	//-----------------------------------------------------------------------------------------------------
	//        Object Geometry Init
	//-----------------------------------------------------------------------------------------------------
	
	m_radixBox->SetOffset(m_shapes->AddGeometry(MODEL_TYPE_PLAIN));
	m_statue->SetOffset(m_shapes->AddModelFromFile("Resources/Models/couch.obj"));

	//-----------------------------------------------------------------------------------------------------
	//        Object World Init
	//-----------------------------------------------------------------------------------------------------
	m_radixBox->SetPosition(0.0f, 1.0f, 5.0f);
	m_radixBox->SetScale(80.0f, 80.0f, 80.0f);
	m_radixBox->SetTexTransformScale(10.0f, 10.0f, 3.0f);
	m_statue->SetPosition(offsetX, offsetY - 4.0f, offsetZ + 3.0f);
	m_statue->SetScale(1.0f, 1.0f, 1.0f);
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
		if (m_scene->GetLight()->GetPointLight(0).LightColor.w == 0.0f)
		{
			m_scene->GetLight()->GetModPointLight(0)->LightColor.w = 0.5f;
			m_scene->GetLight()->GetModPointLight(1)->LightColor.w = 0.5f;
			m_scene->GetLight()->GetModSpotLight(0)->LightColor.w = 2.0f;
			m_scene->GetLight()->GetModSpotLight(1)->LightColor.w = 2.0f;
			m_scene->GetLight()->GetModSpotLight(2)->LightColor.w = 2.0f;
		}
		else
		{
			m_scene->GetLight()->GetModPointLight(0)->LightColor.w = 0.0f;
			m_scene->GetLight()->GetModPointLight(1)->LightColor.w = 0.0f;
			m_scene->GetLight()->GetModSpotLight(0)->LightColor.w = 0.0f;
			m_scene->GetLight()->GetModSpotLight(1)->LightColor.w = 0.0f;
			m_scene->GetLight()->GetModSpotLight(2)->LightColor.w = 0.0f;
		}
		
	}
	if (GetAsyncKeyState('K') & 0x8000)
	{
		for (int i = 1; i < 11; ++i)
		{
			m_scene->GetObjectByTag(1 + i)->SetDisabled(false);
			m_scene->GetObjectByTag(20 + i)->SetDisabled(false);
			m_radixBox->SetDisabled(false);
		}

		m_scene->GetObjectByName("wall0")->SetDisabled(true);
		m_scene->GetObjectByName("wall1")->SetDisabled(true);
		m_scene->GetObjectByName("wall2")->SetDisabled(true);
		m_scene->GetObjectByName("wall3")->SetDisabled(true);
		m_scene->GetObjectByName("wall4")->SetDisabled(true);
		m_scene->GetObjectByName("vase")->SetDisabled(true);
		m_scene->GetObjectByName("centerTable")->SetDisabled(true);
		m_statue->SetDisabled(true);
	}

	if (GetAsyncKeyState('J') & 0x8000)
	{
		for (int i = 1; i < 11; ++i)
		{
			m_scene->GetObjectByTag(1 + i)->SetDisabled(true);
			m_scene->GetObjectByTag(20 + i)->SetDisabled(true);
			m_radixBox->SetDisabled(true);
		}

		m_scene->GetObjectByName("wall0")->SetDisabled(false);
		m_scene->GetObjectByName("wall1")->SetDisabled(false);
		m_scene->GetObjectByName("wall2")->SetDisabled(false);
		m_scene->GetObjectByName("wall3")->SetDisabled(false);
		m_scene->GetObjectByName("wall4")->SetDisabled(false);
		m_scene->GetObjectByName("vase")->SetDisabled(false);
		m_scene->GetObjectByName("centerTable")->SetDisabled(false);
		m_statue->SetDisabled(false);
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
#include "Includes/LESystem.h"
#include "Includes/LEGraphics.h"
#include <stdlib.h>
#include <time.h>
//16:9 resolutions
//480p - WVGA (854 x 480)
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
	void CreateTree();

private:
	Model* m_shapes;
	TextureManager m_texureManager;

	Camera m_smoothCamera;

	//Shader for sky
	SkyDome m_rendererSky;

	Scene* m_scene;
	
	//Objects
	Object* m_radixBox;
	Object* m_statue;
	ID3D11ShaderResourceView* m_treeRoughness;
	ID3D11ShaderResourceView* m_noMetal;
	ID3D11ShaderResourceView* m_cleanNormal;
	ID3D11ShaderResourceView* m_treeAlbedo;

	DirectX::XMMATRIX m_baseViewMatrix;
	float m_speedMult;

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

	m_speedMult = 1.0f;

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
	m_treeRoughness = roughnessArray[8];
	m_noMetal = roughnessArray[0];
	m_treeAlbedo = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/autumn.png");
	m_cleanNormal = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/cleanNormal.jpg");
	m_scene->SetIrradiance(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/Irradiance/Irradiance.dds"));
	m_scene->SetEnvMap(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/dayCube.dds"));

	/*
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

	auto ballColor = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/red.jpg");
	auto metallic = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/full-value.jpg");
	auto dielectric = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/no-value.jpg");

	ID3D11ShaderResourceView* testNormal = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/noNormal.jpg");
	ID3D11ShaderResourceView* wallNormal = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/wallNormal.jpg");

	for (int i = 1; i < 11; ++i)
	{
		//metals
		metalBalls[i] = new Object();
		m_scene->AddObject(metalBalls[i]);
		metalBalls[i]->SetTexture(ballColor, TEXTURE_TYPE::ALBEDO);
		metalBalls[i]->SetTexture(roughnessArray[i], TEXTURE_TYPE::ROUGHNESS);
		metalBalls[i]->SetTexture(metallic, TEXTURE_TYPE::METALLIC);
		metalBalls[i]->SetTexture(testNormal, TEXTURE_TYPE::NORMAL);
		metalBalls[i]->SetDynamic(false);
		metalBalls[i]->SetOffset(m_shapes->AddModelFromFile("Resources/Models/unreal.obj"));
		metalBalls[i]->SetPositionZ(i * 1.5f);
		metalBalls[i]->SetPositionY(3.0f);
		metalBalls[i]->SetTag(1 + i);
		metalBalls[i]->SetScale(0.005f, 0.005f, 0.005f);

		//dielectrics
		dielectricBalls[i] = new Object();
		m_scene->AddObject(dielectricBalls[i]);
		dielectricBalls[i]->SetTexture(ballColor, TEXTURE_TYPE::ALBEDO);
		dielectricBalls[i]->SetTexture(roughnessArray[i], TEXTURE_TYPE::ROUGHNESS);
		dielectricBalls[i]->SetTexture(dielectric, TEXTURE_TYPE::METALLIC);
		dielectricBalls[i]->SetTexture(testNormal, TEXTURE_TYPE::NORMAL);
		dielectricBalls[i]->SetDynamic(false);
		dielectricBalls[i]->SetOffset(m_shapes->AddModelFromFile("Resources/Models/unreal.obj"));
		dielectricBalls[i]->SetPositionX(1.5f);
		dielectricBalls[i]->SetPositionZ(i * 1.5f);
		dielectricBalls[i]->SetTag(20 + i);
		dielectricBalls[i]->SetPositionY(3.0f);
		dielectricBalls[i]->SetScale(0.005f, 0.005f, 0.005f);
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
		walls[i]->SetTexture(wallNormal, TEXTURE_TYPE::NORMAL);
		walls[i]->SetDynamic(false);
		walls[i]->SetOffset(m_shapes->AddGeometry(MODEL_TYPE_CUBE));
	}

	walls[4]->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/mahogfloor/oakfloor_basecolor.png"), TEXTURE_TYPE::ALBEDO);
	walls[4]->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/mahogfloor/mahogfloor_normal.png"), TEXTURE_TYPE::NORMAL);
	//walls[4]->SetTexture(roughnessArray[8], TEXTURE_TYPE::ROUGHNESS);
	walls[4]->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/mahogfloor/mahogfloor_roughness.png"), TEXTURE_TYPE::ROUGHNESS);
	walls[4]->SetTexTransformScale(5.0f, 2.0f, 1.0f);
	//walls[4]->SetTexture(roughnessArray[3], TEXTURE_TYPE::ROUGHNESS);

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
	centerTable->SetTexture(testNormal, TEXTURE_TYPE::NORMAL);
	centerTable->SetPosition(offsetX, offsetY - 4.0f, offsetZ);
	centerTable->SetName("centerTable");
	centerTable->SetScale(2.5f, 2.5f, 2.5f);
	centerTable->SetRotationY(37.0f);

	//flower vase without flower
	Object* vase = new Object();
	m_scene->AddObject(vase);
	vase->SetOffset(m_shapes->AddModelFromFile("Resources/Models/vase.obj"));
	vase->SetTexture(roughnessArray[6], TEXTURE_TYPE::ALBEDO);
	vase->SetTexture(roughnessArray[10], TEXTURE_TYPE::METALLIC);
	vase->SetTexture(roughnessArray[5], TEXTURE_TYPE::ROUGHNESS);
	vase->SetTexture(testNormal, TEXTURE_TYPE::NORMAL);
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
		spots[i]->SetTexture(testNormal, TEXTURE_TYPE::NORMAL);
		spots[i]->SetPosition(offsetX, offsetY + walls[2]->GetScaleY() * 0.5f, offsetZ + walls[4]->GetScaleZ() * 0.48f);
		spots[i]->SetScale(0.1f, 0.1f, 0.1f);
		spots[i]->SetRotationY(-90);
		spots[i]->SetName("spots" + std::to_string(i));
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
		points[i]->SetTexture(testNormal, TEXTURE_TYPE::NORMAL);
		int side = (i * 2) - 1;
		points[i]->SetPosition(offsetX + 3.0f * side, offsetY + walls[2]->GetScaleY() * 0.5f, offsetZ);
		points[i]->SetScale(1.5f, 1.5f, 1.5f);
		points[i]->SetName("points" + std::to_string(i));
	}

	Object* frame;
	frame = new Object();
	m_scene->AddObject(frame);
	frame->SetName("frame");
	frame->SetOffset(m_shapes->AddModelFromFile("Resources/Models/frame.obj"));
	frame->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/afrormosia.jpg"), TEXTURE_TYPE::ALBEDO);
	frame->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	frame->SetTexture(roughnessArray[3], TEXTURE_TYPE::ROUGHNESS);
	frame->SetTexture(testNormal, TEXTURE_TYPE::NORMAL);
	frame->SetPosition(offsetX, offsetY + 0.5f, offsetZ + walls[4]->GetScaleZ() * 0.49f);
	frame->SetScale(0.4f, 0.4f, 0.4f);
	frame->SetRotationZ(90);
	frame->SetRotationY(180);

	Object* drawing = new Object();
	m_scene->AddObject(drawing);
	drawing->SetName("drawing");
	drawing->SetOffset(m_shapes->AddGeometry(MODEL_TYPE_PLAIN));
	drawing->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/drawing.jpg"), TEXTURE_TYPE::ALBEDO);
	drawing->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	drawing->SetTexture(roughnessArray[9], TEXTURE_TYPE::ROUGHNESS);
	drawing->SetTexture(testNormal, TEXTURE_TYPE::NORMAL);
	drawing->SetPosition(frame->GetPosition());
	drawing->SetRotationX(-90);
	drawing->SetScale(2.0f, 1.0f, 1.4f);

	Object* carpet = new Object();
	m_scene->AddObject(carpet);
	carpet->SetName("carpet");
	carpet->SetOffset(m_shapes->AddGeometry(MODEL_TYPE_PLAIN));
	carpet->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/carpet.jpg"), TEXTURE_TYPE::ALBEDO);
	carpet->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	carpet->SetTexture(roughnessArray[8], TEXTURE_TYPE::ROUGHNESS);
	carpet->SetTexture(testNormal, TEXTURE_TYPE::NORMAL);
	carpet->SetPosition(offsetX, offsetY - (walls[2]->GetScaleY() * 0.4999f), offsetZ + 1.0f);
	carpet->SetScale(6.0f, 1.0f, 7.0f);
	carpet->SetRotationY(90);

	Object* moon = new Object();
	m_scene->AddObject(moon);
	moon->SetName("moon");
	moon->SetOffset(m_shapes->AddGeometry(MODEL_TYPE_SPHERE));
	moon->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/moonAlbedo.jpg"), TEXTURE_TYPE::ALBEDO);
	moon->SetTexture(roughnessArray[8], TEXTURE_TYPE::ROUGHNESS);
	moon->SetTexture(testNormal, TEXTURE_TYPE::NORMAL);
	moon->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	moon->SetPosition(10.0f, 10.0f, 10.0f);
	moon->SetScale(40.0f, 40.0f, 40.0f);
	moon->SetDisabled(true);

	//-----------------------------------------------------------------------------------------------------
	//        Texture Init
	//-----------------------------------------------------------------------------------------------------
	
	auto fullValueTexture = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/full-value.jpg");

	m_scene->SetIrradiance(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/Irradiance/Irradiance.dds"));
	m_scene->SetEnvMap(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/grasscube1024.dds"));
	
	//m_radixBox->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/mud/pjDto2_2K_Albedo.jpg"), 0);
	//m_radixBox->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/mud/mudNormal.png"), TEXTURE_TYPE::NORMAL);
	//m_radixBox->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/mud/pjDto2_2K_Roughness.jpg"), TEXTURE_TYPE::ROUGHNESS);
	
	m_radixBox->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/grass/GroundForest003_COL_VAR2_1K.jpg"), 0);
	m_radixBox->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/grass/GroundForest003_NRM_1K.jpg"), TEXTURE_TYPE::NORMAL);
	m_radixBox->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/grass/GroundForest003_ROUGHNESS_1K.jpg"), TEXTURE_TYPE::ROUGHNESS);
	m_radixBox->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	m_radixBox->SetDisabled(false);

	m_statue->SetTexture(roughnessArray[6], TEXTURE_TYPE::ALBEDO);
	m_statue->SetTexTransformScale(3.0f, 3.0f, 3.0f);
	m_statue->SetTexture(roughnessArray[8], TEXTURE_TYPE::ROUGHNESS);
	m_statue->SetTexture(dielectric, TEXTURE_TYPE::METALLIC);
	m_statue->SetTexture(testNormal, TEXTURE_TYPE::NORMAL);
	m_statue->SetDisabled(false);

	//-----------------------------------------------------------------------------------------------------
	//        Light Init
	//-----------------------------------------------------------------------------------------------------
	std::srand(time(NULL));
	
	auto lighting = m_scene->GetLight();

	auto dirL = lighting->GetModDirectionalLight();
	dirL->LightIntensity[0] = 4.0f;
	dirL->LightIntensity[1] = 0.1f;
	dirL->LightColor[0] = 1.0f;
	dirL->Direction[0] = 0.3f;
	dirL->Direction[2] = -0.3f;
	dirL->LightColor[1] = 0.976f;
	dirL->LightColor[2] = 0.89f;

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
	spotLight.LightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 4.0f);
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
	m_radixBox->SetScale(50.0f, 50.0f, 50.0f);
	m_radixBox->SetTexTransformScale(5.0f, 5.0f, 5.0f);
	m_statue->SetPosition(offsetX, offsetY - 4.0f, offsetZ + 3.0f);
	m_statue->SetScale(1.0f, 1.0f, 1.0f);

	//Foliage Testing
	Object* plant = new Object();
	m_scene->AddObject(plant);
	plant->SetOffset(m_shapes->AddModelFromFile("Resources/Models/foliage/Fern_bush4.obj"));
	plant->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/foliage/map_d.png"), TEXTURE_TYPE::ALBEDO);
	plant->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	plant->SetTexture(roughnessArray[6], TEXTURE_TYPE::ROUGHNESS);
	plant->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/foliage/map_n.png"), TEXTURE_TYPE::NORMAL);
	plant->SetPosition(-1.0f, m_radixBox->GetPositionY(), 0.0f);
	plant->SetScale(0.08f, 0.08f, 0.08f);
	plant->SetName("fern");

	Object* pgRock = new Object();
	m_scene->AddObject(pgRock);
	pgRock->SetOffset(m_shapes->AddModelFromFile("Resources/Stones/rock_sandstone/Aset_rock_sandstone_S_plww4_LOD0.obj"));
	pgRock->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Stones/rock_sandstone/Aset_rock_sandstone_S_plww4_2K_Albedo_bright.jpg"), TEXTURE_TYPE::ALBEDO);
	pgRock->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	pgRock->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Stones/rock_sandstone/Aset_rock_sandstone_S_plww4_2K_Roughness.jpg"), TEXTURE_TYPE::ROUGHNESS);
	//pgRock->SetTexture(wallNormal, TEXTURE_TYPE::NORMAL);
	pgRock->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Stones/rock_sandstone/Aset_rock_sandstone_S_plww4_2K_Normal_LOD0_fix.jpg"), TEXTURE_TYPE::NORMAL);
	pgRock->SetPositionY(m_radixBox->GetPositionY());
	pgRock->SetPositionX(-3.0f);
	pgRock->SetScale(0.05f, 0.05f, 0.05f);
	pgRock->SetName("rock");

	auto woodLog = new Object();
	m_scene->AddObject(woodLog);
	woodLog->SetOffset(m_shapes->AddModelFromFile("Resources/woodLog/Aset_wood_log_M_qdtdP_LOD0.obj"));
	woodLog->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/woodLog/Aset_wood_log_M_qdtdP_2K_Albedo.jpg"), TEXTURE_TYPE::ALBEDO);
	woodLog->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	woodLog->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/woodLog/Aset_wood_log_M_qdtdP_2K_Roughness.jpg"), TEXTURE_TYPE::ROUGHNESS);
	woodLog->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/woodLog/woodNormal.jpg"), TEXTURE_TYPE::NORMAL);
	woodLog->SetPositionY(m_radixBox->GetPositionY());
	woodLog->SetPositionX(0.0f);
	woodLog->SetScale(0.05f, 0.05f, 0.05f);
	//woodLog->SetRotationZ(-90.0f);
	woodLog->SetRotationY(10);
	//woodLog->SetRotationX(5);
	woodLog->SetName("log");
	*/

	auto dirL = m_scene->GetLight()->GetModDirectionalLight();

	dirL->LightColor[0] = 1.0f;
	dirL->LightColor[1] = 0.76f;
	dirL->LightColor[2] = 0.21f;

	dirL->LightIntensity[0] = 2.8f;
	dirL->LightIntensity[1] = 0.1f;

	dirL->Direction[0] = 0.1f;
	dirL->Direction[1] = -0.6f;
	dirL->Direction[2] = -0.8f;

	m_scene->GetCamera()->SetPosition(0.0f, 7.0f, -6.0f);
	m_smoothCamera.SetPosition(0.0f, 7.0f, -6.0f);

	auto terrain = new Object();
	m_scene->AddObject(terrain);
	terrain->SetOffset(m_shapes->AddGeometry(MODEL_TYPE_PLAIN));
	/*
	terrain->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/mud/pjDto2_2K_Albedo.jpg"), TEXTURE_TYPE::ALBEDO);
	terrain->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/mud/mudNormal.png"), TEXTURE_TYPE::NORMAL);
	terrain->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/mud/pjDto2_2K_Roughness.jpg"), TEXTURE_TYPE::ROUGHNESS);
	*/
	terrain->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/forestGround/albedo.png"), TEXTURE_TYPE::ALBEDO);
	terrain->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/forestGround/normal.jpg"), TEXTURE_TYPE::NORMAL);
	terrain->SetTexture(roughnessArray[9], TEXTURE_TYPE::ROUGHNESS);
	terrain->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	terrain->SetTexTransformScale(2.0f, 2.0f, 2.0f);
	terrain->SetScale(10.0f, 10.0f, 10.0f);

	/*
	auto metalBall = new Object();
	m_scene->AddObject(metalBall);
	metalBall->SetOffset(m_shapes->AddModelFromFile("Resources/Models/unreal.obj"));
	metalBall->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/material/greasyMetal/albedo.png"), TEXTURE_TYPE::ALBEDO);
	metalBall->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/material/greasyMetal/normal1K.jpg"), TEXTURE_TYPE::NORMAL);
	metalBall->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/material/greasyMetal/roughness.jpg"), TEXTURE_TYPE::ROUGHNESS);
	metalBall->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/material/greasyMetal/metallic.png"), TEXTURE_TYPE::METALLIC);
	metalBall->SetScale(0.005f, 0.005f, 0.005f);

	auto brickBall = new Object();
	m_scene->AddObject(brickBall);
	brickBall->SetOffset(m_shapes->AddModelFromFile("Resources/Models/unreal.obj"));
	brickBall->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/material/blocks/albedo.png"), TEXTURE_TYPE::ALBEDO);
	brickBall->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/material/blocks/normal512.jpg"), TEXTURE_TYPE::NORMAL);
	brickBall->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/material/blocks/roughness.png"), TEXTURE_TYPE::ROUGHNESS);
	brickBall->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	brickBall->SetScale(0.005f, 0.005f, 0.005f);
	brickBall->SetPosition(0.0f, 0.0f, 2.0f);

	auto fabricBall = new Object();
	m_scene->AddObject(fabricBall);
	fabricBall->SetOffset(m_shapes->AddModelFromFile("Resources/Models/unreal.obj"));
	fabricBall->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/material/fabric/albedo.png"), TEXTURE_TYPE::ALBEDO);
	fabricBall->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/material/fabric/normal512.jpg"), TEXTURE_TYPE::NORMAL);
	fabricBall->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/material/fabric/roughness.png"), TEXTURE_TYPE::ROUGHNESS);
	fabricBall->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	fabricBall->SetScale(0.005f, 0.005f, 0.005f);
	fabricBall->SetPosition(0.0f, 0.0f, 6.0f);

	auto plasticBall = new Object();
	m_scene->AddObject(plasticBall);
	plasticBall->SetOffset(m_shapes->AddModelFromFile("Resources/Models/unreal.obj"));
	plasticBall->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/Textures/red.jpg"), TEXTURE_TYPE::ALBEDO);
	plasticBall->SetTexture(m_cleanNormal, TEXTURE_TYPE::NORMAL);
	plasticBall->SetTexture(roughnessArray[2], TEXTURE_TYPE::ROUGHNESS);
	plasticBall->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	plasticBall->SetScale(0.005f, 0.005f, 0.005f);
	plasticBall->SetPosition(0.0f, 0.0f, 4.0f);
	*/

	/*
	auto mountain01 = new Object();
	m_scene->AddObject(mountain01);
	mountain01->SetOffset(m_shapes->AddModelFromFile("Resources/blacksmith/mountain.obj"));
	mountain01->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/blacksmith/BridgeTerrain_01_Diff.jpg"), TEXTURE_TYPE::ALBEDO);
	mountain01->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/blacksmith/BridgeTerrain_01_NM.jpg"), TEXTURE_TYPE::NORMAL);
	mountain01->SetTexture(roughnessArray[8], TEXTURE_TYPE::ROUGHNESS);
	mountain01->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	//mountain01->SetScale(0.005f, 0.005f, 0.005f);
	mountain01->SetPosition(0.0f, -30.0f, 0.0f);
	*/

	auto grassText = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/foliage/grass/T_FieldGrass_01_D.png");
	srand(time(NULL));
	
	/*
	for (UINT i = 0; i < 10; ++i)
	{
		for (UINT j = 0; j < 10; ++j)
		{
			auto grass = new Object();
			m_scene->AddObject(grass);
			grass->SetBackFaceCulling(false);
			grass->SetOffset(m_shapes->AddModelFromFile("Resources/foliage/grass/grass.obj"));
			grass->SetTexture(grassText, TEXTURE_TYPE::ALBEDO);
			grass->SetTexture(m_cleanNormal, TEXTURE_TYPE::NORMAL);
			grass->SetTexture(roughnessArray[9], TEXTURE_TYPE::ROUGHNESS);
			grass->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
			grass->SetPositionX((i * 3) - 15.0f);
			grass->SetPositionZ((j * 3) - 15.0f);
			float rotation = rand() % 360;
			grass->SetRotationY(rotation);
		}
	}

	for (UINT i = 0; i < 10; ++i)
	{
		CreateTree();
	}
	*/
	
	auto cloverAlbedo = TextureLoader::CreateTexture(m_d3dDevice, L"Resources/foliage/clover/albedo.png");

	auto cloverOffsetX = -4.5f;
	auto cloverOffsetZ = -4.5f;

	for (UINT i = 0; i < 16; ++i)
	{
		for (UINT j = 0; j < 16; ++j)
		{
			if (i == 8 || i == 7)
			{

			}
			else
			{
				int percent = rand() % 101;
				//10% not showing
				if (percent < 90)
				{
					auto clover = new Object();
					m_scene->AddObject(clover);
					clover->SetOffset(m_shapes->AddModelFromFile("Resources/foliage/clover/cloverGroup.obj"));
					clover->SetTexture(cloverAlbedo, TEXTURE_TYPE::ALBEDO);
					clover->SetTexture(m_cleanNormal, TEXTURE_TYPE::NORMAL);
					clover->SetTexture(roughnessArray[5], TEXTURE_TYPE::ROUGHNESS);
					clover->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
					clover->SetPositionX((i * 0.6f) + cloverOffsetX);
					clover->SetPositionZ((j * 0.6f) + cloverOffsetZ);
					clover->SetScale(0.5f);
					float rotation = rand() % 360;
					clover->SetRotationY(rotation);
				}
			}
		}
	}
	

	auto fern = new Object();
	m_scene->AddObject(fern);
	fern->SetOffset(m_shapes->AddModelFromFile("Resources/foliage/fern/fern.obj"));
	fern->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/foliage/fern/T_Fern_01_D.png"), TEXTURE_TYPE::ALBEDO);
	fern->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/foliage/fern/T_Fern_01_N.jpg"), TEXTURE_TYPE::NORMAL);
	fern->SetTexture(roughnessArray[4], TEXTURE_TYPE::ROUGHNESS);
	fern->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	fern->SetPosition(3.2f, 0.0f, -3.3f);
	fern->SetRotationY(180);
	fern->SetScale(0.3f);
	fern->SetBackFaceCulling(false);

	auto fern2 = new Object();
	m_scene->AddObject(fern2);
	fern2->SetOffset(m_shapes->AddModelFromFile("Resources/foliage/fern/fern.obj"));
	fern2->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/foliage/fern/T_Fern_01_D.png"), TEXTURE_TYPE::ALBEDO);
	fern2->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/foliage/fern/T_Fern_01_N.jpg"), TEXTURE_TYPE::NORMAL);
	fern2->SetTexture(roughnessArray[4], TEXTURE_TYPE::ROUGHNESS);
	fern2->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	fern2->SetPosition(-2.7f, 0.0f, 0.4f);
	fern2->SetRotationY(120);
	fern2->SetScale(0.5f);
	fern2->SetBackFaceCulling(false);

	auto fern3 = new Object();
	m_scene->AddObject(fern3);
	fern3->SetOffset(m_shapes->AddModelFromFile("Resources/foliage/fern/fern.obj"));
	fern3->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/foliage/fern/T_Fern_01_D.png"), TEXTURE_TYPE::ALBEDO);
	fern3->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/foliage/fern/T_Fern_01_N.jpg"), TEXTURE_TYPE::NORMAL);
	fern3->SetTexture(roughnessArray[4], TEXTURE_TYPE::ROUGHNESS);
	fern3->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	fern3->SetPosition(-2.9f, 0.0f, -3.0f);
	fern3->SetRotationY(40);
	fern3->SetScale(0.45f);
	fern3->SetBackFaceCulling(false);

	auto fern4 = new Object();
	m_scene->AddObject(fern4);
	fern4->SetOffset(m_shapes->AddModelFromFile("Resources/foliage/fern/fern.obj"));
	fern4->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/foliage/fern/T_Fern_01_D.png"), TEXTURE_TYPE::ALBEDO);
	fern4->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/foliage/fern/T_Fern_01_N.jpg"), TEXTURE_TYPE::NORMAL);
	fern4->SetTexture(roughnessArray[4], TEXTURE_TYPE::ROUGHNESS);
	fern4->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	fern4->SetPosition(1.9f, 0.0f, 2.4f);
	fern4->SetRotationY(-80);
	fern4->SetScale(0.7f);
	fern4->SetBackFaceCulling(false);

	/*
	auto revealRock = new Object();
	m_scene->AddObject(revealRock);
	revealRock->SetOffset(m_shapes->AddModelFromFile("Resources/rocks/SM_GroundRevealRock002.OBJ"));
	revealRock->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/rocks/T_GroundRevealRock002_D_CC_R.png"), TEXTURE_TYPE::ALBEDO);
	revealRock->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/rocks/T_GroundRevealRock002_N.jpg"), TEXTURE_TYPE::NORMAL);
	revealRock->SetTexture(roughnessArray[9], TEXTURE_TYPE::ROUGHNESS);
	revealRock->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	revealRock->SetPositionX(10.0f);
	revealRock->SetPositionZ(10.0f);
	revealRock->SetScale(0.05f);
	*/
	auto boulder = new Object();
	m_scene->AddObject(boulder);
	boulder->SetOffset(m_shapes->AddModelFromFile("Resources/rocks/SM_Boulder05a.OBJ"));
	boulder->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/rocks/T_Boulder05_D_CC_R.png"), TEXTURE_TYPE::ALBEDO);
	boulder->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/rocks/T_Boulder05_N.jpg"), TEXTURE_TYPE::NORMAL);
	boulder->SetTexture(roughnessArray[9], TEXTURE_TYPE::ROUGHNESS);
	boulder->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	boulder->SetPosition(-3.0f, -0.3f, 3.0f);
	boulder->SetScale(0.05f);
	boulder->SetRotationY(-200);

	auto woodLog = new Object();
	m_scene->AddObject(woodLog);
	woodLog->SetOffset(m_shapes->AddModelFromFile("Resources/woodLog/Aset_wood_log_M_qdtdP_LOD1.obj"));
	woodLog->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/woodLog/Aset_wood_log_M_qdtdP_2K_Albedo.jpg"), TEXTURE_TYPE::ALBEDO);
	woodLog->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/woodLog/woodNormal.jpg"), TEXTURE_TYPE::NORMAL);
	woodLog->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/woodLog/Aset_wood_log_M_qdtdP_2K_Roughness.jpg"), TEXTURE_TYPE::ROUGHNESS);
	woodLog->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	woodLog->SetPosition(3.0f, 0.0f, -2.6f);
	woodLog->SetScale(0.05f);
	woodLog->SetRotationY(-20.0f);
	
	auto riverRock = new Object();
	m_scene->AddObject(riverRock);
	riverRock->SetOffset(m_shapes->AddModelFromFile("Resources/rocks/SM_River_Rock_01.OBJ"));
	riverRock->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/rocks/T_River_Rock_01_D_CC_R.png"), TEXTURE_TYPE::ALBEDO);
	riverRock->SetTexture(TextureLoader::CreateTexture(m_d3dDevice, L"Resources/rocks/T_River_Rock_01_N.jpg"), TEXTURE_TYPE::NORMAL);
	riverRock->SetTexture(roughnessArray[9], TEXTURE_TYPE::ROUGHNESS);
	riverRock->SetTexture(roughnessArray[0], TEXTURE_TYPE::METALLIC);
	riverRock->SetPosition(1.5f, -1.3f, 0.0f);
	riverRock->SetScale(0.05f);

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
		m_smoothCamera.Walk(-10.0f*dt * m_speedMult);
	}
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_smoothCamera.Walk(10.0f*dt* m_speedMult);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_smoothCamera.Strafe(-10.0f*dt * m_speedMult);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_smoothCamera.Strafe(10.0f*dt * m_speedMult);
	}
	if (GetAsyncKeyState('R') & 0x8000)
	{
		m_smoothCamera.SetPosition(0.0f, 10.0f, 0.0f);
	}
	if (GetAsyncKeyState('C') & 0x8000)
	{
		/*
		if (m_scene->GetLight()->GetPointLight(0).LightColor.w == 0.0f)
		{
			m_scene->GetLight()->GetModPointLight(0)->LightColor.w = 1.8f;
			m_scene->GetLight()->GetModPointLight(1)->LightColor.w = 1.8f;
			m_scene->GetLight()->GetModSpotLight(0)->LightColor.w = 10.0f;
			m_scene->GetLight()->GetModSpotLight(1)->LightColor.w = 10.0f;
			m_scene->GetLight()->GetModSpotLight(2)->LightColor.w = 10.0f;
		}
		else
		{
			m_scene->GetLight()->GetModPointLight(0)->LightColor.w = 0.0f;
			m_scene->GetLight()->GetModPointLight(1)->LightColor.w = 0.0f;
			m_scene->GetLight()->GetModSpotLight(0)->LightColor.w = 0.0f;
			m_scene->GetLight()->GetModSpotLight(1)->LightColor.w = 0.0f;
			m_scene->GetLight()->GetModSpotLight(2)->LightColor.w = 0.0f;
		}
		*/
		
	}
	if (GetAsyncKeyState('K') & 0x8000)
	{
		/*
		//-----------Disable Room
		m_scene->GetObjectByName("wall0")->SetDisabled(true);
		m_scene->GetObjectByName("wall1")->SetDisabled(true);
		m_scene->GetObjectByName("wall2")->SetDisabled(true);
		m_scene->GetObjectByName("wall3")->SetDisabled(true);
		m_scene->GetObjectByName("wall4")->SetDisabled(true);
		m_scene->GetObjectByName("vase")->SetDisabled(true);
		m_scene->GetObjectByName("centerTable")->SetDisabled(true);
		m_scene->GetObjectByName("carpet")->SetDisabled(true);
		m_scene->GetObjectByName("drawing")->SetDisabled(true);
		m_scene->GetObjectByName("frame")->SetDisabled(true);
		m_scene->GetObjectByName("points0")->SetDisabled(true);
		m_scene->GetObjectByName("points1")->SetDisabled(true);
		m_scene->GetObjectByName("spots0")->SetDisabled(true);
		m_scene->GetObjectByName("spots1")->SetDisabled(true);
		m_scene->GetObjectByName("spots2")->SetDisabled(true);
		m_statue->SetDisabled(true);

		//--------Enable field 
		m_scene->GetLight()->GetModDirectionalLight()->LightIntensity[0] = 1.0f;
		m_scene->GetLight()->GetModDirectionalLight()->LightIntensity[1] = 0.25f;

		for (int i = 1; i < 11; ++i)
		{
			m_scene->GetObjectByTag(1 + i)->SetDisabled(false);
			m_scene->GetObjectByTag(20 + i)->SetDisabled(false);
		}

		m_radixBox->SetDisabled(false);
		m_scene->GetObjectByName("fern")->SetDisabled(false);
		*/
	}

	if (GetAsyncKeyState('J') & 0x8000)
	{
		/*
		//----------Enable room
		m_scene->GetObjectByName("wall0")->SetDisabled(false);
		m_scene->GetObjectByName("wall1")->SetDisabled(false);
		m_scene->GetObjectByName("wall2")->SetDisabled(false);
		m_scene->GetObjectByName("wall3")->SetDisabled(false);
		m_scene->GetObjectByName("wall4")->SetDisabled(false);
		m_scene->GetObjectByName("vase")->SetDisabled(false);
		m_scene->GetObjectByName("centerTable")->SetDisabled(false);
		m_scene->GetObjectByName("carpet")->SetDisabled(false);
		m_scene->GetObjectByName("drawing")->SetDisabled(false);
		m_scene->GetObjectByName("frame")->SetDisabled(false);
		m_scene->GetObjectByName("points0")->SetDisabled(false);
		m_scene->GetObjectByName("points1")->SetDisabled(false);
		m_scene->GetObjectByName("spots0")->SetDisabled(false);
		m_scene->GetObjectByName("spots1")->SetDisabled(false);
		m_scene->GetObjectByName("spots2")->SetDisabled(false);
		m_statue->SetDisabled(false);

		//--------disable field
		m_scene->GetLight()->GetModDirectionalLight()->LightIntensity[0] = 0.0f;
		m_scene->GetLight()->GetModDirectionalLight()->LightIntensity[1] = 0.015f;
		for (int i = 1; i < 11; ++i)
		{
			m_scene->GetObjectByTag(1 + i)->SetDisabled(true);
			m_scene->GetObjectByTag(20 + i)->SetDisabled(true);
		}
		m_radixBox->SetDisabled(true);
		m_scene->GetObjectByName("fern")->SetDisabled(true);
		*/

		CreateTree();
		m_shapes->Shutdown();
		m_shapes->Initialize(m_d3dDevice);
		
	}
	if (GetAsyncKeyState('1') & 0x8000)
	{
		//m_scene->GetFog().Enabled = true;
	}
	if (GetAsyncKeyState('2') & 0x8000)
	{
		//m_fog.Enabled = false;
	}

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		m_speedMult = 4.0f;
	}
	else
	{
		m_speedMult = 1.0f;
	}

	m_scene->GetCamera()->SetPosition(MathHelper::lerp(m_scene->GetCamera()->GetPosition(), m_smoothCamera.GetPosition(), 0.3f));
	m_scene->GetCamera()->SetLook(MathHelper::lerp(m_scene->GetCamera()->GetLook(), m_smoothCamera.GetLook(), 0.3f));
	m_scene->GetCamera()->SetRight(MathHelper::lerp(m_scene->GetCamera()->GetRight(), m_smoothCamera.GetRight(), 0.3f));
	m_scene->GetCamera()->SetUp(MathHelper::lerp(m_scene->GetCamera()->GetUp(), m_smoothCamera.GetUp(), 0.3f));
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

		m_smoothCamera.Pitch(dy);
		m_smoothCamera.RotateY(dx);
	}

	m_lastMousePos.x = x;
	m_lastMousePos.y = y;
}

void SimpleApp::CreateTree()
{
	auto tree = new Object();
	m_scene->AddObject(tree);
	tree->SetOffset(m_shapes->AddModelFromFile("Resources/Models/trees/pin.obj"));
	tree->SetTexture(m_treeAlbedo, TEXTURE_TYPE::ALBEDO);
	tree->SetTexture(m_cleanNormal, TEXTURE_TYPE::NORMAL);
	tree->SetTexture(m_treeRoughness, TEXTURE_TYPE::ROUGHNESS);
	tree->SetTexture(m_noMetal, TEXTURE_TYPE::METALLIC);
	tree->SetBackFaceCulling(false);
	srand(time(NULL));
	float scaleXZ = rand() % 6 + 2;
	float scaleY = rand() % 6 + 2;
	tree->SetScale(scaleXZ, scaleY, scaleXZ);
	float posX = rand() % 50;
	posX -= 25.0f;
	float posZ = rand() % 50;
	posZ -= 25.0f;
	tree->SetPosition(posX, 0.0f, posZ);
	float rotation = rand() % 360;
	tree->SetRotationY(rotation);
}
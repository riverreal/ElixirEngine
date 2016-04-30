#include "Includes/LESystem.h"
#include "Includes/LEGraphics.h"

const int SCREEN_WIDTH = 1280;
const int SCRENN_HEIGHT = 720;

class SimpleApp : public BaseApp
{
public:
	SimpleApp(HINSTANCE instance, int width, int height);
	~SimpleApp();

	bool Init() override;
	void Update(float dt) override;
	void Draw() override;
	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;
	bool SceneInit();

private:
	Camera m_camera;
	Model m_shapes;
	//scene will be rendered as a 2d post processing
	OrthoManager m_ortho;

	TextureManager m_texureManager;
	//Light
	BasicLight m_basicLight;
	//Fog
	Fog m_fog;
	//buffer managers
	DeferredRendering* m_deferredBuffers;
	//Renderers

	//Light shader for forward rendering
	LightShader m_rendererLightShader;
	//Light Shader for deferred rendering
	DeferredLightShader m_rendererDeferredLS;
	//Deferred Shader
	DeferredShader m_rendererDeferredShader;
	//Shader for sky
	SkyDome m_rendererSky;

	//Objects
	Object* m_plane[3];
	Object* m_sky;
	Object* m_radixBox;

	DirectX::XMFLOAT3 m_eyePos;
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
	m_deferredBuffers->Shutdown();
	m_rendererSky.Shutdown();
	m_rendererLightShader.Shutdown();
	m_rendererDeferredLS.Shutdown();
	m_rendererDeferredShader.Shutdown();
	m_ortho.Shutdown();
	m_shapes.Shutdown();

	delete m_radixBox;
	m_radixBox = 0;

	delete m_sky;
	m_sky = 0;

	for (UINT i = 0; i < 3; ++i)
	{
		delete m_plane[i];
		m_plane[i] = 0;
	}

	delete m_deferredBuffers;
	m_deferredBuffers = 0;
	
	BlendState::Shutdown();
}

bool SimpleApp::SceneInit()
{
	//Object Init
	m_sky = new Object;
	m_radixBox = new Object;
	//-----------------------------------------------------------------------------------------------------
	//        Texture Init
	//-----------------------------------------------------------------------------------------------------
	
	for (UINT i = 0; i < 3; ++i)
	{
		m_plane[i] = new Object;
		m_plane[i]->SetTexTransformScale(1.0f, 1.0f, 1.0f);
		m_plane[i]->SetTexture(TextureLoader::CreateWICTexture(m_d3dDevice, L"Resources/Textures/carbonfiber.jpg"), 0); //albedo
		m_plane[i]->SetTexture(TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/Irradiance/Irradiance.dds"), 1); //irradiance
		m_plane[i]->SetTexture(TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/grasscube1024.dds"), 2); //env map
		m_plane[i]->SetTexture(TextureLoader::CreateWICTexture(m_d3dDevice, L"Resources/Textures/full-value.jpg"), 3); //roughness
		m_plane[i]->SetTexture(TextureLoader::CreateWICTexture(m_d3dDevice, L"Resources/Textures/no-value2.jpg"), 4); //metallic
	}

	m_plane[0]->SetTexture(TextureLoader::CreateWICTexture(m_d3dDevice, L"Resources/Textures/yellow2.jpg"), 0);
	m_plane[0]->SetTexture(TextureLoader::CreateWICTexture(m_d3dDevice, L"Resources/Textures/semi-value.jpg"), 3);
	m_plane[0]->SetTexture(TextureLoader::CreateWICTexture(m_d3dDevice, L"Resources/Textures/full-value.jpg"), 4);

	m_plane[2]->SetTexture(TextureLoader::CreateWICTexture(m_d3dDevice, L"Resources/Textures/stone.jpg"), 0);
	m_plane[1]->SetTexture(TextureLoader::CreateWICTexture(m_d3dDevice, L"Resources/Textures/low-value.jpg"), 3);
	

	m_radixBox->SetTexture(TextureLoader::CreateWICTexture(m_d3dDevice, L"Resources/Textures/radixbox-albedo.jpg"), 0);
	m_radixBox->SetTexture(TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/Irradiance/Irradiance.dds"), 1);
	m_radixBox->SetTexture(TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/grasscube1024.dds"), 2);
	m_radixBox->SetTexture(TextureLoader::CreateWICTexture(m_d3dDevice, L"Resources/Textures/radixbox-roughness.jpg"), 3);
	m_radixBox->SetTexture(TextureLoader::CreateWICTexture(m_d3dDevice, L"Resources/Textures/radixbox-metallic.jpg"), 4);
	m_radixBox->SetTexture(TextureLoader::CreateWICTexture(m_d3dDevice, L"Resources/Textures/normal-map-fabric.jpg"), 5);

	m_sky->SetTexture(TextureLoader::CreateDDSTexture(m_d3dDevice, L"Resources/Textures/Cubemaps/grasscube1024.dds"), 0);
	//-----------------------------------------------------------------------------------------------------
	//        Light Init
	//-----------------------------------------------------------------------------------------------------
	m_basicLight.Directional.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_basicLight.Directional.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_basicLight.Directional.Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_basicLight.Directional.Direction = XMFLOAT3(1.0f, -1.0f, 0.0f);
	/*
	m_basicLight.Spot.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_basicLight.Spot.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_basicLight.Spot.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_basicLight.Spot.Att = XMFLOAT3(0.3f, 0.0f, 0.0f);
	m_basicLight.Spot.Spot = 400.0f;
	m_basicLight.Spot.Range = 1000.0f;
	m_basicLight.Spot.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
	//m_basicLight.Spot.Position = XMFLOAT3(0.0f, 10.0f, 10.0f);
	*/
	/*
	m_basicLight.Point.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_basicLight.Point.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.8f, 1.0f);
	m_basicLight.Point.Specular =XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	m_basicLight.Point.Att = XMFLOAT3(1.0f, 0.1f, 0.01f);
	m_basicLight.Point.Range = 200.0f;
	m_basicLight.Point.Position = XMFLOAT3(m_pointBulbPos);
	*/
	//-----------------------------------------------------------------------------------------------------
	//        Fog Init
	//-----------------------------------------------------------------------------------------------------
	m_fog.Enabled = false;
	m_fog.FogColor = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_fog.FogStart = 15.0f;
	m_fog.FogRange = 175.0f;
	//-----------------------------------------------------------------------------------------------------
	//        Material Init
	//-----------------------------------------------------------------------------------------------------
	
	m_plane[0]->SetMaterialAmbient(XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f));
	m_plane[0]->SetMaterialDiffuse(XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
	m_plane[0]->SetMaterialSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 80.0f));
	m_plane[0]->SetMaterialProperties(XMFLOAT4(0.0f, 1.0f, 0.8f, 1.0f));

	m_plane[1]->SetMaterialAmbient(XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f));
	m_plane[1]->SetMaterialDiffuse(XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
	m_plane[1]->SetMaterialSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 80.0f));
	m_plane[1]->SetMaterialProperties(XMFLOAT4(0.2f, 0.0f, 0.1f, 1.0f));

	m_plane[2]->SetMaterialAmbient(XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f));
	m_plane[2]->SetMaterialDiffuse(XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
	m_plane[2]->SetMaterialSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 80.0f));
	m_plane[2]->SetMaterialProperties(XMFLOAT4(0.2f, 0.0f, 0.8f, 1.0f));

	m_sky->SetMaterialAmbient(XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f));
	m_sky->SetMaterialDiffuse(XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
	m_sky->SetMaterialSpecular(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	//-----------------------------------------------------------------------------------------------------
	//        Object Geometry Init
	//-----------------------------------------------------------------------------------------------------
	
	for (UINT i = 0; i < 3; ++i)
	{
		m_plane[i]->SetOffset(m_shapes.AddGeometry(MODEL_TYPE_SPHERE));
	}
	//m_plane[0]->SetOffset(m_shapes.AddCustomGeometry(L"Resources/Models/skull.txt"));
	m_sky->SetOffset(m_shapes.AddGeometry(MODEL_TYPE_SPHERE));
	m_radixBox->SetOffset(m_shapes.AddGeometry(MODEL_TYPE_PLAIN));
	//-----------------------------------------------------------------------------------------------------
	//        Object World Init
	//-----------------------------------------------------------------------------------------------------
	m_plane[0]->SetScale(1.0f, 1.0f, 1.0f);
	m_plane[1]->SetScale(1.0f, 1.0f, 1.0f);
	m_plane[2]->SetScale(1.0f, 1.0f, 1.0f);
	m_plane[0]->SetPosition(0.0f, 3.0f, 5.0f);
	m_plane[1]->SetPosition(3.0f, 3.0f, 5.0f);
	m_plane[2]->SetPosition(-3.0f, 3.0f, 5.0f);
	m_radixBox->SetPosition(0.0f, -1.0f, 5.0f);
	m_radixBox->SetScale(10.0f, 10.0f, 10.0f);
	m_sky->SetPosition(0.0f, 0.0f, 0.0f);
	m_sky->SetScale(30.0f, 30.0f, 30.0f);
	//-----------------------------------------------------------------------------------------------------
	//        Renderer Init
	//-----------------------------------------------------------------------------------------------------
	m_deferredBuffers = new DeferredRendering();

	XMFLOAT2 specResolution = GetSpecResolution(m_width, m_height);
	if (!m_deferredBuffers->Initialize(m_d3dDevice, specResolution.x, specResolution.y, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(0, L"Cant initalize deferred buffers", L"Error", MB_OK);
		return false;
	}

	if (!m_shapes.Initialize(m_d3dDevice))
	{
		return false;
	}

	if (!m_ortho.Initialize(m_d3dDevice, m_width, m_height))
	{
		MessageBox(0, L"Cant initalize ortho manager", L"Error", MB_OK);
		return false;
	}

	if (!m_rendererLightShader.Initialize(m_d3dDevice, m_hWnd))
	{
		return false;
	}

	if (!m_rendererDeferredLS.Initialize(m_d3dDevice, m_hWnd))
	{
		MessageBox(0, L"Cant initalize deferred light shader", L"Error", MB_OK);
		return false;
	}

	if (!m_rendererDeferredShader.Initialize(m_d3dDevice, m_hWnd))
	{
		MessageBox(0, L"Cant initalize deferred shader", L"Error", MB_OK);
		return false;
	}

	if (!m_rendererSky.Initialize(m_d3dDevice, m_hWnd))
	{
		return false;
	}

	m_camera.SetProjection(m_projectionMatrix);
	m_camera.SetPosition(0.0f, 3.0f, 0.0f);

	for (UINT i = 0; i < 3; ++i)
	{
		m_plane[i]->Update();
	}
	m_radixBox->Update();

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
		m_camera.Walk(-10.0f*dt);
	}
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_camera.Walk(10.0f*dt);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_camera.Strafe(-10.0f*dt);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_camera.Strafe(10.0f*dt);
	}
	if (GetAsyncKeyState('R') & 0x8000)
	{
		m_camera.SetPosition(0.0f, 10.0f, 0.0f);
	}
	if (GetAsyncKeyState('C') & 0x8000)
	{
	}
	if (GetAsyncKeyState('J') & 0x8000)
	{	
	}
	if (GetAsyncKeyState('1') & 0x8000)
	{
		m_fog.Enabled = true;
	}
	if (GetAsyncKeyState('2') & 0x8000)
	{
		m_fog.Enabled = false;
	}

	//dynamic object
	//m_plane->SetPosition(m_camera.GetPosition());
	//m_plane->Update();

	m_sky->SetPosition(m_camera.GetPosition());
	m_sky->Update();

	m_camera.Update();
}

void SimpleApp::Draw()
{
	assert(m_d3dDeviceContext);
	assert(m_swapChain);

	
	XMMATRIX view;
	view = m_camera.GetViewMatrix();
	m_eyePos = m_camera.GetPosition();

	//render g-buffers
	m_deferredBuffers->SetRenderTargets(m_d3dDeviceContext);
	m_deferredBuffers->ClearRenderTargets(m_d3dDeviceContext);
	m_shapes.Render(m_d3dDeviceContext);
	
	//objects to render in deferred renderer
	m_d3dDeviceContext->RSSetViewports(1, &m_deferredViewport);
	for (UINT i = 0; i < 3; ++i)
	{
		m_rendererDeferredShader.Render(m_d3dDeviceContext, m_plane[i], m_camera);
	}
	m_rendererDeferredShader.Render(m_d3dDeviceContext, m_radixBox, m_camera);

	//-------------Sky
	m_d3dDeviceContext->RSSetViewports(1, &m_deferredSkyViewport);
	m_d3dDeviceContext->RSSetState(m_solidNoCullRS);
	m_rendererSky.Render(m_d3dDeviceContext, m_sky, m_camera, m_projectionMatrix, m_basicLight, m_fog);
	m_d3dDeviceContext->RSSetState(m_solidRS);
	m_d3dDeviceContext->RSSetViewports(1, &m_defaultViewport);

	SetDefaultRenderTargetOn();

	//-----render begin
	float color[4] = {0, 0, 0, 255};
	m_d3dDeviceContext->ClearRenderTargetView(m_renderTargetView, color);
	m_d3dDeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dDeviceContext->RSSetState(m_solidRS);
	//----------Rendering----------------------------------------------------------------------
	//m_rendererLightShader.Render(m_d3dDeviceContext, m_plane[1], m_camera, m_projectionMatrix, m_basicLight, m_fog);
	/*
	for (UINT i = 0; i < 3; ++i)
	{
		//m_rendererLightShader.Render(m_d3dDeviceContext, m_plane[i], m_camera, m_projectionMatrix, m_basicLight, m_fog);
	}
	
	m_d3dDeviceContext->RSSetState(m_solidNoCullRS);
	m_d3dDeviceContext->OMSetDepthStencilState(m_skyDSS, 1);
	m_rendererSky.Render(m_d3dDeviceContext, m_sky, m_camera, m_projectionMatrix, m_basicLight, m_fog);
	m_d3dDeviceContext->RSSetState(m_solidRS);
	m_d3dDeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	*/

	m_ortho.Render(m_d3dDeviceContext);
	m_rendererDeferredLS.Render(m_d3dDeviceContext, offsetData(m_ortho.GetIndexCount(), 0, 0), m_plane[0], m_deferredBuffers->GetShaderResourceView(0), m_deferredBuffers->GetShaderResourceView(1), m_deferredBuffers->GetShaderResourceView(2), m_deferredBuffers->GetShaderResourceView(3), m_basicLight, m_camera.GetPosition(), m_fog);

	//-------render end
	if (VSYNC_ENABLED)
	{
		m_swapChain->Present(1, 0);
	}
	else
	{
		m_swapChain->Present(0, 0);
	}
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

		m_camera.Pitch(dy);
		m_camera.RotateY(dx);
	}

	m_lastMousePos.x = x;
	m_lastMousePos.y = y;
}
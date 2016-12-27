#include "ElixirEditor.h"
#pragma execution_character_set("utf-8")

Elixir::Editor::Editor()
	:m_language(ENGLISH),
	m_ScenePropOpen(false),
	m_objectListOpen(false),
	m_leftUIWidth(350)
{
}

Elixir::Editor::~Editor()
{
}

void Elixir::Editor::Initialize(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, float width, float height, SceneManager* scenemanager)
{
	InitIMGUI(hwnd, device, deviceContext);

	m_screenWidth = width;
	m_screenHeight = height;
	m_sceneManager = scenemanager;
}

void Elixir::Editor::Frame()
{
	NewFrameIMGUI();
}

void Elixir::Editor::Update()
{
	UpdateIMGUI();
}

void Elixir::Editor::Shutdown()
{
	ShutdownIMGUI();
}

void Elixir::Editor::Input()
{
	
}

void Elixir::Editor::InitIMGUI(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ImGui_ImplDX11_Init(hwnd, device, deviceContext);

	m_testWindowEnabled = false;

	ChangeIMGUIStyle();
}

void Elixir::Editor::NewFrameIMGUI()
{
	ImGui_ImplDX11_NewFrame();
}

void Elixir::Editor::UpdateIMGUI()
{
	//Debug
	if(m_testWindowEnabled)
		ImGui::ShowTestWindow(&m_testWindowEnabled);

	MainMenuBar();
	ScenePropertyWindow();
	ObjectListWindow();
	ObjectComponentWindow();

	ImGui::Render();
}

void Elixir::Editor::ShutdownIMGUI()
{
	ImGui_ImplDX11_Shutdown();
}

void Elixir::Editor::MainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(m_langTerm[FILE].GetTerm(m_language).c_str()))
		{
			if (ImGui::MenuItem(m_langTerm[NEW_SCENE].GetTerm(m_language).c_str()))
			{
			}

			if (ImGui::MenuItem(m_langTerm[OPEN_SCENE].GetTerm(m_language).c_str()))
			{
			}

			if (ImGui::MenuItem(m_langTerm[SAVE_SCENE].GetTerm(m_language).c_str(), "Ctrl+S"))
			{
			}

			ImGui::Separator();

			if (ImGui::MenuItem(m_langTerm[QUIT].GetTerm(m_language).c_str(), "Alt+F4"))
			{
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(m_langTerm[WINDOW].GetTerm(m_language).c_str()))
		{
			if (ImGui::MenuItem(m_langTerm[SCENE_PROPERTIES].GetTerm(m_language).c_str(), NULL, m_ScenePropOpen))
			{
				m_ScenePropOpen ^= 1;
			}
			SetToolTip(m_langTerm[SCENE_PROP_TOOLTIP].GetTerm(m_language).c_str());

			if (ImGui::MenuItem(m_langTerm[OBJECT_LIST].GetTerm(m_language).c_str(), NULL, m_objectListOpen))
			{
				m_objectListOpen ^= 1;
			}
			SetToolTip(m_langTerm[OBJ_LIST_TOOLTIP].GetTerm(m_language).c_str());

			if (ImGui::MenuItem(m_langTerm[MORE].GetTerm(m_language).c_str()))
			{
				m_testWindowEnabled ^= 1;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(m_langTerm[OPTION].GetTerm(m_language).c_str()))
		{
			if (ImGui::BeginMenu(m_langTerm[LANGUAGE].GetTerm(m_language).c_str()))
			{
				const char* languageList[] = {
					"English",
					"Japanese",
					"Spanish"
				};

				static int selectedLanguage = 0;
				ImGui::PushItemWidth(90);
				ImGui::ListBox("", &selectedLanguage, languageList, 3);

				switch (selectedLanguage)
				{
				case 0:
					m_language = ENGLISH;
					break;

				case 1:
					m_language = JAPANESE;
					break;
				case 2:
					m_language = SPANISH;
					break;

				default:
					m_language = ENGLISH;
					break;
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Dear ImGui"))
			{
			}

			ImGui::EndMenu();
		}


		ImGui::EndMainMenuBar();
	}
}

void Elixir::Editor::ScenePropertyWindow()
{
	if (m_ScenePropOpen)
	{
		ImGuiWindowFlags window_flags = 0;
		//window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		ImGui::SetNextWindowSizeConstraints(ImVec2(m_leftUIWidth, m_screenHeight - 30), ImVec2(m_leftUIWidth, m_screenHeight - 30));
		ImGui::SetNextWindowPos(ImVec2(m_screenWidth - m_leftUIWidth, 26));
		std::string windowName = m_langTerm[SCENE_PROPERTIES].GetTerm(m_language) + "##Window";
		ImGui::Begin(windowName.c_str(), &m_ScenePropOpen, window_flags);
		//Window content here------------------------

		ImGui::End();
	}
	
}

void Elixir::Editor::ObjectListWindow()
{
	if (m_objectListOpen)
	{
		ImGuiWindowFlags window_flags = 0;
		//window_flags |= ImGuiWindowFlags_NoTitleBar;
		//window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		ImGui::SetNextWindowSizeConstraints(ImVec2(m_leftUIWidth, 200), ImVec2(m_leftUIWidth, FLT_MAX));
		ImGui::SetNextWindowPos(ImVec2(0, 26));
		std::string windowName = m_langTerm[OBJECT_LIST].GetTerm(m_language) + "##Window";
		ImGui::Begin(windowName.c_str(), &m_objectListOpen, window_flags);
		m_objListWindowHeight = ImGui::GetWindowHeight();
		//Window content here------------------------
		ImGui::BeginChild("objList", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.98f, ImGui::GetWindowHeight() - 45), false, ImGuiWindowFlags_HorizontalScrollbar);
		for (auto &obj : m_sceneManager->GetCurrentScene()->GetChildren())
		{
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			ImGui::TreeNodeEx(obj->GetName().c_str(), nodeFlags);
			if (ImGui::IsItemClicked())
			{
				m_selectedObject = obj;
				m_objComponentOpen = true;
			}
		}
		ImGui::EndChild();

		ImGui::End();
	}
	else
	{
		m_objComponentOpen = false;
	}
}

void Elixir::Editor::ObjectComponentWindow()
{
	if (m_objComponentOpen)
	{
		ImGuiWindowFlags window_flags = 0;
		//window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		ImGui::SetNextWindowSizeConstraints(ImVec2(m_leftUIWidth, m_screenHeight - m_objListWindowHeight - 30), ImVec2(m_leftUIWidth, m_screenHeight - m_objListWindowHeight - 30));
		ImGui::SetNextWindowPos(ImVec2(0, m_objListWindowHeight + 27));
		std::string windowName = m_langTerm[OBJ_COMPONENT].GetTerm(m_language) + "##Window";
		ImGui::Begin(windowName.c_str(), &m_objComponentOpen, window_flags);
		//Window content here------------------------

		//Name
		char buf[64];
		strncpy(buf, m_selectedObject->GetName().c_str(), sizeof(buf));
		buf[sizeof(buf) - 1] = 0;
		ImGui::Text(m_langTerm[NAME].GetTerm(m_language).c_str());
		ImGui::SameLine();
		ImGui::InputText("##name", buf, 64);
		m_selectedObject->SetName(buf);

		static int selectedComp = -1;

		if (ImGui::Button(m_langTerm[ADD_COMPONENT].GetTerm(m_language).c_str()))
		{
			
			ImGui::OpenPopup("compPopup");
		}

		if (ImGui::BeginPopup("compPopup"))
		{

			if (ImGui::Selectable("Transform##select"))
			{
				m_selectedObject->AddComponent<Transform>();
			}

			if (ImGui::Selectable("Renderer##select"))
			{
				auto comp = m_selectedObject->AddComponent<Renderer3D>();
				comp->Enabled = false;
			}

			ImGui::EndPopup();
		}



		TransformEditor();

		RenderEditor();

		ImGui::End();
	}

}

void Elixir::Editor::TransformEditor()
{
	if (m_selectedObject->GetComponent<Transform>() != nullptr)
	{
		ImGui::Separator();

		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Dynamic", &m_selectedObject->GetTransform()->Dynamic);
			SetToolTip(m_langTerm[DYNAMIC_TOOLTIP].GetTerm(m_language).c_str());

			ImGui::Spacing();

			ImGui::Text("   X       Y       Z");
			float pos[3] = { m_selectedObject->GetTransform()->Position.x, m_selectedObject->GetTransform()->Position.y, m_selectedObject->GetTransform()->Position.z };
			ImGui::DragFloat3(m_langTerm[POSITION].GetTerm(m_language).c_str(), pos, 0.01f);
			m_selectedObject->GetTransform()->Position = Vec3f(pos[0], pos[1], pos[2]);

			float rotation[3] = { m_selectedObject->GetTransform()->Rotation.x, m_selectedObject->GetTransform()->Rotation.y, m_selectedObject->GetTransform()->Rotation.z };
			ImGui::DragFloat3(m_langTerm[ROTATION].GetTerm(m_language).c_str(), rotation, 0.1f);
			m_selectedObject->GetTransform()->Rotation = Vec3f(rotation[0], rotation[1], rotation[2]);

			float scale[3] = { m_selectedObject->GetTransform()->Scale.x, m_selectedObject->GetTransform()->Scale.y, m_selectedObject->GetTransform()->Scale.z };
			ImGui::DragFloat3(m_langTerm[SCALE].GetTerm(m_language).c_str(), scale, 0.01f);
			m_selectedObject->GetTransform()->Scale = Vec3f(scale[0], scale[1], scale[2]);

			ImGui::Spacing();

			if (ImGui::TreeNode("Texture Transform"))
			{
				ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
		
				ImGui::Text("      U             V");
				
				float tpos[2] = { m_selectedObject->GetTransform()->TexturePosition.x , m_selectedObject->GetTransform()->TexturePosition.y};
				auto labelName = m_langTerm[POSITION].GetTerm(m_language) + "##texture";
				ImGui::DragFloat2(labelName.c_str(), tpos, 0.01f);
				m_selectedObject->GetTransform()->TexturePosition = Vec3f(tpos[0], tpos[1], 0.0f);

				/*
				labelName = m_langTerm[ROTATION].GetTerm(m_language) + "##texture";
				float trot[3] = { m_selectedObject->GetTransform()->TextureRotation.x, m_selectedObject->GetTransform()->TextureRotation.y, m_selectedObject->GetTransform()->TextureRotation.z };
				ImGui::DragFloat3(labelName.c_str(), trot, 0.1f);
				m_selectedObject->GetTransform()->TextureRotation = Vec3f(trot[0], trot[1], trot[2]);
				*/

				labelName = m_langTerm[SCALE].GetTerm(m_language) + "##texture";
				float tscale[2] = { m_selectedObject->GetTransform()->TextureScale.x, m_selectedObject->GetTransform()->TextureScale.y};
				ImGui::DragFloat2(labelName.c_str(), tscale, 0.01f);
				m_selectedObject->GetTransform()->TextureScale = Vec3f(tscale[0], tscale[1], 0.0f);
				
				ImGui::TreePop();	
				ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
			}

			if (ImGui::Button(m_langTerm[REMOVE_COMPONENT].GetTerm(m_language).c_str()))
			{
				m_selectedObject->RemoveComponent<Transform>();
				m_selectedObject->RemoveComponent<Renderer3D>();
			}
		}
	}
}

void Elixir::Editor::RenderEditor()
{
	if (m_selectedObject->GetComponent<Renderer3D>() != nullptr)
	{
		ImGui::Separator();

		if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
		{
			
			//ElixirLog(std::to_string(m_selectedObject->GetRenderer()->Model.vertexOffset));
			if (m_selectedObject->GetRenderer()->Model.vertexOffset != 0)
			{
				ImGui::Checkbox("Enabled", &m_selectedObject->GetRenderer()->Enabled);
			}
			
			ImGui::Checkbox("BackFaceCulling", &m_selectedObject->GetRenderer()->EnableBackFaceCulling);
			ImGui::Checkbox("Cast Shadow", &m_selectedObject->GetRenderer()->CastShadow);

			auto name = m_langTerm[REMOVE_COMPONENT].GetTerm(m_language) + "##Render";
			if (ImGui::Button(name.c_str()))
			{
				m_selectedObject->RemoveComponent<Renderer3D>();
			}
		}

		
	}
}

void Elixir::Editor::ChangeIMGUIStyle()
{
	auto io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("Resources/Fonts/03SmartFont-P.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

	auto style = &ImGui::GetStyle();
	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 0.80f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.70f, 1.00f, 0.73f, 0.38f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.44f, 0.43f, 0.49f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CloseButton] = ImVec4(0.75f, 0.39f, 0.38f, 0.16f);
	style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.90f, 0.39f, 0.38f, 0.39f);
	style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}

void Elixir::Editor::SetToolTip(const char * text)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.0f);
		ImGui::TextUnformatted(text);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

﻿#pragma once
#pragma execution_character_set("utf-8")

#include "../../../External Soruce/imgui/imgui.h"
#include "../../../External Soruce/imgui/examples/directx11_example/imgui_impl_dx11.h"
#define DIRECTINPUT_VERSION 0x0800
#include <string>
#include "../SceneManager.h"


namespace Elixir
{
	class Editor
	{
	public :
		Editor();
		~Editor();

		void Initialize(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, float width, float height, SceneManager* scenemanager);
		void Frame();
		void Update();
		void Shutdown();

		void Input();

	private:
		enum LANGUAGES
		{
			ENGLISH,
			JAPANESE,
			SPANISH
		};

		struct UITerm
		{
			UITerm(std::string e, std::string j, std::string s)
				:english(e), japanese(j), spanish(s)
			{}

			std::string GetTerm(LANGUAGES lang)
			{
				switch (lang)
				{
				case LANGUAGES::ENGLISH:
					return english;
					break;
				case LANGUAGES::JAPANESE:
					return japanese;
					break;
				case LANGUAGES::SPANISH:
					return spanish;
					break;

				default:
					return english;
					break;
				}
			};
			std::string english;
			std::string japanese;
			std::string spanish;
		};

		enum UIPART
		{
			FILE,
			NEW_SCENE,
			OPEN_SCENE,
			SAVE_SCENE,
			QUIT,
			WINDOW,
			SCENE_PROPERTIES,
			OBJECT_LIST,
			MORE,
			OPTION,
			LANGUAGE,
			LANG_ENG,
			LANG_JAP,
			LANG_SPA,
			SCENE_PROP_TOOLTIP,
			OBJ_LIST_TOOLTIP,
			OBJ_COMPONENT,
			NAME,
			POSITION,
			ROTATION,
			SCALE,
			DYNAMIC_TOOLTIP,
			ADD_COMPONENT,
			REMOVE_COMPONENT
		};

		UITerm m_langTerm[24] = 
		{
			UITerm("File", "ファイル", "Archivo"),
			UITerm("New Scene", "新しいシーン", "Nueva Escena"),
			UITerm("Open Scene", "シーンを開く", "Abrir Escena"),
			UITerm("Save Scene", "シーンを保存", "Guardar Escena"),
			UITerm("Quit", "終了", "Salir"),
			UITerm("Window", "ウィンドウ", "Ventana"),
			UITerm("Scene Properties", "シーンプロパティー", "Propiedad de Escena"),
			UITerm("Object List", "オブジェクトリスト", "Lista de Objetos"),
			UITerm("More...", "追加", "Mas..."),
			UITerm("Option", "オプション", "Opciones"),
			UITerm("Language", "言語", "Lenguaje"),
			UITerm("English", "英語", "Ingles"),
			UITerm("Japanese", "日本語", "Japones"),
			UITerm("Spanish", "スペイン語", "Español"),
			UITerm("Change Scene properties.", "シーンの設定などを変える", "Cambiar propiedades de Escena."),
			UITerm("Display all Objects from current Scene.", "現在のシーンのオブジェクトをリストで表します", "Mostrar lista de todos los Objetos en esta Escena."),
			UITerm("Object Components", "オブジェクトコンポーネント", "Componentes de Objeto"),
			UITerm("Name", "名前", "Nombre"),
			UITerm("Position", "位置", "Posicion"),
			UITerm("Rotation", "回転", "Rotacion"),
			UITerm("Scale", "スケール", "Escala"),
			UITerm("A non-dynamic Object can not be moved once put.", "配置後ダイナミックではないオブジェクトは動けません", "Un Objeto no dinamico no puede moverse una vez puesto."),
			UITerm("Add Component", "コンポーネント追加", "Agregar Componente"),
			UITerm("Remove Component", "コンポーネント削除", "Eliminar Componente")
		};

		void InitIMGUI(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		void NewFrameIMGUI();
		void UpdateIMGUI();
		void ShutdownIMGUI();

		void MainMenuBar();
		void ScenePropertyWindow();
		void ObjectListWindow();
		void ObjectComponentWindow();

		void TransformEditor();
		void RenderEditor();

		void ChangeIMGUIStyle();
		void SetToolTip(const char* text);

	private:
		LANGUAGES m_language;

		float m_screenWidth, m_screenHeight;

		float m_objListWindowHeight;
		float m_leftUIWidth;

		bool m_ScenePropOpen;
		bool m_objectListOpen;
		bool m_objComponentOpen;
		bool m_testWindowEnabled;

		GameObject* m_selectedObject;

		SceneManager* m_sceneManager;
	};
}
#pragma once

namespace Elixir
{
	class SceneManager;

	class GameManager
	{
	public:
		static GameManager& GetInstance();

		GameManager(GameManager const&) = delete;
		void operator=(GameManager const&) = delete;

		void PrepareGame(SceneManager* sceneManager, int screenWidth, int screenHeight);
		SceneManager* GetSceneManager();
		int GetScreenWidth();
		int GetScreenHeight();

	private:
		GameManager() {};

	private:
		SceneManager* m_sceneManager;
		int m_width;
		int m_height;

	};
}
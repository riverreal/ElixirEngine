#include "GameManager.h"
#include "../Includes/LESystem.h"
#include "SceneManager.h"

using namespace Elixir;

GameManager& GameManager::GetInstance()
{
	static GameManager instance;

	return instance;
}

void Elixir::GameManager::PrepareGame(SceneManager * sceneManager, int screenWidth, int screenHeight)
{
	m_sceneManager = sceneManager;
	m_width = screenWidth;
	m_height = screenHeight;
}

SceneManager * Elixir::GameManager::GetSceneManager()
{
	return m_sceneManager;
}

int Elixir::GameManager::GetScreenWidth()
{
	return m_width;
}

int Elixir::GameManager::GetScreenHeight()
{
	return m_height;
}

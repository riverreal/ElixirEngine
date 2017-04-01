#include "AudioManager.h"

using namespace Elixir;
using namespace irrklang;

AudioManager& Elixir::AudioManager::GetInstance()
{
	static AudioManager instance;

	instance.Init();

	return instance;
}

Elixir::AudioManager::~AudioManager()
{
	if(m_controlledMusic)
		m_controlledMusic->drop();

	m_sEngine->drop();
}

void Elixir::AudioManager::PlaySoundEffect(std::string filename)
{
	if (m_initFailed)
		return;

	m_sEngine->play2D(filename.c_str());
}

void Elixir::AudioManager::PlayMusic(std::string filename, bool loop)
{
	if (m_initFailed)
		return;

	m_sEngine->play2D(filename.c_str(), loop);
}

void Elixir::AudioManager::StopAllMusic()
{
	if (m_initFailed)
		return;

	m_sEngine->stopAllSounds();
}

void Elixir::AudioManager::AddControlledMusic(std::string filename)
{
	if (m_initFailed)
		return;

	if(m_controlledMusic)
		m_controlledMusic->drop();

	m_controlledMusic = m_sEngine->play2D(filename.c_str(), false, true, true);
	if (!m_controlledMusic)
	{
		ElixirLog("AudioManager: Could not add " + filename);
	}
}

ISound * Elixir::AudioManager::GetControlledMusic()
{
	return m_controlledMusic;
}

void Elixir::AudioManager::Init()
{
	if (!m_sEngine)
	{
		m_sEngine = createIrrKlangDevice();
		if (!m_sEngine)
		{
			m_initFailed = true;
			ElixirLog("AudioManager Failed to initialize \n");
		}
	}
}


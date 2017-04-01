#pragma once
#pragma comment(lib, "irrKlang.lib")

#include <irrKlang.h>
#include "../../Helper/GeneralHelper.h"

namespace Elixir
{
	//Singleton class of AudioManager
	//Implementation of irrKlang for easier sound manipulation.
	class AudioManager
	{
	public:
		static AudioManager& GetInstance();
		~AudioManager();

		//Delete methods that are unacceptable (No copy, no assign)
		AudioManager(AudioManager const&) = delete;
		void operator=(AudioManager const&) = delete;

		//Audio Oriented Functions----------

		//Plays a Sound Effect
		//Can not be controlled
		void PlaySoundEffect(std::string filename);
		//Plays a Music
		//Only 1 music supported at a time
		//Loop is disabled at default
		void PlayMusic(std::string filename, bool loop = false);
		//Stops all currently playing music (And Sound Effects)
		void StopAllMusic();
		//Adds a Controlled music
		//Only 1 controlled music supported at a time
		//It can be controlled with the ISound Interface
		//Use GetControlledMusic()
		void AddControlledMusic(std::string filename);
		//Returns the ISound interface
		//Use to control the Controlled Music
		irrklang::ISound* GetControlledMusic();

	private:
		AudioManager() {};
		void Init();

	private:
		irrklang::ISoundEngine* m_sEngine;
		bool m_initFailed;
		irrklang::ISound* m_controlledMusic;
	};

}
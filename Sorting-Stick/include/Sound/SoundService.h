#pragma once
#include "SFML/Audio.hpp"

namespace Sound
{
	enum class SoundType
	{
		BUTTON_CLICK,
		COMPARE_SFX,
		SCREAM,
	};

	class SoundService
	{
	private:
		const int background_music_volume = 30;
		const int sound_effect_volume = 200;

		sf::Music background_music;
		sf::Sound sound_effect;
		sf::SoundBuffer buffer_button_click;
		sf::SoundBuffer buffer_compare_sfx;

		void loadBackgroundMusicFromFile();
		void loadSoundFromFile();

	public:
		void initialize();

		void playSound(SoundType soundType);
		void playBackgroundMusic();
	};
}
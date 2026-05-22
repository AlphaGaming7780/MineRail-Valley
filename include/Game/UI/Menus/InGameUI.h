#pragma once

#include <string>
#include <Game/UI.hpp>
#include <Game/UI/Widgets/UIMusicPlayer.hpp>

namespace Game
{
	class InGameUI : public UIGroup
	{
	public:
		InGameUI();
		~InGameUI();

		void UpdateLayout(const sf::View& view) override;

		void Draw(sf::RenderTexture& canvas) override;

		void SetStatus(const std::string& text);

		// Returns the embedded music player so the game layer can wire its
		// callbacks (Prev / Play-Pause / Next / Stop) directly to AudioManager.
		// See UI_INTEGRATION.md for the recommended wiring snippet.
		UIMusicPlayer* GetMusicPlayer() { return m_MusicPlayer; }

	private:
		sf::Texture* m_ButtonTexture = nullptr;
		sf::Texture* m_ButtonHoverTexture = nullptr;
		sf::Texture* m_ButtonClickedTexture = nullptr;
		sf::Font* m_Font = nullptr;

		UIButton* m_DefaultToolButton = nullptr;
		UIButton* m_TrackToolButton = nullptr;
		UIButton* m_BulldozeToolButton = nullptr;
		UILabel* m_TitleLabel = nullptr;
		UILabel* m_DefaultToolLabel = nullptr;
		UILabel* m_TrackToolLabel = nullptr;
		UILabel* m_BulldozeToolLabel = nullptr;
		UILabel* m_StatusLabel = nullptr;

		UIButton* m_PauseButton = nullptr;

		// New: persistent mini music player in the top-right corner.
		UIMusicPlayer* m_MusicPlayer = nullptr;

		void SelectDefaultTool();
		void SelectTrackTool();
		void SelectBulldozeTool();
		void TogglePause();
	};
}

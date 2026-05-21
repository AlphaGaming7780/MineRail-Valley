#pragma once

#include <string>
#include <Game/UI.hpp>

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

		void SelectDefaultTool();
		void SelectTrackTool();
		void SelectBulldozeTool();
		void TogglePause();
	};
}

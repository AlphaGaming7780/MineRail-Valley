#pragma once

#include <string>

#include <Game/UI.hpp>

namespace Game
{
	class UILoadingScreen : public UIGroup
	{
	public:
		UILoadingScreen();

		void UpdateLayout(const sf::View& view) override;

		void Draw(sf::RenderTexture& canvas) override;

		void SetValue(float v);

	private:

		UIProgressBar* pb = nullptr;
	};
}
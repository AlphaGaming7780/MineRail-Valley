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

		void SetValue(float v);

	private:
		UIProgressBar* pb = nullptr;
	};
}
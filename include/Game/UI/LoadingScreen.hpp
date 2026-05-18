#pragma once

#include <PallasEngine/PallasEngine.hpp>
#include <string>

namespace Game
{
	class UILoadingScreen : public pallas::UIGroup
	{
	public:
		UILoadingScreen();

		void UpdateLayout(const sf::View& view) override;

		void Draw(sf::RenderTexture& canvas) override;

		void SetValue(float v);

	private:

		pallas::UIProgressBar* pb = nullptr;
	};
}
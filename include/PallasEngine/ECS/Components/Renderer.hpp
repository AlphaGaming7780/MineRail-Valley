#pragma once

#include <PallasEngine/AssetDatabase/AssetTypes/TextureAsset.hpp>

namespace pallas 
{

	struct Renderer
	{
		sf::Color m_Color = sf::Color::White;
		sf::Texture* m_Texture = nullptr;
		int m_zIndex = 0;
		bool visible = true;
	};
}

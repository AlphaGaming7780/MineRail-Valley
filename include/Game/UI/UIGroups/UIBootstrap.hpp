#pragma once

#include <PallasEngine/UI/UIGroup.hpp>
#include <PallasEngine/UI/Widgets/UIButton.hpp>

namespace pallas
{
	class UIBootstrap : public UIGroup
	{
	public:

		UIBootstrap()
		{
			namespace fs = std::filesystem;
			std::string path = (fs::current_path() / "assets" / "PallasEngine" / "Bootstrap.png").string();

			if (image.loadFromFile(path)) return;

			path = (fs::current_path() / "assets" / "PallasEngine" / "DEFAULT.png").string();

			if (image.loadFromFile(path)) return;

			throw "Failed to load Bootstrap image.";
		}

		void UpdateLayout(const sf::View& view) override
		{
			m_Size = view.getSize();
			UIGroup::UpdateLayout(view);
		}

		void Draw(sf::RenderTexture& canvas) override
		{
			sf::Sprite s = sf::Sprite(image); 
			s.setOrigin(s.getLocalBounds().size / 2.f);
			s.setPosition((sf::Vector2f)canvas.getSize() / 2.f);
			canvas.clear({10, 21, 33});
			canvas.draw(s);
			UIGroup::Draw(canvas);
		}

	private:

		sf::Texture image;

	};
}
#pragma once

#include <PallasEngine/UI/UIGroup.hpp>
#include <PallasEngine/UI/Widgets/UIButton.hpp>

namespace pallas
{
	class UIRoot : public UIGroup
	{
	public:

		UIRoot() 
		{
		}

		void UpdateLayout(const sf::View& view) override
		{

			m_Size = view.getSize();
			UIGroup::UpdateLayout(view);
		}

		void Draw(sf::RenderTexture& canvas) override
		{
			canvas.clear(sf::Color::Transparent);
			UIGroup::Draw(canvas);
		}

	private:


	};
}
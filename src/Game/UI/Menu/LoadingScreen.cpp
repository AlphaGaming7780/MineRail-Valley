#include <Game/UI/LoadingScreen.hpp>

namespace Game
{
	UILoadingScreen::UILoadingScreen() : pallas::UIGroup()
	{
		pb = new pallas::UIProgressBar();
		pb->SetAnchor(pallas::UIAnchor::BottomLeft);
		AddChild(pb);
	}

	void UILoadingScreen::UpdateLayout(const sf::View& view)
	{
		m_Size = view.getSize();
		pb->SetSize({ m_Size.x, 100.f });
		UIGroup::UpdateLayout(view);
	}

	void UILoadingScreen::Draw(sf::RenderTexture& canvas)
	{
		canvas.clear({19,20,22});
		UIGroup::Draw(canvas);
	}

	void UILoadingScreen::SetValue(float v)
	{
		pb->SetValue(v);
	}
}

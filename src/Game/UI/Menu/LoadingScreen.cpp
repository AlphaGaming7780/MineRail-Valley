#include <Game/UI/Menus/LoadingScreen.hpp>

namespace Game
{
	UILoadingScreen::UILoadingScreen() : UIGroup()
	{
		TextureDatabase& tDB = TextureDatabase::Instance();

		sf::Texture* frameTex = tDB.Load("UI\\Loading_bar_frame.png");
		sf::Texture* barTex = tDB.Load("UI\\Loading_bar_inner.png");
		sf::Texture* endCapTex = tDB.Load("UI\\Progress_pivot.png");

		pb = new UIProgressBar(frameTex, barTex, endCapTex);
		pb->SetAnchor(UIAnchor::BottomLeft);
		AddChild(pb);
	}

	UILoadingScreen::~UILoadingScreen()
	{
		TextureDatabase& tDB = TextureDatabase::Instance();
		tDB.Unload("UI\\Loading_bar_frame.png");
		tDB.Unload("UI\\Loading_bar_inner.png");
		tDB.Unload("UI\\Progress_pivot.png");
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

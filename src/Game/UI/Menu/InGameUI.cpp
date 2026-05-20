#include <Game/UI/Menus/InGameUI.h>

#include <Game/AssetDatabase.h>
#include <Game/Systems.hpp>
#include <Game/World.h>

namespace Game
{
    InGameUI::InGameUI()
    {
        TextureDatabase& textures = TextureDatabase::Instance();
        FontDatabase& fonts = FontDatabase::Instance();

        m_ButtonTexture = textures.Load("UI\\Button.png");
        m_ButtonHoverTexture = textures.Load("UI\\Button_hover.png");
        m_ButtonClickedTexture = textures.Load("UI\\Button_clicked.png");
        m_Font = fonts.Load("Fonts\\LiberationSans.ttf");

        m_TitleLabel = new UILabel("MineRail Valley", { 24.f, 18.f }, UIAnchor::TopLeft, m_Font, 28, { 236, 222, 190 });
        AddChild(m_TitleLabel);

        m_DefaultToolButton = new UIButton(
            { 24.f, 10.f },
            { 132.f, 58.f },
            UIAnchor::BottomLeft,
            m_ButtonTexture,
            m_ButtonHoverTexture,
            m_ButtonClickedTexture
        );
        m_DefaultToolButton->SetCallback([this]() { SelectDefaultTool(); });

        m_DefaultToolLabel = new UILabel("Main", { 0, 0 }, UIAnchor::Center, m_Font, 20, { 243, 231, 207 });
        m_DefaultToolButton->AddChild(m_DefaultToolLabel);
        AddChild(m_DefaultToolButton);

        m_TrackToolButton = new UIButton(
            { 168.f, 10.f },
            { 132.f, 58.f },
            UIAnchor::BottomLeft,
            m_ButtonTexture,
            m_ButtonHoverTexture,
            m_ButtonClickedTexture
        );
        m_TrackToolButton->SetCallback([this]() { SelectTrackTool(); });
        
        m_TrackToolLabel = new UILabel("Rail", { 0, 0 }, UIAnchor::Center, m_Font, 20, { 243, 231, 207 });
        m_TrackToolButton->AddChild(m_TrackToolLabel);
        AddChild(m_TrackToolButton);

        m_BulldozeToolButton = new UIButton(
            { 312.f, 10.f },
            { 132.f, 58.f },
            UIAnchor::BottomLeft,
            m_ButtonTexture,
            m_ButtonHoverTexture,
            m_ButtonClickedTexture
        );
        m_BulldozeToolButton->SetCallback([this]() {SelectBulldozeTool(); });

        m_BulldozeToolLabel = new UILabel("Bulldoze", { 0,0 }, UIAnchor::Center, m_Font, 20, { 243, 231, 207 });
        m_BulldozeToolButton->AddChild(m_BulldozeToolLabel);
        AddChild(m_BulldozeToolButton);

        m_StatusLabel = new UILabel("Mode: Rail", { 456.f, 0.f }, UIAnchor::BottomLeft, m_Font, 22, { 236, 222, 190 });
        AddChild(m_StatusLabel);
    }

    InGameUI::~InGameUI()
    {
        TextureDatabase& textures = TextureDatabase::Instance();
        FontDatabase& fonts = FontDatabase::Instance();

        textures.Unload("UI\\Button.png");
        textures.Unload("UI\\Button_hover.png");
        textures.Unload("UI\\Button_clicked.png");
        textures.Unload("UI\\Loading_bar_frame.png");
        textures.Unload("UI\\Loading_bar_inner.png");
        textures.Unload("UI\\Progress_pivot.png");
        fonts.Unload("Fonts\\LiberationSans.ttf");
    }

    void InGameUI::UpdateLayout(const sf::View& view)
    {
        m_Size = view.getSize();
        UIGroup::UpdateLayout(view);
    }

    void InGameUI::Draw(sf::RenderTexture& canvas)
    {
        canvas.clear(sf::Color::Transparent);
        UIGroup::Draw(canvas);
    }

    void InGameUI::SetStatus(const std::string& text)
    {
        if (m_StatusLabel)
            m_StatusLabel->SetText(text);
    }

    void InGameUI::SelectDefaultTool()
    {
        World& world = World::Instance();
        world.GetOrCreateSystem<BulldozeToolSystem>()->SetEnable(false);
        world.GetOrCreateSystem<TrackToolSystem>()->SetEnable(false);
        world.GetOrCreateSystem<DefaultToolSystem>()->SetEnable(true);

        SetStatus("Mode: Main");
    }

    void InGameUI::SelectTrackTool()
    {
        World& world = World::Instance();
        world.GetOrCreateSystem<BulldozeToolSystem>()->SetEnable(false);
        world.GetOrCreateSystem<DefaultToolSystem>()->SetEnable(false);
        world.GetOrCreateSystem<TrackToolSystem>()->SetEnable(true);

        SetStatus("Mode: Rail");
    }
    void InGameUI::SelectBulldozeTool()
    {
        World& world = World::Instance();
        world.GetOrCreateSystem<DefaultToolSystem>()->SetEnable(false);
        world.GetOrCreateSystem<TrackToolSystem>()->SetEnable(false);
        world.GetOrCreateSystem<BulldozeToolSystem>()->SetEnable(true);

        SetStatus("Mode: Bulldoze");
    }
}

#pragma once
#include <SFML/Graphics.hpp>
#include <Game/UI/UIWidget.hpp>
#include <functional>

namespace Game
{
    // UISlider: horizontal value slider with a draggable handle.
    //
    // Value range is configurable (min/max, default 0..1). Two textures:
    //   - trackTex  : background bar drawn along the whole widget width
    //   - handleTex : the draggable knob (drawn centered on the current value)
    //
    // The slider tracks dragging via the standard UIElement mouse plumbing:
    //   - OnMouseDown anywhere on the slider snaps the value to the click x
    //   - while m_IsDragging == true, every UpdateLayout call resyncs from
    //     the latest mouse position (see UIManager::Update for how mouse
    //     position is delivered each frame).
    //
    // The continuous callback `m_OnChange` is fired every time the value moves
    // (during drag). If you only want to react when the user releases the
    // mouse, use SetCallbackCommit() instead — that one fires once on mouse-up.
    class UISlider : public UIWidget
    {
    public:
        UISlider(
            sf::Texture* trackTex   = nullptr,
            sf::Texture* handleTex  = nullptr,
            sf::Vector2f position   = { 0, 0 },
            sf::Vector2f size       = { 200, 24 },
            UIAnchor anchor         = UIAnchor::TopLeft,
            float minValue          = 0.f,
            float maxValue          = 1.f,
            float initialValue      = 0.5f
        );

        virtual ~UISlider();

        void UpdateLayout(const sf::View& view) override;
        void Draw(sf::RenderTexture& canvas) override;
        void Update() override;             // drives the drag follow each frame

        void OnMouseDown() override;
        void OnMouseUp()   override;

        // Value API
        float GetValue() const   { return m_Value; }
        void  SetValue(float v);                  // clamped to [min,max], fires OnChange

        void  SetRange(float minV, float maxV);

        // Callbacks
        void SetCallback(std::function<void(float)> onChange)       { m_OnChange = std::move(onChange); }
        void SetCallbackCommit(std::function<void(float)> onCommit) { m_OnCommit = std::move(onCommit); }

        // Drag bookkeeping. UIManager calls this from its per-frame Update with
        // the current mouse position. While m_IsDragging is true we re-derive
        // the value from mouseX.
        void OnMouseDragTo(const sf::Vector2f& mouseUI);

        bool IsDragging() const { return m_IsDragging; }

    private:
        sf::Texture* m_TrackTex  = nullptr;
        sf::Texture* m_HandleTex = nullptr;
        sf::Sprite*  m_TrackSprite  = nullptr;
        sf::Sprite*  m_HandleSprite = nullptr;

        float m_Min = 0.f;
        float m_Max = 1.f;
        float m_Value = 0.f;

        bool  m_IsDragging = false;

        std::function<void(float)> m_OnChange;   // fires continuously
        std::function<void(float)> m_OnCommit;   // fires on mouse-up

        // Compute the X (UI coords) of the handle for the current value.
        float ValueToX() const;
        float XToValue(float x) const;
    };
}

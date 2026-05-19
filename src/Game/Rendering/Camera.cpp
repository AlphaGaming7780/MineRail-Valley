#include <Game/Rendering/Camera.hpp>
#include <Game/Rendering/GameWindow.hpp>

namespace Game
{
    Camera* Camera::s_instance = nullptr;

    Camera::Camera()
    {
        if (!s_instance)
            s_instance = this;

        m_view = sf::View();
        m_view.setSize(m_BaseSize);
        m_view.setCenter(m_BaseSize * 0.5f);

        onResize((sf::Event::Resized)GameWindow::Instance().GetSize());

        EventManager::Instance();
        ev.on<sf::Event::Resized>([this](sf::Event::Resized ev) {this->onResize(ev); });
    }

    Camera::~Camera()
    {
        if (s_instance == this)
            s_instance = nullptr;
    }

    float Camera::GetZoom() const
    {
        sf::Vector2f current = m_view.getSize();
        return current.x / m_BaseSize.x;
    }

    Camera& Camera::Instance()
    {
        return *s_instance;
    }

    void Camera::onResize(sf::Event::Resized ev)
    {
        sf::Vector2f defaultSize = { 1920, 1080 }; // sf::VideoMode::getDesktopMode().size;
        float windowWidth = static_cast<float>(ev.size.x);
        float windowHeight = static_cast<float>(ev.size.y);

        float windowRatio = windowWidth / windowHeight;
        float targetRatio = defaultSize.x / defaultSize.y;

		float zoomFactor = GetZoom();

        // Si la fenêtre est plus large → élargir horizontalement
        if (windowRatio > targetRatio)
        {
            float newWidth = defaultSize.y * windowRatio;
            SetSize({ newWidth, defaultSize.y });
        }
        else
        {
            // Si la fenêtre est plus haute → élargir verticalement
            float newHeight = defaultSize.x / windowRatio;
            SetSize({defaultSize.x, newHeight });
        }

		SetZoom(zoomFactor); // Conserver le zoom actuel après redimensionnement

        // Toujours remplir toute la fenêtre
        m_view.setViewport({ {0.f, 0.f,}, { 1.f, 1.f } });
    }

    void Camera::SetCenter(const sf::Vector2f& pos)
    {
        m_view.setCenter(pos);
    }

    void Camera::Move(const sf::Vector2f& delta)
    {
        m_view.move(delta);
    }

    void Camera::SetSize(const sf::Vector2f& size)
    {
        m_BaseSize = size;
        m_view.setSize(size);
    }

    void Camera::SetRotation(const sf::Angle& anlge)
    {
        m_view.setRotation(anlge);
    }

    void Camera::Zoom(float factor)
    {
        m_view.zoom(factor);
    }
    void Camera::SetZoom(float zoom)
    {
        sf::Vector2f newSize = m_BaseSize * zoom;
        m_view.setSize(newSize);
    }
}
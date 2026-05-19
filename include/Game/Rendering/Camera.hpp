#pragma once
#include <SFML/Graphics.hpp>
#include <Game/Events.h>
#include <PallasEngine/Logging/Logger.hpp>

namespace Game
{
    class Camera : IEventObserver<sf::Event::Resized>
    {
    public:
        Camera();
        ~Camera();

        void SetCenter(const sf::Vector2f& pos);
        sf::Vector2f GetCenter() { return m_view.getCenter(); }
        void Move(const sf::Vector2f& delta);

        // Définit la taille de base ET la taille actuelle
        void SetSize(const sf::Vector2f& size);
        void SetRotation(const sf::Angle& anlge);

        void Zoom(float factor);                  // Zoom relatif (SFML)
        void SetZoom(float zoom);                 // Zoom absolu basé sur la taille de base

        // --- Nouveaux getters propres ---
        sf::Vector2f GetBaseSize() const { return m_BaseSize; }
        sf::Vector2f GetCurrentSize() const { return m_view.getSize(); }

        // Renvoie les "local bounds" : la zone de référence sans zoom
        sf::FloatRect GetLocalBounds() const
        {
            return sf::FloatRect(
                {
                    0.f,
                    0.f
                }, {
                    m_BaseSize.x,
                    m_BaseSize.y
                }
            );
        }

        // Renvoie les "global bounds" : la zone visible dans le monde après zoom
        sf::FloatRect GetGlobalBounds() const
        {
            const sf::Vector2f size = m_view.getSize();
            const sf::Vector2f center = m_view.getCenter();

            return sf::FloatRect(
                {
                    center.x - size.x * 0.5f,
                    center.y - size.y * 0.5f
                }, {
                    size.x,
                    size.y
                }
            );
        }


        // Zoom actuel = taille actuelle / taille de base
        float GetZoom() const;

        const sf::View& GetView() const { return m_view; }
        sf::View& GetView() { return m_view; }

        static Camera& Instance();

        void OnEvent(const sf::Event::Resized& event) override;


    private:
        sf::View m_view;

        pallas::Logger m_Logger = pallas::Logger("Camera");

        // Taille de référence (sans zoom)
        sf::Vector2f m_BaseSize = { 1920.f, 1080.f };

        static Camera* s_instance;

        void onResize(sf::Event::Resized ev);

    };
}


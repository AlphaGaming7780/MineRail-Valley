#include <Game/GameObjects//Trains/TrainObject.h>
#include <Game/Utils/Time.hpp>
#include <PallasEngine/Logging.hpp>

namespace Game
{

    void TrainObject::Update()
    {

        if (!m_Current || !m_Next)
        {
            return;
        }

        sf::Vector2f pos = GetPosition();
        sf::Vector2f target = m_Next->GetPosition(); // déjà centré

        sf::Vector2f dir = target - pos;
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        const float epsilon = 1.0f;

        if (dist <= epsilon)
        {
            SetPosition(target);

            TrackObjectBase* next = nullptr;
            if (m_Next->m_First && m_Next->m_First != m_Current)
            {
                next = m_Next->m_First;
            }
            else if (m_Next->m_Second && m_Next->m_Second != m_Current)
            {
                next = m_Next->m_Second;
            }

            m_Current = m_Next;
            m_Next = next;
            return;
        }

        dir /= dist;

        sf::Vector2f move = dir * m_Speed * Time::GetDeltaTime().asSeconds();

        Move(move);
        UpdateSprite(dir);
    }

    void TrainObject::UpdateSprite(const sf::Vector2f& dir)
    {
        if (dir.x != 0)
        {
            m_Sprite->setRotation(sf::degrees(0));
        }
        else if (dir.y != 0)
        {
            m_Sprite->setRotation(sf::degrees(90));
        }
    }


}

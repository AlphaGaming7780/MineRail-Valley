#include <Game/GameObjects//Trains/TrainObject.h>
#include <Game/Utils/Time.hpp>
#include <PallasEngine/Logging.hpp>

namespace Game
{

    void TrainObject::Update()
    {
        pallas::Logger logger("TrainObject");

        if (!m_Current || !m_Next)
        {
            logger.InfoO("m_Current ou m_Next est null");
            return;
        }

        sf::Vector2f pos = GetPosition();
        sf::Vector2f target = m_Next->GetPosition(); // déjà centré

        sf::Vector2f dir = target - pos;
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        logger.InfoO("dist = ", dist);

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
            else
            {
                logger.ErrorO("Can't find the next track");
            }

            m_Current = m_Next;
            m_Next = next;
            return;
        }

        dir /= dist;

        sf::Vector2f move = dir * m_Speed * Time::GetDeltaTime().asSeconds();

        Move(move);
    }


}

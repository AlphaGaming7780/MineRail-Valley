#include <Game/GameObjects//Trains/TrainObject.h>
#include <Game/Utils/Time.hpp>
#include <PallasEngine/Logging.hpp>
#include <Game/Events.h>

namespace Game
{
    void TrainObject::Update()
    {
        GameObject::Update();

        if (!m_Current || !m_Next)
        {
            m_Enabled = false;
            EventManager::Instance().Notify<TrainStopped>({ this });
            return;
        }

        float remaining = m_Speed * Time::GetDeltaTime().asSeconds();

        // On boucle tant qu'il reste du mouvement
        while (remaining > 0.f && m_Next)
        {
            sf::Vector2f pos = GetPosition();
            sf::Vector2f target = m_Next->GetPosition();

            sf::Vector2f dir = target - pos;
            float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

            if (dist < 0.001f)
            {
                // Déjà sur la tuile
                SetPosition(target);
                AdvanceToNextTile();
                continue;
            }

            dir /= dist;

            if (remaining >= dist)
            {
                // On atteint la tuile et il reste du mouvement
                SetPosition(target);
                remaining -= dist;
                AdvanceToNextTile();
            }
            else
            {
                // On avance partiellement vers la tuile
                Move(dir * remaining);
                remaining = 0.f;
            }

            UpdateSprite(dir);
        }
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

    void TrainObject::AdvanceToNextTile()
    {
        TrackObjectBase* next = nullptr;

        if (m_Next->m_First && m_Next->m_First != m_Current && !m_Next->m_First->m_Temp)
            next = m_Next->m_First;
        else if (m_Next->m_Second && m_Next->m_Second != m_Current && !m_Next->m_Second->m_Temp)
            next = m_Next->m_Second;

        m_Current = m_Next;
        m_Next = next;
    }
}

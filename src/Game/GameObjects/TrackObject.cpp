#include <Game/GameObjects/Tracks/TrackObject.h>
#include <Game/GameObjects/Tiles/TileObject.h>

namespace Game
{
    void TrackObject::Update()
    {
        TrackObjectBase::Update();
        UpdateSprite();
    }

	void TrackObject::OnDestroy()
	{
        TrackObjectBase::OnDestroy();
		TextureDatabase::Instance().Unload(m_StraightTexturePath);
		TextureDatabase::Instance().Unload(m_BendTexturePath);
	}

    void TrackObject::UpdateSprite()
    {
        if (!m_Tile) return;

        sf::Vector2i center = m_Tile->m_Index;
        sf::Vector2i d1 = m_First ? (m_First->m_Tile->m_Index - center) : sf::Vector2i(0, 0);
        sf::Vector2i d2 = m_Second ? (m_Second->m_Tile->m_Index - center) : sf::Vector2i(0, 0);

        // -------------------------
        // 1) RAIL DROIT
        // -------------------------
        if ((d1.x == 0 && d2.x == 0) || (d1.y == 0 && d2.y == 0))
        {
            SetTexture(*m_StraightTexture, true);

            if (d1.x != 0 || d2.x != 0) // horizontal
                m_Sprite->setRotation(sf::degrees(90));
            else           // vertical
                m_Sprite->setRotation(sf::Angle::Zero);

            return;
        }

        // -------------------------
        // 2) VIRAGE
        // -------------------------
        SetTexture(*m_BendTexture, true);

        // RIGHT + DOWN
        if ((d1 == sf::Vector2i(1, 0) && d2 == sf::Vector2i(0, 1)) ||
            (d2 == sf::Vector2i(1, 0) && d1 == sf::Vector2i(0, 1)))
        {
            m_Sprite->setRotation(sf::degrees(0));
            return;
        }

        // DOWN + LEFT
        if ((d1 == sf::Vector2i(0, 1) && d2 == sf::Vector2i(-1, 0)) ||
            (d2 == sf::Vector2i(0, 1) && d1 == sf::Vector2i(-1, 0)))
        {
            m_Sprite->setRotation(sf::degrees(90));
            return;
        }

        // LEFT + UP
        if ((d1 == sf::Vector2i(-1, 0) && d2 == sf::Vector2i(0, -1)) ||
            (d2 == sf::Vector2i(-1, 0) && d1 == sf::Vector2i(0, -1)))
        {
            m_Sprite->setRotation(sf::degrees(180));
            return;
        }

        // UP + RIGHT
        if ((d1 == sf::Vector2i(0, -1) && d2 == sf::Vector2i(1, 0)) ||
            (d2 == sf::Vector2i(0, -1) && d1 == sf::Vector2i(1, 0)))
        {
            m_Sprite->setRotation(sf::degrees(270));
            return;
        }

        // fallback
        SetTexture(*m_StraightTexture, true);
        m_Sprite->setRotation(sf::Angle::Zero);
    }
}



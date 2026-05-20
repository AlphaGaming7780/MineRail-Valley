#include <Game/GameObjects/Tracks/TrackObject.h>
#include <Game/GameObjects/Tiles/TileObject.h>

namespace Game
{
    void TrackObject::Update()
    {
        m_Enabled = false;
        ResolveConnections();
        UpdateSprite();
    }

	void TrackObject::OnDestroy()
	{
		if (m_Tile != nullptr && m_Tile->m_PlacedTrack == this)
		{
			m_Tile->m_PlacedTrack = nullptr;
		}

		TextureDatabase::Instance().Unload(m_StraightTexturePath);
		TextureDatabase::Instance().Unload(m_BendTexturePath);
	}

	void TrackObject::SetTile(TileObject* tile)
	{
        if (m_Tile && m_Tile->m_PlacedTrack == this) m_Tile->m_PlacedTrack = nullptr;
        tile->m_PlacedTrack = this;
		m_Tile = tile;
		SetPosition(tile->GetPosition());
        //m_First = nullptr;
        //m_Second = nullptr;
        //Update();
        m_Enabled = true;
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

    void TrackObject::ResolveConnections()
    {
        if (m_Tile == nullptr) return;
        std::vector<TrackObject*> neighbors = GetAdjacentTracks();

        // --- 1) Vérifier si m_First est encore valide ---
        if (m_First)
        {
            bool stillValid = false;
            for (TrackObject* n : neighbors)
            {
                if (n == m_First)
                {
                    stillValid = true;
                    break;
                }
            }

            if (!stillValid)
            {
                //Update après (évite les boucles infinis)
                m_First->m_Enabled = true;
                m_First = nullptr;
            }
        }

        // --- 2) Vérifier si m_Second est encore valide ---
        if (m_Second)
        {
            bool stillValid = false;
            for (TrackObject* n : neighbors)
            {
                if (n == m_Second)
                {
                    stillValid = true;
                    break;
                }
            }

            if (!stillValid)
            {
                //Update après (évite les boucles infinis)
                m_Second->m_Enabled = true;
                m_Second = nullptr;
            }
        }

        // --- 3) Si m_First est invalide → en choisir un nouveau ---
        if (!m_First && !neighbors.empty())
        {
            for (TrackObject* n : neighbors)
            {
                if (n != m_Second)
                {
                    m_First = n;
                    //m_First->Update(); // mise à jour du voisin
                    m_First->m_Enabled = true;
                    break;
                }
            }
        }

        // --- 4) Si m_Second est invalide → en choisir un autre ---
        if (!m_Second && neighbors.size() > 1)
        {
            for (TrackObject* n : neighbors)
            {
                if (n != m_First)
                {
                    m_Second = n;
                    //m_Second->Update(); // mise à jour du voisin
                    m_Second->m_Enabled = true;
                    break;
                }
            }
        }
    }


    std::vector<TrackObject*> TrackObject::GetAdjacentTracks() const
    {
        std::vector<TrackObject*> result;
        if (!m_Tile) return result;

        auto check = [&](TileObject* t)
            {
                if (!t || !t->m_PlacedTrack) return;
                if (auto track = dynamic_cast<TrackObject*>(t->m_PlacedTrack))
                    result.push_back(track);
            };

        check(m_Tile->m_Up);
        check(m_Tile->m_Right);
        check(m_Tile->m_Down);
        check(m_Tile->m_Left);

        return result;
    }
}



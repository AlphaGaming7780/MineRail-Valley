#include <Game/GameObjects/Tracks/TrackObjectBase.h>
#include <Game/GameObjects/Tracks/TrackObject.h>
#include <Game/GameObjects/Tracks/StationObject.h>
#include <Game/GameObjects/Tiles/TileObject.h>

namespace Game
{
	void TrackObjectBase::SetTile(TileObject* tile)
	{
		if (m_Tile && m_Tile->m_PlacedTrack == this) m_Tile->m_PlacedTrack = nullptr;
		tile->m_PlacedTrack = this;
		m_Tile = tile;
		SetPosition(tile->GetPosition());
        DisconnectTrack();
        RequireUpdate();
	}

    void TrackObjectBase::ResolveConnections()
    {
        if (!m_Tile)
            return;

        std::vector<TrackObjectBase*> neighbors = GetAdjacentTracks();

        if (m_First)
        {
            bool stillValid = false;
            for (TrackObjectBase* n : neighbors)
            {
                if (n == m_First)
                {
                    stillValid = true;
                    break;
                }
            }

            if (!stillValid)
            {
                if (m_First->m_First == this) m_First->m_First = nullptr;
                if (m_First->m_Second == this) m_First->m_Second = nullptr;
                m_First->RequireUpdate();
                m_First = nullptr;
            }
        }

        if (m_Second)
        {
            bool stillValid = false;
            for (TrackObjectBase* n : neighbors)
            {
                if (n == m_Second)
                {
                    stillValid = true;
                    break;
                }
            }

            if (!stillValid)
            {
                if (m_Second->m_First == this) m_Second->m_First = nullptr;
                if (m_Second->m_Second == this) m_Second->m_Second = nullptr;
                m_Second->RequireUpdate();
                m_Second = nullptr;
            }
        }

        if (!m_First && !neighbors.empty())
        {
            for (TrackObjectBase* n : neighbors)
            {
                if (n == m_Second)
                    continue;

                if (!n->m_First || !n->m_Second)
                {
 
                    m_First = n;
                    if (!n->m_First) n->m_First = this;
                    else if (!n->m_Second) n->m_Second = this;

                    m_First->RequireUpdate();
                    break;
                }
            }
        }

        if (!m_Second && neighbors.size() > 1)
        {
            for (TrackObjectBase* n : neighbors)
            {
                if (n == m_First)
                    continue;

                if (!n->m_First || !n->m_Second)
                {
                    m_Second = n;
                    if (!n->m_First) n->m_First = this;
                    else if (!n->m_Second) n->m_Second = this;

                    m_Second->RequireUpdate();
                    break;
                }
            }
        }
    }


    void TrackObjectBase::Update()
    {
        m_Enabled = false;
        ResolveConnections();
        UpdateSprite();
    }

    void TrackObjectBase::OnDestroy()
    {
        GameObject::OnDestroy();

        DisconnectTrack();

        if (m_Tile && m_Tile->m_PlacedTrack == this)
        {
            m_Tile->m_PlacedTrack = nullptr;
            m_Tile->m_CanBuild = true;
        }
    }

    void TrackObjectBase::DisconnectTrack()
    {
        if (m_First)
        {
            if (m_First->m_First == this) m_First->m_First = nullptr;
            if (m_First->m_Second == this) m_First->m_Second = nullptr;
            m_First->RequireUpdate();
            m_First = nullptr;
        }
        if (m_Second)
        {
            if (m_Second->m_First == this) m_Second->m_First = nullptr;
            if (m_Second->m_Second == this) m_Second->m_Second = nullptr;
            m_Second->RequireUpdate();
            m_Second = nullptr;
        }

        RequireUpdate();
    }

    void TrackObjectBase::ConnectTrack(TrackObjectBase* a, TrackObjectBase* b)
    {
        DisconnectTrack();
        if (!a->m_First || !a->m_Second)
        {
            if (!a->m_First) a->m_First = this;
            else if (!a->m_Second) a->m_Second = this;
            a->RequireUpdate();
            m_First = a;
        }

        if (!b->m_First || !b->m_Second)
        {
            if (!b->m_First) b->m_First = this;
            else if (!b->m_Second) b->m_Second = this;
            b->RequireUpdate();
            m_Second = b;
        }
        RequireUpdate();
    }

    std::vector<TrackObjectBase*> TrackObjectBase::GetAdjacentTracks() const
    {
        std::vector<TrackObjectBase*> result;
        if (!m_Tile) return result;

        auto check = [&](TileObject* t)
            {
                if (!t || !t->m_PlacedTrack) return;
                if (auto track = dynamic_cast<TrackObject*>(t->m_PlacedTrack))
                    result.push_back(track);
                else if (auto station = dynamic_cast<StationObject*>(t->m_PlacedTrack))
                    if(m_Tile->m_Index == station->m_AllowedTileConnection) result.push_back(station);
            };

        check(m_Tile->m_Up);
        check(m_Tile->m_Right);
        check(m_Tile->m_Down);
        check(m_Tile->m_Left);

        return result;
    }

    std::vector<TrackObject*> TrackObjectBase::GetAdjacentTrackObjects() const
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



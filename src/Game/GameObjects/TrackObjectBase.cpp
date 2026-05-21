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
		//m_First = nullptr;
		//m_Second = nullptr;
		//Update();
		m_Enabled = true;
	}

    void TrackObjectBase::OnDestroy()
    {
        GameObject::OnDestroy();
        if (m_Tile != nullptr && m_Tile->m_PlacedTrack == this)
        {
            m_Tile->m_PlacedTrack = nullptr;
            m_Tile->m_CanBuild = true;
        }

        if (m_First) m_First->m_Enabled = true;
        if (m_Second) m_Second->m_Enabled = true;
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



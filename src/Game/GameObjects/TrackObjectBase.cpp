#include <Game/GameObjects/Tracks/TrackObjectBase.h>
#include <Game/GameObjects/Tracks/TrackObject.h>
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

    std::vector<TrackObjectBase*> TrackObjectBase::GetAdjacentTracks() const
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



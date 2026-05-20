#include <Game/GameObjects/Tracks/TrackObject.h>

namespace Game
{
	void TrackObject::Update()
	{
	}
	void TrackObject::OnDestroy()
	{
	}
	void TrackObject::SetTile(TileObject* tile)
	{
		m_Tile = tile;
		SetPosition(tile->GetPosition());
		Update();
	}
}



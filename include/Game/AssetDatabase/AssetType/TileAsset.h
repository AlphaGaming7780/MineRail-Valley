#pragma once
#include <Game/AssetDatabase.h>
#include <Game/AssetDatabase/AssetType/ObjectData.h>
#include <string>

namespace Game
{
	struct TileData : public ObjectData
	{
		bool m_CanBuild = false;
	};

	struct TileAsset : ObjectAsset
	{
		bool m_CanBuild = false;
	};
}
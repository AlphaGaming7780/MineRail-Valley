#include <iostream>
#include <PallasEngine/AssetDatabase/AssetDatabase.hpp>
#include <PallasEngine/AssetDatabase/AssetTypes/FontAsset.hpp>

namespace pallas {
	FontAsset* FontAsset::GetDefault()
	{
		return AssetDatabase::Instance().GetAsset<FontAsset>(FontAsset::DefaultPath);
	}
	sf::Font* FontAsset::GetDefaultObject()
	{
		return FontAsset::GetDefault()->LoadObject();
	}
	void FontAsset::Load_Impl()
	{
		m_Instance = new sf::Font();
		std::string fullPath = AssetDatabase::Instance().GetDatabasePath() + "\\" + m_Path;
		if (!m_Instance->openFromFile(fullPath)) {
			m_Logger->ErrorO("[FontAsset] Failed to load font: ", fullPath);
			delete m_Instance;
			m_Instance = nullptr;
		}
	}
}

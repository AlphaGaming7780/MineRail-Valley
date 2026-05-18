#include <iostream>
#include <PallasEngine/AssetDatabase/AssetDatabase.hpp>
#include <PallasEngine/AssetDatabase/AssetTypes/TextureAsset.hpp>

namespace pallas {


    TextureAsset* TextureAsset::GetDefault()
    {
        return AssetDatabase::Instance().GetAsset<TextureAsset>(TextureAsset::DefaultTexturePath);
    }

    sf::Texture* TextureAsset::GetDefaultObject()
    {
        return TextureAsset::GetDefault()->LoadObject();
    }

    void TextureAsset::Load_Impl()
    {
        m_Instance = new sf::Texture();
        std::string fullPath = AssetDatabase::Instance().GetDatabasePath() + "\\" + m_Path;
        if (!m_Instance->loadFromFile(fullPath)) {
            m_Logger->ErrorO("[TextureAsset] Failed to load texture: ", fullPath);
            delete m_Instance;
            m_Instance = nullptr;
        }
    }

}

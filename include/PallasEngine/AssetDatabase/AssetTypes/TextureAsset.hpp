#pragma once

#include <PallasEngine/AssetDatabase/IAsset.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

namespace pallas {

    class TextureAsset : public IObjectAsset<sf::Texture> {
    public:
        //TextureAsset(const std::string& path) : IObjectAsset(path) {}
        static constexpr const char* DefaultTexturePath = "PallasEngine\\DEFAULT.png";

        static TextureAsset* GetDefault();
        
        sf::Texture* GetDefaultObject() override;


    protected:
        void Load_Impl() override;
    };

}

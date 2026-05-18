#pragma once

#include <PallasEngine/AssetDatabase/IAsset.hpp>
#include <SFML/Graphics.hpp>

namespace pallas
{
    class FontAsset : public IObjectAsset<sf::Font> {
    public:
        //FontAsset(const std::string& path) : IObjectAsset(path) {}
        static constexpr const char* DefaultPath = "Fonts\\LiberationSans.ttf";
        static FontAsset* GetDefault();

        sf::Font* GetDefaultObject() override;


    protected:
        void Load_Impl() override;
    };
}
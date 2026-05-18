#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <filesystem>
#include <iostream>

#include "PallasEngine/Logging/Logger.hpp"

#include <PallasEngine/AssetDatabase/IAsset.hpp>
#include <PallasEngine/AssetDatabase/AssetTypes.hpp>

namespace pallas {

    class AssetDatabase {
    public:

        const std::string DatabaseFolder = "assets";

        ~AssetDatabase();

        // --- Singleton ---
        static AssetDatabase& Instance();

        // --- Ajouter un type supporté ---
        template<typename T>
        void AddSupportedType(const std::vector<std::string>& extensions);

        std::string GetDatabasePath() 
        {
            namespace fs = std::filesystem;
            return (fs::current_path() / DatabaseFolder).string();
        }

        // --- Charger la database ---
        void Load();
        void Unload();

        // --- Récupérer un asset ---
        IAsset* GetAsset(const std::string& path);

        bool TryGetAsset(const std::string& path, IAsset*& outAsset);

        template<typename T, typename = std::enable_if_t<std::is_base_of<IAsset, T>::value>>
        T* GetAsset(const std::string& path);

        template<typename T, typename = std::enable_if_t<std::is_base_of<IAsset, T>::value>>
        bool TryGetAsset(const std::string& path, T*& outAsset);

        template<typename T, typename = std::enable_if_t<std::is_base_of<IAsset, T>::value>>
        std::vector<T*> GetAssetsOfType();

    private:
        AssetDatabase() {
            AddSupportedType<TextureAsset>({ ".png", ".jpg" });
            AddSupportedType<FontAsset>({ ".ttf" });
        }

        template<typename T>
        static IAsset* CreateAsset(Logger* logger, const std::string& path) 
        { 
            IAsset* a = new T();
            a->m_Path = path;
            a->m_Logger = logger;
            return a;
        }

        Logger m_Logger = Logger("AssetDatabase");

        std::unordered_map<std::string, IAsset*> m_Assets;
        std::unordered_map<std::string, IAsset* (*)(Logger* logger, const std::string&)> m_SupportedTypes;
    };

    template<typename T>
    void AssetDatabase::AddSupportedType(const std::vector<std::string>& extensions)
    {
        static_assert(std::is_base_of<IAsset, T>::value);

        for (const auto& ext : extensions)
            m_SupportedTypes[ext] = &AssetDatabase::CreateAsset<T>;
    }

    template<typename T, typename>
    T* AssetDatabase::GetAsset(const std::string& path) {
        auto base = GetAsset(path);
        return dynamic_cast<T*>(base);
    }

    template<typename T, typename>
    bool AssetDatabase::TryGetAsset(const std::string& path, T*& outAsset)
    {
        if (outAsset != nullptr) {
            m_Logger.ErrorF("Gived outAsset wasn't a nullptr for '%s'", path.c_str());
            return false;
        }
        outAsset = GetAsset<T>(path);
        return outAsset != nullptr;
    }

    template<typename T, typename>
    std::vector<T*> AssetDatabase::GetAssetsOfType()
    {
        std::vector<T*> result;
        result.reserve(m_Assets.size());

        for (auto& [path, asset] : m_Assets)
        {
            if (auto casted = dynamic_cast<T*>(asset))
                result.push_back(casted);
        }

        return result;
    }

}
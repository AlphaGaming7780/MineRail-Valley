#include <PallasEngine/AssetDatabase/AssetDatabase.hpp>

namespace pallas {
    AssetDatabase::~AssetDatabase()
    {
        Unload();
    }

    AssetDatabase& AssetDatabase::Instance() {
        static AssetDatabase instance;
        return instance;
    }

    void AssetDatabase::Load()
    {
        namespace fs = std::filesystem;

        fs::path folder = fs::current_path() / DatabaseFolder;

        m_Logger.InfoO("Loading Database at ", folder.string());

        if (!fs::exists(folder) || !fs::is_directory(folder)) {
            m_Logger.InfoO("Assets folder not found: ", folder);
            return;
        }

        for (const auto& entry : fs::recursive_directory_iterator(folder)) {
            if (!entry.is_regular_file())
                continue;

            std::string ext = entry.path().extension().string();
            auto it = m_SupportedTypes.find(ext);

            if (it != m_SupportedTypes.end()) {

                // --- Création de l'asset via ton nouveau système ---
                std::string relativePath = fs::relative(entry.path(), folder).string();
                IAsset* asset = it->second(&m_Logger, relativePath); // new T(relativePath)

                //// --- Setup interne --- déplacer dans Create()
                //asset->m_Logger = &m_Logger;

                m_Logger.InfoO("Asset at ", asset->GetPath());

                m_Assets[asset->GetPath()] = asset;
            }
            else {
                m_Logger.InfoO("Ignored file (unsupported extension): ", entry.path());
            }
        }

        m_Logger.InfoF("Found %i assets.", m_Assets.size());
    }

    void AssetDatabase::Unload()
    {
        m_Logger.Info("Unloading AssetDatabase...");

        for (auto& [path, asset] : m_Assets)
        {
            if (!asset)
                continue;

            delete asset; // Le destructeur appelle déjà Unload(true)
        }

        m_Assets.clear();
        m_SupportedTypes.clear();

        m_Logger.Info("AssetDatabase unloaded.");
    }

    IAsset* AssetDatabase::GetAsset(const std::string& path) {
        auto it = m_Assets.find(path);
        if (it != m_Assets.end())
            return it->second;
        m_Logger.InfoO("Failed to get the asset at path ", path);
        return nullptr;
    }

    bool AssetDatabase::TryGetAsset(const std::string& path, IAsset*& outAsset) {
        if (outAsset != nullptr) {
            m_Logger.ErrorF("Gived outAsset wasn't a nullptr for '%s'", path.c_str());
            return false;
        }
        outAsset = GetAsset(path);
        return outAsset != nullptr;
    }

}
#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <filesystem>
#include <iostream>

#include <PallasEngine/Logging.hpp>

namespace Game
{
	template<typename T>
	class AssetDatabase
	{

	public:

		const std::string DatabasePath = GetDatabasePath();

		~AssetDatabase();

		AssetDatabase(const AssetDatabase&) = delete;
		AssetDatabase& operator=(const AssetDatabase&) = delete;

		void UnloadAssets();
		virtual void Unload(const std::string& path, bool force = false);
		void Unload( T*, bool force = false);

		bool IsLoaded(const std::string& path) const;

		std::vector<T*> GetAssets();

		T* GetAsset(const std::string& path);
		bool TryGetAsset(const std::string& path, T*& asset);

		virtual T* Load(const std::string& path);

		T* GetDefault();

	protected:

		AssetDatabase(const std::string& loggerName);

		std::unordered_map<std::string, T*> m_Assets;
		std::unordered_map<std::string, int> m_RefCount;
		pallas::Logger m_Logger;

		virtual std::string GetDefaultPath() = 0;
		virtual T* Load_Impl(const std::string& path) = 0;

		bool IncrementRefCount(const std::string& path);
		bool DecrementRefCount(const std::string& path, bool force = false);

	private:
		
		static std::string GetDatabasePath()
		{
			namespace fs = std::filesystem;
			return (fs::current_path() / "assets").string();
		}
	};

	template<typename T>
	AssetDatabase<T>::~AssetDatabase()
	{
		UnloadAssets();
	}

	template<typename T>
	T* AssetDatabase<T>::GetAsset(const std::string& path) {
		return m_Assets.contains(path) ? m_Assets[path] : nullptr;
	}

	template<typename T>
	bool AssetDatabase<T>::TryGetAsset(const std::string& path, T*& outAsset)
	{
		if (outAsset != nullptr) {
			m_Logger.ErrorF("Gived outAsset wasn't a nullptr for '%s'", path.c_str());
			return false;
		}
		outAsset = GetAsset(path);
		return outAsset != nullptr;
	}

	template<typename T>
	T* AssetDatabase<T>::Load(const std::string& path)
	{
		// Vérif déjà charger
		auto it = m_Assets.find(path);
		if (it != m_Assets.end())
		{
			IncrementRefCount(path);
			return it->second;
		}

		// Pas encore chargé -> charger
		m_Logger.InfoO("Load(): Loading asset: ", path);

		T* instance = Load_Impl(path);

		if (!instance)
		{
			m_Logger.ErrorO("Load(): Failed to load asset: ", path);
			instance = GetDefault();
		}

		m_Assets[path] = instance;
		m_RefCount[path] = 1;

		return instance;
	}

	template<typename T>
	inline T* AssetDatabase<T>::GetDefault()
	{
		std::string path = GetDefaultPath();
		if (m_Assets.contains(path))
			return m_Assets[path];

		// Pas encore chargé -> charger
		m_Logger.InfoO("GetDefault(): Loading asset: ", path);

		T* instance = Load_Impl(path);

		if (!instance)
		{
			m_Logger.ErrorO("GetDefault(): Failed to load default asset: ", path);
			throw std::runtime_error("Failed to load default asset: " + path);
		}

		m_Assets[path] = instance;
		m_RefCount[path] = 1;

		return instance;

	}

	template<typename T>
	AssetDatabase<T>::AssetDatabase(const std::string& loggerName) : m_Logger(loggerName)
	{

	}

	template<typename T>
	void AssetDatabase<T>::UnloadAssets()
	{
		for (auto it = m_Assets.begin(); it != m_Assets.end(); )
		{
			const std::string& path = it->first;
			T* asset = it->second;

			int& ref = m_RefCount[path];

			if (ref > 0)
			{
				m_Logger.WarnO("UnloadAssets(): Forcing unload of '", path, "' with refCount=", ref);
			}

			delete asset;

			it = m_Assets.erase(it);
			m_RefCount.erase(path);
		}
	}

	template<typename T>
	void AssetDatabase<T>::Unload(const std::string& path, bool force)
	{
		if (!m_Assets.contains(path))
			return;

		if (!DecrementRefCount(path, force))
			return;

		delete m_Assets[path];
		m_Assets.erase(path);
		m_RefCount.erase(path);
	}

	template<typename T>
	void AssetDatabase<T>::Unload(const T* asset, bool force)
	{
		if (!asset)
			return;

		for (auto it = m_Assets.begin(); it != m_Assets.end(); ++it)
		{
			if (it->second == asset)
			{
				Unload(it->first, force);
				return;
			}
		}

		m_Logger.WarnO("Unload(T*): asset pointer not found in database.");
	}

	template<typename T>
	inline bool AssetDatabase<T>::IsLoaded(const std::string& path) const
	{
		return m_Assets.contains(path);
	}

	template<typename T>
	inline std::vector<T*> AssetDatabase<T>::GetAssets()
	{
		std::vector<T*> result;
		result.reserve(m_Assets.size());

		for (auto& [path, asset] : m_Assets)
			result.push_back(asset);

		return result;
	}

	template<typename T>
	bool AssetDatabase<T>::IncrementRefCount(const std::string& path)
	{
		if (!m_RefCount.contains(path)) m_RefCount.emplace(path, 0);
		int& refCount = m_RefCount[path];
		if (++refCount <= 0)
		{
			refCount = 1;
		}
		return refCount == 1;
	}

	template<typename T>
	bool AssetDatabase<T>::DecrementRefCount(const std::string& path, bool force)
	{
		int& refCount = m_RefCount[path];
		if (force)
		{
			if (refCount > 0)
				m_Logger.WarnO("DecrementRefCount(): Forced to 0 but there was still ", refCount, " reference. ", path);

			refCount = 0;
			return true;
		}

		if (refCount <= 0) {
			m_Logger.WarnO("RefCount already zero for ", path);
			return true;
		}

		return --refCount <= 0;
	}
}

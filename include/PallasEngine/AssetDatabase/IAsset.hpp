#pragma once
#include <string>
#include <PallasEngine/Logging.hpp>

namespace pallas {

    class IAsset
    {
    public:
        virtual ~IAsset() = default;

        const std::string& GetPath() const { return m_Path; }

    protected:
        friend class AssetDatabase; // pour que AssetDatabase puisse assigner le logger et le path.
        std::string m_Path;
        Logger* m_Logger = nullptr;
    };

    class ILoadableAsset : public IAsset
    {
    public:
        
        //virtual ~ILoadableAsset()
        //{
        //    Unload(true);
        //}

        virtual bool IsLoaded() const = 0;

        void Load()
        {
            if (IncrementRefCount())
            {
                m_Logger->InfoO(typeid(*this).name(), ".Load(): Loading asset: ", m_Path);
                Load_Impl();
            }
            else if (!IsLoaded())
            {
                m_Logger->ErrorO(typeid(*this).name(), ".Load(): Asset reference count is out of sync: ", m_Path); //Maybe force to 1.
                Load_Impl();
            }

            if (!IsLoaded())
                m_Logger->ErrorO(typeid(*this).name(), ".Load(): Failed to load asset: ", m_Path);
        }

        void Unload(bool force = false)
        {
            if (DecrementRefCount(force))
            {
                m_Logger->InfoO(typeid(*this).name(), ".Unload(): ", m_Path);
                Unload_Impl();
            }
        }


    protected:

        virtual void Load_Impl() = 0;
        virtual void Unload_Impl() = 0;

        bool IncrementRefCount()
        {
            if (++m_InstanceRefCount <= 0)
            {
                m_InstanceRefCount = 1;
            }
            return m_InstanceRefCount == 1;
        }

        bool DecrementRefCount(bool force = false)
        {
            if (force)
            {
                if (m_InstanceRefCount > 0)
                    m_Logger->WarnO(typeid(*this).name(), "DecrementRefCount(): Forced to 0 but there was still ", m_InstanceRefCount, " reference. ", m_Path);

                m_InstanceRefCount = 0;
                return true;
            }
            return --m_InstanceRefCount == 0;
        }

    private:
        int m_InstanceRefCount = 0;

    };

    class IDataAsset : public ILoadableAsset
    {

    };

    class IObjectAssetBase : public ILoadableAsset
    {

    };

    template<typename T>
    class IObjectAsset : public IObjectAssetBase
    {
    public:

        virtual ~IObjectAsset()
        {
            Unload(true);
        }

        bool IsLoaded() const override final
        {
            return m_Instance != nullptr;
        }

        T* LoadObject()
        {
            Load();
            return m_Instance ? m_Instance : GetDefaultObject();
        }

        virtual T* GetDefaultObject() = 0 ;

    protected:

        T* m_Instance = nullptr;

        void Unload_Impl() override final
        {
            //if (m_Instance == nullptr) return;
            delete m_Instance;
            m_Instance = nullptr;
        }

    private:
    };
}


#pragma once

namespace pallas
{
    template<typename... Args>
    class Delegate
    {
    public:
        using Stub = std::function<void(void*, Args...)>;

        Delegate() = default;

        // Fonction libre
        Delegate(void(*func)(Args...))
            : m_Instance(nullptr)
        {
            m_Stub = [func](void*, Args... args) { func(args...); };
            m_MethodPtr = reinterpret_cast<void*>(func);
        }

        // Méthode d'instance
        template<typename T>
        Delegate(T* instance, void (T::* method)(Args...))
            : m_Instance(instance)
        {
            m_Stub = [method](void* obj, Args... args)
                {
                    T* o = static_cast<T*>(obj);
                    (o->*method)(args...);
                };

            m_MethodPtr = *reinterpret_cast<void**>(&method);
        }

        bool operator==(const Delegate& other) const
        {
            return m_Instance == other.m_Instance &&
                m_MethodPtr == other.m_MethodPtr;
        }

        void operator()(Args... args) const
        {
            m_Stub(m_Instance, args...);
        }

    private:
        void* m_Instance = nullptr;
        Stub m_Stub;
        void* m_MethodPtr = nullptr;
    };

}

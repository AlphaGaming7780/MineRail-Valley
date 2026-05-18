#pragma once
#include <chrono>
#include <unordered_map>
#include <mutex>

namespace pallas
{
#ifdef DEBUG
    class PerformanceLogger
    {
    public:
        struct Stats
        {
            double avg = 0.0;
            double min = std::numeric_limits<double>::max();
            double max = 0.0;
            size_t count = 0;
        };

        class Scoped
        {
        public:
            Scoped(const char* id);
            ~Scoped();
            void End();

        private:
            const char* m_Id;
            bool m_Ended = false;
            std::chrono::high_resolution_clock::time_point m_Start;
        };

    public:
        static PerformanceLogger& Get();
        void Record(const std::string& id, double ms);
        void SaveToFile();
        void SaveToFile(const std::string& path);
        void Reset();

    private:
        std::mutex m_Mutex;
        std::unordered_map<std::string, Stats> m_Data;
    };
#endif // DEBUG
}
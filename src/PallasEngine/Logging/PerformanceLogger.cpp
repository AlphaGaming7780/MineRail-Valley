#include <fstream>
#include <filesystem>
#include <PallasEngine/Logging/PerformanceLogger.hpp>

namespace pallas
{
#ifdef DEBUG
    PerformanceLogger& PerformanceLogger::Get()
    {
        static PerformanceLogger instance;
        return instance;
    }

    void PerformanceLogger::Record(const std::string& id, double ms)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        auto& s = m_Data[id];
        s.count++;
        s.avg += (ms - s.avg) / s.count;
        if (ms < s.min) s.min = ms;
        if (ms > s.max) s.max = ms;
    }

    void PerformanceLogger::SaveToFile()
    {
        namespace fs = std::filesystem;
        fs::path logDir = fs::current_path() / "Logs";
        if (!fs::exists(logDir)) fs::create_directories(logDir);
        SaveToFile((logDir / "PerformanceLogger.log").string());
    }

    void PerformanceLogger::SaveToFile(const std::string& path)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        std::ofstream file(path);
        for (auto& [id, s] : m_Data)
        {
            file << id << " : avg=" << s.avg
                << " ms, min=" << s.min
                << " ms, max=" << s.max
                << " ms, samples=" << s.count
                << "\n";
        }
    }
    void PerformanceLogger::Reset()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Data.clear();
    }
    PerformanceLogger::Scoped::Scoped(const char* id)
        : m_Id(id), m_Start(std::chrono::high_resolution_clock::now())
    {
    }

    PerformanceLogger::Scoped::~Scoped()
    {
        End();
    }

    void PerformanceLogger::Scoped::End()
    {
        if (m_Ended) return;
        m_Ended = true;

        auto end = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(end - m_Start).count();
        PerformanceLogger::Get().Record(m_Id, ms);

    }
#endif // DEBUG
}
#pragma once
#include <thread>
#include <future>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace pallas
{
    class ThreadPool
    {
    public:
        static ThreadPool& Instance()
        {
            static ThreadPool pool;
            return pool;
        }

        template<typename F>
        auto submit(F&& fn)
        {
            using Ret = std::invoke_result_t<F>;

            auto prom = std::make_shared<std::promise<Ret>>();

            {
                std::unique_lock lock(m);
                jobs.emplace(prom, fn = std::forward<F>(fn) mutable {
                    try {
                        if constexpr (std::is_void_v<Ret>) {
                            fn();
                            prom->set_value();
                        }
                        else {
                            prom->set_value(fn());
                        }
                    }
                    catch (...) {
                        prom->set_exception(std::current_exception());
                    }
                });
            }

            cv.notify_one();
            return prom->get_future();
        }

    private:
        ThreadPool(unsigned threadCount = std::thread::hardware_concurrency())
        {
            for (unsigned i = 0; i < threadCount; ++i)
            {
                workers.emplace_back([this] {
                    while (true)
                    {
                        std::function<void()> job;
                        {
                            std::unique_lock lock(m);
                            cv.wait(lock, [&] {
                                return stop || !jobs.empty();
                                });

                            if (stop && jobs.empty())
                                return;

                            job = std::move(jobs.front());
                            jobs.pop();
                        }
                        job();
                    }
                    });
            }
        }

        ~ThreadPool()
        {
            {
                std::unique_lock lock(m);
                stop = true;
            }
            cv.notify_all();

            for (auto& w : workers)
                w.join();
        }

        std::vector<std::thread> workers;
        std::queue<std::function<void()>> jobs;
        std::mutex m;
        std::condition_variable cv;
        bool stop = false;
    };
}
#pragma once
#include <functional>
#include <algorithm>
#include <future>
#include <mutex>
#include <utility>
#include <vector>
#include <queue>
#include <thread>

// comment this if you do not want multithreading
#define PARALLEL

class ThreadPool {
public:
    ThreadPool(size_t threads) : thread_size(threads), stop(false)
    {
        for (size_t i = 0; i < threads; ++i)
            workers.emplace_back(
                [this]
                {
                    while (!stop)
                    {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            this->condition.wait(lock,
                                [this] { return this->stop || !this->tasks.empty(); });
                            if (this->stop && this->tasks.empty())
                                return;
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }

                        task();
                    }
                }
        );
    }

    size_t size() { return thread_size; }
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<void>
    {

        auto task = std::make_shared< std::packaged_task<void()> >(
            [Func = std::forward<F>(f)] { return Func(); }
        );

        std::future<void> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // don't allow enqueueing after stopping the pool
            if (stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers)
            worker.join();
    }

private:
    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
    // the task queue
    std::queue< std::function<void()> > tasks;
    size_t thread_size;
    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};


class Concurrent final {
public:
    template <class It, class Fn>
    static void ForEach(It first, It last, Fn func) {
#ifdef PARALLEL
        ThreadPool& pool = getThreadPool();

        auto totalElements = std::distance(first, last);
        auto chunkSize = totalElements / pool.size();
        auto remainder = totalElements % pool.size();

        std::vector<std::future<void>> futures;

        auto begin = first;
        for (unsigned int i = 0; i < pool.size(); ++i) {
            auto end = std::next(begin, chunkSize + (remainder > 0 ? 1 : 0));
            if (remainder > 0) --remainder;
            futures.push_back(pool.enqueue([=, &func]() {
                std::for_each(begin, end, func);
            }));
            begin = end;
        }

        for (auto& future : futures) {
            future.get(); // Wait for each task to complete
        }
#else
        std::for_each(first, last, func);
#endif
    }
private:
    static ThreadPool& getThreadPool() {
        static ThreadPool pool(std::thread::hardware_concurrency());
        return pool;
    }
};

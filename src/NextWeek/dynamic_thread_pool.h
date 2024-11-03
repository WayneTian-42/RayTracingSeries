
#ifndef DYNAMICTHREADPOOL_HPP
#define DYNAMICTHREADPOOL_HPP

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>

// 线程池类
class DynamicThreadPool
{
  public:
    // 构造函数，传入线程数
    DynamicThreadPool(size_t threads = 0);
    // 析构
    ~DynamicThreadPool();

    // 入队任务(传入函数和函数的参数)
    // template <class F, class... Args>
    // auto enqueue(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>;
    // 一个最简单的函数包装模板可以这样写(C++11)适用于任何函数(变参、成员都可以)
    // template<class F, class... Args>
    // auto enqueue(F&& f, Args&&... args) -> decltype(declval<F>()(declval<Args>()...))
    // {    return f(args...); }
    // C++14更简单
    template <class F, class... Args> auto enqueue(F &&f, Args &&...args);
    // {    return f(args...); }

    // 停止线程池
    void stopAll();

  private:
    void newThread();

  private:
    std::atomic_int run_workers; // 当前运行的工作线程数
    int max_workers;             // 最大工作线程数
    // 任务队列
    std::queue<std::function<void()>> tasks;

    // synchronization 异步
    std::mutex queue_mutex;            // 队列互斥锁
    std::condition_variable condition; // 条件变量
    bool stop;                         // 停止标志
};

// 构造函数仅启动一些工作线程
inline DynamicThreadPool::DynamicThreadPool(size_t threads) : run_workers(0), max_workers(threads), stop(false)
{
    if (max_workers == 0 || max_workers > (int)std::thread::hardware_concurrency())
    {
        max_workers = std::thread::hardware_concurrency();
    }
}

// 添加一个新的工作任务到线程池
// template <class F, class... Args>
// auto DynamicThreadPool::enqueue(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
template <class F, class... Args> auto DynamicThreadPool::enqueue(F &&f, Args &&...args)
{
    using return_type = typename std::result_of<F(Args...)>::type;

    // 将任务函数和其参数绑定，构建一个packaged_task
    auto task =
        std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    // 获取任务的future
    std::future<return_type> res = task->get_future();

    size_t tasks_size = 0;
    {
        std::lock_guard<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        // 不允许入队到已经停止的线程池
        if (stop)
        {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        // 将任务添加到任务队列
        tasks.emplace([task]() { (*task)(); });
        tasks_size = tasks.size();
    }
    // 判断当前的任务积累数，如果任务积累太多，就再创建一个线程
    if (run_workers < max_workers)
    {
        newThread(); // 创建新线程
    }
    // 发送通知，唤醒某一个工作线程取执行任务
    condition.notify_one();
    return res;
}

inline DynamicThreadPool::~DynamicThreadPool()
{
    stopAll();
}

inline void DynamicThreadPool::stopAll()
{
    {
        // 拿锁
        std::unique_lock<std::mutex> lock(queue_mutex);
        // 停止标志置true
        stop = true;
    }
    // 通知所有工作线程，唤醒后因为stop为true了，所以都会结束
    condition.notify_all();
    // 等待所有线程结束
    while (run_workers > 0)
    {
        // 如果工作线程是意外结束的，没有将 run_workers 减一，那么这里会陷入死循环
        // 所以这里也可以修改为循环一定次数后就退出，不等了
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

inline void DynamicThreadPool::newThread()
{
    ++run_workers; // 运行线程数加一

    std::thread thr([this]() {
        while (true)
        {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(this->queue_mutex);

                // 使用条件变量等待新任务到来或停止信号
                this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });

                if (this->stop && this->tasks.empty())
                {
                    // 如果停止标志为真且任务队列为空，退出线程
                    break;
                }

                // 从任务队列取出一个任务
                task = std::move(this->tasks.front());
                this->tasks.pop();
            }
            // 执行任务
            task();
        }
        --run_workers; // 运行线程数减一
    });

    thr.detach(); // 分离执行
}

#endif // DYNAMICTHREADPOOL_HPP

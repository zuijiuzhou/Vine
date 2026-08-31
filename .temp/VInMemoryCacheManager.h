#pragma once
#include <VmrSceneGlobal.h>

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace VMR_ROOT_NS
{
    /*
     * @brief 内存缓存管理器（线程安全，支持 TTL 过期回收）
     *
     * - 所有接口均可在多线程下调用；
     * - 过期策略：ttl > 0 的条目在写入后 ttl 秒过期，ttl <= 0 表示永久存活；
     * - 惰性删除：get/contains 命中已过期条目时立即删除；
     * - 后台回收：同一模板实例化类型的所有实例共享一个回收线程（类内静态成员），
     *   start() 后本实例参与回收，stop() 或析构后退出；
     *   最后一个实例退出时后台线程自动结束。
     *
     * @tparam TKey 键类型（需支持 std::hash 与 operator==）
     * @tparam TVal 缓存值类型（需支持拷贝/移动构造）
     */
    template <typename TKey, typename TVal>
    class VInMemoryCacheManager
    {
      public:
        using Clock = std::chrono::steady_clock;

        /*
         * @brief 缓存条目
         */
        struct Cache
        {
            Cache() = default;

            Cache(TVal v, int ttl, Clock::time_point expire_at)
              : val(std::move(v))
              , ttl_(ttl)
              , expire_at_(expire_at)
            {
            }

            TVal val;
            // 存活时间（秒），<=0 表示永久存活
            int ttl_{ -1 };
            // 过期时间点，ttl_ <= 0 时无效
            Clock::time_point expire_at_{};
        };

      public:
        VInMemoryCacheManager() noexcept = default;
        ~VInMemoryCacheManager()
        {
            shutdown();
        }

        VInMemoryCacheManager(const VInMemoryCacheManager&) = delete;
        VInMemoryCacheManager(VInMemoryCacheManager&&) = delete;
        VInMemoryCacheManager& operator=(const VInMemoryCacheManager&) = delete;
        VInMemoryCacheManager& operator=(VInMemoryCacheManager&&) = delete;

      public:
        /*
         * @brief 添加或更新缓存条目
         * @param ttl 存活时间（秒），<=0 表示永久存活
         */
        void push(const TKey& key, const TVal& val, int ttl = -1)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            caches_.insert_or_assign(key, Cache(val, ttl, makeExpireAt(ttl)));
        }

        ///*
        // * @brief 添加或更新缓存条目（右值版本）
        // */
        // void push(const TKey& key, TVal&& val, int ttl = -1)
        //{
        //    std::lock_guard<std::mutex> lock(mutex_);
        //    caches_.insert_or_assign(key, Cache(std::move(val), ttl, makeExpireAt(ttl)));
        //}

        /*
         * @brief 获取缓存值
         * @return 命中且未过期时返回缓存值，否则返回 std::nullopt
         */
        std::optional<TVal> get(const TKey& key) const
        {
            const auto now = Clock::now();
            std::lock_guard<std::mutex> lock(mutex_);

            const auto it = caches_.find(key);
            if (it == caches_.end())
            {
                return std::nullopt;
            }

            if (isExpired(it->second, now))
            {
                caches_.erase(it);
                return std::nullopt;
            }

            return it->second.val;
        }

        /*
         * @brief 获取缓存值到输出参数，避免一次拷贝
         * @return true 命中且未过期，false 不存在或已过期
         */
        bool get(const TKey& key, TVal& out) const
        {
            const auto now = Clock::now();
            std::lock_guard<std::mutex> lock(mutex_);

            const auto it = caches_.find(key);
            if (it == caches_.end())
            {
                return false;
            }

            if (isExpired(it->second, now))
            {
                caches_.erase(it);
                return false;
            }

            out = it->second.val;
            return true;
        }

        /*
         * @brief 判断是否存在未过期的缓存条目
         */
        bool contains(const TKey& key) const
        {
            const auto now = Clock::now();
            std::lock_guard<std::mutex> lock(mutex_);

            const auto it = caches_.find(key);
            if (it == caches_.end())
            {
                return false;
            }

            if (isExpired(it->second, now))
            {
                caches_.erase(it);
                return false;
            }

            return true;
        }

        /*
         * @brief 移除指定缓存条目
         * @return 移除的条目数量（0 或 1）
         */
        size_t remove(const TKey& key)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return caches_.erase(key);
        }

        /*
         * @brief 清理所有已过期条目
         * @return 清理的条目数量
         */
        size_t removeExpired() const
        {
            const auto now = Clock::now();
            std::lock_guard<std::mutex> lock(mutex_);

            size_t removed = 0;
            for (auto it = caches_.begin(); it != caches_.end();)
            {
                if (isExpired(it->second, now))
                {
                    it = caches_.erase(it);
                    ++removed;
                }
                else
                {
                    ++it;
                }
            }

            return removed;
        }

        /*
         * @brief 清空所有条目
         */
        void clear()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            caches_.clear();
        }

        /*
         * @brief 获取当前条目数量
         * @note 可能包含已过期但尚未清理的条目
         */
        size_t size() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return caches_.size();
        }

        /*
         * @brief 判断缓存是否为空
         */
        bool empty() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return caches_.empty();
        }

        /*
         * @brief 预分配桶空间
         */
        void reserve(size_t count)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            caches_.reserve(count);
        }

      public:
        /*
         * @brief 启动后台回收，使当前实例参与回收列表
         * - 同一模板实例化类型的实例共享同一个后台线程；
         * - 线程已运行时，period 为最后一个调用者设置的值；
         * - 重复调用仅更新 period。
         * @param period 回收周期
         */
        void start()
        {
            std::lock_guard<std::mutex> lock(s_mutex_);
            s_instances_.insert(this);
            if (!s_thread_.joinable())
            {
                s_stopping_ = false;
                s_thread_ = std::thread(&VInMemoryCacheManager::cleanupLoop);
            }
        }

        /*
         * @brief 停止后台回收，使当前实例退出回收列表
         * - 当前实例是最后一个参与者时，后台线程结束；
         * - 未启动过时为空操作。
         */
        void stop()
        {
            shutdown();
        }

        /*
         * @brief 判断当前实例是否参与后台回收
         */
        bool isCleanupRunning() const
        {
            std::lock_guard<std::mutex> lock(s_mutex_);
            return s_instances_.contains(this);
        }

      private:
        /*
         * @brief 计算过期时间点
         */
        static Clock::time_point makeExpireAt(int ttl)
        {
            if (ttl > 0)
            {
                return Clock::now() + std::chrono::seconds(ttl);
            }
            return {};
        }

        /*
         * @brief 判断条目是否已过期
         */
        bool isExpired(const Cache& cache, Clock::time_point now) const noexcept
        {
            return cache.ttl_ > 0 && cache.expire_at_ <= now;
        }

        /*
         * @brief 使当前实例退出回收列表，必要时结束后台线程
         * - stop 与析构共用；
         * - 注销期间持有全局锁，保证后台线程不会访问已析构实例。
         */
        void shutdown()
        {
            std::thread to_join;
            bool is_last = false;
            {
                std::lock_guard<std::mutex> lock(s_mutex_);
                s_instances_.erase(this);
                if (s_instances_.empty() && s_thread_.joinable())
                {
                    s_stopping_ = true;
                    // 把线程对象移出成员：成员立即变为非 joinable，
                    // 之后并发 start() 会直接启动新线程，不会与本线程对象竞争。
                    to_join = std::move(s_thread_);
                    is_last = true;
                }
            }

            // 不是最后一个 manager
            if (!is_last)
            {
                return;
            }

            // join 在锁外进行：回收循环被唤醒后需重新取得全局锁才能退出
            s_cv_.notify_all();
            to_join.join();

            // join 期间若已有新实例注册，启动新线程兜底
            std::lock_guard<std::mutex> lock(s_mutex_);
            if (!s_instances_.empty() && !s_thread_.joinable())
            {
                s_stopping_ = false;
                s_thread_ = std::thread(&VInMemoryCacheManager::cleanupLoop);
            }
        }

        /*
         * @brief 后台回收循环
         * - 持有静态锁遍历实例并清理，与实例析构互斥，避免悬空访问。
         */
        static void cleanupLoop()
        {
            std::unique_lock<std::mutex> lock(s_mutex_);
            while (true)
            {
                s_cv_.wait_for(lock, s_period_, []
                { return s_stopping_; });
                if (s_stopping_)
                {
                    return;
                }

                for (const auto* instance : s_instances_)
                {
                    instance->removeExpired();
                }
            }
        }

      private:
        mutable std::mutex mutex_;
        mutable std::unordered_map<TKey, Cache> caches_;

        // ---- 同一模板实例化类型共享的后台线程状态 ----
        inline static std::mutex s_mutex_;
        inline static std::condition_variable s_cv_;
        inline static std::thread s_thread_;
        inline static std::unordered_set<const VInMemoryCacheManager*> s_instances_;
        inline static std::chrono::milliseconds s_period_{ std::chrono::seconds(1) };
        inline static bool s_stopping_{ false };
    };
} // namespace VMR_ROOT_NS
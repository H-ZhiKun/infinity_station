#pragma once

#include <atomic>
#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>

namespace _Kits
{

    namespace ObjectPoolDetails
    {
        // 检测 T 是否有 reset() 成员函数
        template <typename, typename = std::void_t<>>
        struct has_reset : std::false_type
        {
        };

        template <typename T>
        struct has_reset<T, std::void_t<decltype(std::declval<T &>().reset())>> : std::true_type
        {
        };

    } // namespace ObjectPoolDetails

    template <typename T>
    class ObjectPool : public std::enable_shared_from_this<ObjectPool<T>>
    {
      private:
        struct ObjectNode
        {
            std::unique_ptr<T> obj{nullptr};
            ObjectNode *next = nullptr;
            explicit ObjectNode(std::unique_ptr<T> &&o) : obj(std::move(o))
            {
            }
        };

        std::atomic<ObjectNode *> m_head{nullptr}; // 栈顶指针
        std::atomic<size_t> m_count{0};            // 池中对象计数
        size_t m_maxCount = 1024;                  // 默认最大池子大小
      public:
        ObjectPool(size_t maxCount = 1024) : m_maxCount(maxCount)
        {
        }
        ~ObjectPool()
        {
            auto node = m_head.load(std::memory_order_acquire);
            while (node)
            {
                auto next = node->next;
                delete node;
                node = next;
            }
        }

        // 版本1 获取对象，支持传参构造
        template <typename... Args>
        std::shared_ptr<T> getObject(Args &&...args)
        {
            auto node = popNode();
            std::unique_ptr<T> guard;
            if (node)
            {
                guard = std::move(node->obj);
                delete node;
            }
            else
            {
                guard = std::make_unique<T>(std::forward<Args>(args)...);
            }

            std::weak_ptr<ObjectPool> weakSelf = this->shared_from_this();
            auto rawPtr = guard.release();
            return std::shared_ptr<T>(rawPtr, [weakSelf](T *ptr) {
                if (auto self = weakSelf.lock())
                {
                    self->pushNode(std::unique_ptr<T>(ptr));
                }
                else
                {
                    delete ptr;
                }
            });
        }

        // 版本2：使用构造器初始化对象，可调用对象参数为(T*)
        template <typename Initializer, typename Reset, std::enable_if_t<std::is_invocable_v<Initializer, T *> && std::is_invocable_v<Reset, T *>, int> = 0>
        std::shared_ptr<T> getObject(Initializer &&initFunc, Reset &&resetFunc)
        {
            auto node = popNode();
            std::unique_ptr<T> guard;
            if (node)
            {
                guard = std::move(node->obj);
                delete node;
                resetFunc(guard.get());
            }
            else
            {
                guard = std::make_unique<T>();
                initFunc(guard.get());
            }

            std::weak_ptr<ObjectPool> weakSelf = this->shared_from_this();
            auto rawPtr = guard.release();
            return std::shared_ptr<T>(rawPtr, [weakSelf](T *ptr) {
                if (auto self = weakSelf.lock())
                {
                    self->pushNode(std::unique_ptr<T>(ptr));
                }
                else
                {
                    delete ptr;
                }
            });
        }

        // 获取当前池子里可用对象数
        size_t availableCount() const
        {
            return m_count.load(std::memory_order_relaxed);
        }

      private:
        void pushNode(std::unique_ptr<T> obj)
        {
            if (m_count.load(std::memory_order_relaxed) >= m_maxCount)
            {
                return;
            }
            auto *node = new ObjectNode(std::move(obj));
            ObjectNode *old_head = nullptr;

            do
            {
                old_head = m_head.load(std::memory_order_relaxed);
                node->next = old_head;
            } while (!m_head.compare_exchange_weak(old_head, node, std::memory_order_release, std::memory_order_relaxed));

            m_count.fetch_add(1, std::memory_order_relaxed);
        }

        ObjectNode *popNode()
        {
            ObjectNode *old_head = nullptr;

            do
            {
                old_head = m_head.load(std::memory_order_acquire);
                if (!old_head)
                    return nullptr;
            } while (!m_head.compare_exchange_weak(old_head, old_head->next, std::memory_order_acquire, std::memory_order_relaxed));

            m_count.fetch_sub(1, std::memory_order_relaxed);
            return old_head;
        }
    };

} // namespace _Kits

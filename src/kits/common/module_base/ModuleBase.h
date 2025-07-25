#pragma once
#include "kits/common/factory/ModuleRegister.h"
#include <QObject>
#include <QVariant>
#include <any>
#include <qlogging.h>
#include <qmetaobject.h>
#include <qthread.h>
#include <qvariant.h>
#include <unordered_map>
#include <utility>
#include <yaml-cpp/yaml.h>

namespace _Kits
{
    /**
     * @class ModuleBase
     * @brief 抽象基类，提供模块化的基础功能。
     *
     * 该类封装了模块的启动、停止、信号发送及槽函数调用的机制，
     * 并支持同步与异步调用子类的槽函数。
     */
    using RegisterKey = std::variant<std::string, int>;
    class ModuleBase : public QObject
    {
        Q_OBJECT
      public:
        explicit ModuleBase(QObject *parent = nullptr);
        virtual ~ModuleBase();
        virtual bool init(const YAML::Node &config) = 0; // 仅完成初始化模板方法， 注意信号只能放在start之中或者之后才可以正确触发
        virtual bool start() = 0;                        // 初始化后启动，如线程启动，信号触发等模板方法
        virtual bool stop() = 0;                         // 框架调用，不需要显式调用。 线程终止，终结信号发送，资源回收
        virtual bool customization(const std::unordered_map<RegisterKey, std::list<std::any>> &lvTasks);
        void setCustomName(const std::string &name);
        void tidyMetaMethod();
        QMetaMethod getMetaMethod(const std::string &funcName);
        /**
         * @brief 异步调用子类中的槽函数或信号。
         *
         * 该方法不会阻塞调用线程，调用后立即返回。
         * 适用于动态调用信号或槽函数。
         *
         * @note 考虑到异步框架的安全性和隔离性，
         *
         *       请使用此接口调用派生类中的槽函数或信号。
         *       槽函数的访问限定应使用 `private slots:` 进行修饰，
         *       信号不需要访问限定。
         *
         * @param funcName 需要调用的槽函数或信号的名称。
         * @param args 不定变参。
         * @return 是否成功调用槽函数或信号。
         */
        template <typename... Args>
        bool invokeNameAsync(const std::string &funcName, Args &&...args);
        template <typename... Args>
        bool invokeAsync(const QMetaMethod &method, Args &&...args);
        /**
         * @brief 同步调用子类中的槽函数或信号。
         *
         * 该方法会阻塞当前调用线程，直到槽函数或信号执行完毕并返回结果，
         * 或者超时退出（默认超时时间为 2000 ms）。
         * 适用于需要等待执行结果的场景。
         *
         * @note 槽函数的访问限定应使用 `private slots:` 进行修饰，
         *       信号不需要访问限定。
         *
         * @param funcName 需要调用的槽函数或信号的名称。
         * @param args 不定参数。
         * @return 槽函数或信号的返回结果。如果调用失败或超时，返回空的
         * QVariant。
         */
        template <typename Ret, typename... Args>
        Ret invokeNameSync(const std::string &funcName, Args &&...args);
        template <typename Ret, typename... Args>
        Ret invokeSync(const QMetaMethod &method, Args &&...args);
        static bool tisMetaConnect(ModuleBase *objSignal, const std::string &signalName, ModuleBase *objSlot, const std::string &slotName);

      private:
        std::unordered_map<std::string, QMetaMethod> m_mapMetaMethods;
        std::string m_objectName;
    };

    template <typename... Args>
    inline bool ModuleBase::invokeNameAsync(const std::string &funcName, Args &&...args)
    {
        auto finder = m_mapMetaMethods.find(funcName);
        if (finder == m_mapMetaMethods.end())
        {
            return false;
        }

        return invokeAsync(finder->second, std::forward<Args>(args)...);
    }
    template <typename... Args>
    inline bool ModuleBase::invokeAsync(const QMetaMethod &method, Args &&...args)
    {
        bool success = method.invoke(this, std::forward<Args>(args)...);
        if (!success)
        {
            return false;
        }
        return true;
    }

    template <typename Ret, typename... Args>
    inline Ret ModuleBase::invokeNameSync(const std::string &funcName, Args &&...args)
    {
        Ret result;
        auto finder = m_mapMetaMethods.find(funcName);
        if (finder == m_mapMetaMethods.end())
        {
            qWarning() << "Function not found:" << QString::fromStdString(funcName);
            return result;
        }
        return invokeSync(finder->second, std::forward<Args>(args)...);
    }

    template <typename Ret, typename... Args>
    inline Ret ModuleBase::invokeSync(const QMetaMethod &method, Args &&...args)
    {
        Ret result;
        Qt::ConnectionType connType = (QThread::currentThread() == this->thread()) ? Qt::DirectConnection : Qt::BlockingQueuedConnection;

        bool ret = method.invoke(this, connType, Q_RETURN_ARG(Ret, result), std::forward<Args>(args)...);
        return result;
    }
} // namespace _Kits
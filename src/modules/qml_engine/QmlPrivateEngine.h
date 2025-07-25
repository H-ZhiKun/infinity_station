#pragma once
#include "kits/common/module_base/ModuleBase.h"
#include "kits/common/qml_kits/QmlCommunication.h"
#include "tis_global/EnumClass.h"
#include <functional>
#include <json/json.h>
#include <qmetaobject.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <unordered_map>
#include <utility>

namespace _Modules
{

    /**
     * @class QmlPrivateEngine
     * @brief QmlPrivateEngine 继承自 ModuleBase，用于处理 QML 交互。
     */
    class QmlPrivateEngine : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(QmlPrivateEngine)

      public:
        explicit QmlPrivateEngine(QObject *parent = nullptr);
        virtual ~QmlPrivateEngine();

        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;
        virtual bool customization(const std::unordered_map<_Kits::RegisterKey, std::list<std::any>> &lvTasks) override;
      private slots:
        void afterQmlActions(_Kits::QmlCommunication::QmlActions qmlActions, const QVariant &value);

        /**
         * @brief 通过 invokeModule 触发的信号。
         * 在 controller 中使用 QVariant 作为参数调用此信号。
         * @param value 传递的数据。
         */
        void callFromCpp(TIS_Info::QmlCommunication::ForQmlSignals ForQmlSignals, const QVariant &value);

      private:
        QString findQmlFileInResources(const QString &fileName);
        void storeCppTask();
        void cppCallWithStart();
        std::deque<std::pair<int, QVariant>> m_storeCallsFromCpp;
        bool m_isQmlLoaded = false;
        std::unique_ptr<QObject> m_qmlEngine = nullptr;                                             /**< QML 引擎实例。*/
        bool m_bRunning = true;                                                                     /**< 运行状态标志。*/
        std::unique_ptr<_Kits::QmlCommunication> m_communication = nullptr;                         /**< QML 交互对象。*/
        std::unordered_map<int, std::vector<std::function<void(const QVariant &)>>> m_storeQmlTask; // qml actions执行者
        std::unordered_map<int, QMetaMethod> m_storeCppTask;                                        // cpp到qml的执行者
        QString m_sDisplayPage = QStringLiteral("main.qml"); // 当前显示的页面名称，默认为 main_page。
        QStringList enabledModules;
        int m_cameraNmber = 0; // 相机数量
                               // 获取已启用的模块列表
      public:
        /**
         * @brief 将T枚举值转换为对应的字符串。
         * @param 枚举值。
         * @return QString 对应的字符串名称。
         */
        template <typename T>
        static QString enumToString(T value)
        {
            // 获取枚举类型的元信息
            QMetaEnum metaEnum = QMetaEnum::fromType<T>();
            // 将枚举值转换为字符串
            const char *key = metaEnum.valueToKey(static_cast<int>(value));
            return key ? QString(key) : QString();
        }
        template <typename T>
        static int stringToEnum(const char *key)
        {
            QMetaEnum metaEnum = QMetaEnum::fromType<T>();
            return metaEnum.keysToValue(key);
        }
    };

} // namespace _Modules

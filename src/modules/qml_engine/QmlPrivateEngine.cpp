#include "QmlPrivateEngine.h"
#include "kits/common/qml_kits/ImagePainter.h"
#include "kits/common/log/CRossLogger.h"
#include "kits/common/thread_pool/ConcurrentPool.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <any>
#include <json/value.h>
#include <memory>
#include <qdir.h>
#include <qdiriterator.h>
#include <qglobal.h>
#include <qlogging.h>
#include <qmetaobject.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qthread.h>
#include <qtmetamacros.h>
#include <utility>
#include "kits/common/system_config/SystemConfig.h"

using namespace _Modules;
using namespace _Kits;
QmlPrivateEngine::QmlPrivateEngine(QObject *parent) : m_bRunning(true), ModuleBase(parent)
{
}
QmlPrivateEngine::~QmlPrivateEngine()
{
}
bool QmlPrivateEngine::init(const YAML::Node &config)
{
    // 读取 config_path
    if (!config["config_path"])
    {
        return false;
    }

    std::string config_path = config["config_path"].as<std::string>();
    if (config_path.empty())
    {
        qWarning() << "No configuration path found.";
        return false;
    }

    // 加载配置文件
    auto &sysConfig = _Kits::SystemConfig::instance();
    auto loadFilePath = sysConfig.configVersionPath() + config_path;
    YAML::Node config_node = _Kits::SystemConfig::instance().loadYamlFile(loadFilePath);

    // 读取 display_page 字段
    if (config_node["qmlconfig"])
    {
        try
        {
            // 支持 qmlconfig 是序列或映射
            if (config_node["qmlconfig"].IsSequence() && config_node["qmlconfig"].size() > 0 && config_node["qmlconfig"][0]["display_page"])
            {
                m_sDisplayPage = QString::fromStdString(config_node["qmlconfig"][0]["display_page"].as<std::string>());
            }
            else if (config_node["qmlconfig"]["display_page"])
            {
                m_sDisplayPage = QString::fromStdString(config_node["qmlconfig"]["display_page"].as<std::string>());
            }
        }
        catch (...)
        {
            qWarning() << "QML display_page 配置读取失败，使用默认页面";
        }
    }

    for (const auto &name : config_node["qmlconfig"][0]["enabledModules"])
    {
        QString modName = QString::fromStdString(name.as<std::string>());
        enabledModules << modName;
    }
    // 创建通信对象
    auto communication = std::make_unique<QmlCommunication>();
    QObject::connect(communication.get(), &QmlCommunication::behaviorFromQml, this, &QmlPrivateEngine::afterQmlActions);
    m_communication = std::move(communication);
    storeCppTask();
    return true;
}

void QmlPrivateEngine::storeCppTask()
{
    const QMetaObject *metaObject = m_communication->metaObject();
    for (int i = 0; i < metaObject->methodCount(); ++i)
    {
        QMetaMethod method = metaObject->method(i);
        if (method.methodType() == QMetaMethod::Signal)
        {
            std::string sigName = method.name().toStdString();
            auto pos = sigName.find("FromCpp");
            if (pos != std::string::npos)
            {
                std::string sigKey = sigName.substr(0, pos);
                int enumValue = stringToEnum<QmlCommunication::ForQmlSignals>(sigKey.c_str());
                m_storeCppTask.emplace(enumValue, method);
            }
        }
    }
}

bool QmlPrivateEngine::start()
{
    QQuickStyle::setStyle("FluentWinUI3"); // 强制设置 Material 风格
    qDebug() << QQuickStyle::name() << "\n";
    // 注册 C++ 类型到 QML
    qmlRegisterType<QmlCommunication>("InfinityStation", 1, 0, "QmlCommunication");
    qmlRegisterType<_Kits::ImagePainter>("InfinityStation", 1, 0, "ImagePainter");

    // 创建Qml引擎必须在主线程（GUI 线程)
    auto engine = std::make_unique<QQmlApplicationEngine>();

    // 需要导入相对路径，在qml中才可以正确的导入模块
    engine->addImportPath("qrc:/infinity_station/res/qml");

    qDebug() << "QML import paths:" << engine->importPathList();
    engine->rootContext()->setContextProperty("qmlCommunication", m_communication.get());
    engine->rootContext()->setContextProperty("enabledModules", QVariant::fromValue(enabledModules));

    // 拼接完整 QML 路径
    QString qmlPath = findQmlFileInResources("main.qml");
    if (!qmlPath.isEmpty())
    {
        engine->load(qmlPath);
        // 判断加载是否成功
        if (engine->rootObjects().isEmpty())
        {
            LogError("{} not load", qmlPath.toStdString());
            return false;
        }
        m_qmlEngine = std::move(engine);
        m_isQmlLoaded = true;
    }
    else
    {
        LogError("main.qml not found in resources");
        return false;
    }
    return true;
}

QString QmlPrivateEngine::findQmlFileInResources(const QString &fileName)
{
    QDirIterator it(":/", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString path = it.next();
        if (path.endsWith(fileName))
        {
            return path;
        }
    }
    return {};
}

bool QmlPrivateEngine::stop()
{
    m_isQmlLoaded = false;
    m_bRunning = false;
    if (m_qmlEngine)
    {
        auto engine = qobject_cast<QQmlApplicationEngine *>(m_qmlEngine.get());
        engine->rootContext()->setContextProperty("qmlCommunication", nullptr);
        // 清空组件缓存，确保 QML 内部组件被提前释放
        engine->clearComponentCache();

        // 遍历所有 QML 根对象，使用 deleteLater() 让它们延迟删除
        const auto roots = engine->rootObjects();
        for (QObject *root : roots)
        {
            if (root)
            {
                root->deleteLater();
            }
        }

        // 可选：处理一下事件，确保 deleteLater() 操作被执行
        QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);

        // 清理 QML 引擎对象
        m_qmlEngine.reset();
        m_communication.reset();
    }
    return true;
}

bool QmlPrivateEngine::customization(const std::unordered_map<_Kits::RegisterKey, std::list<std::any>> &lvTasks)
{
    for (auto &[key, tasks] : lvTasks)
    {
        int nKey = std::get<1>(key);
        for (auto &task : tasks)
        {
            m_storeQmlTask[nKey].push_back(std::any_cast<std::function<void(const QVariant &)>>(task));
        }
    }
    return true;
}

void QmlPrivateEngine::cppCallWithStart()
{
    for (const auto &item : m_storeCallsFromCpp)
    {
        auto finder = m_storeCppTask.find(item.first);
        if (finder != m_storeCppTask.end())
        {
            auto &method = finder->second;
            method.invoke(m_communication.get(), item.second);
        }
    }
    m_storeCallsFromCpp.clear();
}

void QmlPrivateEngine::callFromCpp(TIS_Info::QmlCommunication::ForQmlSignals ForQmlSignals, const QVariant &value)
{
    int index = static_cast<int>(ForQmlSignals);
    auto finder = m_storeCppTask.find(index);
    if (finder == m_storeCppTask.end())
    {
        return;
    }
    if (!m_isQmlLoaded)
    {
        m_storeCallsFromCpp.push_back(std::pair<int, QVariant>(index, value));
        return;
    }

    if (!m_storeCallsFromCpp.empty())
    {
        cppCallWithStart();
    }

    auto &method = finder->second;
    method.invoke(m_communication.get(), value);
}

void QmlPrivateEngine::afterQmlActions(_Kits::QmlCommunication::QmlActions qmlAction, const QVariant &value)
{
    int index = static_cast<int>(qmlAction);
    auto finder = m_storeQmlTask.find(index);
    if (finder == m_storeQmlTask.end())
    {
        LogError("qml call cpp not register in controllers: {}", enumToString(qmlAction).toStdString());
        return;
    }
    auto &lvFuncs = finder->second;
    for (auto &func : lvFuncs)
    {
        _Kits::ConcurrentPool::runTask([func, value]() mutable { func(value); });
    }
}

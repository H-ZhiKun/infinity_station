#include "ConfigOperations.h"
#include <QFile>
#include <QTextStream>
#include <fstream>
#include "kits/common/log/CRossLogger.h"

using namespace _Kits;

void ConfigOperations::copyFile(const QString &sourcePath, const QString &destPath)
{
    QFile::remove(destPath); // 先删除目标文件，避免覆盖失败
    QFile::copy(sourcePath, destPath);
}

void ConfigOperations::deleteFile(const QString &filePath)
{
    QFile::remove(filePath);
}

QVariantList ConfigOperations::loadConfig(const QString &filePath)
{
    QFile file(filePath);
    QVariantList ret;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        LogWarn("无法打开配置文件");
        return ret;
    }

    QTextStream in(&file);
    QString fileContent = in.readAll();
    file.close();

    try
    {
        YAML::Node node = YAML::Load(fileContent.toStdString());
        ret = convertYamlToModel(node);
    }
    catch (const YAML::Exception &e)
    {
        LogWarn("YAML解析失败: {}", e.what());
    }
    return ret;
}

void ConfigOperations::OnSaveConfig(const QString &filePath, const QVariantList &selectedKeys)
{
    emit configSaved(filePath);
}

static QVariant yamlNodeToVariant(const YAML::Node &node)
{
    if (node.IsScalar())
    {
        return QString::fromStdString(node.as<std::string>());
    }
    else if (node.IsSequence())
    {
        QVariantList list;
        for (const auto &item : node)
        {
            list << yamlNodeToVariant(item);
        }
        return list;
    }
    else if (node.IsMap())
    {
        QVariantMap map;
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            QString key = QString::fromStdString(it->first.as<std::string>());
            map[key] = yamlNodeToVariant(it->second);
        }
        return map;
    }
    return QVariant();
}

QVariantList ConfigOperations::convertYamlToModel(const YAML::Node &node)
{
    QVariantList model;
    if (node.IsSequence())
    {
        // 根节点是数组
        for (const auto &item : node)
        {
            model << yamlNodeToVariant(item);
        }
    }
    else if (node.IsMap())
    {
        // 根节点是对象，转为单元素list，便于QML统一处理
        model << yamlNodeToVariant(node);
    }
    // 其它情况返回空
    return model;
}

// QVariant -> YAML::Node 递归转换
static YAML::Node variantToYamlNode(const QVariant &var)
{
    if (var.type() == QVariant::Map)
    {
        YAML::Node node(YAML::NodeType::Map);
        QVariantMap map = var.toMap();
        for (auto it = map.begin(); it != map.end(); ++it)
        {
            node[it.key().toStdString()] = variantToYamlNode(it.value());
        }
        return node;
    }
    else if (var.type() == QVariant::List)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        QVariantList list = var.toList();
        for (const QVariant &item : list)
        {
            node.push_back(variantToYamlNode(item));
        }
        return node;
    }
    else
    {
        // 标量，全部转字符串
        return YAML::Node(var.toString().toStdString());
    }
}

// 写入操作示例
bool ConfigOperations::saveDataToYaml(const QString &filePath, const QVariant &data)
{
    try
    {
        // 将 QVariant 转换为 YAML::Node
        YAML::Node node = variantToYamlNode(data);

        // 打开文件进行写入
        std::ofstream fout(filePath.toStdString());
        if (!fout.is_open())
        {
            // 如果文件打开失败，返回 false
            return false;
        }

        // 将 YAML::Node 写入文件
        fout << node;
        fout.close();

        // 如果一切顺利，返回 true
        return true;
    }
    catch (const YAML::Exception &e)
    {
        LogWarn("YAML error: {}", e.what());
        return false;
    }
    catch (const std::exception &e)
    {
        LogWarn("Standard error: {}", e.what());
        return false;
    }
}

#pragma once
#include <QObject>
#include <QString>
#include <QVariantList>
#include <yaml-cpp/yaml.h>


namespace _Kits
{
class ConfigOperations : public QObject
{
    Q_OBJECT

    public:
     ConfigOperations() = default;
     ~ConfigOperations() = default;

     void copyFile(const QString &sourcePath, const QString &destPath);
     void deleteFile(const QString &filePath);
     QVariantList loadConfig(const QString &filePath);
     bool saveDataToYaml(const QString &filePath, const QVariant &data);



     signals:
     void configLoaded(const QVariantList &config);
     void configSaved(const QString &filePath);

     private slots:
     void OnSaveConfig(const QString &filePath, const QVariantList &selectedKeys);

     private:
     QVariantList convertYamlToModel(const YAML::Node &node);


};
}
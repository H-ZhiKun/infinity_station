#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/yaml.h>
#include <filesystem>

namespace _Kits
{
    class SystemConfig
    {
      public:
        ~SystemConfig() noexcept = default;
        SystemConfig(const SystemConfig &) = delete;
        SystemConfig &operator=(const SystemConfig &) = delete;

        static SystemConfig &instance()
        {
            static SystemConfig ins;
            return ins;
        }

        const std::filesystem::path &rootPath();
        const std::string &configVersionPath();
        const std::string &version();
        const YAML::Node &mainNode();

        bool load();
        YAML::Node loadYamlFile(const std::string &path);

      private:
        SystemConfig() = default;

        bool loadVersion();
        bool loadMainNode();

        std::string m_version;
        std::string m_configVersionPath;
        std::filesystem::path m_rootPath;
        YAML::Node m_mainNode;
    };

    inline const std::filesystem::path &SystemConfig::rootPath()
    {
        return m_rootPath;
    }

    inline const std::string &SystemConfig::configVersionPath()
    {
        return m_configVersionPath;
    }

    inline const std::string &SystemConfig::version()
    {
        return m_version;
    }

    inline const YAML::Node &SystemConfig::mainNode()
    {
        return m_mainNode;
    }

    inline bool SystemConfig::load()
    {
        m_rootPath = std::filesystem::current_path();
        if (!loadVersion())
        {
            return false;
        }
        if (!loadMainNode())
        {
            return false;
        }
        m_configVersionPath = m_rootPath.string() + "/config/" + m_version + "/";
        return true;
    }

    inline YAML::Node SystemConfig::loadYamlFile(const std::string &path)
    {
        YAML::Node node;
        try
        {
            node = YAML::LoadFile(path);
        }
        catch (const YAML::Exception &e)
        {
            std::cout << "Error: Loaded YAML is null.";
        }
        return node;
    }

    inline bool SystemConfig::loadVersion()
    {
        auto filePath = m_rootPath / std::filesystem::path(".version");
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            std::cout << "open file error: " << filePath << std::endl;
            return false;
        }
        std::string line;
        while (std::getline(file, line))
        {
            size_t posVersion = line.find("CURRENT_VERSION");
            size_t posSign = line.find("=");
            if (posVersion != std::string::npos && posSign != std::string::npos)
            {
                m_version = line.substr(posSign + 1);
                m_version.erase(std::remove_if(m_version.begin(), m_version.end(), ::isspace), m_version.end());
                std::transform(m_version.begin(), m_version.end(), m_version.begin(), [](unsigned char c) { return std::tolower(c); });
            }
        }
        return true;
    }

    inline bool SystemConfig::loadMainNode()
    {
        std::string yamlPath = m_rootPath.string() + "/config/config.yaml";
        m_mainNode = loadYamlFile(yamlPath);
        if (m_mainNode.IsNull())
        {
            return false;
        }
        return true;
    }
} // namespace _Kits
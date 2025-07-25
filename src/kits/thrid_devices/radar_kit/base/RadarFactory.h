#ifndef _MODULES_RADAR_FACTORY_H_
#define _MODULES_RADAR_FACTORY_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include "DeviceDeal.h"

namespace _Modules {

class RadarFactory {
public:
    static RadarFactory& getInstance() {
        static RadarFactory instance;
        return instance;
    }

    template<typename T>
    void registerRadar(const std::string& type) {
        creators_[type] = []() { return std::make_shared<T>(); };
    }

    std::shared_ptr<_Modules::DeviceDeal> createRadar(const std::string& type) {
        auto it = creators_.find(type);
        if (it != creators_.end()) {
            return it->second();
        }
        return nullptr;
    }

private:
    RadarFactory() = default;
    ~RadarFactory() = default;
    RadarFactory(const RadarFactory&) = delete;
    RadarFactory& operator=(const RadarFactory&) = delete;

    std::unordered_map<std::string, std::function<std::shared_ptr<_Modules::DeviceDeal>()>> creators_;
};

} // namespace _Modules

#endif // _MODULES_RADAR_FACTORY_H_
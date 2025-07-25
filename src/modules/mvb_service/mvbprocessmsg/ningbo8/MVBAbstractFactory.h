#pragma once

// #include "MVBNBMsgHandle.h"
#include "MVBNBDataGenerator.h"
#include "MVBNBParseMsg.h"
#include <memory>

//宁波8项目 创建协议解析和组装对象
class MVBAbstractFactory final
{
private:
    /* data */
public:
    MVBAbstractFactory() = default;
    ~MVBAbstractFactory() = default;
    std::unique_ptr<MVBDataGenerator> createDataGenerator()
    {
        return std::make_unique<MVBNBDataGenerator>();
    }

    std::shared_ptr<MVBParseMsg> createParseMsg()
    {
        return std::make_shared<MVBNBParseMsg>();
    }
};


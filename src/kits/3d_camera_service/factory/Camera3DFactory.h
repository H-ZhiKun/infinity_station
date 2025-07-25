#pragma once
#include "Camera3DInterface.h"
#include <memory>

class  Camera3DFactory
{

public:
    Camera3DFactory(/* args */) = default;
    ~Camera3DFactory() = default;

    static std::shared_ptr<Camera3DInterface> createCamera3D(int type);

};


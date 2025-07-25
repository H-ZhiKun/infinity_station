#include "Camera3DFactory.h"

class camera ;
static std::shared_ptr<Camera3DInterface> Camera3DFactory::createCamera3D(int type)
{
    switch (type)
    {
    case 1: {
        return std::make_shared<camera>();
    }
    break;

    default:
        break;
        return nullptr;
    }
    return nullptr;
}
#ifndef _NO_CHANGE_CENTER_HPP_
#define _NO_CHANGE_CENTER_HPP_

#include "ChangeCenterBase.h"
#include <qvectornd.h>

namespace _Modules{

    class NoChangeCenter : public _Modules::ChangeCenterBase {

    public:
        NoChangeCenter() = default;
        
        virtual ~NoChangeCenter() = default;

        virtual void ChangeCenterPoint(std::vector<QVector2D> &device_singledata, const ChangeCenterData &change_center_data) override final{
            // do nothing
        }

    };


}

#endif
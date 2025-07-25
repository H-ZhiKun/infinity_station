#pragma once
#include <QObject>
#include <QImage>

namespace _Kits {


class AlgorithmGuiyang :public QObject
{  
    Q_OBJECT
    public:
        explicit AlgorithmGuiyang(QObject *parent = nullptr);
        virtual ~AlgorithmGuiyang() noexcept;
        int init();
        void inferenceAlgorithmModel(QImage &input, std::unordered_map<int,QRect>& rects);
        int moduleFree();
        void parseJsonResult(std::string &jsonResult,std::unordered_map<int,QRect>& res);

};

}

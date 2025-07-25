
#pragma once
#include "encodevideo.h"
#include "tis_global/Struct.h"
#include <QImage>
#include <QObject>
#include <QVariant>

namespace _Kits
{
    namespace SaveVideoNew
    {

        class SaveVideoBase : public QObject
        {

            Q_OBJECT

          public:
            SaveVideoBase(QObject *parent = nullptr)
            {
            }
            virtual ~SaveVideoBase()
            {
            }
            virtual int initSaveVideo(const int &threadNum) = 0;

          public slots:

            /// @brief 使用原始图片数据
            /// @param img 原始图片数据结构体
            virtual void pushOriginalImage(std::shared_ptr<TIS_Info::ImageBuffer> img)
            {
                return;
            }

            virtual int startRecord(QString &filepath)
            {
                return 0;
            };
            virtual void stopRecord()
            {
                return;
            }
        };
    } // namespace SaveVideoNew
} // namespace _Kits
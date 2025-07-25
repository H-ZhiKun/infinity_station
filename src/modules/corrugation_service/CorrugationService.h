#pragma once
// #include "./mvblib/src/MvbBaseInterface.h"
#include "kits/common/module_base/ModuleBase.h"
#include <QTimer>
#include <json/json.h>
#include <memory>
#include <qtmetamacros.h>

namespace _Modules
{
    class CorrugationService : public ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(CorrugationService)
      public:
        explicit CorrugationService(QObject *parent = nullptr);
        virtual ~CorrugationService() noexcept;
        CorrugationService(const CorrugationService &) = delete;
        CorrugationService &operator=(const CorrugationService &) = delete;
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;
        void setupData(int n);

      private:
        /**********************
         * @brief 接收波磨数据
         * @param CorrugationData  数据
         * @return
         ************************ */
        void recvCorrgationData(const CorrugationData &corrugationData);
      signals:
        void updateData(const QVariant &);

      private slots:
        /**********************
         * @brief 发送波磨数据
         * @param msg  数据
         * @return true 表示成功，false表示失败
         ************************ */
        bool sendData(const QVariant &);

      private:
        std::shared_ptr<CorrugationBaseInterface> m_CorrugationBaseptr = nullptr;
    };
} // namespace _Modules

#pragma once

#include <atomic>

class TriggerSaveDB
{

  public:
    TriggerSaveDB(double dis_cm);

    virtual ~TriggerSaveDB() noexcept;

    void setcurDis(double dis_cm);
    // 零米
    bool isTrigger();

  private:
    std::atomic<double> m_curDis = 0.0;       // cm
    std::atomic<double> m_preDis = 0.0;       // cm
    std::atomic<double> m_intervalDis = 25.0; // cm
};
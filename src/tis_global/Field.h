#pragma once
namespace TIS_Info
{
    namespace QmlCommunication
    {
        namespace Radar
        {
            constexpr char strData[] = "data";
            constexpr char stateSendQml[] = "stateSendQml";
            constexpr char dataSendQml[] = "dataSendQml";
            constexpr char overInfoSendQml[] = "overInfoSendQml";
            constexpr char overConfigSendQml[] = "overConfigSendQml";
        } // namespace Radar
    } // namespace QmlCommunication

    namespace HttpService
    {
        namespace HttpRoutes
        {
            constexpr char database_select[] = "/database/select";
            constexpr char database_insert[] = "/database/insert";
            constexpr char api_communication[] = "/api/communication";
        } // namespace HttpRoutes
    } // namespace HttpService
    namespace EventThread
    {
        constexpr char eventloop_main[] = "eventloop_main";
        constexpr char eventloop_invoke[] = "eventloop_invoke";
        constexpr char eventloop_module[] = "eventloop_module";
        constexpr char eventloop_thread_pool[] = "eventloop_thread_pool";
    } // namespace EventThread

} // namespace TIS_Info
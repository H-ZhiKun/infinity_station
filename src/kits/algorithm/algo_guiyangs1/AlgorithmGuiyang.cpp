#include "AlgorithmGuiyang.h"
#include "guiyangs1_temp_algo/TogeeAlgorithmLib.h"
#include "kits/common/log/CRossLogger.h"
#include "json/json.h"
namespace _Kits {

 AlgorithmGuiyang::AlgorithmGuiyang(QObject *parent)
 {

 }
AlgorithmGuiyang::~AlgorithmGuiyang()
{

}

int AlgorithmGuiyang::init()
{

    try
    {
        ProjectCode id;
        id.m_projectID = JC23_03GuiyangS1;
        id.m_businessID = InfraredDetection;
        FeaturesCamerasInfo info;
        AlgoErrorCode ret = DetectionRecognitionNNModel_Init(id,&info);
        if(ret!=0)
        {
            LogError("DetectionRecognitionNNModel_Init error");
            return -1;
        }
        else
        {
            LogInfo("DetectionRecognitionNNModel_Init success");
            return 0;
        }   
    }
    catch(const std::exception& e)
    {
        LogError("DetectionRecognitionNNModel_Init std::exception: {}",e.what());
         return -1;
    }
   
}

void AlgorithmGuiyang::inferenceAlgorithmModel(QImage &image, std::unordered_map<int,QRect>& rects)
{
    if(image.isNull())
    {
        LogError("TemperatureController::initAlgorithmModel image is null");
        return;
    }
    //image.save("test11.jpg");
    InputImageInfo imgInfo;
    imgInfo.m_width = image.width();
    imgInfo.m_height = image.height();
    imgInfo.m_channel = 3;
    imgInfo.m_buffer = image.bits();
    imgInfo.m_cameraID = 0;
    std::vector<char> imgData(1024 * 1024 * 3); // 3MB buffer
    imgData[0]='\0';
    DetectionRecognitionNNModel_Process(imgInfo, imgData.data());
    std::string resultJson;
    for (size_t i = 0; i < imgData.size(); ++i) 
    {
        if (imgData[i] == '\0')
        {
            break;
        } 
        resultJson += imgData[i];
    }
    LogInfo("模型结果:{}",resultJson);
    parseJsonResult(resultJson,rects);

}
void AlgorithmGuiyang::parseJsonResult(std::string &jsonResult,std::unordered_map<int,QRect>& res)
{
    Json::Value value;
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    std::string errors;
    //value = stringToJson(jsonResult);
    if (reader->parse(jsonResult.c_str(), jsonResult.c_str() + strlen(jsonResult.c_str()), &value, &errors)) 
    {
        LogInfo("解析成功 {}", jsonResult);
    }
    else 
    {
        LogInfo("解析失败: {}", errors);
        return;
    }
    if (value["resultNum"] > 0) 
    {
        int m_resultNum = value["resultNum"].asInt();
        for (int i = 0; i < m_resultNum; ++i)
        {
            // 提取坐标并检查类型
            int x1 = value["result"][i]["x1"].asInt();
            int x2 = value["result"][i]["x2"].asInt();
            int y1 = value["result"][i]["y1"].asInt();
            int y2 = value["result"][i]["y2"].asInt();
            int defectItemID = value["result"][i]["defectItemID"].asInt();
            int componentNameID = value["result"][i]["componentNameID"].asInt();
            float confidence = value["result"][i]["confidence"].asFloat();
            //只需要坐标
            res[i] = QRect(x1, y1, x2 - x1, y2 - y1);
            LogInfo("坐标: x1={}, y1={}, x2={}, y2={}, defectItemID={}, componentNameID={}, confidence={}", 
                    x1, y1, x2, y2, defectItemID, componentNameID, confidence);

        }

        
    }


}
int AlgorithmGuiyang::moduleFree()
{
    int ret = DetectionRecognitionNNModel_Free();
    LogInfo("DetectionRecognitionNNModel_Free ret: {}", std::to_string(ret));
    return 0;
}


}
#pragma once

#include <string>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32
#ifdef JC23_05_ningBo_5CPantographDetection_EXPORTS
#define PantographDetection_APIDLL __declspec(dllexport) // Windows 上导出符号
#else
#define PantographDetection_APIDLL __declspec(dllimport) // Windows 上导入符号
#endif
#elif defined(__linux__)
#define PantographDetection_APIDLL // Linux 上不需要额外的修饰符
#else
#error "Unsupported platform"
#endif
#define MODEL_MAX_NUMBER 50
#define RESULT_MAX_NUMBER 100

    typedef struct
    {
        char *m_model_paths[MODEL_MAX_NUMBER];
        int m_model_width;
        int m_model_height;
        int m_model_channel;
    } pantographDetectionModel;

    typedef struct
    {
        int m_img_width;
        int m_img_height;
        int m_img_channel;
        unsigned char *m_img_buffer;
    } pantographDetectionInput;

    typedef enum
    {
        Pantograph_claw_missing = 0,
        Pantograph_foreign_body
    } PantographDefectName;

    typedef struct
    {
        float x, y, w, h;
        float confidence;
        PantographDefectName label_ID;
    } objectBox;

    typedef struct
    {
        objectBox m_results[RESULT_MAX_NUMBER];
        int m_result_num;
    } detectResults;

    PantographDetection_APIDLL int PantographDetection_Init(const pantographDetectionModel input);
    PantographDetection_APIDLL void PantographDetection_Free(void);
    PantographDetection_APIDLL int PantographDetection_Process(const pantographDetectionInput input, detectResults *output);

#ifdef __cplusplus
}
#endif

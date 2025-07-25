#pragma once
#include <fstream>
#include <chrono>
#include "opencv2/opencv.hpp"

#ifdef _WIN32
	#ifdef CorePipleline_EXPORTS
	#define APIDLL __declspec(dllexport)
	#else
	#define APIDLL __declspec(dllimport)
	#endif
#elif defined(__linux__)
	#define APIDLL  // Linux 上不需要额外的修饰符
#else
	#error "Unsupported platform"
#endif
/*******************************************************************************************
Struct Name��struct BoxInfo
Description: box��ز���
Author: zgq 2025.1.7
*******************************************************************************************/
typedef struct BoxInfo
{
	float x1;
	float y1;
	float x2;
	float y2;
	float score;
	int label;
} BoxInfo;


typedef struct Object
{
	cv::Rect_<float> rect;
	int label = 0;
	float prob = 0.0;
}Object;


class APIDLL ModelInferenceFrameWork_TensorRT
{
public:
	cv::Mat m_tmp;  // receive image from external, for preprocess	
	std::vector<void*> m_host_ptrs;  // buffer of tensor_output 
	int engineID;

public:
	ModelInferenceFrameWork_TensorRT(const std::string& engineFile);
	 ~ModelInferenceFrameWork_TensorRT();
	virtual void preprocess(const cv::Mat& image) ;
	void inference_execute();
	virtual void postprocess() ;
};
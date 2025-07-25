#pragma once

#include "modelinferenceframework_tensorrt.h"

// #ifdef JC23_05_ningBo_5CPantographDetection_EXPORTS
// #define APIDLL __declspec(dllexport)
// #else
// #define APIDLL __declspec(dllimport)
// #endif

#include "TogeeAlgorithmLib.h"

class ModelInferenceFrameWork_TensorRT_Instantiate :public ModelInferenceFrameWork_TensorRT
{
	struct PreParam
	{
		float ratio = 1.0f;
		float dw = 0.0f;
		float dh = 0.0f;
		float height = 0;
		float width = 0;
	};
protected:
	static ModelInferenceFrameWork_TensorRT_Instantiate* instance_;	
	//ModelInferenceFrameWork_TensorRT_Instantiate(const std::string& model_path) = delete;
	
public:

	using ModelInferenceFrameWork_TensorRT::ModelInferenceFrameWork_TensorRT;
    /**
     * Singletons should not be cloneable.
     */
   // ModelInferenceFrameWork_TensorRT_Instantiate(ModelInferenceFrameWork_TensorRT_Instantiate &other) = delete;
    /**
     * Singletons should not be assignable.
     */
    void operator=(const ModelInferenceFrameWork_TensorRT_Instantiate &) = delete;

	static ModelInferenceFrameWork_TensorRT_Instantiate *GetInstance(const pantographDetectionModel& input);


	~ModelInferenceFrameWork_TensorRT_Instantiate(void) {};
	void preprocess(const cv::Mat& image)  ;
	void postprocess(std::vector<BoxInfo>& generate_boxes);
	void postprocess_yolo(std::vector<Object>& generate_boxes);

public:
	//model parameters
	int InpWidth = 320;
	int InpHeight = 320;
	int Num_class = 80;
	int Reg_max = 7;
	const int Num_stages = 4;
	const int Stride[4] = { 8,16,32,64 };

	//const float mean[3] = { 103.53, 116.28, 123.675 };
	//const float stds[3] = { 57.375, 57.12, 58.395 };
	float score_threshold = 0.4;
	float nms_threshold = 0.6;

	//Ԥ���������ò���
	PreParam pparam;
};
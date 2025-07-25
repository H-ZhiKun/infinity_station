#pragma once
#include <string>
#include <vector>

#ifdef _WIN32
#define HOGSVM_API __declspec(dllexport)
#else
#define HOGSVM_API
#endif

//#define RESULT_MAX_LENGTH_BYTE 256

class HOGSVM_API HOGSVMClassifier {
public:
	HOGSVMClassifier();
	~HOGSVMClassifier();

	// 加载模型和参数
	bool load(const std::string& modelFile,
		const std::string& labelMapFile,
		const std::string& hogParamsFile);

	// 预测一张图像的分类
	bool predict(const unsigned char* buffer,
		int bufferSize,
		std::string& outJson) const;

private:
	class Impl;
	Impl* impl_;  // Pimpl 隐藏类型细节
};

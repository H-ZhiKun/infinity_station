#include "TestCudaController.h"
#include <opencv2/opencv.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>
#include "kits/common/cuda/YuvConverterImpl.h"
using namespace _Controllers;
using namespace _Kits;



TestCudaController::TestCudaController()
{
    
    cv::Mat bgrMat = cv::imread("test.jpg");
    int height_ = bgrMat.rows;
    int width_ = bgrMat.cols;
    // YuvConverterImpl yuvConverter;
    // std::vector<uint8_t> yv12Data(width_ * height_ * 3 / 2);
    // yuvConverter.BGRToYV12(bgrMat.data, yv12Data.data());

    // std::vector<uint8_t> bgrData(width_ * height_ * 3);
    // yuvConverter.YV12ToBGR(yv12Data.data(), bgrData.data());

    
    // cv::Mat reconstructedMat(height_, width_, CV_8UC3, bgrData.data());

    // cv::imwrite("./reconstructed.jpg", reconstructedMat);


}
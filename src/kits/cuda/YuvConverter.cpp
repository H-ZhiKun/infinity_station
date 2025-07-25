#include "YuvConverter.h"
#include "YuvConverterImpl.cuh" // 包含所有 CUDA 实现
#include <string>

using namespace _Kits;

// ---------- 构造 / 析构 ----------
YuvConverter::YuvConverter() = default;
YuvConverter::~YuvConverter() noexcept
{
    release();
}

// ---------- 资源管理 ----------
void YuvConverter::init(int width, int height)
{
    if (width <= 0 || height <= 0)
        throw std::invalid_argument("invalid width/height");

    release(); // 先释放旧资源
    width_ = width;
    height_ = height;

    // 注意：这里把 **指针变量本身的地址** 传给 initCudaMemory，
    // 否则函数内部只是修改了形参，外层仍为 nullptr
    if (int err = initCudaMemory(&d_yv12_, &d_bgr_, &d_rgb_, &d_temp_, width_, height_) != 0)
        throw std::runtime_error("CUDA memory allocation failed, code=" + std::to_string(err));
}

void YuvConverter::release() noexcept
{
    freeCudaMemory(d_yv12_, d_bgr_, d_rgb_, d_temp_);
    d_yv12_ = d_bgr_ = d_rgb_ = nullptr;
    d_temp_ = nullptr;
    width_ = height_ = 0;
}

// ---------- 转换接口 ----------
void YuvConverter::yv12ToBgr_cuda(const uint8_t *yv12, uint8_t *bgr)
{
    if (!d_yv12_ || !d_bgr_)
        throw std::runtime_error("not initialized");
    YV12ToBGR(yv12, bgr, d_yv12_, d_bgr_, width_, height_);
}

void YuvConverter::bgrToYv12_cuda(const uint8_t *bgr, uint8_t *yv12)
{
    if (!d_yv12_ || !d_bgr_)
        throw std::runtime_error("not initialized");
    BGRToYV12(bgr, yv12, d_bgr_, d_yv12_, width_, height_);
}

void YuvConverter::yv12ToRgb_cuda(const uint8_t *yv12, uint8_t *rgb)
{
    if (!d_yv12_ || !d_rgb_)
        throw std::runtime_error("not initialized");
    YV12ToRGB(yv12, rgb, d_yv12_, d_rgb_, width_, height_);
}

void YuvConverter::rgbToYv12_cuda(const uint8_t *rgb, uint8_t *yv12)
{
    if (!d_yv12_ || !d_rgb_)
        throw std::runtime_error("not initialized");
    RGBToYV12(rgb, yv12, d_rgb_, d_yv12_, width_, height_);
}
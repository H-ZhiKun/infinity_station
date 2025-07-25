#include "YuvConverterImpl.cuh"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <thrust/device_vector.h>
#include <thrust/transform.h>
#include <thrust/execution_policy.h>

__constant__ float yuv2rgb[9] = {1.164f, 0.000f, 1.596f, 1.164f, -0.392f, -0.813f, 1.164f, 2.017f, 0.000f};

__constant__ float rgb2yuv[9] = {0.257f, 0.504f, 0.098f, -0.148f, -0.291f, 0.439f, 0.439f, -0.368f, -0.071f};

// YUV转BGR的CUDA核函数
__global__ void YV12ToBGR_Kernel(const uint8_t *y_data, const uint8_t *u, const uint8_t *v, uint8_t *bgr, int width, int height)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y_idx = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= width || y_idx >= height)
        return;

    // 计算UV分量索引 (YV12格式UV分量是下采样2x2的)
    int uv_x = x / 2;
    int uv_y = y_idx / 2;
    int uv_index = uv_y * (width / 2) + uv_x;

    // 读取YUV分量并归一化
    float Y = y_data[y_idx * width + x] - 16.0f; // 使用修改后的参数名
    float U = u[uv_index] - 128.0f;
    float V = v[uv_index] - 128.0f;

    // 矩阵乘法计算RGB
    float r = Y * yuv2rgb[0] + U * yuv2rgb[1] + V * yuv2rgb[2];
    float g = Y * yuv2rgb[3] + U * yuv2rgb[4] + V * yuv2rgb[5];
    float b = Y * yuv2rgb[6] + U * yuv2rgb[7] + V * yuv2rgb[8];

    // 裁剪并转换回uint8
    int index = (y_idx * width + x) * 3; // 使用修改后的变量名
    bgr[index] = static_cast<uint8_t>(fminf(fmaxf(b, 0.0f), 255.0f));
    bgr[index + 1] = static_cast<uint8_t>(fminf(fmaxf(g, 0.0f), 255.0f));
    bgr[index + 2] = static_cast<uint8_t>(fminf(fmaxf(r, 0.0f), 255.0f));
}

// BGR转YUV的CUDA核函数
__global__ void BGRToYV12_Kernel(const uint8_t *bgr, uint8_t *y, uint8_t *u, uint8_t *v, int width, int height)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y_idx = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= width || y_idx >= height)
        return;

    int index = (y_idx * width + x) * 3;
    float B = bgr[index];
    float G = bgr[index + 1];
    float R = bgr[index + 2];

    // 计算YUV分量
    float Y = R * rgb2yuv[0] + G * rgb2yuv[1] + B * rgb2yuv[2] + 16.0f;
    float U = R * rgb2yuv[3] + G * rgb2yuv[4] + B * rgb2yuv[5] + 128.0f;
    float V = R * rgb2yuv[6] + G * rgb2yuv[7] + B * rgb2yuv[8] + 128.0f;

    // 写入Y分量
    y[y_idx * width + x] = static_cast<uint8_t>(fminf(fmaxf(Y, 0.0f), 255.0f));

    // 只处理偶数位置像素来计算UV分量 (2x2下采样)
    if (x % 2 == 0 && y_idx % 2 == 0)
    {
        int uv_index = (y_idx / 2) * (width / 2) + (x / 2);
        u[uv_index] = static_cast<uint8_t>(fminf(fmaxf(U, 0.0f), 255.0f));
        v[uv_index] = static_cast<uint8_t>(fminf(fmaxf(V, 0.0f), 255.0f));
    }
}

// YuvConverterImpl::YuvConverterImpl(int width, int height)
//     : width_(width), height_(height)
// {
//     // 分配设备内存
//     cudaMalloc(&d_yv12_, width * height * 3 / 2);  // YV12大小: w*h + w*h/4 + w*h/4
//     cudaMalloc(&d_bgr_, width * height * 3);       // BGR大小: w*h*3
//     cudaMalloc(&d_temp_, width * height * sizeof(float) * 3); // 临时内存
// }
// YuvConverterImpl::~YuvConverterImpl() {
//     cudaFree(d_yv12_);
//     cudaFree(d_bgr_);
//     cudaFree(d_temp_);
// }
int initCudaMemory(uint8_t **d_yv12, uint8_t **d_bgr, uint8_t **d_rgb, float **d_temp, int w, int h)
{
    cudaMalloc((void **)d_yv12, w * h * 3 / 2);
    cudaMalloc((void **)d_bgr, w * h * 3);
    cudaMalloc((void **)d_rgb, w * h * 3);
    cudaMalloc((void **)d_temp, w * h * sizeof(float) * 3);
    return cudaGetLastError(); // 返回错误码
}

void freeCudaMemory(uint8_t *&d_yv12, uint8_t *&d_bgr, uint8_t *&d_rgb, float *&d_temp)
{
    if (d_yv12)
    {
        cudaFree(d_yv12);
        d_yv12 = nullptr;
    }
    if (d_bgr)
    {
        cudaFree(d_bgr);
        d_bgr = nullptr;
    }
    if (d_rgb)
    {
        cudaFree(d_rgb);
        d_rgb = nullptr;
    }
    if (d_temp)
    {
        cudaFree(d_temp);
        d_temp = nullptr;
    }
}

int YV12ToBGR(const uint8_t *yv12Data, uint8_t *bgrData, uint8_t *d_yv12_, uint8_t *d_bgr_, int width_, int height_)
{
    // 将YV12数据拷贝到设备
    cudaMemcpy(d_yv12_, yv12Data, width_ * height_ * 3 / 2, cudaMemcpyHostToDevice);

    // 设置线程块和网格大小
    dim3 block(16, 16);
    dim3 grid((width_ + block.x - 1) / block.x, (height_ + block.y - 1) / block.y);

    // 获取YUV分量指针
    uint8_t *d_y = d_yv12_;
    uint8_t *d_u = d_y + width_ * height_;
    uint8_t *d_v = d_u + (width_ * height_) / 4;

    // 调用核函数
    YV12ToBGR_Kernel<<<grid, block>>>(d_y, d_u, d_v, d_bgr_, width_, height_);

    // 将结果拷贝回主机
    cudaMemcpy(bgrData, d_bgr_, width_ * height_ * 3, cudaMemcpyDeviceToHost);
    return 0; // 返回成功
}

// BGR转YV12实现
int BGRToYV12(const uint8_t *bgrData, uint8_t *yv12Data, uint8_t *d_bgr_, uint8_t *d_yv12_, int width_, int height_)
{
    // 将BGR数据拷贝到设备
    cudaMemcpy(d_bgr_, bgrData, width_ * height_ * 3, cudaMemcpyHostToDevice);

    // 设置线程块和网格大小
    dim3 block(16, 16);
    dim3 grid((width_ + block.x - 1) / block.x, (height_ + block.y - 1) / block.y);

    // 获取YUV分量指针
    uint8_t *d_y = d_yv12_;
    uint8_t *d_u = d_y + width_ * height_;
    uint8_t *d_v = d_u + (width_ * height_) / 4;

    // 调用核函数
    BGRToYV12_Kernel<<<grid, block>>>(d_bgr_, d_y, d_u, d_v, width_, height_);

    // 将结果拷贝回主机
    cudaMemcpy(yv12Data, d_yv12_, width_ * height_ * 3 / 2, cudaMemcpyDeviceToHost);
    return 0;
}

// ============== YV12 -> RGB =================
__global__ void YV12ToRGB_Kernel(const uint8_t *y_data,
                                 const uint8_t *u,
                                 const uint8_t *v,
                                 uint8_t *rgb, // 输出：R→G→B
                                 int width,
                                 int height)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= width || y >= height)
        return;

    int uv_x = x >> 1;
    int uv_y = y >> 1;
    int uv_idx = uv_y * (width >> 1) + uv_x;

    float Y = y_data[y * width + x] - 16.0f;
    float U = u[uv_idx] - 128.0f;
    float V = v[uv_idx] - 128.0f;

    float r = Y * yuv2rgb[0] + U * yuv2rgb[1] + V * yuv2rgb[2];
    float g = Y * yuv2rgb[3] + U * yuv2rgb[4] + V * yuv2rgb[5];
    float b = Y * yuv2rgb[6] + U * yuv2rgb[7] + V * yuv2rgb[8];

    int idx = (y * width + x) * 3;
    rgb[idx] = static_cast<uint8_t>(fminf(fmaxf(r, 0.0f), 255.0f));     // R
    rgb[idx + 1] = static_cast<uint8_t>(fminf(fmaxf(g, 0.0f), 255.0f)); // G
    rgb[idx + 2] = static_cast<uint8_t>(fminf(fmaxf(b, 0.0f), 255.0f)); // B
}

int YV12ToRGB(const uint8_t *yv12Data, uint8_t *rgbData, uint8_t *d_yv12_, uint8_t *d_rgb_, int width, int height)
{
    cudaMemcpy(d_yv12_, yv12Data, width * height * 3 / 2, cudaMemcpyHostToDevice);

    dim3 block(16, 16);
    dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);

    uint8_t *d_y = d_yv12_;
    uint8_t *d_u = d_y + width * height;
    uint8_t *d_v = d_u + (width * height) / 4;

    YV12ToRGB_Kernel<<<grid, block>>>(d_y, d_u, d_v, d_rgb_, width, height);

    cudaMemcpy(rgbData, d_rgb_, width * height * 3, cudaMemcpyDeviceToHost);
    return 0;
}

// ============== RGB -> YV12 =================
__global__ void RGBToYV12_Kernel(const uint8_t *rgb, // 输入：R→G→B
                                 uint8_t *y,
                                 uint8_t *u,
                                 uint8_t *v,
                                 int width,
                                 int height)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y_idx = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= width || y_idx >= height)
        return;

    int idx = (y_idx * width + x) * 3;
    float R = rgb[idx];
    float G = rgb[idx + 1];
    float B = rgb[idx + 2];

    float Yf = R * rgb2yuv[0] + G * rgb2yuv[1] + B * rgb2yuv[2] + 16.0f;
    float Uf = R * rgb2yuv[3] + G * rgb2yuv[4] + B * rgb2yuv[5] + 128.0f;
    float Vf = R * rgb2yuv[6] + G * rgb2yuv[7] + B * rgb2yuv[8] + 128.0f;

    y[y_idx * width + x] = static_cast<uint8_t>(fminf(fmaxf(Yf, 0.0f), 255.0f));

    if ((x & 1) == 0 && (y_idx & 1) == 0)
    {
        int uv_idx = (y_idx >> 1) * (width >> 1) + (x >> 1);
        u[uv_idx] = static_cast<uint8_t>(fminf(fmaxf(Uf, 0.0f), 255.0f));
        v[uv_idx] = static_cast<uint8_t>(fminf(fmaxf(Vf, 0.0f), 255.0f));
    }
}

int RGBToYV12(const uint8_t *rgbData, uint8_t *yv12Data, uint8_t *d_rgb_, uint8_t *d_yv12_, int width, int height)
{
    cudaMemcpy(d_rgb_, rgbData, width * height * 3, cudaMemcpyHostToDevice);

    dim3 block(16, 16);
    dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);

    uint8_t *d_y = d_yv12_;
    uint8_t *d_u = d_y + width * height;
    uint8_t *d_v = d_u + (width * height) / 4;

    RGBToYV12_Kernel<<<grid, block>>>(d_rgb_, d_y, d_u, d_v, width, height);

    cudaMemcpy(yv12Data, d_yv12_, width * height * 3 / 2, cudaMemcpyDeviceToHost);
    return 0;
}
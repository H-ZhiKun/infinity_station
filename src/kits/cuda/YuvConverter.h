#pragma once
#include <cstdint>
#include <stdexcept>

#ifdef _WIN32
#ifdef CUDAYV12RGB_EXPORTS
#define CUDAYV12RGB_API __declspec(dllexport)
#else
#define CUDAYV12RGB_API __declspec(dllimport)
#endif
#else
#define CUDAYV12RGB_API __attribute__((visibility("default")))
#endif

namespace _Kits
{
    class CUDAYV12RGB_API YuvConverter
    {
      public:
        YuvConverter();
        ~YuvConverter() noexcept;

        void init(int width, int height); // 分配显存
        void release() noexcept;          // 手动释放

        // YV12<->BGR
        void yv12ToBgr_cuda(const uint8_t *yv12, uint8_t *bgr);
        void bgrToYv12_cuda(const uint8_t *bgr, uint8_t *yv12);

        // YV12<->RGB (R-G-B 连续)
        void yv12ToRgb_cuda(const uint8_t *yv12, uint8_t *rgb);
        void rgbToYv12_cuda(const uint8_t *rgb, uint8_t *yv12);

        size_t getBgrSize() const
        {
            return width_ * height_ * 3;
        }
        size_t getRgbSize() const
        {
            return width_ * height_ * 3;
        }
        size_t getYv12Size() const
        {
            return width_ * height_ * 3 / 2;
        }

      private:
        int width_ = 0;
        int height_ = 0;

        uint8_t *d_yv12_ = nullptr;
        uint8_t *d_bgr_ = nullptr;
        uint8_t *d_rgb_ = nullptr; // 新增 RGB 显存
        float *d_temp_ = nullptr;
    };
} // namespace _Kits
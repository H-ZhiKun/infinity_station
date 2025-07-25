#include <cstdlib>
#include <iostream>
#include <stdint.h>

// 开辟内存空间
int initCudaMemory(uint8_t **d_yv12, uint8_t **d_bgr, uint8_t **d_rgb, float **d_temp, int w, int h);
void freeCudaMemory(uint8_t *&d_yv12, uint8_t *&d_bgr, uint8_t *&d_rgb, float *&d_temp);

// yv12Data yv12图像数据，bgrData转换后的bgr图像数据，d_yv12_ 设备端的yv12数据，d_bgr_设备的bgr数据， width_，height_ 传入的图像宽高
int YV12ToBGR(const uint8_t *yv12Data, uint8_t *bgrData, uint8_t *d_yv12_, uint8_t *d_bgr_, int width, int height);

int BGRToYV12(const uint8_t *bgrData, uint8_t *yv12Data, uint8_t *d_bgr_, uint8_t *d_yv12_, int width, int height);

int YV12ToRGB(const uint8_t *yv12Data, uint8_t *rgbData, uint8_t *d_yv12_, uint8_t *d_rgb_, int width, int height);

int RGBToYV12(const uint8_t *rgbData, uint8_t *yv12Data, uint8_t *d_rgb_, uint8_t *d_yv12_, int width, int height);
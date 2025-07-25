#pragma once
#include "kits/common/log/CRossLogger.h"
#include <bitset>
#include <iomanip>
#include <sstream>
#include <vector>

namespace _Kits
{
    class ByteArrayTools
    {
      public:
        /**
         * 通用字节写入（大端序）
         * @tparam T      数据类型（uint8_t, uint16_t, uint32_t）
         * @param data    目标 vector
         * @param value   要写入的值
         * @param offset  起始位置（默认 0）
         */
        template <typename T>
        static void writeToVectorBE(std::vector<uint8_t> &data, T value, size_t offset)
        {
            static_assert(std::is_unsigned_v<T>, "T must be unsigned integer (uint8_t, uint16_t, uint32_t)");
            constexpr size_t valueSize = sizeof(T);

            if (data.size() < offset + valueSize)
            {
                _Kits::LogError("size error: offset ={}, value size = {}, vector size = {}", offset, valueSize, data.size());
                return;
            }

            for (size_t i = 0; i < valueSize; ++i)
            {
                data[offset + i] = static_cast<uint8_t>(value >> ((valueSize - 1 - i) * 8));
            }
        }
        template <typename T>
        static T readFromVectorBE(const std::vector<uint8_t> &data, size_t offset)
        {
            static_assert(std::is_unsigned_v<T>, "T must be unsigned integer (uint8_t, uint16_t, uint32_t)");
            constexpr size_t valueSize = sizeof(T);

            if (data.size() < offset + valueSize)
            {
                _Kits::LogError("read error: offset = {}, value size = {}, vector size = {}", offset, valueSize, data.size());
                return 0;
            }

            T value = 0;
            for (size_t i = 0; i < valueSize; ++i)
            {
                value |= static_cast<T>(data[offset + i]) << ((valueSize - 1 - i) * 8);
            }
            return value;
        }

        /**
         * 通用字节写入（小端序）
         * @tparam T      数据类型（uint8_t, uint16_t, uint32_t）
         * @param data    目标 vector
         * @param value   要写入的值
         * @param offset  起始位置（默认 0）
         */
        template <typename T>
        void writeToVectorLE(std::vector<uint8_t> &data, T value, size_t offset)
        {
            static_assert(std::is_unsigned_v<T>, "T must be unsigned integer (uint8_t, uint16_t, uint32_t)");
            if (data.size() < offset + sizeof(T))
            {
                _Kits::LogError("size error: offset ={}, value size = {},vector size = {}", offset, sizeof(T), data.size());
            }
            std::memcpy(data.data() + offset, &value, sizeof(T));
        }
        /**
         * 通用字节读取（小端序）
         * @tparam T      数据类型（uint8_t, uint16_t, uint32_t）
         * @param data    源 vector
         * @param offset  起始位置（默认 0）
         * @return        读取出的值
         */
        template <typename T>
        T readFromVectorLE(const std::vector<uint8_t> &data, size_t offset)
        {
            static_assert(std::is_unsigned_v<T>, "T must be unsigned integer (uint8_t, uint16_t, uint32_t)");
            constexpr size_t valueSize = sizeof(T);

            if (data.size() < offset + valueSize)
            {
                _Kits::LogError("read error: offset = {}, value size = {}, vector size = {}", offset, valueSize, data.size());
                return 0;
            }

            T value = 0;
            std::memcpy(&value, data.data() + offset, valueSize);
            return value;
        }

        /**
         * 设置/清除某一位
         * @param data     目标 vector
         * @param offset   字节位置
         * @param bit      位索引（0~7，0=LSB）
         * @param set      true=置1，false=置0
         */
        void setBitInVectorLSB(std::vector<uint8_t> &data, size_t offset, uint8_t bit, bool set)
        {
            if (data.size() <= offset)
            {
                _Kits::LogError("size error: offset ={}", offset);
            }
            std::bitset<8> bits(data[offset]);
            bits.set(bit, set); // 设置或清除位
            data[offset] = static_cast<uint8_t>(bits.to_ulong());
        }

        /**
         * 设置/清除某一位
         * @param data     目标 vector
         * @param offset   字节位置
         * @param bit      位索引（0~7，0=MSB）
         * @param set      true=置1，false=置0
         */
        void setBitInVectorMSB(std::vector<uint8_t> &data, size_t offset, uint8_t bit, bool set)
        {
            if (data.size() <= offset)
            {
                _Kits::LogError("size error: offset ={}", offset);
            }
            int bitOffset = 7 - bit;
            if (bitOffset < 0)
            {
                _Kits::LogError("bit offset error: offset ={}", bitOffset);
            }
            std::bitset<8> bits(data[offset]);
            bits.set(bitOffset, set); // 设置或清除位
            data[offset] = static_cast<uint8_t>(bits.to_ulong());
        }
        static std::string hexString(const uint8_t *data, size_t len)
        {
            std::ostringstream oss;
            oss << std::hex << std::setfill('0');
            // 格式只需要设置一次
            for (size_t i = 0; i < len; i++)
            {
                if (i != 0)
                { // 不是第一个元素时添加逗号
                    oss << ",";
                }
                oss << std::setw(2) << static_cast<int>(data[i]);
            }

            return oss.str();
        }
    };
} // namespace _Kits
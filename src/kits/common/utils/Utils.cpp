#include <QFile>
#include "Utils.h"
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <fmt/format.h>

using namespace _Kits;
std::vector<std::string> Utils::split(const std::string &str, const std::string &delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos)
    {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }

    tokens.push_back(str.substr(start));
    return tokens;
}

int Utils::encryptAES(
    const unsigned char *plaintext, int plaintext_len, const unsigned char *key, unsigned char *iv, unsigned char *ciphertext)
{
    // EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    // if (ctx == nullptr)
    // {
    //     return -1;
    // }

    // // 初始化加密上下文
    // if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key, iv))
    // {
    //     return -1;
    // }

    // int len;
    // int ciphertext_len;

    // // 执行加密
    // if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    // {
    //     EVP_CIPHER_CTX_free(ctx);
    //     return -1;
    // }
    // ciphertext_len = len;

    // // 完成加密
    // if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
    // {
    //     EVP_CIPHER_CTX_free(ctx);
    //     return -1;
    // }
    // ciphertext_len += len;

    // EVP_CIPHER_CTX_free(ctx);
    // return ciphertext_len;
    return 0;
}

int Utils::decryptAES(
    const unsigned char *ciphertext, int ciphertext_len, const unsigned char *key, unsigned char *iv, unsigned char *plaintext)
{
    //     EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    //     if (ctx == nullptr)
    //     {
    //         return -1;
    //     }

    //     // 初始化解密上下文
    //     if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key, iv))
    //     {
    //         EVP_CIPHER_CTX_free(ctx);
    //         return -1;
    //     }

    //     int len;
    //     int plaintext_len;

    //     // 执行解密
    //     if (1 !=
    //         EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    //     {
    //         EVP_CIPHER_CTX_free(ctx);
    //         return -1;
    //     }
    //     plaintext_len = len;

    //     // 完成解密
    //     if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
    //     {
    //         EVP_CIPHER_CTX_free(ctx);
    //         return -1;
    //     }
    //     plaintext_len += len;

    //     EVP_CIPHER_CTX_free(ctx);
    //     return plaintext_len;
    return 0;
}

#ifdef _WIN32
LONG WINAPI _Kits::Utils::exceptionHandler(EXCEPTION_POINTERS *pException)
{
    // 默认生成在D盘下
    HANDLE hFile =
        CreateFileW(L"D:\\togee_infinity_station_crashdump.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ClientPointers = FALSE;
        // 生成中型转储文件包含：基本异常信息、线程栈、模块列表、内存区域信息
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemoryInfo, &dumpInfo, NULL, NULL);
        CloseHandle(hFile);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void _Kits::Utils::initCrashHandler()
{
#ifdef _WIN32
    SetUnhandledExceptionFilter(exceptionHandler);
#endif
}

void _Kits::Utils::writeDaemon(int runState)
{
#ifdef NDEBUG
    QFile file("daemon.io");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QTextStream out(&file);
        out << fmt::format("run_state={}\n", runState).c_str(); // 启动时写入1；退出时写入0
    }
#endif
    return;
}
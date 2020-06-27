#pragma once

#include <type_traits>
#include <stdio.h>
#include <comdef.h>

#define STRINGIFY(value) STRINGIFY2(value)
#define STRINGIFY2(value) #value
#define CONCAT(value0, value1) CONCAT2(value0, value1)
#define CONCAT2(value0, value1) value0##value1

#define ARRAY_SIZE(A) std::extent<decltype(A)>::value

#define KiB(value) (size_t(1024)*value)
#define MiB(value) (KiB(value)*size_t(1024))
#define GiB(value) (MiB(value)*size_t(1024))
#define TiB(value) (GiB(value)*size_t(1024))

// Microsoft Windows (built with MSVC)
#ifdef _MSC_VER

    #define EXPORT_SYMBOL __declspec(dllexport)
    #define PLATFORM_WINDOWS 1
    #define INCLUDE_IMPLEMENTATION(fileName) STRINGIFY(CONCAT(CONCAT(Implementations/Windows/, fileName), .inl))

    #ifdef _DEBUG
        #define CHECK_HRESULT_RETURN_VALUE_INTERNAL(hr, x, v)                                                                           \
        {                                                                                                                               \
            HRESULT hr = (x);                                                                                                           \
            if (FAILED(hr))                                                                                                             \
            {                                                                                                                           \
                char tmp[1024];                                                                                                         \
                sprintf_s(tmp, ARRAY_SIZE(tmp), "Statement %s failed with HRESULT %lX: %ls", #x, hr, _com_error(hr).ErrorMessage());    \
                int selection = MessageBoxA(NULL, tmp, "Assert", MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_TASKMODAL);                    \
                if (selection == IDRETRY) DebugBreak();                                                                                 \
                else if (selection == IDABORT) exit(1);                                                                                 \
                return v;                                                                                                               \
            }                                                                                                                           \
        }
    #else
        #define CHECK_HRESULT_RETURN_VALUE_INTERNAL(hr, x, v)                                                           \
        {                                                                                                               \
            HRESULT hr = (x);                                                                                           \
            if (FAILED(hr))                                                                                             \
            {                                                                                                           \
                return v;                                                                                               \
            }                                                                                                           \
        }
    #endif

    #define CHECK_HRESULT(x) CHECK_HRESULT_RETURN_VALUE_INTERNAL(hr_##__FILE__##__LINE__, x, hr_##__FILE__##__LINE__)
    #define CHECK_HRESULT_RETURN_VALUE(x, v) CHECK_HRESULT_RETURN_VALUE_INTERNAL(hr_##__FILE__##__LINE__, x, v)

    #ifdef _DEBUG
        #define SALVATION_ASSERT_MSG(x, msg)                                                                            \
        {                                                                                                               \
            if (!(x))                                                                                                   \
            {                                                                                                           \
                char tmp[1024];                                                                                         \
                sprintf_s(tmp, ARRAY_SIZE(tmp), "Assertion failed: %s\n%s", #x, msg);                                   \
                printf_s("%s", tmp);                                                                                    \
                int selection = MessageBoxA(NULL, tmp, "Assert", MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_TASKMODAL);    \
                if (selection == IDRETRY) DebugBreak();                                                                 \
                else if (selection == IDABORT) exit(1);                                                                 \
            }                                                                                                           \
        }

        #define SALVATION_ASSERT(x) SALVATION_ASSERT_MSG(x, "")
        #define SALVATION_ASSERT_ALWAYS_EXEC(x) SALVATION_ASSERT(x)
    #else
        #define SALVATION_ASSERT_MSG(x, msg)
        #define SALVATION_ASSERT(x)
        #define SALVATION_ASSERT_ALWAYS_EXEC(x) (x)
    #endif

#endif
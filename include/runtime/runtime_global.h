#pragma once

#ifdef VI_RUNTIME_LIB
#define VI_RUNTIME_API __declspec(dllexport)
#else
#define VI_RUNTIME_API __declspec(dllimport)
#endif
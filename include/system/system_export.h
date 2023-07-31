#pragma once

#ifdef ETD_RUNTIME_LIB
#define ETD_RUNTIME_API __declspec(dllexport)
#else
#define ETD_RUNTIME_API __declspec(dllimport)
#endif
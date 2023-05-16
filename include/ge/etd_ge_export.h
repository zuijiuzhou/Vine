#pragma once

#ifdef ETD_GE_LIB
#define ETD_GE_API __declspec(dllexport)
#else
#define ETD_GE_API __declspec(dllimport)
#endif
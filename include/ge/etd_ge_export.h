#pragma once

#ifdef ETD_GE_LIB
#define ETD_GE_EXPORT __declspec(dllexport)
#else
#define ETD_GE_EXPORT __declspec(dllimport)
#endif
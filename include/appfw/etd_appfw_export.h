#pragma once

#ifdef ETD_APPFW_LIB
#define ETD_APPFW_API __declspec(dllexport)
#else
#define ETD_APPFW_API __declspec(dllimport)
#endif
#pragma once

#ifdef VINE_RUNTIME_LIB
#define VINE_RUNTIME_API __declspec(dllexport)
#else
#define VINE_RUNTIME_API __declspec(dllimport)
#endif
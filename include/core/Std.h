#pragma once
#include "core_global.h"

#if defined(__LP64__) || defined(_LP64) || defined(_WIN64) || defined(__x86_64__)
#define VI_64
#endif

VI_CORE_NS_BEGIN

using byte = unsigned char;
using Char = char32_t; 

using Int8 = signed char;
using Int16 = short;
using Int32 = int;
using Int64 = long long;
using UInt8 = unsigned char;
using UInt16 = unsigned short;
using UInt32 = unsigned int;
using UInt64 = unsigned long long;

using Double = double;
using Float = float;
using Boolean = bool;

VI_CORE_NS_END
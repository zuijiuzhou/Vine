#pragma once

#include "Std.h"
#include "Platform.h"

#define VI_DISABLE_COPY(Cls) \
    Cls(const Cls &) = delete;\
    Cls &operator=(const Cls &) = delete;

#define VI_DISABLE_MOVE(Cls) \
    Cls(Class &&) = delete; \
    Cls &operator=(Cls &&) = delete;

#define VI_DISABLE_COPY_MOVE(Cls) \
    VI_DISABLE_COPY(Cls) \
    VI_DISABLE_MOVE(Cls)
#pragma once

#include "etd_ge_export.h"

namespace etd{
    namespace ge{
        class ETD_GE_EXPORT Vector2d{
            public:
            Vector2d();
            Vector2d(double xx, double yy);
            public:
            double length() const;
            public:
            double x;
            double y;
        };
    }
}
#pragma once

#include "ge_global.h"

VI_GE_NS_BEGIN

class VI_GE_API Quaternion {
public:
    Quaternion();
    Quaternion(double x, double y, double z, double w);

private:
    double x;
    double y;
    double z;
    double w;
};

VI_GE_NS_END
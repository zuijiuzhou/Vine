#include <vine/ge/Vector.h>

VI_GE_NS_BEGIN

static void xx() {
    Vector<int, 15> v1, v2;
    auto            n  = v1.DIM;
    auto            ns = typename Vector<int, 15>::ValType(5);

    int  dim = v1.DIM;
    auto p1  = &v1.DIM;
    auto p2  = &v2.DIM;

    int x = sizeof(Vector<int, 15>);
}

struct IN {
    IN() { xx(); }
} v;


VI_GE_NS_END
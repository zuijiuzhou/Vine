#include <gtest/gtest.h>

TEST(TestNumCast, NumCast)
{
    auto x = static_cast<int>(123456789000.);

    printf("\nstatic_cast<int>(123456789000.)=%i\n", x);
}
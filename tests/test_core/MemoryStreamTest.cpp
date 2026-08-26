#include <gtest/gtest.h>
#include <vine/MemoryStream.hpp>

#include <cstdint>
#include <string>

using vine::MemoryStream;

TEST(MemoryStream, WrapsByteRange)
{
    const std::uint8_t bytes[] = { 0x01, 0x02, 0x03, 0x04 };
    const MemoryStream stream(bytes, sizeof bytes);

    EXPECT_EQ(stream.size(), 4u);
    ASSERT_NE(stream.data(), nullptr);
    EXPECT_EQ(stream.data()[0], 0x01);
    EXPECT_EQ(stream.data()[3], 0x04);
}

TEST(MemoryStream, ReadableThroughRdbuf)
{
    // The wrapped bytes live in the get area, so rdbuf()->sgetn reads them.
    const char text[] = "hello";
    MemoryStream stream(text, sizeof text - 1);

    std::string out;
    out.resize(stream.size());
    auto* buf = stream.rdbuf();
    const auto n = buf->sgetn(out.data(), static_cast<std::streamsize>(out.size()));
    EXPECT_EQ(static_cast<std::size_t>(n), out.size());
    EXPECT_EQ(out, "hello");
}

TEST(MemoryStream, EmptyIsSafe)
{
    MemoryStream stream(nullptr, 0);
    EXPECT_EQ(stream.size(), 0u);
    EXPECT_EQ(stream.data(), nullptr);

    // Reading an empty stream yields nothing.
    char byte = 0;
    EXPECT_EQ(stream.rdbuf()->sgetn(&byte, 1), 0);
}

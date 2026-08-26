#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include <vine/crypto/Hash.hpp>
#include <vine/crypto/HashCalculator.hpp>
#include <vine/MemoryStream.hpp>

using vine::crypto::Hash;
using vine::crypto::HashCalculator;
using vine::MemoryStream;

namespace
{

template <std::size_t N>
std::string toHex(const std::array<std::uint8_t, N>& digest)
{
    static constexpr char hex[] = "0123456789abcdef";
    std::string           out;
    out.reserve(N * 2);
    for (std::uint8_t byte : digest) {
        out.push_back(hex[byte >> 4]);
        out.push_back(hex[byte & 0xF]);
    }
    return out;
}

std::string toHex(const std::vector<unsigned char>& data)
{
    static constexpr char hex[] = "0123456789abcdef";
    std::string           out;
    out.reserve(data.size() * 2);
    for (unsigned char byte : data) {
        out.push_back(hex[byte >> 4]);
        out.push_back(hex[byte & 0xF]);
    }
    return out;
}

TEST(CryptoTest, Sha256KnownVector)
{
    // Well-known SHA-256 of the ASCII string "abc".
    EXPECT_EQ(toHex(Hash::sha256("abc", 3)), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

TEST(CryptoTest, Sha256Empty)
{
    // SHA-256 of an empty input.
    EXPECT_EQ(toHex(Hash::sha256("", 0)), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

TEST(CryptoTest, Md5KnownVector)
{
    // Well-known MD5 of the ASCII string "abc".
    EXPECT_EQ(toHex(Hash::md5("abc", 3)), "900150983cd24fb0d6963f7d28e17f72");
}

TEST(CryptoTest, Sha1KnownVector)
{
    // Well-known SHA-1 of the ASCII string "abc".
    EXPECT_EQ(toHex(Hash::sha1("abc", 3)), "a9993e364706816aba3e25717850c26c9cd0d89d");
}

TEST(CryptoTest, Sha224KnownVector)
{
    // Well-known SHA-224 of the ASCII string "abc".
    EXPECT_EQ(toHex(Hash::sha224("abc", 3)), "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7");
}

TEST(CryptoTest, Sha384KnownVector)
{
    // Well-known SHA-384 of the ASCII string "abc".
    EXPECT_EQ(toHex(Hash::sha384("abc", 3)),
              "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7");
}

TEST(CryptoTest, Sha512KnownVector)
{
    // Well-known SHA-512 of the ASCII string "abc".
    EXPECT_EQ(toHex(Hash::sha512("abc", 3)),
              "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f");
}

TEST(CryptoTest, Sha3_224KnownVector)
{
    // Well-known SHA3-224 of the ASCII string "abc".
    EXPECT_EQ(toHex(Hash::sha3_224("abc", 3)), "e642824c3f8cf24ad09234ee7d3c766fc9a3a5168d0c94ad73b46fdf");
}

TEST(CryptoTest, Sha3_256KnownVector)
{
    // Well-known SHA3-256 of the ASCII string "abc".
    EXPECT_EQ(toHex(Hash::sha3_256("abc", 3)),
              "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532");
}

TEST(CryptoTest, Sha3_384KnownVector)
{
    // Well-known SHA3-384 of the ASCII string "abc".
    EXPECT_EQ(toHex(Hash::sha3_384("abc", 3)),
              "ec01498288516fc926459f58e2c6ad8df9b473cb0fc08c2596da7cf0e49be4b298d88cea927ac7f539f1edf228376d25");
}

TEST(CryptoTest, Sha3_512KnownVector)
{
    // Well-known SHA3-512 of the ASCII string "abc".
    EXPECT_EQ(toHex(Hash::sha3_512("abc", 3)),
              "b751850b1a57168a5693cd924b6b096e08f621827444f70d884f5d0240d2712e10e116e9192af3c91a7ec57647e3934057340b4cf408d5a56592f8274eec53f0");
}

TEST(CryptoTest, MemoryStreamOverloadMatchesByteRange)
{
    // A MemoryStream is an std::ostream wrapping the byte range; hashing it
    // must produce the same digest as the byte-range overloads (which wrap
    // the buffer into a MemoryStream and delegate to the stream version).
    // Reading a stream consumes it, so wrap a fresh stream per algorithm.
    MemoryStream md5_stream("abc", 3);
    EXPECT_EQ(toHex(Hash::md5(md5_stream)), "900150983cd24fb0d6963f7d28e17f72");

    MemoryStream sha256_stream("abc", 3);
    EXPECT_EQ(toHex(Hash::sha256(sha256_stream)), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");

    MemoryStream sha3_512_stream("abc", 3);
    EXPECT_EQ(toHex(Hash::sha3_512(sha3_512_stream)),
              "b751850b1a57168a5693cd924b6b096e08f621827444f70d884f5d0240d2712e10e116e9192af3c91a7ec57647e3934057340b4cf408d5a56592f8274eec53f0");
}

TEST(CryptoTest, EmptyMemoryStream)
{
    // An empty (null) stream must be safe to hash.
    MemoryStream stream(nullptr, 0);
    EXPECT_EQ(stream.size(), 0u);
    EXPECT_EQ(toHex(Hash::md5(stream)), "d41d8cd98f00b204e9800998ecf8427e");
}

TEST(CryptoTest, HashCalculatorByEnum)
{
    // Feed "abc" in two chunks through operator<< and finalize.
    HashCalculator calc(HashCalculator::Algorithm::ALG_SHA256);
    calc << u8"ab";
    calc << u8"c";
    EXPECT_EQ(calc.algorithm(), HashCalculator::Algorithm::ALG_SHA256);
    EXPECT_EQ(calc.digestSize(), 32u);
    EXPECT_EQ(toHex(calc.digest()), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

TEST(CryptoTest, HashCalculatorByName)
{
    // Names are case-insensitive and accept '-' or '_' separators.
    HashCalculator calc(u8"SHA3-512");
    calc.write("abc", 3);
    EXPECT_EQ(toHex(calc.digest()),
              "b751850b1a57168a5693cd924b6b096e08f621827444f70d884f5d0240d2712e10e116e9192af3c91a7ec57647e3934057340b4cf408d5a56592f8274eec53f0");
}

TEST(CryptoTest, HashCalculatorFromMemoryStream)
{
    MemoryStream stream("abc", 3);
    HashCalculator calc(u8"md5");
    calc << stream;
    EXPECT_EQ(toHex(calc.digest()), "900150983cd24fb0d6963f7d28e17f72");
}

TEST(CryptoTest, HashCalculatorInvalidName)
{
    EXPECT_THROW(HashCalculator(u8"nope"), std::invalid_argument);
}

TEST(CryptoTest, HashCalculatorRepeatedDigest)
{
    // digest() is cached; repeated calls return the same bytes.
    HashCalculator calc(HashCalculator::Algorithm::ALG_SHA1);
    calc << u8"abc";
    const auto first  = calc.digest();
    const auto second = calc.digest();
    EXPECT_EQ(first, second);
    EXPECT_EQ(toHex(first), "a9993e364706816aba3e25717850c26c9cd0d89d");
}

} // namespace

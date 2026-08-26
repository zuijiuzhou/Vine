#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include <vine/io/Zip.hpp>
#include <vine/io/ZipArchive.hpp>

using vine::io::Zip;
using vine::io::ZipArchive;

namespace
{

/**
 * @brief Creates a unique temporary directory that is removed on destruction.
 */
class TempDir
{
  public:
    TempDir()
    {
        static std::atomic<unsigned long long> counter{ 0 };
        std::error_code                        ec;
        path_ = std::filesystem::temp_directory_path(ec) /
                ("vine_iobase_" + std::to_string(counter.fetch_add(1)));
        std::filesystem::create_directories(path_, ec);
    }

    ~TempDir()
    {
        std::error_code ec;
        std::filesystem::remove_all(path_, ec);
    }

    const std::filesystem::path& path() const { return path_; }

  private:
    std::filesystem::path path_;
};

TEST(IoBaseTest, CompressRoundTrip)
{
    const std::string text = "The quick brown fox jumps over the lazy dog. ";
    std::string       payload;
    for (int i = 0; i < 50; ++i) {
        payload += text;
    }

    std::vector<unsigned char> compressed;
    ASSERT_TRUE(Zip::compress(payload.data(), payload.size(), compressed));
    EXPECT_LT(compressed.size(), payload.size());

    std::vector<unsigned char> decompressed;
    ASSERT_TRUE(Zip::decompress(compressed.data(), compressed.size(), decompressed));
    EXPECT_EQ(std::string(reinterpret_cast<const char*>(decompressed.data()), decompressed.size()), payload);
}

TEST(IoBaseTest, DecompressRejectsGarbage)
{
    const unsigned char garbage[] = { 0xFF, 0xFE, 0x00, 0x01, 0x02 };
    std::vector<unsigned char> out;
    EXPECT_FALSE(Zip::decompress(garbage, sizeof(garbage), out));
}

TEST(IoBaseTest, ZipArchiveAddFileAndReadEntry)
{
    TempDir                 dir;
    const std::filesystem::path zip_path = dir.path() / "single.zip";
    const std::string           content  = "hello zip world";

    ZipArchive archive;
    ASSERT_TRUE(archive.addFile(u8"greeting.txt", content.data(), content.size()));
    ASSERT_TRUE(archive.save(zip_path));

    std::vector<unsigned char> out;
    ASSERT_TRUE(ZipArchive::readEntry(zip_path, u8"greeting.txt", out));
    EXPECT_EQ(std::string(out.begin(), out.end()), content);
}

TEST(IoBaseTest, ZipArchiveAddDirectoryRoundTrip)
{
    TempDir                 dir;
    const std::filesystem::path source   = dir.path() / "src";
    const std::filesystem::path dest     = dir.path() / "dest";
    const std::filesystem::path zip_path = dir.path() / "dir.zip";
    std::filesystem::create_directories(source / "sub");
    {
        std::ofstream f1(source / "a.txt");
        f1 << "alpha";
        std::ofstream f2(source / "sub" / "b.txt");
        f2 << "beta";
    }

    ZipArchive archive;
    ASSERT_TRUE(archive.addDirectory(source));
    ASSERT_TRUE(archive.save(zip_path));

    const std::vector<vine::String> names = ZipArchive::entryNames(zip_path);
    EXPECT_NE(std::find(names.begin(), names.end(), vine::String(u8"a.txt")), names.end());
    EXPECT_NE(std::find(names.begin(), names.end(), vine::String(u8"sub/b.txt")), names.end());

    ASSERT_TRUE(ZipArchive::decompressFile(zip_path, dest));
    std::ifstream in(dest / "sub" / "b.txt");
    ASSERT_TRUE(in.good());
    const std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "beta");
}

TEST(IoBaseTest, ZipArchiveMissingFileFails)
{
    EXPECT_TRUE(ZipArchive::entryNames("no_such_file.zip").empty());
    std::vector<unsigned char> out;
    EXPECT_FALSE(ZipArchive::readEntry("no_such_file.zip", u8"x", out));
}

TEST(IoBaseTest, ZipCompressDirectoryToFile)
{
    TempDir                     dir;
    const std::filesystem::path source   = dir.path() / "src";
    const std::filesystem::path zip_path = dir.path() / "dir.zip";
    std::filesystem::create_directories(source / "sub");
    {
        std::ofstream f(source / "a.txt");
        f << "hello";
    }

    ASSERT_TRUE(Zip::compressDirectory(source, zip_path));
    EXPECT_TRUE(std::filesystem::exists(zip_path));

    const std::vector<vine::String> names = ZipArchive::entryNames(zip_path);
    EXPECT_NE(std::find(names.begin(), names.end(), vine::String(u8"a.txt")), names.end());
}

TEST(IoBaseTest, ZipDecompressFileToDirectory)
{
    TempDir                     dir;
    const std::filesystem::path source   = dir.path() / "src";
    const std::filesystem::path dest     = dir.path() / "out";
    const std::filesystem::path zip_path = dir.path() / "dir.zip";
    std::filesystem::create_directories(source);
    {
        std::ofstream f(source / "b.txt");
        f << "world";
    }

    ASSERT_TRUE(Zip::compressDirectory(source, zip_path));
    ASSERT_TRUE(Zip::decompressFile(zip_path, dest));

    std::ifstream in(dest / "b.txt");
    ASSERT_TRUE(in.good());
    const std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "world");
}

} // namespace

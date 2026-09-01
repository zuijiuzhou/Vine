#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <vine/String.hpp>
#include <vine/io/DirectoryVfs.hpp>
#include <vine/io/IMemoryVfs.hpp>
#include <vine/io/ZipMemoryVfs.hpp>

using vine::io::DirectoryVfs;
using vine::io::ZipMemoryVfs;

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
                ("vine_vfs_" + std::to_string(counter.fetch_add(1)));
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

std::vector<unsigned char> bytesOf(const std::string& text)
{
    return { text.begin(), text.end() };
}

bool contains(const std::vector<vine::String>& list, const vine::String& name)
{
    return std::find(list.begin(), list.end(), name) != list.end();
}

} // namespace

TEST(VfsTest, ZipWriteReadRoundTrip)
{
    ZipMemoryVfs vfs;
    ASSERT_TRUE(vfs.writeFile(u8"workcell.xml", u8"<workcell name=\"demo\"/>"));
    const std::string binary = "\x00\x01\x02hello";
    ASSERT_TRUE(vfs.writeFile(u8"geoms/base.bin", bytesOf(binary)));

    std::vector<unsigned char> xml;
    ASSERT_TRUE(vfs.readFile(u8"workcell.xml", xml));
    EXPECT_EQ(xml, bytesOf("<workcell name=\"demo\"/>"));

    std::vector<unsigned char> bin;
    ASSERT_TRUE(vfs.readFile(u8"geoms/base.bin", bin));
    EXPECT_EQ(bin, bytesOf(binary));

    // Directory semantics.
    EXPECT_TRUE(vfs.exists(u8""));
    EXPECT_TRUE(vfs.exists(u8"workcell.xml"));
    EXPECT_TRUE(vfs.exists(u8"geoms"));
    EXPECT_TRUE(vfs.isFile(u8"workcell.xml"));
    EXPECT_TRUE(vfs.isDirectory(u8"geoms"));
    EXPECT_FALSE(vfs.isFile(u8"geoms"));
    EXPECT_FALSE(vfs.exists(u8"nope"));
}

TEST(VfsTest, ZipListAndRemove)
{
    ZipMemoryVfs vfs;
    vfs.writeFile(u8"a.txt", u8"1");
    vfs.writeFile(u8"b/c.txt", u8"2");
    vfs.writeFile(u8"b/d.txt", u8"3");
    vfs.writeFile(u8"e/f/g.txt", u8"4");

    const auto top = vfs.list(u8"");
    ASSERT_EQ(top.size(), 3u); // a.txt, b, e
    EXPECT_TRUE(contains(top, vine::String(u8"a.txt")));
    EXPECT_TRUE(contains(top, vine::String(u8"b")));
    EXPECT_TRUE(contains(top, vine::String(u8"e")));

    EXPECT_EQ(vfs.list(u8"b").size(), 2u);

    // Remove a directory subtree.
    ASSERT_TRUE(vfs.remove(u8"b"));
    EXPECT_FALSE(vfs.exists(u8"b"));
    EXPECT_TRUE(vfs.exists(u8"a.txt"));

    // Remove a single file.
    ASSERT_TRUE(vfs.remove(u8"a.txt"));
    EXPECT_FALSE(vfs.exists(u8"a.txt"));
}

TEST(VfsTest, ZipSaveOpenFileRoundTrip)
{
    const TempDir temp;
    const auto    pkg = temp.path() / "pkg.zip";

    {
        ZipMemoryVfs vfs;
        vfs.writeFile(u8"workcell.xml", u8"<workcell name=\"demo\"/>");
        vfs.writeFile(u8"devices/robot.vdev", u8"<device name=\"robot\"/>");
        ASSERT_TRUE(vfs.save(pkg));
    }

    auto opened = ZipMemoryVfs::openZip(pkg);
    ASSERT_NE(opened, nullptr);
    std::vector<unsigned char> xml;
    ASSERT_TRUE(opened->readFile(u8"workcell.xml", xml));
    EXPECT_EQ(xml, bytesOf("<workcell name=\"demo\"/>"));
    EXPECT_TRUE(opened->isDirectory(u8"devices"));
    EXPECT_TRUE(opened->exists(u8"devices/robot.vdev"));
}

TEST(VfsTest, ZipSaveMemoryRoundTrip)
{
    std::vector<unsigned char> pkg;
    {
        ZipMemoryVfs vfs;
        vfs.writeFile(u8"workcell.xml", u8"<workcell/>");
        ASSERT_TRUE(vfs.save(pkg));
        EXPECT_FALSE(pkg.empty());
    }

    auto opened = ZipMemoryVfs::openZip(pkg.data(), pkg.size());
    ASSERT_NE(opened, nullptr);
    std::vector<unsigned char> xml;
    ASSERT_TRUE(opened->readFile(u8"workcell.xml", xml));
    EXPECT_EQ(xml, bytesOf("<workcell/>"));
}

TEST(VfsTest, ZipSaveStreamRoundTrip)
{
    std::ostringstream stream;
    {
        ZipMemoryVfs vfs;
        vfs.writeFile(u8"a.txt", u8"hello");
        ASSERT_TRUE(vfs.save(stream));
    }
    const std::string data = stream.str();
    ASSERT_FALSE(data.empty());

    auto opened = ZipMemoryVfs::openZip(data.data(), data.size());
    ASSERT_NE(opened, nullptr);
    std::vector<unsigned char> out;
    ASSERT_TRUE(opened->readFile(u8"a.txt", out));
    EXPECT_EQ(out, bytesOf("hello"));
}

TEST(VfsTest, ZipMountFile)
{
    const TempDir temp;
    const auto    src = temp.path() / "mesh.bin";
    {
        std::ofstream out(src, std::ios::binary);
        out << "mesh-bytes";
    }

    std::vector<unsigned char> pkg;
    {
        ZipMemoryVfs vfs;
        ASSERT_TRUE(vfs.mountFile(u8"geoms/mesh.bin", src));
        ASSERT_TRUE(vfs.save(pkg));
    }

    auto opened = ZipMemoryVfs::openZip(pkg.data(), pkg.size());
    ASSERT_NE(opened, nullptr);
    std::vector<unsigned char> mesh;
    ASSERT_TRUE(opened->readFile(u8"geoms/mesh.bin", mesh));
    EXPECT_EQ(mesh, bytesOf("mesh-bytes"));
}

TEST(VfsTest, DirectoryVfsRoundTrip)
{
    const TempDir     temp;
    const auto        root = temp.path() / "tree";
    std::error_code   ec;
    std::filesystem::create_directories(root, ec);
    ASSERT_FALSE(ec);

    auto dir = DirectoryVfs::openDirectory(root);
    ASSERT_NE(dir, nullptr);
    ASSERT_TRUE(dir->writeFile(u8"workcell.xml", u8"<workcell/>"));
    ASSERT_TRUE(dir->writeFile(u8"geoms/a.bin", bytesOf("abc")));

    std::vector<unsigned char> xml;
    ASSERT_TRUE(dir->readFile(u8"workcell.xml", xml));
    EXPECT_EQ(xml, bytesOf("<workcell/>"));
    EXPECT_TRUE(dir->isDirectory(u8"geoms"));
    EXPECT_TRUE(dir->isFile(u8"geoms/a.bin"));
    EXPECT_TRUE(std::filesystem::is_regular_file(root / "workcell.xml", ec));

    // Consistency with the zip backend: the same tree yields the same entries.
    std::vector<unsigned char> pkg;
    {
        ZipMemoryVfs vfs;
        vfs.writeFile(u8"workcell.xml", u8"<workcell/>");
        vfs.writeFile(u8"geoms/a.bin", bytesOf("abc"));
        ASSERT_TRUE(vfs.save(pkg));
    }
    auto zip = ZipMemoryVfs::openZip(pkg.data(), pkg.size());
    ASSERT_NE(zip, nullptr);

    auto dir_list = dir->list(u8"");
    auto zip_list = zip->list(u8"");
    std::sort(dir_list.begin(), dir_list.end());
    std::sort(zip_list.begin(), zip_list.end());
    EXPECT_EQ(dir_list, zip_list);
}

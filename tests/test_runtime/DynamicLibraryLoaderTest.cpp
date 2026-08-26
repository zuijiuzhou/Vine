#include <gtest/gtest.h>

#include <vine/runtime/DynamicLibraryLoader.hpp>

using vine::String;
using vine::runtime::DynamicLibraryLoader;

namespace
{

TEST(DynamicLibraryLoaderTest, LoadMissingReturnsNull)
{
    DynamicLibraryLoader loader;
    EXPECT_EQ(loader.load(u8"vine_no_such_library_xyz"), nullptr);
}

TEST(DynamicLibraryLoaderTest, ReusesAlreadyLoadedLibrary)
{
    DynamicLibraryLoader loader;
#ifdef _WIN32
    const String path = u8"kernel32.dll";
#else
    const String path = u8"/proc/self/exe";
#endif

    auto* first  = loader.load(path);
    auto* second = loader.load(path);
    ASSERT_NE(first, nullptr);
    EXPECT_EQ(first, second);
    EXPECT_EQ(loader.count(), 1u);
    EXPECT_NE(first->handle(), nullptr);
    EXPECT_TRUE(first->fileName() == path);
    EXPECT_EQ(loader.find(path), first);
}

TEST(DynamicLibraryLoaderTest, FindReturnsNullWhenNotLoaded)
{
    DynamicLibraryLoader loader;
    EXPECT_EQ(loader.find(u8"vine_no_such_library_xyz"), nullptr);
}

TEST(DynamicLibraryLoaderTest, SharedInstanceIsSingle)
{
    auto& a = DynamicLibraryLoader::instance();
    auto& b = DynamicLibraryLoader::instance();
    EXPECT_EQ(&a, &b);
    EXPECT_EQ(a.load(u8"vine_no_such_library_xyz"), nullptr);
}

TEST(DynamicLibraryLoaderTest, ResolvesKnownSymbol)
{
    DynamicLibraryLoader loader;
#ifdef _WIN32
    auto* lib = loader.load(u8"kernel32.dll");
    ASSERT_NE(lib, nullptr);
    EXPECT_NE(lib->resolveSymbol<int()>(u8"GetModuleHandleW"), nullptr);
#else
    auto* lib = loader.load(u8"/proc/self/exe");
    ASSERT_NE(lib, nullptr);
    EXPECT_NE(lib->resolveSymbol<int()>(u8"main"), nullptr);
#endif
}

TEST(DynamicLibraryLoaderTest, SearchPathResolvesAndDeduplicates)
{
    DynamicLibraryLoader loader;
#ifdef _WIN32
    const String dir  = u8"C:/Windows/System32";
    const String name = u8"kernel32.dll";
#else
    const String dir  = u8"/proc/self";
    const String name = u8"exe";
#endif

    auto* direct = loader.load(dir + u8"/" + name);
    ASSERT_NE(direct, nullptr);

    loader.addSearchPath(dir);
    EXPECT_EQ(loader.searchPaths().size(), 1u);
    auto* via_search = loader.load(name);
    ASSERT_NE(via_search, nullptr);
    // Both names resolve to the same path, so one cached instance is reused.
    EXPECT_EQ(via_search, direct);
    EXPECT_EQ(loader.count(), 1u);
    EXPECT_EQ(loader.find(name), direct);
}

TEST(DynamicLibraryLoaderTest, ManageSearchPaths)
{
    DynamicLibraryLoader loader;
    loader.addSearchPath(u8"/a");
    loader.addSearchPath(u8"/b");
    EXPECT_EQ(loader.searchPaths().size(), 2u);
    loader.removeSearchPath(u8"/a");
    EXPECT_EQ(loader.searchPaths().size(), 1u);
    loader.clearSearchPaths();
    EXPECT_TRUE(loader.searchPaths().empty());
}

TEST(DynamicLibraryLoaderTest, ManageDependencyPaths)
{
    DynamicLibraryLoader loader;
    loader.addDependencyPath(u8"/a");
    loader.addDependencyPath(u8"/b");
    EXPECT_EQ(loader.dependencyPaths().size(), 2u);
    loader.removeDependencyPath(u8"/a");
    EXPECT_EQ(loader.dependencyPaths().size(), 1u);
    loader.clearDependencyPaths();
    EXPECT_TRUE(loader.dependencyPaths().empty());
}

} // namespace

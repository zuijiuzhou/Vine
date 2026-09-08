#pragma once

#include <cstddef>
#include <filesystem>
#include <memory>
#include <ostream>
#include <vector>

#include <vine/io/IMemoryVfs.hpp>
#include <vine/io/io_global.hpp>
#include <vine/String.hpp>

V_IO_NS_BEGIN

/**
 * @brief Real-directory backed VFS (debug backend).
 *
 * Maps the virtual tree directly onto a real directory: writeFile writes a
 * real file, readFile reads one, and save() is effectively a no-op because
 * writes are immediate. Useful for inspecting or debugging what would go
 * into a ZIP package.
 */
class V_IOBASE_API DirectoryVfs : public IMemoryVfs
{
  public:
    /**
     * @brief Constructs a VFS rooted at an existing directory.
     *
     * @param root The real directory to map onto.
     */
    explicit DirectoryVfs(const std::filesystem::path& root);

    /**
     * @brief Destroys the VFS.
     */
    ~DirectoryVfs() override;

    /**
     * @brief Opens a real directory as a VFS.
     *
     * @param dir The directory to map onto (must exist).
     * @return The VFS, or null when the directory does not exist.
     */
    static std::unique_ptr<DirectoryVfs> openDirectory(const std::filesystem::path& dir);

    // IMemoryVfs
    /**
     * @brief Checks whether a virtual file or directory exists.
     *
     * @param path The virtual path to check.
     * @return true when the mapped real path exists.
     */
    bool exists(const String& path) const override;

    /**
     * @brief Checks whether a virtual path names a file (not a directory).
     *
     * @param path The virtual path to check.
     * @return true when the mapped real path is a regular file.
     */
    bool isFile(const String& path) const override;

    /**
     * @brief Checks whether a virtual path names a directory.
     *
     * @param path The virtual path to check.
     * @return true when the mapped real path is a directory.
     */
    bool isDirectory(const String& path) const override;

    /**
     * @brief Lists the direct children (files and directories) under a directory.
     *
     * @param dir The virtual directory to list; empty denotes the root.
     * @return The direct child names.
     */
    std::vector<String> list(const String& dir = {}) const override;

    /**
     * @brief Removes a virtual file or the whole subtree under a directory.
     *
     * @param path The virtual file or directory to remove.
     * @return true when anything was removed.
     */
    bool remove(const String& path) override;

    /**
     * @brief Writes a byte range as a real file under the root.
     *
     * @param path The virtual file path.
     * @param data Pointer to the bytes to store, or null for an empty file.
     * @param size Number of bytes.
     * @return true on success.
     */
    bool writeFile(const String& path, const void* data, std::size_t size) override;

    /**
     * @brief Writes an in-memory buffer as a real file under the root.
     *
     * @param path The virtual file path.
     * @param data Bytes to store.
     * @return true on success.
     */
    bool writeFile(const String& path, const std::vector<unsigned char>& data) override;

    /**
     * @brief Writes UTF-8 text as a real file under the root.
     *
     * @param path The virtual file path.
     * @param text The UTF-8 text to store.
     * @return true on success.
     */
    bool writeFile(const String& path, const String& text) override;

    /**
     * @brief Copies a real file into the tree.
     *
     * Unlike the zip backend, the copy is immediate.
     *
     * @param vfs_path The virtual file path.
     * @param real_path The physical file to copy.
     * @return true on success.
     */
    bool mountFile(const String& vfs_path, const std::filesystem::path& real_path) override;

    /**
     * @brief Reads a virtual file's bytes.
     *
     * @param path The virtual file path.
     * @param out Receives the file bytes.
     * @return true when the file exists and was read.
     */
    bool readFile(const String& path, std::vector<unsigned char>& out) const override;

    /**
     * @brief Persists the tree.
     *
     * Writes are immediate; only a directory target is meaningful (a no-op).
     *
     * @param path The directory to confirm as the target.
     * @return true when path is an existing directory.
     */
    bool save(const std::filesystem::path& path) override;

    /**
     * @brief Persists the tree as bytes.
     *
     * Not supported by a directory backend.
     *
     * @param out Unused.
     * @return false always.
     */
    bool save(std::vector<unsigned char>& out) override;

    /**
     * @brief Persists the tree into an output stream.
     *
     * Not supported by a directory backend.
     *
     * @param out Unused.
     * @return false always.
     */
    bool save(std::ostream& out) override;

  private:
    /**
     * @brief Maps a virtual path to a real path under the root.
     *
     * @param vfs_path The virtual path.
     * @return The mapped real path, or an empty path on invalid input.
     */
    std::filesystem::path toReal(const String& vfs_path) const;

    std::filesystem::path root_;
};

V_IO_NS_END

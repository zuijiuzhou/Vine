#pragma once

#include <cstddef>
#include <filesystem>
#include <ostream>
#include <vector>

#include <vine/io/io_global.hpp>
#include <vine/String.hpp>

V_IO_NS_BEGIN

/**
 * @brief An in-memory virtual file system (VFS).
 *
 * A memory-level virtual file tree addressed by '/' separated paths. Files
 * can be written from bytes or text, or mounted from a real file (read at
 * save time). The tree can be persisted (save) and opened back, with no
 * extraction to disk. See ZipMemoryVfs and DirectoryVfs for backends.
 */
class V_IOBASE_API IMemoryVfs
{
  public:
    /**
     * @brief Destroys the virtual file system.
     */
    virtual ~IMemoryVfs();

    // ---- path / directory operations ----
    /**
     * @brief Checks whether a virtual file or directory exists.
     *
     * The virtual root always exists.
     *
     * @param path The virtual path to check.
     * @return true when path names an existing file, directory or the root.
     */
    virtual bool exists(const String& path) const = 0;

    /**
     * @brief Checks whether a virtual path names a file (not a directory).
     *
     * @param path The virtual path to check.
     * @return true when path names an existing file.
     */
    virtual bool isFile(const String& path) const = 0;

    /**
     * @brief Checks whether a virtual path names a directory.
     *
     * The virtual root is always a directory.
     *
     * @param path The virtual path to check.
     * @return true when path names an existing directory or the root.
     */
    virtual bool isDirectory(const String& path) const = 0;

    /**
     * @brief Lists the direct children (files and directories) under a directory.
     *
     * @param dir The virtual directory to list; empty denotes the root.
     * @return The direct child names, empty when dir does not exist.
     */
    virtual std::vector<String> list(const String& dir = {}) const = 0;

    /**
     * @brief Removes a virtual file or the whole subtree under a directory.
     *
     * The virtual root cannot be removed.
     *
     * @param path The virtual file or directory to remove.
     * @return true when anything was removed.
     */
    virtual bool remove(const String& path) = 0;

    // ---- file writes ----
    /**
     * @brief Writes a byte range as a virtual file.
     *
     * @param path The virtual file path; parent directories are implied.
     * @param data Pointer to the bytes to store, or null for an empty file.
     * @param size Number of bytes.
     * @return true on success.
     */
    virtual bool writeFile(const String& path, const void* data, std::size_t size) = 0;

    /**
     * @brief Writes an in-memory buffer as a virtual file.
     *
     * @param path The virtual file path.
     * @param data Bytes to store.
     * @return true on success.
     */
    virtual bool writeFile(const String& path, const std::vector<unsigned char>& data) = 0;

    /**
     * @brief Writes UTF-8 text as a virtual file.
     *
     * @param path The virtual file path.
     * @param text The UTF-8 text to store.
     * @return true on success.
     */
    virtual bool writeFile(const String& path, const String& text) = 0;

    /**
     * @brief Binds a virtual path to a real file.
     *
     * The real file is read when save() runs, so it does not need to be
     * buffered in memory up front.
     *
     * @param vfs_path The virtual file path.
     * @param real_path The physical file to read from.
     * @return true on success.
     */
    virtual bool mountFile(const String& vfs_path, const std::filesystem::path& real_path) = 0;

    // ---- file reads ----
    /**
     * @brief Reads a virtual file's bytes.
     *
     * @param path The virtual file path.
     * @param out Receives the file bytes; cleared on failure.
     * @return true when the file exists and was read.
     */
    virtual bool readFile(const String& path, std::vector<unsigned char>& out) const = 0;

    // ---- persistence backends ----
    /**
     * @brief Persists the tree.
     *
     * A directory backend writes immediately and treats a directory path as
     * a no-op.
     *
     * @param path The target path (a .zip file, or a directory for the
     *             directory backend).
     * @return true on success.
     */
    virtual bool save(const std::filesystem::path& path) = 0;

    /**
     * @brief Persists the tree as bytes.
     *
     * Unsupported by a directory backend.
     *
     * @param out Receives the persisted bytes.
     * @return true on success.
     */
    virtual bool save(std::vector<unsigned char>& out) = 0;

    /**
     * @brief Persists the tree into an output stream.
     *
     * Unsupported by a directory backend.
     *
     * @param out Target output stream.
     * @return true on success.
     */
    virtual bool save(std::ostream& out) = 0;
};

V_IO_NS_END

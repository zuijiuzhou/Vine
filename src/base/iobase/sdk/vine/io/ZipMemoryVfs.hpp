#pragma once

#include <cstddef>
#include <filesystem>
#include <map>
#include <memory>
#include <ostream>
#include <vector>

#include <vine/io/IMemoryVfs.hpp>
#include <vine/io/io_global.hpp>
#include <vine/String.hpp>

V_IO_NS_BEGIN

class ZipArchive;

/**
 * @brief ZIP-backed in-memory virtual file system.
 *
 * Holds the virtual tree in memory (writeFile / mountFile), persists it to a
 * ZIP archive (save) and can open an existing ZIP back into memory (openZip).
 * Opening never extracts anything to disk; entries are read directly from the
 * archive.
 */
class V_IOBASE_API ZipMemoryVfs : public IMemoryVfs
{
  public:
    /**
     * @brief Constructs an empty, writable VFS.
     */
    ZipMemoryVfs();

    /**
     * @brief Destroys the VFS.
     */
    ~ZipMemoryVfs() override;

    /**
     * @brief Opens a ZIP file into a new in-memory VFS.
     *
     * No entry is extracted to disk; every entry is read into memory.
     *
     * @param path The .zip file path.
     * @return The VFS, or null when the file is not a readable zip.
     */
    static std::unique_ptr<ZipMemoryVfs> openZip(const std::filesystem::path& path);

    /**
     * @brief Opens an in-memory ZIP into a new VFS.
     *
     * No entry is extracted to disk; every entry is read into memory.
     *
     * @param data The ZIP bytes.
     * @param size Number of bytes.
     * @return The VFS, or null on failure.
     */
    static std::unique_ptr<ZipMemoryVfs> openZip(const void* data, std::size_t size);

    // IMemoryVfs
    /**
     * @brief Checks whether a virtual file or directory exists.
     *
     * @param path The virtual path to check.
     * @return true when path names an existing file, directory or the root.
     */
    bool exists(const String& path) const override;

    /**
     * @brief Checks whether a virtual path names a file (not a directory).
     *
     * @param path The virtual path to check.
     * @return true when path names an existing file.
     */
    bool isFile(const String& path) const override;

    /**
     * @brief Checks whether a virtual path names a directory.
     *
     * @param path The virtual path to check.
     * @return true when path names an existing directory or the root.
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
     * @brief Writes a byte range as a virtual file.
     *
     * @param path The virtual file path.
     * @param data Pointer to the bytes to store, or null for an empty file.
     * @param size Number of bytes.
     * @return true on success.
     */
    bool writeFile(const String& path, const void* data, std::size_t size) override;

    /**
     * @brief Writes an in-memory buffer as a virtual file.
     *
     * @param path The virtual file path.
     * @param data Bytes to store.
     * @return true on success.
     */
    bool writeFile(const String& path, const std::vector<unsigned char>& data) override;

    /**
     * @brief Writes UTF-8 text as a virtual file.
     *
     * @param path The virtual file path.
     * @param text The UTF-8 text to store.
     * @return true on success.
     */
    bool writeFile(const String& path, const String& text) override;

    /**
     * @brief Binds a virtual path to a real file.
     *
     * @param vfs_path The virtual file path.
     * @param real_path The physical file to read from.
     * @return true on success.
     */
    bool mountFile(const String& vfs_path, const std::filesystem::path& real_path) override;

    /**
     * @brief Reads a virtual file's bytes.
     *
     * @param path The virtual file path.
     * @param out Receives the file bytes; cleared on failure.
     * @return true when the file exists and was read.
     */
    bool readFile(const String& path, std::vector<unsigned char>& out) const override;

    /**
     * @brief Persists the tree to a ZIP file.
     *
     * @param path Output .zip file path.
     * @return true on success.
     */
    bool save(const std::filesystem::path& path) override;

    /**
     * @brief Persists the tree as ZIP bytes.
     *
     * @param out Receives the ZIP bytes.
     * @return true on success.
     */
    bool save(std::vector<unsigned char>& out) override;

    /**
     * @brief Persists the tree as ZIP bytes into an output stream.
     *
     * @param out Target output stream.
     * @return true on success.
     */
    bool save(std::ostream& out) override;

  private:
    /**
     * @brief Adds every entry (inline bytes or file-backed) to a ZipArchive.
     *
     * @param archive The target archive.
     * @return true on success.
     */
    bool fillArchive(ZipArchive& archive) const;

    struct Entry
    {
        std::vector<unsigned char> data;
        std::filesystem::path      src;
        bool                       from_file{ false };
    };

    std::map<String, Entry> entries_;
};

V_IO_NS_END

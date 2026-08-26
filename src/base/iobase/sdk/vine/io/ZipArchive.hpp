#pragma once
#include "io_global.hpp"

#include <cstddef>
#include <filesystem>
#include <vector>

#include <vine/String.hpp>

V_IO_NS_BEGIN

/**
 * @brief Creates and extracts ZIP archives.
 *
 * Entries are addressed with '/' separators, so a directory tree becomes
 * entries such as "subdir/file.txt". Entries added for writing are buffered
 * in memory until save() writes them to the output file.
 */
class V_IOBASE_API ZipArchive
{
  public:
    ZipArchive();
    ~ZipArchive();

    ZipArchive(const ZipArchive&) = delete;
    ZipArchive& operator=(const ZipArchive&) = delete;

    /**
     * @brief Adds an in-memory byte range as an archive entry.
     *
     * @param name Entry name, using '/' separators.
     * @param data Pointer to the bytes to store.
     * @param size Number of bytes.
     * @return true on success.
     */
    bool addFile(const String& name, const void* data, std::size_t size);

    /**
     * @brief Adds an in-memory buffer as an archive entry.
     *
     * @param name Entry name, using '/' separators.
     * @param data Bytes to store.
     * @return true on success.
     */
    bool addFile(const String& name, const std::vector<unsigned char>& data);

    /**
     * @brief Recursively adds all files under a directory.
     *
     * Entry names are relative to dir_path and use '/' separators. Empty
     * subdirectories are preserved as entries whose name ends with '/'.
     *
     * @param dir_path Directory to walk.
     * @return true on success.
     */
    bool addDirectory(const std::filesystem::path& dir_path);

    /**
     * @brief Writes the added entries to a ZIP file.
     *
     * @param path Output .zip file path.
     * @return true on success.
     */
    bool save(const std::filesystem::path& path);

    /**
     * @brief Lists the entry names stored in a ZIP file.
     *
     * @param path Input .zip file path.
     * @return Entry names, or empty on failure.
     */
    static std::vector<String> entryNames(const std::filesystem::path& path);

    /**
     * @brief Decompresses all entries of a ZIP file into a directory.
     *
     * Entry names that could escape the destination directory are rejected.
     *
     * @param path Input .zip file path.
     * @param dir_path Destination directory, created if missing.
     * @return true on success.
     */
    static bool decompressFile(const std::filesystem::path& path, const std::filesystem::path& dir_path);

    /**
     * @brief Reads one entry of a ZIP file into a buffer.
     *
     * @param path Input .zip file path.
     * @param name Entry name to read.
     * @param out Receives the entry bytes.
     * @return true on success.
     */
    static bool readEntry(const std::filesystem::path& path, const String& name, std::vector<unsigned char>& out);

  private:
    struct Entry
    {
        String                     name;
        std::vector<unsigned char> data;
    };

    std::vector<Entry> entries_;
};

V_IO_NS_END

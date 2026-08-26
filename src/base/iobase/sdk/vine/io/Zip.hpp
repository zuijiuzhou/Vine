#pragma once
#include "io_global.hpp"

#include <cstddef>
#include <filesystem>
#include <vector>

V_IO_NS_BEGIN

/**
 * @brief Compression and ZIP archive helpers.
 *
 * A static utility class: compress/decompress operate on byte ranges with the
 * zlib format; compressDirectory/decompressFile move a whole directory tree
 * into or out of a ZIP file.
 */
class V_IOBASE_API Zip
{
  public:
    Zip() = delete;

    /**
     * @brief Compresses a byte range with the zlib format.
     *
     * @param data Pointer to the input bytes.
     * @param size Number of input bytes.
     * @param out Receives the compressed bytes.
     * @return true on success.
     */
    static bool compress(const void* data, std::size_t size, std::vector<unsigned char>& out);

    /**
     * @brief Decompresses a zlib-format byte range.
     *
     * The output buffer is grown as needed until decompression succeeds.
     *
     * @param data Pointer to the compressed bytes.
     * @param size Number of compressed bytes.
     * @param out Receives the decompressed bytes.
     * @return true on success.
     */
    static bool decompress(const void* data, std::size_t size, std::vector<unsigned char>& out);

    /**
     * @brief Compresses a directory tree into a ZIP file.
     *
     * Entry names are relative to dir_path and use '/' separators.
     *
     * @param dir_path Directory to compress.
     * @param zip_path Output .zip file path.
     * @return true on success.
     */
    static bool compressDirectory(const std::filesystem::path& dir_path, const std::filesystem::path& zip_path);

    /**
     * @brief Decompresses a ZIP file into a directory.
     *
     * @param zip_path Input .zip file path.
     * @param dir_path Destination directory, created if missing.
     * @return true on success.
     */
    static bool decompressFile(const std::filesystem::path& zip_path, const std::filesystem::path& dir_path);
};

V_IO_NS_END

#include <vine/io/ZipArchive.hpp>

#include <array>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>

#include <zip.h>

V_IO_NS_BEGIN

namespace
{

/**
 * @brief Converts a filesystem path to the UTF-8 byte form libzip expects.
 */
std::string toUtf8(const std::filesystem::path& path)
{
    const std::u8string u8 = path.u8string();
    return std::string(u8.begin(), u8.end());
}

/**
 * @brief Converts a libzip UTF-8 entry name to a vine::String.
 */
String fromEntryName(const char* name, std::size_t length)
{
    return String(reinterpret_cast<const char8_t*>(name), static_cast<String::size_type>(length));
}

/**
 * @brief Returns true when an entry name cannot escape the extraction root.
 */
bool isSafeEntry(const std::string& name)
{
    return !name.empty() && name.front() != '/' && name.find("..") == std::string::npos;
}

/**
 * @brief Reads a whole stream into a byte buffer.
 */
std::vector<unsigned char> readStream(std::istream& in)
{
    return { std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };
}

} // namespace

ZipArchive::ZipArchive() = default;
ZipArchive::~ZipArchive() = default;

bool ZipArchive::addFile(const String& name, const void* data, std::size_t size)
{
    if (name.empty()) {
        return false;
    }
    if (data == nullptr) {
        if (size > 0) {
            return false;
        }
        entries_.push_back({ name, {} });
        return true;
    }
    const auto* bytes = static_cast<const unsigned char*>(data);
    entries_.push_back({ name, std::vector<unsigned char>(bytes, bytes + size) });
    return true;
}

bool ZipArchive::addFile(const String& name, const std::vector<unsigned char>& data)
{
    return addFile(name, data.data(), data.size());
}

bool ZipArchive::addDirectory(const std::filesystem::path& dir_path)
{
    std::error_code ec;
    if (!std::filesystem::is_directory(dir_path, ec) || ec) {
        return false;
    }
    std::filesystem::recursive_directory_iterator it(dir_path, ec);
    if (ec) {
        return false;
    }
    bool ok = true;
    const std::filesystem::recursive_directory_iterator end;
    for (; it != end; it.increment(ec)) {
        if (ec) {
            ok = false;
            break;
        }
        const std::filesystem::path relative = std::filesystem::relative(it->path(), dir_path, ec);
        if (ec) {
            ok = false;
            break;
        }
        std::string name = relative.generic_string();
        if (it->is_directory(ec)) {
            if (ec) {
                ok = false;
                break;
            }
            name.push_back('/');
            entries_.push_back({ fromEntryName(name.c_str(), name.size()), {} });
            continue;
        }
        if (!it->is_regular_file(ec) || ec) {
            ok = false;
            break;
        }
        std::ifstream in(it->path(), std::ios::binary);
        if (!in) {
            ok = false;
            break;
        }
        entries_.push_back({ fromEntryName(name.c_str(), name.size()), readStream(in) });
    }
    return ok;
}

bool ZipArchive::save(const std::filesystem::path& path)
{
    const std::string path_utf8 = toUtf8(path);
    int               error     = 0;
    zip_t* const      archive   = zip_open(path_utf8.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);
    if (archive == nullptr) {
        return false;
    }
    bool ok = true;
    for (const Entry& entry : entries_) {
        const auto*     name   = reinterpret_cast<const char*>(entry.name.data());
        zip_source_t* const source = zip_source_buffer(archive, entry.data.data(), entry.data.size(), 0);
        if (source == nullptr || zip_file_add(archive, name, source, ZIP_FL_ENC_UTF_8) < 0) {
            zip_source_free(source);
            ok = false;
            break;
        }
    }
    if (zip_close(archive) < 0) {
        ok = false;
    }
    return ok;
}

std::vector<String> ZipArchive::entryNames(const std::filesystem::path& path)
{
    std::vector<String> names;
    const std::string   path_utf8 = toUtf8(path);
    int                 error     = 0;
    zip_t* const        archive   = zip_open(path_utf8.c_str(), ZIP_RDONLY, &error);
    if (archive == nullptr) {
        return names;
    }
    const zip_int64_t count = zip_get_num_entries(archive, 0);
    for (zip_int64_t i = 0; i < count; ++i) {
        struct zip_stat st;
        zip_stat_init(&st);
        if (zip_stat_index(archive, i, 0, &st) == 0 && st.name != nullptr) {
            names.push_back(fromEntryName(st.name, std::strlen(st.name)));
        }
    }
    zip_close(archive);
    return names;
}

bool ZipArchive::decompressFile(const std::filesystem::path& path, const std::filesystem::path& dir_path)
{
    const std::string path_utf8 = toUtf8(path);
    int               error     = 0;
    zip_t* const      archive   = zip_open(path_utf8.c_str(), ZIP_RDONLY, &error);
    if (archive == nullptr) {
        return false;
    }
    std::error_code ec;
    if (!std::filesystem::create_directories(dir_path, ec) && ec) {
        zip_close(archive);
        return false;
    }
    bool ok = true;
    const zip_int64_t count = zip_get_num_entries(archive, 0);
    for (zip_int64_t i = 0; i < count && ok; ++i) {
        struct zip_stat st;
        zip_stat_init(&st);
        if (zip_stat_index(archive, i, 0, &st) != 0 || st.name == nullptr) {
            ok = false;
            break;
        }
        const std::string name = st.name;
        if (!isSafeEntry(name)) {
            ok = false;
            break;
        }
        const std::filesystem::path target =
            dir_path / std::filesystem::path(std::u8string_view(reinterpret_cast<const char8_t*>(st.name)));
        if (name.back() == '/') {
            if (!std::filesystem::create_directories(target, ec) && ec) {
                ok = false;
                break;
            }
            continue;
        }
        zip_file_t* const file = zip_fopen_index(archive, i, 0);
        if (file == nullptr) {
            ok = false;
            break;
        }
        std::ofstream out(target, std::ios::binary);
        if (!out) {
            zip_fclose(file);
            ok = false;
            break;
        }
        std::array<char, 65536> buffer;
        zip_uint64_t            total = 0;
        while (total < st.size) {
            const zip_int64_t n = zip_fread(file, buffer.data(), buffer.size());
            if (n <= 0) {
                break;
            }
            out.write(buffer.data(), n);
            total += static_cast<zip_uint64_t>(n);
        }
        zip_fclose(file);
        if (total != st.size) {
            ok = false;
            break;
        }
    }
    zip_close(archive);
    return ok;
}

bool ZipArchive::readEntry(const std::filesystem::path& path, const String& name, std::vector<unsigned char>& out)
{
    const std::string path_utf8 = toUtf8(path);
    const auto*       name_utf8 = reinterpret_cast<const char*>(name.data());
    int               error     = 0;
    zip_t* const      archive   = zip_open(path_utf8.c_str(), ZIP_RDONLY, &error);
    if (archive == nullptr) {
        return false;
    }
    const zip_int64_t index = zip_name_locate(archive, name_utf8, ZIP_FL_ENC_UTF_8);
    if (index < 0) {
        zip_close(archive);
        return false;
    }
    struct zip_stat st;
    zip_stat_init(&st);
    if (zip_stat_index(archive, index, 0, &st) != 0) {
        zip_close(archive);
        return false;
    }
    zip_file_t* const file = zip_fopen_index(archive, index, 0);
    if (file == nullptr) {
        zip_close(archive);
        return false;
    }
    out.clear();
    out.resize(static_cast<std::size_t>(st.size));
    zip_uint64_t total = 0;
    while (total < st.size) {
        const zip_int64_t n = zip_fread(file, out.data() + total, st.size - total);
        if (n <= 0) {
            break;
        }
        total += static_cast<zip_uint64_t>(n);
    }
    zip_fclose(file);
    zip_close(archive);
    if (total != st.size) {
        out.clear();
        return false;
    }
    return true;
}

V_IO_NS_END

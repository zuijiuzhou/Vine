#include "ConsoleUserIO.hpp"

#include <iostream>
#include <string>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(ConsoleUserIO, UserIO)

namespace
{

String toVineString(const std::string& s)
{
    return String(reinterpret_cast<const char8_t*>(s.data()), s.size());
}

} // namespace

ConsoleUserIO::ConsoleUserIO() = default;

void ConsoleUserIO::putString(const String& str)
{
    std::cout << str.stdstr() << std::endl;
}

void ConsoleUserIO::clear()
{
    // ANSI: clear the screen and move the cursor home.
    std::cout << "\033[2J\033[1;1H" << std::flush;
}

vine::async::Task<std::optional<String>> ConsoleUserIO::getStringAsync(const String& prompt)
{
    if (!prompt.empty())
    {
        putString(prompt);
    }

    std::string line;
    std::getline(std::cin, line);
    if (std::cin.eof())
    {
        co_return std::nullopt;
    }
    co_return toVineString(line);
}

vine::async::Task<std::optional<int8_t>> ConsoleUserIO::getIntAsync(const String& prompt)
{
    if (!prompt.empty())
    {
        putString(prompt);
    }

    std::string line;
    std::getline(std::cin, line);
    if (std::cin.eof())
    {
        co_return std::nullopt;
    }

    bool      ok = false;
    const int v  = toVineString(line).trimmed().toInt(&ok);
    if (!ok)
    {
        co_return std::nullopt;
    }
    co_return static_cast<int8_t>(v);
}

vine::async::Task<std::optional<double>> ConsoleUserIO::getDoubleAsync(const String& prompt)
{
    if (!prompt.empty())
    {
        putString(prompt);
    }

    std::string line;
    std::getline(std::cin, line);
    if (std::cin.eof())
    {
        co_return std::nullopt;
    }

    bool   ok = false;
    double v  = toVineString(line).trimmed().toDouble(&ok);
    if (!ok)
    {
        co_return std::nullopt;
    }
    co_return v;
}

vine::async::Task<std::optional<math::Point3d>> ConsoleUserIO::getPoint3dAsync(const String& prompt)
{
    if (!prompt.empty())
    {
        putString(prompt);
    }

    std::string line;
    std::getline(std::cin, line);
    if (std::cin.eof())
    {
        co_return std::nullopt;
    }

    const auto parts = toVineString(line).split(u8',');
    if (parts.size() == 3)
    {
        bool   xOk = false;
        bool   yOk = false;
        bool   zOk = false;
        double x   = parts[0].trimmed().toDouble(&xOk);
        double y   = parts[1].trimmed().toDouble(&yOk);
        double z   = parts[2].trimmed().toDouble(&zOk);
        if (xOk && yOk && zOk)
        {
            math::Point3d p;
            p.x = x;
            p.y = y;
            p.z = z;
            co_return p;
        }
    }
    co_return std::nullopt;
}

V_APPFW_NS_END

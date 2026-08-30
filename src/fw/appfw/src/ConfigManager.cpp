#include <vine/appfw/ConfigManager.hpp>

#include <QByteArray>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>
#include <QStringList>

#include <map>
#include <type_traits>
#include <variant>
#include <vector>

V_APPFW_NS_BEGIN

namespace
{

using ConfigValue =
    std::variant<std::monostate, String, bool, int64_t, double, std::vector<String>, std::vector<bool>, std::vector<int64_t>, std::vector<double>>;

QString toQString(const String& s)
{
    auto u16 = s.toUtf16();
    return QString::fromStdU16String(u16);
}

String fromQString(const QString& qs)
{
    return String::fromUtf16((const char16_t*)qs.utf16(), qs.size());
}

QByteArray toQByteArray(const String& s)
{
    return QByteArray(reinterpret_cast<const char*>(s.data()), static_cast<int>(s.size()));
}

// Inserts a leaf entry into the nested object layer by layer along the path
// (existing containers are kept; an object overrides on a level conflict).
QJsonObject insertNested(QJsonObject obj, const QStringList& path, const QJsonObject& entry)
{
    const QString head = path.first();
    if (path.size() == 1) {
        obj.insert(head, entry);
        return obj;
    }
    QJsonObject child = obj.value(head).toObject();
    child             = insertNested(child, path.mid(1), entry);
    obj.insert(head, child);
    return obj;
}

// Recursively flattens nested objects: leaf nodes (typed entries with exactly
// a type/value key pair) are written to dotted keys.
void flattenJson(const QJsonObject& obj, const QString& prefix, std::map<String, ConfigValue>& out)
{
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        const QString    name = it.key();
        const QString    full = prefix.isEmpty() ? name : prefix + QStringLiteral(".") + name;
        const QJsonValue val  = it.value();
        if (!val.isObject())
            continue; // plain value not of this format; skip
        const QJsonObject o = val.toObject();
        if (o.contains(QStringLiteral("type")) && o.size() == 2) {
            // leaf entry: parse the type
            const QString    type  = o.value(QStringLiteral("type")).toString();
            const QJsonValue value = o.value(QStringLiteral("value"));
            const String     key   = fromQString(full);
            if (type == QStringLiteral("string")) {
                out[key] = fromQString(value.toString());
            }
            else if (type == QStringLiteral("bool")) {
                out[key] = value.toBool();
            }
            else if (type == QStringLiteral("int")) {
                out[key] = static_cast<int64_t>(value.toDouble());
            }
            else if (type == QStringLiteral("double")) {
                out[key] = value.toDouble();
            }
            else if (type == QStringLiteral("string[]")) {
                std::vector<String> arr;
                const QJsonArray    a = value.toArray();
                arr.reserve(static_cast<size_t>(a.size()));
                for (const QJsonValue& v : a) arr.push_back(fromQString(v.toString()));
                out[key] = std::move(arr);
            }
            else if (type == QStringLiteral("bool[]")) {
                std::vector<bool> arr;
                const QJsonArray  a = value.toArray();
                for (const QJsonValue& v : a) arr.push_back(v.toBool());
                out[key] = std::move(arr);
            }
            else if (type == QStringLiteral("int[]")) {
                std::vector<int64_t> arr;
                const QJsonArray     a = value.toArray();
                arr.reserve(static_cast<size_t>(a.size()));
                for (const QJsonValue& v : a) arr.push_back(static_cast<int64_t>(v.toDouble()));
                out[key] = std::move(arr);
            }
            else if (type == QStringLiteral("double[]")) {
                std::vector<double> arr;
                const QJsonArray    a = value.toArray();
                arr.reserve(static_cast<size_t>(a.size()));
                for (const QJsonValue& v : a) arr.push_back(v.toDouble());
                out[key] = std::move(arr);
            }
            // unknown type: skip this key
        }
        else {
            // nested container: recurse
            flattenJson(o, full, out);
        }
    }
}

} // namespace

V_OBJECT_META_IMPL(ConfigChangedEventArgs, EventArgs)

ConfigChangedEventArgs::ConfigChangedEventArgs(const String& key)
  : key_(key)
{}

const String& ConfigChangedEventArgs::key() const
{
    return key_;
}

struct ConfigManager::Impl {
    std::map<String, ConfigValue> values;
};

ConfigManager::ConfigManager()
  : d(new Impl)
{}

ConfigManager::~ConfigManager()
{
    delete d;
}

bool ConfigManager::contains(const String& key) const
{
    return d->values.find(key) != d->values.end();
}

void ConfigManager::remove(const String& key)
{
    auto it = d->values.find(key);
    if (it == d->values.end())
        return;
    d->values.erase(it);
    ConfigChangedEventArgs args(key);
    changed.trigger(*this, args);
}

void ConfigManager::clear()
{
    if (d->values.empty())
        return;
    d->values.clear();
    auto args = ConfigChangedEventArgs(String());
    changed.trigger(*this, args);
}

void ConfigManager::setString(const String& key, const String& value)
{
    d->values[key] = value;
    ConfigChangedEventArgs args(key);
    changed.trigger(*this, args);
}

String ConfigManager::getString(const String& key, const String& def) const
{
    auto it = d->values.find(key);
    if (it == d->values.end())
        return def;
    const auto* v = std::get_if<String>(&it->second);
    return v ? *v : def;
}

void ConfigManager::setBool(const String& key, bool value)
{
    d->values[key] = value;
    ConfigChangedEventArgs args(key);
    changed.trigger(*this, args);
}

bool ConfigManager::getBool(const String& key, bool def) const
{
    auto it = d->values.find(key);
    if (it == d->values.end())
        return def;
    const auto* v = std::get_if<bool>(&it->second);
    return v ? *v : def;
}

void ConfigManager::setInt(const String& key, int value)
{
    d->values[key] = static_cast<int64_t>(value);
    ConfigChangedEventArgs args(key);
    changed.trigger(*this, args);
}

int ConfigManager::getInt(const String& key, int def) const
{
    auto it = d->values.find(key);
    if (it == d->values.end())
        return def;
    const auto* v = std::get_if<int64_t>(&it->second);
    return v ? static_cast<int>(*v) : def;
}

void ConfigManager::setDouble(const String& key, double value)
{
    d->values[key] = value;
    ConfigChangedEventArgs args(key);
    changed.trigger(*this, args);
}

double ConfigManager::getDouble(const String& key, double def) const
{
    auto it = d->values.find(key);
    if (it == d->values.end())
        return def;
    const auto* v = std::get_if<double>(&it->second);
    return v ? *v : def;
}

void ConfigManager::setStringArray(const String& key, const std::vector<String>& values)
{
    d->values[key] = values;
    ConfigChangedEventArgs args(key);
    changed.trigger(*this, args);
}

std::vector<String> ConfigManager::getStringArray(const String& key) const
{
    std::vector<String> out;
    auto                it = d->values.find(key);
    if (it == d->values.end())
        return out;
    const auto* v = std::get_if<std::vector<String>>(&it->second);
    return v ? *v : out;
}

void ConfigManager::setBoolArray(const String& key, const std::vector<bool>& values)
{
    d->values[key] = values;
    ConfigChangedEventArgs args(key);
    changed.trigger(*this, args);
}

std::vector<bool> ConfigManager::getBoolArray(const String& key) const
{
    std::vector<bool> out;
    auto              it = d->values.find(key);
    if (it == d->values.end())
        return out;
    const auto* v = std::get_if<std::vector<bool>>(&it->second);
    return v ? *v : out;
}

void ConfigManager::setIntArray(const String& key, const std::vector<int>& values)
{
    std::vector<int64_t> v;
    v.reserve(values.size());
    for (int x : values) v.push_back(x);
    d->values[key] = std::move(v);
    ConfigChangedEventArgs args(key);
    changed.trigger(*this, args);
}

std::vector<int> ConfigManager::getIntArray(const String& key) const
{
    std::vector<int> out;
    auto             it = d->values.find(key);
    if (it == d->values.end())
        return out;
    const auto* v = std::get_if<std::vector<int64_t>>(&it->second);
    if (!v)
        return out;
    out.reserve(v->size());
    for (int64_t x : *v) out.push_back(static_cast<int>(x));
    return out;
}

void ConfigManager::setDoubleArray(const String& key, const std::vector<double>& values)
{
    d->values[key] = values;
    ConfigChangedEventArgs args(key);
    changed.trigger(*this, args);
}

std::vector<double> ConfigManager::getDoubleArray(const String& key) const
{
    std::vector<double> out;
    auto                it = d->values.find(key);
    if (it == d->values.end())
        return out;
    const auto* v = std::get_if<std::vector<double>>(&it->second);
    return v ? *v : out;
}

String ConfigManager::toJson() const
{
    QJsonObject obj;
    for (const auto& [key, value] : d->values) {
        QJsonObject entry;
        std::visit(
            [&entry](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, String>) {
                    entry[QStringLiteral("type")]  = QStringLiteral("string");
                    entry[QStringLiteral("value")] = toQString(v);
                }
                else if constexpr (std::is_same_v<T, bool>) {
                    entry[QStringLiteral("type")]  = QStringLiteral("bool");
                    entry[QStringLiteral("value")] = v;
                }
                else if constexpr (std::is_same_v<T, int64_t>) {
                    entry[QStringLiteral("type")]  = QStringLiteral("int");
                    entry[QStringLiteral("value")] = static_cast<double>(v);
                }
                else if constexpr (std::is_same_v<T, double>) {
                    entry[QStringLiteral("type")]  = QStringLiteral("double");
                    entry[QStringLiteral("value")] = v;
                }
                else if constexpr (std::is_same_v<T, std::vector<String>>) {
                    entry[QStringLiteral("type")] = QStringLiteral("string[]");
                    QJsonArray arr;
                    for (const auto& s : v) arr.append(toQString(s));
                    entry[QStringLiteral("value")] = arr;
                }
                else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                    entry[QStringLiteral("type")] = QStringLiteral("bool[]");
                    QJsonArray arr;
                    for (bool b : v) arr.append(b);
                    entry[QStringLiteral("value")] = arr;
                }
                else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    entry[QStringLiteral("type")] = QStringLiteral("int[]");
                    QJsonArray arr;
                    for (int64_t i : v) arr.append(static_cast<double>(i));
                    entry[QStringLiteral("value")] = arr;
                }
                else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    entry[QStringLiteral("type")] = QStringLiteral("double[]");
                    QJsonArray arr;
                    for (double d : v) arr.append(d);
                    entry[QStringLiteral("value")] = arr;
                }
                // monostate: emit nothing
            },
            value);
        if (entry.isEmpty())
            continue;
        // expand dotted key into nested levels
        obj = insertNested(obj, toQString(key).split(QStringLiteral(".")), entry);
    }
    QJsonDocument doc(obj);
    return fromQString(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

bool ConfigManager::loadJson(const String& json)
{
    QJsonParseError err;
    QJsonDocument   doc = QJsonDocument::fromJson(toQByteArray(json), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject())
        return false;

    // replace the existing config (recursively expanding nested objects into dotted keys)
    d->values.clear();
    flattenJson(doc.object(), QString(), d->values);
    return true;
}

bool ConfigManager::save(const String& path) const
{
    QFile file(toQString(path));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    file.write(toQByteArray(toJson()));
    return true;
}

bool ConfigManager::load(const String& path)
{
    QFile file(toQString(path));
    if (!file.open(QIODevice::ReadOnly))
        return false;
    const QByteArray data = file.readAll();
    file.close();
    return loadJson(fromQString(QString::fromUtf8(data)));
}

V_APPFW_NS_END

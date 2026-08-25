#include <vine/appfw/gui/Icon.hpp>

#include <QIcon>

V_APPFWGUI_NS_BEGIN

struct Icon::Data {
    QIcon qicon;
};

Icon::Icon()
    : d(new Data())
{
}

Icon::Icon(const QIcon& qicon)
    : d(new Data())
{
    d->qicon = qicon;
}

Icon::Icon(const String& path)
    : d(new Data())
{
    auto utf16 = path.toUtf16();

    d->qicon = QIcon(QString::fromStdU16String(utf16));
}

Icon::Icon(const Icon& other)
    : d(new Data())
{
    d->qicon = other.d->qicon;
}

Icon& Icon::operator=(const Icon& other)
{
    if (this != &other) { d->qicon = other.d->qicon; }

    return *this;
}

Icon::~Icon()
{
    delete d;
}

const QIcon& Icon::value() const
{
    return d->qicon;
}

bool Icon::isNull() const
{
    return d ? d->qicon.isNull() : true;
}

Icon::operator QIcon() const
{
    return d->qicon;
}

V_APPFWGUI_NS_END

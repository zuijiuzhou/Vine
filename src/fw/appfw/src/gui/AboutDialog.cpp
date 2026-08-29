#include <vine/appfw/gui/AboutDialog.hpp>

#include <QDialog>
#include <QFont>
#include <QLabel>
#include <QVBoxLayout>

#include <vine/appfw/gui/UIElementData.hpp>

V_APPFWGUI_NS_BEGIN

namespace
{

QString toQString(const String& s)
{
    return QString::fromStdU16String(s.toUtf16());
}

} // namespace

V_OBJECT_META_IMPL(AboutDialog, Window)

struct AboutDialog::Data : public UIElementData {
    String app_name;
    String version;
    String description;
    String copyright;

    QLabel* name_label        = nullptr;
    QLabel* version_label     = nullptr;
    QLabel* description_label = nullptr;
    QLabel* copyright_label   = nullptr;
};

AboutDialog::AboutDialog()
  : Window(new Data(), new QDialog())
{
    auto* root = impl<QDialog>();
    root->setWindowTitle(QStringLiteral("关于"));

    auto* lay  = new QVBoxLayout(root);
    auto* data = dptr();

    data->name_label = new QLabel(root);
    QFont name_font  = data->name_label->font();
    name_font.setPointSize(name_font.pointSize() + 6);
    name_font.setBold(true);
    data->name_label->setFont(name_font);
    lay->addWidget(data->name_label);

    data->version_label = new QLabel(root);
    lay->addWidget(data->version_label);

    data->description_label = new QLabel(root);
    data->description_label->setWordWrap(true);
    lay->addWidget(data->description_label);

    data->copyright_label = new QLabel(root);
    lay->addWidget(data->copyright_label);

    lay->addStretch();

    applyContent();
}

AboutDialog::~AboutDialog()
{
    // d is released by UIElement
}

void AboutDialog::setAppName(const String& name)
{
    dptr()->app_name = name;
    applyContent();
}

String AboutDialog::appName() const
{
    return dptr()->app_name;
}

void AboutDialog::setVersion(const String& version)
{
    dptr()->version = version;
    applyContent();
}

String AboutDialog::version() const
{
    return dptr()->version;
}

void AboutDialog::setDescription(const String& description)
{
    dptr()->description = description;
    applyContent();
}

String AboutDialog::description() const
{
    return dptr()->description;
}

void AboutDialog::setCopyright(const String& copyright)
{
    dptr()->copyright = copyright;
    applyContent();
}

String AboutDialog::copyright() const
{
    return dptr()->copyright;
}

void AboutDialog::applyContent()
{
    auto* data = dptr();
    if (data->name_label) {
        data->name_label->setText(toQString(data->app_name));
    }
    if (data->version_label) {
        data->version_label->setText(toQString(data->version));
    }
    if (data->description_label) {
        data->description_label->setText(toQString(data->description));
    }
    if (data->copyright_label) {
        data->copyright_label->setText(toQString(data->copyright));
    }
}

inline auto AboutDialog::dptr() -> Data*
{
    return static_cast<Data*>(UIElement::d);
}

inline auto AboutDialog::dptr() const -> const Data*
{
    return static_cast<const Data*>(UIElement::d);
}

V_APPFWGUI_NS_END

#ifndef SA_RIBBON_H
#define SA_RIBBON_H
//定义此宏，将SA_RIBBON_EXPORT定义为空
#ifndef SA_RIBBON_BAR_NO_EXPORT
#define SA_RIBBON_BAR_NO_EXPORT
#endif
//定义此宏，将SA_COLOR_WIDGETS_API定义为空
#ifndef SA_COLOR_WIDGETS_NO_DLL
#define SA_COLOR_WIDGETS_NO_DLL
#endif

/*** Start of inlined file: SARibbonAmalgamTemplatePublicHeaders.h ***/
// Global

/*** Start of inlined file: SAColorWidgetsGlobal.h ***/
#ifndef SACOLORWIDGETSGLOBAL_H
#define SACOLORWIDGETSGLOBAL_H
#include <QScopedPointer>

/**
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 */
#ifndef SA_COLOR_WIDGETS_VERSION_MAJ
#define SA_COLOR_WIDGETS_VERSION_MAJ 0
#endif
/**
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 */
#ifndef SA_COLOR_WIDGETS_VERSION_MIN
#define SA_COLOR_WIDGETS_VERSION_MIN 1
#endif
/**
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 */
#ifndef SA_COLOR_WIDGETS_VERSION_PAT
#define SA_COLOR_WIDGETS_VERSION_PAT 0
#endif

/**
 * @def   模仿Q_DECLARE_PRIVATE，但不用前置声明而是作为一个内部类
 */
#ifndef SA_COLOR_WIDGETS_DECLARE_PRIVATE
#define SA_COLOR_WIDGETS_DECLARE_PRIVATE(classname)                                                                    \
    class PrivateData;                                                                                                 \
    friend class classname::PrivateData;                                                                               \
    QScopedPointer< PrivateData > d_ptr;
#endif
/**
 * @def   模仿Q_DECLARE_PUBLIC
 */
#ifndef SA_COLOR_WIDGETS_DECLARE_PUBLIC
#define SA_COLOR_WIDGETS_DECLARE_PUBLIC(classname)                                                                     \
    friend class classname;                                                                                            \
    classname* q_ptr { nullptr };
#endif

#ifndef SA_COLOR_WIDGETS_NO_DLL
#if defined(SA_COLOR_WIDGETS_MAKE_LIB)  // 定义此宏将构建library
#ifndef SA_COLOR_WIDGETS_API
#define SA_COLOR_WIDGETS_API Q_DECL_EXPORT
#endif
#else
#ifndef SA_COLOR_WIDGETS_API
#define SA_COLOR_WIDGETS_API Q_DECL_IMPORT
#endif
#endif
#else
#ifndef SA_COLOR_WIDGETS_API
#define SA_COLOR_WIDGETS_API
#endif
#endif

#endif  // SACOLORWIDGETSGLOBAL_H

/*** End of inlined file: SAColorWidgetsGlobal.h ***/

/*** Start of inlined file: SARibbonGlobal.h ***/
#ifndef SARIBBONGLOBAL_H
#define SARIBBONGLOBAL_H
#include <qglobal.h>
#include <memory>

//! 版本记录：
//!
//! 2023-05-28 -> 0.5.0
//! 调整了大按钮模式下的显示方案，去除了原来SARibbonToolButton的Lite和Normal模式，以WordWrap来表征
//! 支持文字自定义换行
//! 调整了RibbonPannel的标题栏的高度计算方案
//!
//! 0.5.1
//! 不使用QString::simplified,而是简单的仅仅替换\n的simplified，这样中文换行不会多出空格
//!
//! 0.5.2
//! SARibbonColorToolButton\SARibbonToolButton修正&操作在三项表达式未加括号问题
//! SARibbonStyleOption添加虚析构函数
//! 原来SARibbonElementCreateDelegate类改名为SARibbonElementFactory
//!

/**
 * @def ribbon的数字版本 {MAJ}.MIN.PAT
 */
#ifndef SA_RIBBON_BAR_VERSION_MAJ
#define SA_RIBBON_BAR_VERSION_MAJ 0
#endif
/**
 * @def ribbon的数字版本 MAJ.{MIN}.PAT
 */
#ifndef SA_RIBBON_BAR_VERSION_MIN
#define SA_RIBBON_BAR_VERSION_MIN 6
#endif
/**
 * @def ribbon的数字版本 MAJ.MIN.{PAT}
 */
#ifndef SA_RIBBON_BAR_VERSION_PAT
#define SA_RIBBON_BAR_VERSION_PAT 0
#endif

/**
 * @def 属性，用于标记是否可以进行自定义，用于动态设置到@ref SARibbonCategory 和@ref SARibbonPannel
 * 值为bool，在为true时，可以通过@ref SARibbonCustomizeWidget 改变这个SARibbonCategory和SARibbonPannel的布局，
 * 默认不会有此属性，仅在有此属性且为true时才会在SARibbonCustomizeWidget中能显示为可设置
 */
#ifndef SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE
#define SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE "_sa_isCanCustomize"
#endif

#ifndef SA_RIBBON_BAR_NO_EXPORT
#if defined(SA_RIBBON_BAR_MAKE_LIB)  // 定义此宏将构建library
#define SA_RIBBON_EXPORT Q_DECL_EXPORT
#else
#define SA_RIBBON_EXPORT Q_DECL_IMPORT
#endif
#endif

#ifndef SA_RIBBON_EXPORT
#define SA_RIBBON_EXPORT
#endif

/**
 * @def   模仿Q_DECLARE_PRIVATE，但不用前置声明而是作为一个内部类
 */
#ifndef SA_RIBBON_DECLARE_PRIVATE
#define SA_RIBBON_DECLARE_PRIVATE(classname)                                                                           \
    class PrivateData;                                                                                                 \
    friend class classname::PrivateData;                                                                               \
    std::unique_ptr< PrivateData > d_ptr;
#endif
/**
 * @def   模仿Q_DECLARE_PUBLIC
 */
#ifndef SA_RIBBON_DECLARE_PUBLIC
#define SA_RIBBON_DECLARE_PUBLIC(classname)                                                                            \
    friend class classname;                                                                                            \
    classname* q_ptr { nullptr };
#endif

#endif  // SARIBBONGLOBAL_H

/*** End of inlined file: SARibbonGlobal.h ***/

// color widget

/*** Start of inlined file: SAColorMenu.h ***/
#ifndef SACOLORMENU_H
#define SACOLORMENU_H
#include <QMenu>

class QWidgetAction;
class SAColorPaletteGridWidget;
class SAColorGridWidget;
class SAColorToolButton;
/**
 * @brief 标准颜色菜单
 */
class SA_COLOR_WIDGETS_API SAColorMenu : public QMenu
{
    Q_OBJECT
    SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorMenu)
public:
    explicit SAColorMenu(QWidget* parent = nullptr);
    explicit SAColorMenu(const QString& title, QWidget* parent = nullptr);
    ~SAColorMenu();
    //快速绑定colorbtn
    void bindToColorToolButton(SAColorToolButton* btn);
    // ThemeColorsPalette对应的action
    QWidgetAction* getThemeColorsPaletteAction() const;
    // CustomColorsWidget对应的action
    QWidgetAction* getCustomColorsWidgetAction() const;
    // 自定义颜色action
    QAction* getCustomColorAction() const;
    //获取ThemeColorsPalette
    SAColorPaletteGridWidget* getColorPaletteGridWidget() const;
    //获取自定义颜色grid
    SAColorGridWidget* getCustomColorsWidget() const;
    //构建无颜色action，默认无颜色action是没有的
    void enableNoneColorAction(bool on = true);
    //获取None Color Action,注意，enableNoneColorAction(true),之后才不是nullptr
    QAction* getNoneColorAction() const;
public slots:
    //这是一个辅助槽函数，为了让用户自定义的其他action也能关联menu，可以调用此槽函数，实现selectedColor信号以及menu的隐藏
    void emitSelectedColor(const QColor& c);
signals:
    /**
     * @brief 选择了颜色触发的信号
     * @param c
     */
    void selectedColor(const QColor& c);
private slots:
    void onCustomColorActionTriggered(bool on);
    void onNoneColorActionTriggered(bool on);

private:
    void init(const QList< QColor >& themeCls);
};

#endif  // SACOLORMENU_H

/*** End of inlined file: SAColorMenu.h ***/

/*** Start of inlined file: SAColorGridWidget.h ***/
#ifndef SACOLORGRIDWIDGET_H
#define SACOLORGRIDWIDGET_H
#include <QWidget>
#include <functional>

class QAbstractButton;
class SAColorToolButton;
/**
 * @brief 一个grid的Color布局
 *
 * 一个形如下面的颜色grid：
 *
 * □□□□□□□□□
 *
 * □□□□□□□□□
 *
 * □□□□□□□□□
 */
class SA_COLOR_WIDGETS_API SAColorGridWidget : public QWidget
{
    Q_OBJECT
    SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorGridWidget)
    Q_PROPERTY(int spacing READ getSpacing WRITE setSpacing)
public:
    using FunColorBtn = std::function< void(SAColorToolButton*) >;

public:
    SAColorGridWidget(QWidget* par = nullptr);
    ~SAColorGridWidget();
    //设置列数，行数量会根据列数量来匹配,如果设置-1或者0，说明不限定列数量，这样会只有一行
    void setColumnCount(int c);
    int getColumnCount() const;
    //设置当前的颜色列表
    void setColorList(const QList< QColor >& cls);
    QList< QColor > getColorList() const;
    //间隔
    int getSpacing() const;
    void setSpacing(int v);
    //获取颜色的数量
    int getColorCount() const;
    //图标的尺寸
    void setColorIconSize(const QSize& s);
    QSize getColorIconSize() const;
    //设置颜色是否是checkable
    void setColorCheckable(bool on = true);
    bool isColorCheckable() const;
    //获取当前选中的颜色
    QColor getCurrentCheckedColor() const;
    //获取index对于的colorbutton
    SAColorToolButton* getColorButton(int index) const;
    //垂直间距
    void setVerticalSpacing(int v);
    int getVerticalSpacing() const;
    //水平间距
    void setHorizontalSpacing(int v);
    int getHorizontalSpacing() const;
    //清除当前选中状态，这时没有颜色是选中的
    void clearCheckedState();
    //对所有的colorbtn就行遍历处理，可以通过此函数进行tooltip设置等操作
    void iterationColorBtns(FunColorBtn fn);
    //设置行最小高度
    void setRowMinimumHeight(int row, int minSize);
    //让颜色块左对齐
    void setHorizontalSpacerToRight(bool on = true);
private slots:
    void onButtonClicked(QAbstractButton* btn);
    void onButtonPressed(QAbstractButton* btn);
    void onButtonReleased(QAbstractButton* btn);
    void onButtonToggled(QAbstractButton* btn, bool on);

signals:
    /**
     * @brief 对于check模式，check的颜色触发的信号
     * @param c
     * @param on
     */
    void colorClicked(const QColor& c);
    void colorPressed(const QColor& c);
    void colorReleased(const QColor& c);
    void colorToggled(const QColor& c, bool on);

public:
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
};
namespace SA
{
/**
 * @brief 获取标准色列表（一共10种颜色）
 * @return
 */
SA_COLOR_WIDGETS_API QList< QColor > getStandardColorList();
}
#endif  // SACOLORGRIDWIDGET_H

/*** End of inlined file: SAColorGridWidget.h ***/

/*** Start of inlined file: SAColorPaletteGridWidget.h ***/
#ifndef SACOLORPALETTEGRIDWIDGET_H
#define SACOLORPALETTEGRIDWIDGET_H
#include <QWidget>

class QMenu;
class SAColorToolButton;
/**
 * @brief 类似office的颜色选择窗口，有一排标准色，下面有一个颜色板，有3行浅色，有2行深色
 */
class SA_COLOR_WIDGETS_API SAColorPaletteGridWidget : public QWidget
{
    Q_OBJECT
    SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorPaletteGridWidget)
public:
    SAColorPaletteGridWidget(QWidget* par = nullptr);
    SAColorPaletteGridWidget(const QList< QColor >& cls, QWidget* par = nullptr);
    ~SAColorPaletteGridWidget();
    //设置窗口维护的colorList
    void setColorList(const QList< QColor >& cls);
    QList< QColor > getColorList() const;
    //设置颜色深浅比例factor，默认为{ 180, 160, 140, 75, 50 }
    void setFactor(const QList< int >& factor);
    QList< int > getFactor() const;
    //设置iconsize
    void setColorIconSize(const QSize& s);
    QSize getColorIconSize() const;
    //设置颜色是否是checkable
    void setColorCheckable(bool on = true);
    bool isColorCheckable() const;
private slots:
    void onMainColorClicked(const QColor& c);
    void onPaletteColorClicked(const QColor& c);

private:
    void init();
signals:
    /**
     * @brief 对于check模式，check的颜色触发的信号
     * @param c
     * @param on
     */
    void colorClicked(const QColor& c);
};

#endif  // SACOLORPALETTEGRIDWIDGET_H

/*** End of inlined file: SAColorPaletteGridWidget.h ***/

/*** Start of inlined file: SAColorToolButton.h ***/
#ifndef SACOLORTOOLBUTTON_H
#define SACOLORTOOLBUTTON_H
#include <QToolButton>

class QPaintEvent;
class QResizeEvent;
class QPainter;
class QStylePainter;
/**
 * @brief 这是一个只显示颜色的toolbutton
 *
 *
 * 在ToolButtonIconOnly模式下，如果没有setIcon,则颜色占用所有区域，如下图所示
 *
 * ┌─────┐
 * │color│
 * └─────┘
 *
 * 如果在ToolButtonIconOnly模式下有图标，图标在上面显示，下面显示颜色，如下图所示
 *
 * ┌─────┐
 * │icon │
 * │color│
 * └─────┘
 *
 * 在ToolButtonTextBesideIcon，ToolButtonTextUnderIcon下，setIconSize 可以指定颜色的大小，
 * 但只在ToolButtonTextBesideIcon，ToolButtonTextUnderIcon下有效
 *
 * 如果没有设置图标，也就是setIcon(QIcon()),iconSize作为颜色块的大小
 *
 * ┌─────────┐
 * │┌─┐      │
 * │└─┘      │
 * └─────────┘
 *
 * 如果有图标，颜色条会在图标下方，为图标高度的1/4 为图标宽度一致，如若超过控件的大小，会自动缩小体积
 *
 */
class SA_COLOR_WIDGETS_API SAColorToolButton : public QToolButton
{
    Q_OBJECT
    SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorToolButton)
public:
    explicit SAColorToolButton(QWidget* parent = nullptr);
    ~SAColorToolButton();
    //获取颜色
    QColor getColor() const;
    //设置Margins
    void setMargins(const QMargins& mg);
    QMargins getMargins() const;
    //绘制无颜色
    static void paintNoneColor(QPainter* p, const QRect& colorRect);
public slots:
    //设置颜色,会发射colorChanged信号
    void setColor(const QColor& c);

protected:
    //获取关键的三个rect位置
    virtual void calcRect(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    virtual void paintButton(QStylePainter* p, const QStyleOptionToolButton& opt);
    virtual void paintIcon(QStylePainter* p, const QRect& iconRect, const QStyleOptionToolButton& opt);
    virtual void paintText(QStylePainter* p, const QRect& textRect, const QStyleOptionToolButton& opt);
    virtual void paintColor(QStylePainter* p, const QRect& colorRect, const QColor& color, const QStyleOptionToolButton& opt);

protected:
    virtual void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
private slots:
    void onButtonClicked(bool checked = false);
signals:
    /**
     * @brief 颜色被点击的响应
     * @param color
     */
    void colorClicked(const QColor& color, bool checked = false);
    /**
     * @brief 颜色改变信号
     * @param color
     */
    void colorChanged(const QColor& color);
};

#endif  // SACOLORTOOLBUTTON_H

/*** End of inlined file: SAColorToolButton.h ***/

// sa ribbon

/*** Start of inlined file: SAFramelessHelper.h ***/
#ifndef SAFRAMELESSHELPER_H
#define SAFRAMELESSHELPER_H

#include <QObject>

class QWidget;

class SA_RIBBON_EXPORT SAFramelessHelper : public QObject
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SAFramelessHelper)
    friend class SAPrivateFramelessWidgetData;

public:
    explicit SAFramelessHelper(QObject* parent);
    ~SAFramelessHelper();
    // 激活窗体
    void activateOn(QWidget* topLevelWidget);

    // 移除窗体
    void removeFrom(QWidget* topLevelWidget);

    // 设置窗体移动
    void setWidgetMovable(bool movable);

    // 设置窗体缩放
    void setWidgetResizable(bool resizable);

    // 设置橡皮筋移动
    void setRubberBandOnMove(bool movable);

    // 设置橡皮筋缩放
    void setRubberBandOnResize(bool resizable);

    // 设置边框的宽度
    void setBorderWidth(int width);

    // 设置标题栏高度
    void setTitleHeight(int height);
    bool widgetResizable();
    bool widgetMovable();
    bool rubberBandOnMove();
    bool rubberBandOnResisze();
    uint borderWidth();
    uint titleHeight();

protected:
    // 事件过滤，进行移动、缩放等
    virtual bool eventFilter(QObject* obj, QEvent* event);
};

#endif  // FRAMELESSHELPER_H

/*** End of inlined file: SAFramelessHelper.h ***/

/*** Start of inlined file: SARibbonApplicationButton.h ***/
#ifndef SARIBBONAPPLICATIONBUTTON_H
#define SARIBBONAPPLICATIONBUTTON_H
#include <QToolButton>

/**
 * @brief The SARibbonApplicationButton class
 *
 * 默认的plicationButton,可以通过样式指定不一样的ApplicationButton
 */
class SA_RIBBON_EXPORT SARibbonApplicationButton : public QToolButton
{
    Q_OBJECT
public:
    SARibbonApplicationButton(QWidget* parent = nullptr);
    SARibbonApplicationButton(const QString& text, QWidget* parent = nullptr);
    SARibbonApplicationButton(const QIcon& icon, const QString& text, QWidget* parent = nullptr);
};

#endif  // SARIBBONAPPLICATIONBUTTON_H

/*** End of inlined file: SARibbonApplicationButton.h ***/

/*** Start of inlined file: SAWindowButtonGroup.h ***/
#ifndef SAWINDOWBUTTONGROUP_H
#define SAWINDOWBUTTONGROUP_H

#include <QWidget>
#include <QPushButton>

///
/// \brief 窗口的最大最小化按钮
///
class SA_RIBBON_EXPORT SAWindowButtonGroup : public QWidget
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SAWindowButtonGroup)
public:
    SAWindowButtonGroup(QWidget* parent);
    SAWindowButtonGroup(QWidget* parent, Qt::WindowFlags flags);
    ~SAWindowButtonGroup();
    void setupMinimizeButton(bool on);
    void setupMaximizeButton(bool on);
    void setupCloseButton(bool on);
    void updateWindowFlag();
    void updateWindowFlag(Qt::WindowFlags flags);

    //设置按钮的宽度比例,最终按钮宽度将按照此比例进行设置
    void setButtonWidthStretch(int close = 4, int max = 3, int min = 3);

    //设置按钮的缩放比例
    void setIconScale(qreal iconscale = 0.5);

    //设置Qt::WindowStates
    void setWindowStates(Qt::WindowStates s);

    //仅获取按钮的状态
    Qt::WindowFlags windowButtonFlags() const;

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    virtual bool eventFilter(QObject* watched, QEvent* e) Q_DECL_OVERRIDE;
    virtual void parentResize();
    virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;

protected slots:
    Q_SLOT void closeWindow();
    Q_SLOT void minimizeWindow();
    Q_SLOT void maximizeWindow();
};

/**
 * @brief The SAWindowToolButton class
 */
class SAWindowToolButton : public QPushButton
{
    Q_OBJECT
public:
    SAWindowToolButton(QWidget* p = nullptr);
};
#endif  // SAWINDOWBUTTONGROUP_H

/*** End of inlined file: SAWindowButtonGroup.h ***/

/*** Start of inlined file: SARibbonToolButton.h ***/
#ifndef SARIBBONTOOLBUTTON_H
#define SARIBBONTOOLBUTTON_H

#include <QToolButton>
#include <QDebug>
/**
 * @brief Ribbon界面适用的toolButton
 *
 * 相对于普通toolbutton，主要多了两个类型设置，@ref setButtonType 和 @ref setLargeButtonType
 *
 * @note @sa setIconSize 函数不在起作用，iconsize是根据当前尺寸动态调整的
 */
class SA_RIBBON_EXPORT SARibbonToolButton : public QToolButton
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonToolButton)
public:
    /**
     * @brief 按钮样式
     */
    enum RibbonButtonType
    {
        LargeButton,
        SmallButton
    };
    Q_ENUM(RibbonButtonType)

public:
    SARibbonToolButton(QWidget* parent = Q_NULLPTR);
    SARibbonToolButton(QAction* defaultAction, QWidget* parent = Q_NULLPTR);
    ~SARibbonToolButton();
    //标记按钮的样式，按钮的样式有不同的渲染方式
    RibbonButtonType buttonType() const;
    void setButtonType(const RibbonButtonType& buttonType);
    //是否是小按钮
    bool isSmallRibbonButton() const;
    //是否是大按钮
    bool isLargeRibbonButton() const;
    //最小尺寸提示
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    //获取间距
    int spacing() const;
    //更新尺寸
    void updateRect();

public:
    //在lite模式下是否允许文字换行
    static void setEnableWordWrap(bool on);
    static bool isEnableWordWrap();

protected:
    virtual void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    virtual void focusOutEvent(QFocusEvent* e) Q_DECL_OVERRIDE;
    virtual void leaveEvent(QEvent* e) Q_DECL_OVERRIDE;
    virtual bool hitButton(const QPoint& pos) const Q_DECL_OVERRIDE;
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    virtual bool event(QEvent* e) Q_DECL_OVERRIDE;
    //事件改变 - 主要为了捕获字体的改变
    virtual void changeEvent(QEvent* e) Q_DECL_OVERRIDE;
    virtual void actionEvent(QActionEvent* e) Q_DECL_OVERRIDE;

protected:
    //绘制按钮
    virtual void paintButton(QPainter& p, const QStyleOptionToolButton& opt);
    //绘制图标
    virtual void paintIcon(QPainter& p, const QStyleOptionToolButton& opt, const QRect& iconDrawRect);
    //绘制文本
    virtual void paintText(QPainter& p, const QStyleOptionToolButton& opt, const QRect& textDrawRect);
    //绘制Indicator
    virtual void paintIndicator(QPainter& p, const QStyleOptionToolButton& opt, const QRect& indicatorDrawRect);

private:
    static void drawArrow(const QStyle* style,
                          const QStyleOptionToolButton* toolbutton,
                          const QRect& rect,
                          QPainter* painter,
                          const QWidget* widget = 0);

protected:
};

namespace SA
{
QDebug operator<<(QDebug debug, const QStyleOptionToolButton& opt);
}
#endif  // SARIBBONTOOLBUTTON_H

/*** End of inlined file: SARibbonToolButton.h ***/

/*** Start of inlined file: SARibbonColorToolButton.h ***/
#ifndef SARIBBONCOLORTOOLBUTTON_H
#define SARIBBONCOLORTOOLBUTTON_H

class SAColorMenu;
/**
 * @brief Refer to the color setting button in the office, which can display the color below the icon(参考office的颜色设置按钮，可以显示颜色在图标下方)
 */
class SA_RIBBON_EXPORT SARibbonColorToolButton : public SARibbonToolButton
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonColorToolButton)
public:
    /**
     * @brief 颜色样式
     */
    enum ColorStyle
    {
        ColorUnderIcon,  ///< 颜色在icon下方，这个要求必须设置icon
        ColorFillToIcon  ///< 颜色作为icon，这个模式下在setColor会自动生成一个颜色icon替换掉原来的icon，因此setIcon函数没有作用
    };

public:
    SARibbonColorToolButton(QWidget* parent = Q_NULLPTR);
    SARibbonColorToolButton(QAction* defaultAction, QWidget* parent = Q_NULLPTR);
    ~SARibbonColorToolButton();
    //获取颜色
    QColor getColor() const;
    //设置颜色显示方案
    void setColorStyle(ColorStyle s);
    ColorStyle colorStyle() const;
    //建立标准的颜色菜单
    SAColorMenu* setupStandardColorMenu();
public slots:
    //设置颜色,会发射colorChanged信号
    void setColor(const QColor& c);
private slots:
    void onButtonClicked(bool checked = false);
signals:
    /**
     * @brief 颜色被点击的响应
     * @param color
     */
    void colorClicked(const QColor& color, bool checked = false);
    /**
     * @brief 颜色改变信号
     * @param color
     */
    void colorChanged(const QColor& color);

protected:
    void paintIcon(QPainter& p, const QStyleOptionToolButton& opt, const QRect& iconDrawRect);
};

#endif  // SARIBBONCOLORTOOLBUTTON_H

/*** End of inlined file: SARibbonColorToolButton.h ***/

/*** Start of inlined file: SARibbonLineWidgetContainer.h ***/
#ifndef SARIBBONLINEWIDGETCONTAINER_H
#define SARIBBONLINEWIDGETCONTAINER_H

#include <QtCore/qglobal.h>
#include <QWidget>
#include <QLabel>

/**
 * @brief 一个窗口容器，把窗口放置中间，前面后面都可以设置文本，主要用于放置在pannel上的小窗口
 *
 * 实现如下效果：
 *
 * PrefixLabel|_Widget_|SuffixLabel
 *
 */
class SA_RIBBON_EXPORT SARibbonLineWidgetContainer : public QWidget
{
public:
    SARibbonLineWidgetContainer(QWidget* par = nullptr);

    //设置widget,不允许设置一个nullptr
    void setWidget(QWidget* innerWidget);

    //设置前缀
    void setPrefix(const QString& str);

    //设置后缀
    void setSuffix(const QString& str);

    //前缀文本框
    QLabel* labelPrefix() const;

    //后缀文本框
    QLabel* labelSuffix() const;

private:
    //两个文本
    QLabel* m_labelPrefix;
    QLabel* m_labelSuffix;
    QWidget* m_innerWidget;
};

#endif  // SARIBBONWIDGETCONTAINER_H

/*** End of inlined file: SARibbonLineWidgetContainer.h ***/

/*** Start of inlined file: SARibbonActionsManager.h ***/
#ifndef SARIBBONACTIONSMANAGER_H
#define SARIBBONACTIONSMANAGER_H

#include <QObject>
#include <QAbstractListModel>
#include <QAction>
#include <QMap>
#include <QString>
#include <QSet>
class SARibbonMainWindow;
class SARibbonCategory;

/**
 * @brief 用于管理SARibbon的所有Action
 *
 * SARibbonActionsManager维护着两个表，一个是tag（标签）对应的Action list，
 * 一个是所有接受SARibbonActionsManager管理的action list。
 *
 * SARibbonActionsManager的标签对应一组actions，每个标签对应的action可以重复出现，
 * 但SARibbonActionsManager维护的action list里只有一份action，不会重复出现。
 *
 * tag用于对action list分组，每个tag的实体名字通过@ref setTagName 进行设置，在语言变化时需要及时调用
 * setTagName设置新的标签对应的文本。
 *
 * SARibbonActionsManager默认预设了6个常用标签见@ref SARibbonActionsManager::ActionTag ，用户自定义标签需要在
 * SARibbonActionsManager::UserDefineActionTag值的基础上进行累加。
 *
 * @ref filter （等同@ref actions ）函数用于提取标签管理的action list，@ref allActions 函数返回SARibbonActionsManager
 * 管理的所有标签。
 *
 * 通过@ref autoRegisteActions 函数可以快速的建立action的管理，此函数会遍历@ref SARibbonMainWindow 下的所有子object，
 * 同时遍历SARibbonMainWindow下所有@ref SARibbonPannel 添加的action,并给予Category建立tag，正常使用用户仅需关注此autoRegisteActions函数即可
 *
 *
 */
class SA_RIBBON_EXPORT SARibbonActionsManager : public QObject
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonActionsManager)
    friend class SARibbonActionsManagerModel;

public:
    /**
     * @brief 定义action的标签
     */
    enum ActionTag
    {
        UnknowActionTag              = 0,     ///< 未知的tag
        CommonlyUsedActionTag        = 0x01,  ///< 预设tag-常用命令
        NotInFunctionalAreaActionTag = 0x02,  ///< 预设tag-不在功能区命令
        AutoCategoryDistinguishBeginTag = 0x1000,  ///< 自动按Category划分的标签起始，在@ref autoRegisteActions 函数会用到
        AutoCategoryDistinguishEndTag = 0x2000,  ///< 自动按Category划分的标签结束，在@ref autoRegisteActions 函数会用到
        NotInRibbonCategoryTag = 0x2001,  ///< 不在功能区的标签@ref autoRegisteActions 函数会遍历所有category的action以及SARibbonMainWindow下的action，如果两个
        UserDefineActionTag    = 0x8000   ///< 自定义标签，所有用户自定义tag要大于此tag
    };
    SARibbonActionsManager(SARibbonMainWindow* p);
    ~SARibbonActionsManager();
    //设置tag对应的名字
    void setTagName(int tag, const QString& name);

    //获取tag对应的名字
    QString tagName(int tag) const;

    //移除tag，注意，这个函数非常耗时
    void removeTag(int tag);

    //注册action
    bool registeAction(QAction* act, int tag, const QString& key = QString(), bool enableEmit = true);

    //取消action的注册
    void unregisteAction(QAction* act, bool enableEmit = true);

    //过滤得到actions对应的引用，实际是一个迭代器
    QList< QAction* >& filter(int tag);

    //通过tag筛选出系列action
    QList< QAction* >& actions(int tag);
    const QList< QAction* > actions(int tag) const;

    //获取所有的标签
    QList< int > actionTags() const;

    //通过key获取action
    QAction* action(const QString& key) const;

    //通过action找到key
    QString key(QAction* act) const;

    //返回所有管理的action数
    int count() const;

    //返回所有管理的actions
    QList< QAction* > allActions() const;

    //自动加载action,返回tag对应的Category指针
    QMap< int, SARibbonCategory* > autoRegisteActions(SARibbonMainWindow* w);

    //自动加载widget下的actions函数返回的action,返回加载的数量，这些
    QSet< QAction* > autoRegisteWidgetActions(QWidget* w, int tag, bool enableEmit = false);

    //根据标题查找action
    QList< QAction* > search(const QString& text);

    //清除
    void clear();

    //获取ribbonwindow
    SARibbonMainWindow* ribbonWindow() const;

signals:

    /**
     * @brief 标签变化触发的信号，变化包括新增和删除
     */
    void actionTagChanged(int tag, bool isdelete);

private slots:
    void onActionDestroyed(QObject* o);
    void onCategoryTitleChanged(const QString& title);

private:
    void removeAction(QAction* act, bool enableEmit = true);
};

/**
 * @brief SARibbonActionsManager 对应的model
 */
class SA_RIBBON_EXPORT SARibbonActionsManagerModel : public QAbstractListModel
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonActionsManagerModel)
public:
    explicit SARibbonActionsManagerModel(QObject* p = nullptr);
    explicit SARibbonActionsManagerModel(SARibbonActionsManager* m, QObject* p = nullptr);
    ~SARibbonActionsManagerModel();
    virtual int rowCount(const QModelIndex& parent) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    void setFilter(int tag);
    void update();
    void setupActionsManager(SARibbonActionsManager* m);
    void uninstallActionsManager();
    QAction* indexToAction(QModelIndex index) const;
    void search(const QString& text);

private slots:
    void onActionTagChanged(int tag, bool isdelete);
};

#endif  // SARIBBONACTIONSMANAGER_H

/*** End of inlined file: SARibbonActionsManager.h ***/

/*** Start of inlined file: SARibbonDrawHelper.h ***/
#ifndef SARIBBONDRAWHELPER_H
#define SARIBBONDRAWHELPER_H
#include <QIcon>
#include <QStylePainter>
#include <QStyleOption>
#include <QPixmap>

/**
 * @def QFontMetrics::horizontalAdvance(str)/QFontMetrics::width(str) 为了兼容5.11以下的qt版本，定义的兼容宏
 */
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
#ifndef SA_FONTMETRICS_WIDTH
#define SA_FONTMETRICS_WIDTH(fm, str) fm.horizontalAdvance(str)
#endif
#else
#ifndef SA_FONTMETRICS_WIDTH
#define SA_FONTMETRICS_WIDTH(fm, str) fm.width(str)
#endif
#endif

///
/// \brief 绘图辅助
///
class SA_RIBBON_EXPORT SARibbonDrawHelper
{
public:
    SARibbonDrawHelper();
    static QPixmap iconToPixmap(const QIcon& icon, QWidget* widget, const QStyleOption* opt, const QSize& icoSize);
    static void drawIcon(const QIcon& icon, QPainter* painter, const QStyleOption* opt, int x, int y, int width, int height);
    static void drawIcon(const QIcon& icon, QPainter* painter, const QStyleOption* opt, const QRect& rect);
    static QSize iconActualSize(const QIcon& icon, const QStyleOption* opt, const QSize& iconSize);

    static void drawText(const QString& text, QStylePainter* painter, const QStyleOption* opt, Qt::Alignment al, int x, int y, int width, int height);
    static void drawText(const QString& text, QStylePainter* painter, const QStyleOption* opt, Qt::Alignment al, const QRect& rect);
};

#endif  // SARIBBONDRAWHELPER_H

/*** End of inlined file: SARibbonDrawHelper.h ***/

/*** Start of inlined file: SARibbonLineEdit.h ***/
#ifndef SARIBBONLINEEDIT_H
#define SARIBBONLINEEDIT_H

#include <QLineEdit>

/**
 * @brief The SARibbonLineEdit class
 */
class SA_RIBBON_EXPORT SARibbonLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    SARibbonLineEdit(QWidget* parent = Q_NULLPTR);
};

#endif  // SARIBBONLINEEDIT_H

/*** End of inlined file: SARibbonLineEdit.h ***/

/*** Start of inlined file: SARibbonCheckBox.h ***/
#ifndef SARIBBONCHECKBOX_H
#define SARIBBONCHECKBOX_H

#include <QCheckBox>

/**
 * @brief The SARibbonCheckBox class
 */
class SA_RIBBON_EXPORT SARibbonCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    SARibbonCheckBox(QWidget* parent = Q_NULLPTR);
};

#endif  // SARIBBONCHECKBOX_H

/*** End of inlined file: SARibbonCheckBox.h ***/

/*** Start of inlined file: SARibbonComboBox.h ***/
#ifndef SARIBBONCOMBOBOX_H
#define SARIBBONCOMBOBOX_H

#include <QComboBox>

/**
 * @brief QComboBox的Ribbon显示，可以显示QIcon和windowTitle在左侧
 */
class SA_RIBBON_EXPORT SARibbonComboBox : public QComboBox
{
    Q_OBJECT
public:
    SARibbonComboBox(QWidget* parent = Q_NULLPTR);
};

#endif  // SARIBBONCOMBOBOX_H

/*** End of inlined file: SARibbonComboBox.h ***/

/*** Start of inlined file: SARibbonButtonGroupWidget.h ***/
#ifndef SARIBBONBUTTONGROUPWIDGET_H
#define SARIBBONBUTTONGROUPWIDGET_H

#include <QToolButton>
#include <QMenu>
#include <QFrame>

/**
 * @brief 用于管理一组Action,类似于QToolBar
 */
class SA_RIBBON_EXPORT SARibbonButtonGroupWidget : public QFrame
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonButtonGroupWidget)
public:
    SARibbonButtonGroupWidget(QWidget* parent = Q_NULLPTR);
    ~SARibbonButtonGroupWidget() Q_DECL_OVERRIDE;

    //生成并添加一个action
    QAction* addAction(QAction* a);
    QAction* addAction(const QString& text, const QIcon& icon, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    QAction* addMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    QAction* addSeparator();
    QAction* addWidget(QWidget* w);
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

signals:

    /**
     * @brief 参考QToolBar::actionTriggered的信号
     * @param action
     */
    void actionTriggered(QAction* action);

protected:
    virtual void actionEvent(QActionEvent* e) Q_DECL_OVERRIDE;
};

#endif  // SARIBBONBUTTONGROUPWIDGET_H

/*** End of inlined file: SARibbonButtonGroupWidget.h ***/

/*** Start of inlined file: SARibbonStackedWidget.h ***/
#ifndef SARIBBONSTACKEDWIDGET_H
#define SARIBBONSTACKEDWIDGET_H
#include <QStackedWidget>

class QHideEvent;
class QResizeEvent;

/**
 * @brief 有qdialog功能的stackwidget，用于在最小化时stack能像dialog那样弹出来
 */
class SA_RIBBON_EXPORT SARibbonStackedWidget : public QStackedWidget
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonStackedWidget)
public:
    SARibbonStackedWidget(QWidget* parent);
    ~SARibbonStackedWidget();
    void setPopupMode();
    bool isPopupMode() const;
    void setNormalMode();
    bool isNormalMode() const;
    void exec();

    //设置stacked管理的窗口会随着stacked的大小变化而变化大小
    void setAutoResize(bool autoresize);
    bool isAutoResize() const;
    void moveWidget(int from, int to);

protected:
    //    void mouseReleaseEvent(QMouseEvent *e);
    void hideEvent(QHideEvent* e) override;

signals:
    /**
     * @brief hidWindow
     */
    void hidWindow();
};

#endif  // SARIBBONSTACKEDWIDGET_H

/*** End of inlined file: SARibbonStackedWidget.h ***/

/*** Start of inlined file: SARibbonSeparatorWidget.h ***/
#ifndef SARIBBONSEPARATORWIDGET_H
#define SARIBBONSEPARATORWIDGET_H

#include <QWidget>
#include <QStyleOption>

///
/// \brief 用于显示分割线
///
class SA_RIBBON_EXPORT SARibbonSeparatorWidget : public QWidget
{
    Q_OBJECT
public:
    SARibbonSeparatorWidget(int height, QWidget* parent = nullptr);
    SARibbonSeparatorWidget(QWidget* parent = nullptr);
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    //设置分割线的上下距离
    void setTopBottomMargins(int top, int bottom);

protected:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

private:
    int m_topMargins;     ///< 顶部间隔,默认4
    int m_bottomMargins;  ///< 底部间隔,默认4
};

#endif  // SARIBBONSEPARATORWIDGET_H

/*** End of inlined file: SARibbonSeparatorWidget.h ***/

/*** Start of inlined file: SARibbonCtrlContainer.h ***/
#ifndef SARIBBONCTROLCONTAINER_H
#define SARIBBONCTROLCONTAINER_H

#include <QWidget>
#include <QScopedPointer>
class QStyleOption;

/**
 * @brief 用于装载一个窗体的容器，这个窗体会布满整个SARibbonCtrlContainer，但会预留空间显示icon或者title
 *
 * ----------------------
 * |icon|text|  widget  |
 * ----------------------
 */
class SA_RIBBON_EXPORT SARibbonCtrlContainer : public QWidget
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonCtrlContainer)
public:
    SARibbonCtrlContainer(QWidget* parent = Q_NULLPTR);
    ~SARibbonCtrlContainer();

    void setEnableShowIcon(bool b);
    void setEnableShowTitle(bool b);
    //判断是否存在容器窗口
    bool hasContainerWidget() const;
    //图标
    void setIcon(const QIcon& i);
    QIcon getIcon() const;
    //图标
    void setText(const QString& t);
    QString getText() const;
    //设置窗口
    void setContainerWidget(QWidget* w);
    QWidget* getContainerWidget();
    const QWidget* getContainerWidget() const;
};

#endif  // SARIBBONCTROLCONTAINER_H

/*** End of inlined file: SARibbonCtrlContainer.h ***/

/*** Start of inlined file: SARibbonQuickAccessBar.h ***/
#ifndef SARIBBONQUICKACCESSBAR_H
#define SARIBBONQUICKACCESSBAR_H

#include <QMenu>
#include <QToolButton>
class SARibbonToolButton;

///
/// \brief ribbon左上顶部的快速响应栏
///
class SA_RIBBON_EXPORT SARibbonQuickAccessBar : public SARibbonCtrlContainer
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonQuickAccessBar)
public:
    SARibbonQuickAccessBar(QWidget* parent = 0);
    ~SARibbonQuickAccessBar();
    void addSeparator();
    void addAction(QAction* act);
    void addWidget(QWidget* w);
    void addMenu(QMenu* m, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
};

#endif  // SARIBBONQUICKACCESSBAR_H

/*** End of inlined file: SARibbonQuickAccessBar.h ***/

/*** Start of inlined file: SARibbonTabBar.h ***/
#ifndef SARIBBONTABBAR_H
#define SARIBBONTABBAR_H

#include <QTabBar>
#include <QMargins>

/**
 * @brief The SARibbonTabBar class
 */
class SA_RIBBON_EXPORT SARibbonTabBar : public QTabBar
{
    Q_OBJECT
public:
    SARibbonTabBar(QWidget* parent = Q_NULLPTR);
    const QMargins& tabMargin() const;
    void setTabMargin(const QMargins& tabMargin);

private:
    QMargins m_tabMargin;
};

#endif  // SARIBBONTABBAR_H

/*** End of inlined file: SARibbonTabBar.h ***/

/*** Start of inlined file: SARibbonControlButton.h ***/
#ifndef SARIBBONCONTROLBUTTON_H
#define SARIBBONCONTROLBUTTON_H
#include <QToolButton>

/**
 * @brief 用于SARibbonPannel的control button
 *
 * 为了防止外部qss的影响，单独继承一个类
 */
class SA_RIBBON_EXPORT SARibbonControlButton : public QToolButton
{
    Q_OBJECT
public:
    SARibbonControlButton(QWidget* parent = 0);
};

#endif  // SARIBBONPANNELTOOLBUTTON_H

/*** End of inlined file: SARibbonControlButton.h ***/

/*** Start of inlined file: SARibbonMenu.h ***/
#ifndef SARIBBONMENU_H
#define SARIBBONMENU_H

#include <QMenu>

///
/// \brief 用在ribbon的menu
/// 可以通过StyleSheet设置样式而不影响QMenu
///
class SA_RIBBON_EXPORT SARibbonMenu : public QMenu
{
    Q_OBJECT
public:
    explicit SARibbonMenu(QWidget* parent = Q_NULLPTR);
    explicit SARibbonMenu(const QString& title, QWidget* parent = Q_NULLPTR);
    QAction* addRibbonMenu(SARibbonMenu* menu);
    SARibbonMenu* addRibbonMenu(const QString& title);
    SARibbonMenu* addRibbonMenu(const QIcon& icon, const QString& title);
    QAction* addWidget(QWidget* w);
};

#endif  // SARIBBONMENU_H

/*** End of inlined file: SARibbonMenu.h ***/

/*** Start of inlined file: SARibbonPannelOptionButton.h ***/
#ifndef SARIBBONPANNELOPTIONBUTTON_H
#define SARIBBONPANNELOPTIONBUTTON_H
#include <QToolButton>

class QAction;

/**
 * @brief Pannel右下角的操作按钮
 *
 * 此按钮和一个action关联，使用@ref SARibbonPannel::addOptionAction 函数用于生成此按钮，正常来说
 * 用户并不需要直接操作此类，仅仅用于样式设计
 * 如果一定要重载此按钮，可以通过重载@ref SARibbonElementCreateDelegate
 * 的 @ref SARibbonElementCreateDelegate::createRibbonPannelOptionButton来实现新的OptionButton
 *
 */
class SA_RIBBON_EXPORT SARibbonPannelOptionButton : public QToolButton
{
    Q_OBJECT
public:
    SARibbonPannelOptionButton(QWidget* parent = Q_NULLPTR);
    //有别于setDefaultAction 此函数只关联action的响应，而不设置text，icon等
    void connectAction(QAction* action);
};

#endif  // SAROBBONPANNELOPTIONBUTTON_H

/*** End of inlined file: SARibbonPannelOptionButton.h ***/

/*** Start of inlined file: SARibbonPannelItem.h ***/
#ifndef SARIBBONPANNELITEM_H
#define SARIBBONPANNELITEM_H

#include <QWidgetItem>
#include <QAction>
#include <functional>
class SARibbonToolButton;
/**
 * @brief 是对pannel所有子窗口的抽象，参考qt的toolbar
 *
 * 参考qt的toolbar，pannel所有子窗口内容都通过QAction进行抽象，包括gallery这些窗口，也是通过QAction进行抽象
 * QAction最终会转换为SARibbonPannelItem，每个SARibbonPannelItem都含有一个widget，SARibbonPannel的布局
 * 就基于SARibbonPannelItem
 *
 * 无窗口的action会在内部生成一个SARibbonToolButton，
 */
class SA_RIBBON_EXPORT SARibbonPannelItem : public QWidgetItem
{
public:
    /**
     * @brief 定义了行的占比，ribbon中有large，media和small三种占比
     */
    enum RowProportion
    {
        None,  ///< 为定义占比，这时候将会依据expandingDirections来判断，如果能有Qt::Vertical，就等同于Large，否则就是Small
        Large,   ///< 大占比，一个widget的高度会充满整个pannel
        Medium,  ///< 中占比，在@ref SARibbonPannel::pannelLayoutMode 为 @ref SARibbonPannel::ThreeRowMode 时才会起作用，且要同一列里两个都是Medium时，会在三行中占据两行
        Small  ///< 小占比，占SARibbonPannel的一行，Medium在不满足条件时也会变为Small，但不会变为Large
    };
    SARibbonPannelItem(QWidget* widget);
    bool isEmpty() const Q_DECL_OVERRIDE;

    short rowIndex;             ///< 记录当前item属于第几行，hide模式下为-1
    int columnIndex;            ///< 记录当前item属于第几列，hide模式下为-1
    QRect itemWillSetGeometry;  ///< 在调用SARibbonPannelLayout::updateGeomArray会更新这个此处，实际设置的时候会QWidgetItem::setGeometry设置Geometry
    QAction* action;            /// < 记录action，参考QToolBarLayoutItem
    bool customWidget;  ///< 对于没有窗口的action，实际也会有一个SARibbonToolButton，在销毁时要delete掉
    SARibbonPannelItem::RowProportion rowProportion;  ///< 行的占比，ribbon中有large，media和small三种占比,见@ref RowProportion
};
#ifndef SARibbonPannelItemRowProportionPropertyName
#define SARibbonPannelItemRowProportionPropertyName "SARibbonPannelItem_RowProportion"
#endif
#endif  // SARIBBONPANNELITEM_H

/*** End of inlined file: SARibbonPannelItem.h ***/

/*** Start of inlined file: SARibbonPannelLayout.h ***/
#ifndef SARIBBONPANNELLAYOUT_H
#define SARIBBONPANNELLAYOUT_H

#include <QLayout>

class SARibbonPannel;

/**
 * @brief 针对SARibbonPannel的布局
 *
 * SARibbonPannelLayout实际是一个列布局，每一列有2~3行，看窗口定占几行
 *
 * 核心函数： @ref SARibbonPannelLayout::createItem
 *
 * @note QLayout::contentsMargins 函数不会启作用,如果要设置contentsMargins，使用@sa setPannelContentsMargins
 */
class SA_RIBBON_EXPORT SARibbonPannelLayout : public QLayout
{
    Q_OBJECT
    friend class SARibbonPannel;

public:
    SARibbonPannelLayout(QWidget* p = 0);
    ~SARibbonPannelLayout();
    virtual int indexOf(QAction* action) const;

    // SARibbonPannelLayout additem 无效
    void addItem(QLayoutItem* item) Q_DECL_OVERRIDE;

    // SARibbonPannel主要通过此函数来添加action
    void insertAction(int index, QAction* act, SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::None);

    //
    QLayoutItem* itemAt(int index) const Q_DECL_OVERRIDE;
    QLayoutItem* takeAt(int index) Q_DECL_OVERRIDE;
    int count() const Q_DECL_OVERRIDE;

    bool isEmpty() const Q_DECL_OVERRIDE;
    void invalidate() Q_DECL_OVERRIDE;
    Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;

    void setGeometry(const QRect& rect) Q_DECL_OVERRIDE;
    QSize minimumSize() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    //通过action获取SARibbonPannelItem
    SARibbonPannelItem* pannelItem(QAction* action) const;

    //获取最后一个添加的item
    SARibbonPannelItem* lastItem() const;

    //获取最后生成的窗口
    QWidget* lastWidget() const;

    //移动两个item
    void move(int from, int to);

    //判断是否需要重新布局
    bool isDirty() const;

    //计算大图标的高度
    static int calcLargeHeight(const QRect& setrect, const SARibbonPannel* pannel);

public:
    //全局的contentsMargins
    static const QMargins& pannelContentsMargins();
    static void setPannelContentsMargins(const QMargins& m);

protected:
    //布局action
    void layoutActions();

    //把action转换为item，对于纯Action，此函数会创建SARibbonToolButton,
    // rp用于告诉Layout生成什么样的窗口，详细见SARibbonPannelItem::RowProportion
    SARibbonPannelItem* createItem(QAction* action, SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::None);
    void updateGeomArray(const QRect& setrect);

    //重新计算扩展条码，此函数必须在updateGeomArray函数之后调用
    void recalcExpandGeomArray(const QRect& setrect);

private:
    //返回所有列的区域
    // QMap<int,QRect> columnsGeometry() const;
    //根据列数，计算窗口的宽度，以及最大宽度
    void columnWidthInfo(int colindex, int& width, int& maximum) const;

private:
    QList< SARibbonPannelItem* > m_items;
    int m_columnCount;  ///< 记录有多少列
    bool m_expandFlag;  ///< 标记是否是会扩展的
    QSize m_sizeHint;   ///< sizeHint返回的尺寸
    bool m_dirty;       ///< 用于标记是否需要刷新元素，参考QToolBarLayout源码
    int m_largeHeight;  ///< 记录大图标的高度
    /**
     * @brief 统一的contentsMargins，此参数作为静态变量，可以进行全局设置
     */
    static QMargins s_contentsMargins;
};

#endif  // SARIBBONPANNELLAYOUT_H

/*** End of inlined file: SARibbonPannelLayout.h ***/

/*** Start of inlined file: SARibbonPannel.h ***/
#ifndef SARIBBONPANNEL_H
#define SARIBBONPANNEL_H

#include <QWidget>
#include <QLayout>

class SARibbonMenu;
class SARibbonGallery;
class QGridLayout;
class SARibbonPannelOptionButton;

/**
 * @brief pannel页窗口，pannel是ribbon的面板用于承放控件
 *
 * ribbon的pannel分为两行模式和三行模式，以office为代表的ribbon为3行模式，以WPS为代表的“紧凑派”就是2行模式，
 * SARibbon可通过SARibbonBar的@ref SARibbonBar::RibbonStyle 来指定模式(通过函数@ref SARibbonBar::setRibbonStyle )
 *
 * 在pannel中，可以通过@ref setExpanding 函数指定pannel水平扩展，如果pannel里面没有能水平扩展的控件，将会留白，因此，
 * 建议在pannel里面有水平扩展的控件如（@ref SARibbonGallery ）才指定这个函数
 *
 * pannel的布局通过@ref SARibbonPannelLayout 来实现，如果有其他布局，可以通过继承@ref
 * SARibbonElementCreateDelegate::createRibbonPannel 函数返回带有自己布局的pannel，但你必须继承对应的虚函数
 */
class SA_RIBBON_EXPORT SARibbonPannel : public QWidget
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonPannel)
    friend class SARibbonCategory;
    friend class SARibbonCategoryPrivate;
    friend class SARibbonCustomizeWidgetPrivate;
    Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
    Q_PROPERTY(bool isExpanding READ isExpanding WRITE setExpanding)
    Q_PROPERTY(QString pannelName READ pannelName WRITE setPannelName)
public:
public:
    SARibbonPannel(QWidget* parent = nullptr);
    SARibbonPannel(const QString& name, QWidget* parent = nullptr);
    ~SARibbonPannel() Q_DECL_OVERRIDE;
    using QWidget::addAction;
    enum PannelLayoutMode
    {
        ThreeRowMode,  ///< 三行布局模式，office就是三行布局模式，pannel能布置3行小toolbutton，默认模式
        TwoRowMode  ///< 两行布局模式，wps的后续布局模式就是两行布局模式，pannel能布置2行小toolbutton
    };

    //把action的行属性设置进action中，action自身携带了行属性
    static void setActionRowProportionProperty(QAction* action, SARibbonPannelItem::RowProportion rp);

    //获取action的行属性
    static SARibbonPannelItem::RowProportion getActionRowProportionProperty(QAction* action);

    //设置action的行行为，行属性决定了ribbon pannel的显示方式
    void setActionRowProportion(QAction* action, SARibbonPannelItem::RowProportion rp);

    //把action加入到pannel
    SARibbonToolButton* addAction(QAction* action, SARibbonPannelItem::RowProportion rp);

    //把action加入到pannel，并以大图标显示
    SARibbonToolButton* addLargeAction(QAction* action);

    //把action加入到pannel，在三行模式下会以中图标显示
    SARibbonToolButton* addMediumAction(QAction* action);

    //把action加入到pannel，并以小图标显示
    SARibbonToolButton* addSmallAction(QAction* action);

    //生成并添加一个action
    void addAction(QAction* act, QToolButton::ToolButtonPopupMode popMode, SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::Large);

    QAction* addAction(const QString& text,
                       const QIcon& icon,
                       QToolButton::ToolButtonPopupMode popMode,
                       SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::Large);

    //添加menu
    SARibbonToolButton* addMenu(QMenu* menu,
                                SARibbonPannelItem::RowProportion rp,
                                QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    //添加action menu
    SARibbonToolButton* addActionMenu(QAction* action, QMenu* menu, SARibbonPannelItem::RowProportion rp);

    // action menu,action menu是一个特殊的menu,即可点击触发action，也可弹出菜单
    SARibbonToolButton* addLargeActionMenu(QAction* action, QMenu* menu);

    //添加普通大菜单
    SARibbonToolButton* addLargeMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    //添加普通小按钮菜单
    SARibbonToolButton* addSmallMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    //添加窗口
    QAction* addWidget(QWidget* w, SARibbonPannelItem::RowProportion rp);

    //添加窗口,占用ribbon的一行
    QAction* addSmallWidget(QWidget* w);

    //添加窗口,占用ribbon的一行
    QAction* addMediumWidget(QWidget* w);

    //添加窗口，占用所有行
    QAction* addLargeWidget(QWidget* w);

    //添加一个Gallery
    SARibbonGallery* addGallery();

    //添加分割线
    QAction* addSeparator(int top = 6, int bottom = 6);

    //从pannel中把action对应的button提取出来，如果action没有对应的button，就返回nullptr
    SARibbonToolButton* actionToRibbonToolButton(QAction* action);

    //添加操作action，如果要去除，传入nullptr指针即可，SARibbonPannel不会对QAction的所有权进行管理
    void addOptionAction(QAction* action);

    //判断是否存在OptionAction
    bool isHaveOptionAction() const;

    //获取所有的buttons
    QList< SARibbonToolButton* > ribbonToolButtons() const;

    //获取PannelLayoutMode
    PannelLayoutMode pannelLayoutMode() const;

    //判断是否为2行模式
    bool isTwoRow() const
    {
        return (TwoRowMode == pannelLayoutMode());
    }

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    //把pannel设置为扩展模式，此时会撑大水平区域
    void setExpanding(bool isExpanding = true);

    //是否是扩展模式
    bool isExpanding() const;

    //标题栏高度，仅在3行模式下生效
    int titleHeight() const;

    // optionActionButton的尺寸
    virtual QSize optionActionButtonSize() const;

    // action对应的布局index，此操作一般用于移动moveAction，其他意义不大
    int actionIndex(QAction* act) const;

    //移动action
    void moveAction(int from, int to);

    //判断是否可以自定义
    bool isCanCustomize() const;
    void setCanCustomize(bool b);

    //标题
    QString pannelName() const;
    void setPannelName(const QString& title);

    //大图标的高度
    int largeHeight() const;
    //获取布局对应的item
    const QList< SARibbonPannelItem* >& ribbonPannelItem() const;
    //全局的标题栏高度
    static int pannelTitleHeight();
    static void setPannelTitleHeight(int h);
signals:

    /**
     * @brief 等同于QToolBar::actionTriggered
     * @param action
     */
    void actionTriggered(QAction* action);

protected:
    //设置PannelLayoutMode，此函数设置为protect避免误调用
    void setPannelLayoutMode(PannelLayoutMode mode);
    void resetLayout(PannelLayoutMode newmode);
    void resetLargeToolButtonStyle();
    static QSize maxHightIconSize(const QSize& size, int h);
    virtual void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    virtual void actionEvent(QActionEvent* e) Q_DECL_OVERRIDE;
};

#endif  // SARIBBONPANNEL_H

/*** End of inlined file: SARibbonPannel.h ***/

/*** Start of inlined file: SARibbonCategory.h ***/
#ifndef SARIBBONCATEGORY_H
#define SARIBBONCATEGORY_H

#include <QWidget>

#include <QScopedPointer>
#include <QPushButton>
#include <QWheelEvent>

class QHBoxLayout;
class QWheelEvent;
class SARibbonBar;

/**
 * @brief 一项ribbon tab页
 * @note SARibbonCategory的windowTitle影响了其在SARibbonBar的标签显示，
 * 如果要改标签名字，直接调用SARibbonCategory的setWindowTitle函数
 */
class SA_RIBBON_EXPORT SARibbonCategory : public QWidget
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonCategory)
    friend class SARibbonBar;
    friend class SARibbonContextCategory;
    Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
    Q_PROPERTY(QString categoryName READ categoryName WRITE setCategoryName)
public:
    SARibbonCategory(QWidget* p = nullptr);
    ~SARibbonCategory();

    // category的名字
    QString categoryName() const;

    //设置category名字，等同setWindowTitle
    void setCategoryName(const QString& title);

    //布局模式
    SARibbonPannel::PannelLayoutMode ribbonPannelLayoutMode() const;

    //添加pannel
    SARibbonPannel* addPannel(const QString& title);

    //添加pannel
    void addPannel(SARibbonPannel* pannel);

    // qt designer专用
    Q_INVOKABLE void addPannel(QWidget* pannel);

    //插入pannel
    SARibbonPannel* insertPannel(const QString& title, int index);

    //通过名字查找pannel
    SARibbonPannel* pannelByName(const QString& title) const;

    //通过ObjectName查找pannel
    SARibbonPannel* pannelByObjectName(const QString& objname) const;

    //通过索引找到pannel，如果超过索引范围，会返回nullptr
    SARibbonPannel* pannelByIndex(int index) const;

    //查找pannel的index
    int pannelIndex(SARibbonPannel* p) const;

    //移动一个Pannel从from index到to index
    void movePannel(int from, int to);

    //把pannel从Category中移除，不会销毁，此时pannel的所有权归还操作者
    bool takePannel(SARibbonPannel* pannel);

    //移除Pannel，Category会直接回收SARibbonPannel内存
    bool removePannel(SARibbonPannel* pannel);
    bool removePannel(int index);

    //设置背景
    void setBackgroundBrush(const QBrush& brush);

    //返回所有的Pannel
    QList< SARibbonPannel* > pannelList() const;

    //
    QSize sizeHint() const Q_DECL_OVERRIDE;

    //如果是ContextCategory，此函数返回true
    bool isContextCategory() const;

    // pannel的个数
    int pannelCount() const;

    //判断是否可以自定义
    bool isCanCustomize() const;
    void setCanCustomize(bool b);

    //获取对应的ribbonbar，如果没有加入ribbonbar的管理，此值为null
    SARibbonBar* ribbonBar() const;

    //刷新category的尺寸布局
    void updateItemGeometry();
protected slots:
    void onLeftScrollButtonClicked();
    void onRightScrollButtonClicked();

protected:
    //事件处理
    bool event(QEvent* e) Q_DECL_OVERRIDE;

    //
    void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;

    //设置pannel的模式
    void setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m);
    bool eventFilter(QObject* watched, QEvent* event) Q_DECL_OVERRIDE;

    //处理滚轮事件
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;

    //标记这个是上下文标签
    void markIsContextCategory(bool isContextCategory = true);

private:
    void setRibbonBar(SARibbonBar* bar);
};

/**
 * @brief SARibbonCategory无法完全显示时，显示的调整按钮
 *
 * 重新定义是为了防止被外部的样式影响,同时可以使用SARibbonCategoryScrollButton的样式定义
 */
class SA_RIBBON_EXPORT SARibbonCategoryScrollButton : public QToolButton
{
    Q_OBJECT
public:
    SARibbonCategoryScrollButton(Qt::ArrowType arr, QWidget* p = nullptr);
};

#endif  // SARIBBONCATEGORY_H

/*** End of inlined file: SARibbonCategory.h ***/

/*** Start of inlined file: SARibbonCategoryLayout.h ***/
#ifndef SARIBBONCATEGORYLAYOUT_H
#define SARIBBONCATEGORYLAYOUT_H

#include <QLayout>
#include <QList>
#include <QMap>

class SARibbonPannel;
class SARibbonCategoryLayoutItem;
class SARibbonSeparatorWidget;

/**
 * @brief The SARibbonCategoryLayout class
 */
class SA_RIBBON_EXPORT SARibbonCategoryLayout : public QLayout
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonCategoryLayout)
public:
    SARibbonCategoryLayout(SARibbonCategory* parent);
    ~SARibbonCategoryLayout();

    SARibbonCategory* ribbonCategory();

    virtual void addItem(QLayoutItem* item) Q_DECL_OVERRIDE;
    virtual QLayoutItem* itemAt(int index) const Q_DECL_OVERRIDE;
    virtual QLayoutItem* takeAt(int index) Q_DECL_OVERRIDE;
    SARibbonCategoryLayoutItem* takePannelItem(int index);
    SARibbonCategoryLayoutItem* takePannel(SARibbonPannel* pannel);
    virtual int count() const Q_DECL_OVERRIDE;

    //追加一个pannel
    void addPannel(SARibbonPannel* pannel);

    //插入一个pannel
    void insertPannel(int index, SARibbonPannel* pannel);

    void setGeometry(const QRect& rect) Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSize() const Q_DECL_OVERRIDE;
    Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;
    void invalidate() Q_DECL_OVERRIDE;

    //更新尺寸
    void updateGeometryArr();

    //执行位置调整
    void doLayout();

    //返回所有pannels
    QList< SARibbonPannel* > pannels() const;

private slots:
    void onLeftScrollButtonClicked();
    void onRightScrollButtonClicked();
};

/**
 * @brief SARibbonCategoryLayoutItem，用于标识SARibbonCategoryLayout的item
 */
class SA_RIBBON_EXPORT SARibbonCategoryLayoutItem : public QWidgetItem
{
public:
    SARibbonCategoryLayoutItem(SARibbonPannel* w);
    SARibbonSeparatorWidget* separatorWidget;
    QRect mWillSetGeometry;           ///< pannel将要设置的Geometry
    QRect mWillSetSeparatorGeometry;  ///< pannel将要设置的Separator的Geometry
};
#endif  // SARIBBONCATEGORYLAYOUT_H

/*** End of inlined file: SARibbonCategoryLayout.h ***/

/*** Start of inlined file: SARibbonContextCategory.h ***/
#ifndef SARIBBONCONTEXTCATEGORY_H
#define SARIBBONCONTEXTCATEGORY_H

#include <QWidget>

/**
 * @brief 管理上下文标签的类
 */
class SA_RIBBON_EXPORT SARibbonContextCategory : public QObject
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonContextCategory)
public:
    SARibbonContextCategory(QWidget* parent = 0);
    ~SARibbonContextCategory();
    //上下文目录添加下属目录
    SARibbonCategory* addCategoryPage(const QString& title);
    void addCategoryPage(SARibbonCategory* category);
    //获取上下文标签下管理的标签个数
    int categoryCount() const;

    //设置id
    void setId(const QVariant& id);
    QVariant id() const;

    //设置上下文颜色
    void setContextColor(const QColor color);
    QColor contextColor() const;

    //上下文标签的内容
    QString contextTitle() const;
    void setContextTitle(const QString& contextTitle);

    //获取对应的tab页
    SARibbonCategory* categoryPage(int index);

    //获取所有的SARibbonCategory*
    QList< SARibbonCategory* > categoryList() const;

    //移除category
    bool takeCategory(SARibbonCategory* category);

    //判断上下文是否维护了此SARibbonCategory
    bool isHaveCategory(SARibbonCategory* category) const;
signals:
    /**
     * @brief 标签加入上下文
     * @param category
     */
    void categoryPageAdded(SARibbonCategory* category);

    /**
     * @brief 标签从上下文移除
     * @param category
     */
    void categoryPageRemoved(SARibbonCategory* category);

    /**
     * @brief 上下文的标题发生改变
     * @param title
     */
    void contextTitleChanged(const QString& title);

    /**
     * @brief 上下文标签维护的标签页名字发生了改变
     * @param category 发生改变的上下文标签页
     * @param title 新名字
     */
    void categoryTitleChanged(SARibbonCategory* category, const QString& title);
private slots:
    void onCategoryTitleChanged(const QString& title);

protected:
    //获取父级窗口
    QWidget* parentWidget() const;
    virtual bool eventFilter(QObject* watched, QEvent* e) override;
};

#endif  // SARIBBONCONTEXTCATEGORY_H

/*** End of inlined file: SARibbonContextCategory.h ***/

/*** Start of inlined file: SARibbonGalleryItem.h ***/
#ifndef SARIBBONGALLERYITEM_H
#define SARIBBONGALLERYITEM_H

#include <QIcon>
#include <QVariant>
#include <QMap>
#include <QAction>
class SARibbonGalleryGroup;

///
/// \brief 类似QStandardItem的GalleryItem
///
class SA_RIBBON_EXPORT SARibbonGalleryItem
{
public:
    SARibbonGalleryItem();
    SARibbonGalleryItem(const QString& text, const QIcon& icon);
    SARibbonGalleryItem(QAction* act);
    virtual ~SARibbonGalleryItem();
    //设置角色
    void setData(int role, const QVariant& data);
    virtual QVariant data(int role) const;

    //设置文字描述
    void setText(const QString& text);
    QString text() const;

    //设置tooltip
    void setToolTip(const QString& text);
    QString toolTip() const;

    //设置图标
    void setIcon(const QIcon& ico);
    QIcon icon() const;

    //设置是否可见
    bool isSelectable() const;
    void setSelectable(bool isSelectable);

    //设置是否可选
    bool isEnable() const;
    void setEnable(bool isEnable);

    //设置item的flag
    void setFlags(Qt::ItemFlags flag);
    virtual Qt::ItemFlags flags() const;

    //设置action
    void setAction(QAction* act);
    QAction* action();

    //文字对齐方式
    void setTextAlignment(Qt::Alignment a);
    Qt::Alignment getTextAlignment() const;

private:
    friend class SARibbonGalleryGroupModel;
    QMap< int, QVariant > m_datas;
    Qt::ItemFlags m_flags;
    QAction* m_action;
};

#endif  // SARIBBONGALLERYITEM_H

/*** End of inlined file: SARibbonGalleryItem.h ***/

/*** Start of inlined file: SARibbonGalleryGroup.h ***/
#ifndef SARIBBONGALLERYGROUP_H
#define SARIBBONGALLERYGROUP_H

#include <QList>
#include <QListView>
#include <QStyledItemDelegate>

///
/// \brief SARibbonGalleryGroup对应的显示代理
///
class SA_RIBBON_EXPORT SARibbonGalleryGroupItemDelegate : public QStyledItemDelegate
{
public:
    SARibbonGalleryGroupItemDelegate(SARibbonGalleryGroup* group, QObject* parent = Q_NULLPTR);
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;

    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
    virtual void paintIconOnly(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void paintIconWithText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void paintIconWithTextWordWrap(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    SARibbonGalleryGroup* m_group;
};

///
/// \brief SARibbonGalleryGroup对应的model
///
class SA_RIBBON_EXPORT SARibbonGalleryGroupModel : public QAbstractListModel
{
    Q_OBJECT
public:
    SARibbonGalleryGroupModel(QObject* parent = Q_NULLPTR);
    ~SARibbonGalleryGroupModel();
    virtual int rowCount(const QModelIndex& parent) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent) const Q_DECL_OVERRIDE;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;
    void clear();
    SARibbonGalleryItem* at(int row) const;
    void insert(int row, SARibbonGalleryItem* item);
    SARibbonGalleryItem* take(int row);
    void append(SARibbonGalleryItem* item);

private:
    QList< SARibbonGalleryItem* > m_items;
};

/**
 * @brief Gallery的组
 *
 * 组负责显示管理Gallery Item
 */
class SA_RIBBON_EXPORT SARibbonGalleryGroup : public QListView
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonGalleryGroup)
public:
    /**
     * @brief GalleryGroup显示的样式
     */
    enum GalleryGroupStyle
    {
        IconWithText,          ///< 图标带文字
        IconWithWordWrapText,  ///< 图标带文字,文字会换行显示，此模式只会对DisplayOneRow生效，如果不是DisplayOneRow，等同IconWithText
        IconOnly               ///< 只有图标
    };

    /**
     * @brief 定义Gallery在一个pannel下面显示的图标行数
     */
    enum DisplayRow
    {
        DisplayOneRow   = 1,  ///< 显示1行，默认
        DisplayTwoRow   = 2,
        DisplayThreeRow = 3
    };

    SARibbonGalleryGroup(QWidget* w = 0);

    virtual ~SARibbonGalleryGroup();
    //重新计算grid尺寸
    void setRecalcGridSizeBlock(bool on = true);
    bool isRecalcGridSizeBlock() const;
    void recalcGridSize();
    void recalcGridSize(int galleryHeight);
    //设置显示的样式
    void setGalleryGroupStyle(GalleryGroupStyle style);
    GalleryGroupStyle getGalleryGroupStyle() const;
    //添加一个item
    void addItem(const QString& text, const QIcon& icon);
    void addItem(SARibbonGalleryItem* item);
    //以一个aciton作为item添加
    void addActionItem(QAction* act);
    void addActionItemList(const QList< QAction* >& acts);

    //构建一个model，这个model的父类是SARibbonGalleryGroup，如果要共享model，需要手动处理model的父类
    void setupGroupModel();
    SARibbonGalleryGroupModel* groupModel();
    // 标题
    void setGroupTitle(const QString& title);
    QString getGroupTitle() const;
    void selectByIndex(int i);
    //设置显示的行数
    void setDisplayRow(DisplayRow r);
    DisplayRow getDisplayRow() const;
    //设置grid最小的宽度，默认为0（不限制），可以限定grid的宽度
    void setGridMinimumWidth(int w);
    int getGridMinimumWidth() const;
    //设置grid最大的宽度，默认为0（不限制），可以限定grid的宽度
    void setGridMaximumWidth(int w);
    int getGridMaximumWidth() const;
    //获取SARibbonGalleryGroup管理的actiongroup
    QActionGroup* getActionGroup() const;
private slots:
    void onItemClicked(const QModelIndex& index);
    void onItemEntered(const QModelIndex& index);
signals:
    void groupTitleChanged(const QString& title);
    /**
     * @brief 等同QActionGroup的triggered
     * 所有加入SARibbonGalleryGroup的action都会被一个QActionGroup管理,可以通过@sa getActionGroup 获取到对应的actiongroup
     * @param action
     */
    void triggered(QAction* action);
    /**
     * @brief 等同QActionGroup的triggered
     * 所有加入SARibbonGalleryGroup的action都会被一个QActionGroup管理,可以通过@sa getActionGroup 获取到对应的actiongroup
     * @note 此属性需要通过QAbstractItemView::entered(const QModelIndex &index)激活，因此要保证设置了setMouseTracking(true)
     * @param action
     */
    void hovered(QAction* action);
};

#endif  // SARIBBONGALLERYGROUP_H

/*** End of inlined file: SARibbonGalleryGroup.h ***/

/*** Start of inlined file: SARibbonGallery.h ***/
#ifndef SARIBBONGALLERY_H
#define SARIBBONGALLERY_H

#include <QFrame>

#include <QSizeGrip>
class QLabel;
class QVBoxLayout;
class SARibbonGalleryViewport;

/**
 * @brief Gallery控件
 *
 * Gallery控件是由一个当前激活的@sa SARibbonGalleryGroup 和弹出的 @sa SARibbonGalleryViewport 组成
 *
 * @sa SARibbonGalleryGroup 是继承@sa QListView actions通过icon展示出来，相关的属性可以按照QListView设置
 *
 * @sa SARibbonGalleryViewport 是一个内部有垂直布局的窗体，在弹出激活时，把管理的SARibbonGalleryGroup都展示出来
 *
 * 示例如下：
 * @code
 * SARibbonGallery* gallery = pannel1->addGallery();
 * QList< QAction* > galleryActions;
 * ...create many actions ...
 * SARibbonGalleryGroup* group1 = gallery->addCategoryActions(tr("Files"), galleryActions);
 * galleryActions.clear();
 * ...create many actions ...
 * gallery->addCategoryActions(tr("Apps"), galleryActions);
 * gallery->setCurrentViewGroup(group1);
 * @endcode
 */
class SA_RIBBON_EXPORT SARibbonGallery : public QFrame
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonGallery)
public:
    SARibbonGallery(QWidget* parent = 0);
    virtual ~SARibbonGallery();
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    //添加一个GalleryGroup
    SARibbonGalleryGroup* addGalleryGroup();
    //添加一个GalleryGroup
    void addGalleryGroup(SARibbonGalleryGroup* group);
    //快速添加一组actions
    SARibbonGalleryGroup* addCategoryActions(const QString& title, QList< QAction* > actions);
    //设置当前显示的SARibbonGalleryGroup
    void setCurrentViewGroup(SARibbonGalleryGroup* group);
    //获取当前显示的SARibbonGalleryGroup
    SARibbonGalleryGroup* currentViewGroup() const;
    //获取弹出窗口指针
    SARibbonGalleryViewport* getPopupViewPort() const;

public:
    //设置最右边三个控制按钮的最大宽度（默认15）
    static void setGalleryButtonMaximumWidth(int w);
signals:
    /**
     * @brief 转发管理的SARibbonGalleryGroup::triggered
     * 所有加入SARibbonGallery的action都会被一个QActionGroup管理,可以通过@sa getActionGroup 获取到对应的actiongroup
     * @param action
     */
    void triggered(QAction* action);
    /**
     * @brief 转发管理的SARibbonGalleryGroup::hovered
     * @note 此属性需要确保SARibbonGalleryGroup::setMouseTracking(true)
     * @param action
     */
    void hovered(QAction* action);

public slots:
    //上翻页
    virtual void pageUp();
    //下翻页
    virtual void pageDown();
    //显示更多触发，默认弹出内部管理的SARibbonGalleryViewport，用户可重载此函数实现自定义的弹出
    virtual void showMoreDetail();
protected slots:
    void onItemClicked(const QModelIndex& index);
    virtual void onTriggered(QAction* action);

private:
    SARibbonGalleryViewport* ensureGetPopupViewPort();

protected:
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};

///
/// \brief SARibbonGallery的Viewport class
///
class SARibbonGalleryViewport : public QWidget
{
    Q_OBJECT
public:
    SARibbonGalleryViewport(QWidget* parent);
    //添加窗口不带标题
    void addWidget(QWidget* w);
    //添加窗口，带标题
    void addWidget(QWidget* w, const QString& title);
    //通过SARibbonGalleryGroup获取对应的标题，用户可以通过此函数设置QLabel的属性
    QLabel* getWidgetTitleLabel(QWidget* w);
    //移除窗口
    void removeWidget(QWidget* w);
public slots:
    void widgetTitleChanged(QWidget* w, const QString& title);

private:
    QVBoxLayout* m_layout;
    QMap< QWidget*, QLabel* > _widgetToTitleLable;  ///< QWidget和lable的对应
};

#endif  // SARIBBONGALLERY_H

/*** End of inlined file: SARibbonGallery.h ***/

/*** Start of inlined file: SARibbonBar.h ***/
#ifndef SARIBBONBAR_H
#define SARIBBONBAR_H

#include <QMenuBar>
#include <QVariant>

#include <QScopedPointer>

class QAbstractButton;
class SARibbonElementFactory;
class SARibbonTabBar;
class SARibbonButtonGroupWidget;
class SARibbonQuickAccessBar;

/**
 * @brief SARibbonBar继承于QMenuBar,在SARibbonMainWindow中直接替换了原来的QMenuBar
 *
 * 通过setRibbonStyle函数设置ribbon的风格:
 *
 * @code
 * void setRibbonStyle(RibbonStyle v);
 * @endcode
 *
 * SARibbonBar参考office和wps，提供了四种风格的Ribbon模式,@ref SARibbonBar::RibbonStyle
 *
 * 如果想ribbon占用的空间足够小，WpsLiteStyleTwoRow模式能比OfficeStyle节省35%的高度空间
 *
 * 如何生成ribbon?先看看一个传统的Menu/ToolBar是如何生成的：
 *
 * @code
 * void MainWindow::MainWindow()
 * {
 *    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
 *    QToolBar *fileToolBar = addToolBar(tr("File"));
 *    //生成action
 *    QAction *newAct = new QAction(newIcon, tr("&New"), this);
 *    fileMenu->addAction(newAct);
 *    fileToolBar->addAction(newAct);
 *
 *    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
 *    fileMenu->addAction(openAct);
 *    fileToolBar->addAction(openAct);
 * }
 * @endcode
 *
 * 传统的Menu/ToolBar主要通过QMenu的addMenu添加菜单,通过QMainWindow::addToolBar生成QToolBar,
 * 再把QAction设置进QMenu和QToolBar中
 *
 * SARibbonBar和传统方法相似，不过相对于传统的Menu/ToolBar QMenu和QToolBar是平级的，
 * Ribbon是有明显的层级关系，SARibbonBar下面是 @ref SARibbonCategory，
 * SARibbonCategory下面是@ref SARibbonPannel ，SARibbonPannel下面是@ref SARibbonToolButton ，
 * SARibbonToolButton管理着QAction
 *
 * 因此，生成一个ribbon只需以下几个函数：
 * @code
 * SARibbonCategory * SARibbonBar::addCategoryPage(const QString& title);
 * SARibbonPannel * SARibbonCategory::addPannel(const QString& title);
 * SARibbonToolButton * SARibbonPannel::addLargeAction(QAction *action);
 * SARibbonToolButton * SARibbonPannel::addSmallAction(QAction *action);
 * @endcode
 *
 * 因此生成步骤如下：
 *
 * @code
 * //成员变量
 * SARibbonCategory* categoryMain;
 * SARibbonPannel* FilePannel;
 *
 * //建立ui
 * void setupRibbonUi()
 * {
 *     ......
 *     //ribbonwindow为SARibbonMainWindow
 *     SARibbonBar* ribbon = ribbonwindow->ribbonBar();
 *     ribbon->setRibbonStyle(SARibbonBar::WpsLiteStyle);
 *     //添加一个Main标签
 *     categoryMain = ribbon->addCategoryPage(QStringLiteral("Main"));
 *     //Main标签下添加一个File Pannel
 *     FilePannel = categoryMain->addPannel(QStringLiteral("FilePannel"));
 *     //开始为File Pannel添加action
 *     FilePannel->addLargeAction(actionNew);
 *     FilePannel->addLargeAction(actionOpen);
 *     FilePannel->addLargeAction(actionSave);
 *     FilePannel->addSmallAction(actionImportMesh);
 *     FilePannel->addSmallAction(actionImportGeometry);
 * }
 * @endcode
 */
class SA_RIBBON_EXPORT SARibbonBar : public QMenuBar
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonBar)
    Q_PROPERTY(RibbonStyle ribbonStyle READ currentRibbonStyle WRITE setRibbonStyle)
    Q_PROPERTY(bool minimumMode READ isMinimumMode WRITE setMinimumMode)
    Q_PROPERTY(bool minimumModeButton READ haveShowMinimumModeButton WRITE showMinimumModeButton)
    Q_PROPERTY(QColor windowTitleTextColor READ windowTitleTextColor WRITE setWindowTitleTextColor)
    Q_PROPERTY(QColor tabBarBaseLineColor READ tabBarBaseLineColor WRITE setTabBarBaseLineColor)
    Q_PROPERTY(Qt::Alignment windowTitleAligment READ windowTitleAligment WRITE setWindowTitleAligment)
public:
    /**
     * @brief 定义ribbon的风格,第一字节代表样式，第二字节代表是否是2行
     */
    enum RibbonStyle
    {
        OfficeStyle        = 0x0000,  ///< 类似office 的ribbon风格
        WpsLiteStyle       = 0x0001,  ///< 类似wps的紧凑风格
        OfficeStyleTwoRow  = 0x0100,  ///< 类似office 的ribbon风格 2行工具栏 三行布局模式，office就是三行布局模式，pannel能布置3行小toolbutton，默认模式
        WpsLiteStyleTwoRow = 0x0101   ///< 类似wps的紧凑风格  2行工具栏
    };
    Q_ENUM(RibbonStyle)

    /**
     * @brief 定义当前ribbon 的状态
     */
    enum RibbonMode
    {
        MinimumRibbonMode,  ///< 缩小模式
        NormalRibbonMode    ///< 正常模式
    };
    Q_ENUM(RibbonMode)

public:
    //判断RibbonStyle是否为2行模式
    static bool isTwoRowStyle(RibbonStyle s);

    //判断是否是office样式
    static bool isOfficeStyle(RibbonStyle s);

    //获取版本信息
    static QString versionString();

    //构造函数
    SARibbonBar(QWidget* parent = nullptr);
    ~SARibbonBar() Q_DECL_OVERRIDE;
    //获取applicationButton
    QAbstractButton* applicationButton();

    //设置applicationButton
    void setApplicationButton(QAbstractButton* btn);

    //获取tabbar
    SARibbonTabBar* ribbonTabBar();

    //添加一个标签
    SARibbonCategory* addCategoryPage(const QString& title);
    void addCategoryPage(SARibbonCategory* category);

    //为了支持Qt designer,添加的一个重载函数
    Q_INVOKABLE void addCategoryPage(QWidget* category);

    //添加一个category，category的位置在index，如果当前category数量少于index，将插入到最后
    SARibbonCategory* insertCategoryPage(const QString& title, int index);
    void insertCategoryPage(SARibbonCategory* category, int index);

    //通过名字查找Category
    SARibbonCategory* categoryByName(const QString& title) const;

    //通过ObjectName查找Category
    SARibbonCategory* categoryByObjectName(const QString& objname) const;

    //通过索引找到category，如果超过索引范围，会返回nullptr
    SARibbonCategory* categoryByIndex(int index) const;

    //隐藏category,并不会删除或者取走，只是隐藏
    void hideCategory(SARibbonCategory* category);

    //显示被隐藏的category
    void showCategory(SARibbonCategory* category);

    //判断这个category是否在显示状态，也就是tabbar有这个category
    bool isCategoryVisible(const SARibbonCategory* c) const;

    //获取category的索引
    int categoryIndex(const SARibbonCategory* c) const;

    //移动一个Category从from index到to index
    void moveCategory(int from, int to);

    //获取当前显示的所有的SARibbonCategory，包含未显示的SARibbonContextCategory的SARibbonCategory也一并返回
    QList< SARibbonCategory* > categoryPages(bool getAll = true) const;

    //移除SARibbonCategory
    void removeCategory(SARibbonCategory* category);

    //添加一个上下文标签
    SARibbonContextCategory* addContextCategory(const QString& title, const QColor& color = QColor(), const QVariant& id = QVariant());
    void addContextCategory(SARibbonContextCategory* context);

    //显示一个上下文标签
    void showContextCategory(SARibbonContextCategory* context);

    //隐藏一个上下文标签
    void hideContextCategory(SARibbonContextCategory* context);

    //判断上下文是否是在显示状态
    bool isContextCategoryVisible(SARibbonContextCategory* context);

    //设置上下文标签的显示或隐藏
    void setContextCategoryVisible(SARibbonContextCategory* context, bool visible);

    //获取所有的上下文标签
    QList< SARibbonContextCategory* > contextCategoryList() const;

    //移除ContextCategory
    void destroyContextCategory(SARibbonContextCategory* context);

    //设置为隐藏模式
    void setMinimumMode(bool isHide);

    //当前Ribbon是否是隐藏模式
    bool isMinimumMode() const;

    //设置显示隐藏ribbon按钮
    void showMinimumModeButton(bool isShow = true);

    //是否显示隐藏ribbon按钮
    bool haveShowMinimumModeButton() const;

    // ribbon tab的高度
    int tabBarHeight() const;

    //标题栏的高度
    int titleBarHeight() const;

    //激活tabbar右边的按钮群
    void activeRightButtonGroup();

    //右侧按钮群
    SARibbonButtonGroupWidget* rightButtonGroup();

    //快速响应栏
    SARibbonQuickAccessBar* quickAccessBar();

    //设置ribbon的风格
    void setRibbonStyle(RibbonStyle v);

    //当前ribbon的风格
    RibbonStyle currentRibbonStyle() const;

    //当前的模式
    RibbonMode currentRibbonState() const;

    //设置当前ribbon的index
    void setCurrentIndex(int index);

    //返回当前的tab索引
    int currentIndex();

    //确保标签显示出来
    void raiseCategory(SARibbonCategory* category);

    //判断当前的样式是否为两行
    bool isTwoRowStyle() const;

    //判断当前的样式是否为office样式
    bool isOfficeStyle() const;

    //告诉saribbonbar，window button的尺寸
    void setWindowButtonSize(const QSize& size);

    //更新ribbon的布局数据，此函数适用于一些关键性尺寸变化，换起ribbon下面元素的布局
    void updateRibbonGeometry();
    // tabbar 底部会绘制一条线条，此接口定义线条颜色
    void setTabBarBaseLineColor(const QColor& clr);
    QColor tabBarBaseLineColor() const;
    //设置标题颜色
    void setWindowTitleTextColor(const QColor& clr);
    QColor windowTitleTextColor() const;
    //设置标题的对齐方式
    void setWindowTitleAligment(Qt::Alignment al);
    Qt::Alignment windowTitleAligment() const;
    //设置按钮允许换行
    void setEnableWordWrap(bool on);
    bool isEnableWordWrap() const;
signals:

    /**
     * @brief 应用按钮点击响应 - 左上角的按钮，通过关联此信号触发应用按钮点击的效果
     */
    void applicationButtonClicked();

    /**
     * @brief 标签页变化触发的信号
     * @param index
     */
    void currentRibbonTabChanged(int index);

    /**
     * @brief ribbon的状态发生了变化后触发此信号
     * @param nowState 变更之后的ribbon状态
     */
    void ribbonModeChanged(RibbonMode nowState);

    /**
     * @brief ribbon的状态发生了变化后触发此信号
     * @param nowStyle 变更之后的ribbon样式
     */
    void ribbonStyleChanged(RibbonStyle nowStyle);

protected:
    bool eventFilter(QObject* obj, QEvent* e) override;

    //根据情况重置tabbar的宽度，主要针对wps模式
    int calcMinTabBarWidth() const;

    //根据currentRibbonStyle计算mainBar的高度
    virtual int mainBarHeight() const;
    //更新
    void updateCategoryTitleToTabName();
protected slots:
    void onWindowTitleChanged(const QString& title);
    void onWindowIconChanged(const QIcon& i);
    void onCategoryWindowTitleChanged(const QString& title);
    void onStackWidgetHided();
    virtual void onCurrentRibbonTabChanged(int index);
    virtual void onCurrentRibbonTabClicked(int index);
    virtual void onCurrentRibbonTabDoubleClicked(int index);
    void onContextsCategoryPageAdded(SARibbonCategory* category);
    void onContextsCategoryCategoryNameChanged(SARibbonCategory* category, const QString& title);
    void onTabMoved(int from, int to);

private:
    int tabIndex(SARibbonCategory* obj);
    void resizeInOfficeStyle();
    void resizeInWpsLiteStyle();
    void paintInNormalStyle();
    void paintInWpsLiteStyle();
    void resizeStackedContainerWidget();

    //刷新所有ContextCategoryManagerData，这个在单独一个Category删除时调用
    void updateContextCategoryManagerData();
    void updateRibbonElementGeometry();

protected:
    void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
    void moveEvent(QMoveEvent* event) Q_DECL_OVERRIDE;
    virtual void paintBackground(QPainter& painter);
    virtual void paintWindowTitle(QPainter& painter, const QString& title, const QRect& titleRegion);
    virtual void paintContextCategoryTab(QPainter& painter, const QString& title, QRect contextRect, const QColor& color);
};

#endif  // SARIBBONBAR_H

/*** End of inlined file: SARibbonBar.h ***/

/*** Start of inlined file: SARibbonStyleOption.h ***/
#ifndef SARIBBONSTYLEOPTION_H
#define SARIBBONSTYLEOPTION_H

#include <QDebug>
/**
 * @brief 定义了saribbon所有尺寸相关信息，saribbon的建立都基于此类的尺寸，如果想调整，
 * 可以通过 @ref SARibbonElementCreateDelegate（通过SARibbonElementManager单例管理） 的 @ref setRibbonStyleOption 函数设置自己的SARibbonStyleOption
 *
 * @sa SARibbonElementManager
 *
 * 一般SARibbonElementCreateDelegate::setRibbonStyleOption函数最好在ribbonbar构建之前调用
 *
 * @note 此类定义了ribbonbar和pannel的高度信息，并通过字体提前计算好一些布局信息
 *
 * @todo 后续开发通过配置文件定义ribbon的尺寸布局
 */
class SA_RIBBON_EXPORT SARibbonStyleOption
{
public:
    SARibbonStyleOption();
    virtual ~SARibbonStyleOption();

public:
    // ribbonBar的高度
    virtual int ribbonBarHeight(SARibbonBar::RibbonStyle s) const;

    //标题栏的高度，对于wps模式，此参数没有用
    virtual int titleBarHeight() const;

    //标签栏高度
    virtual int tabBarHeight() const;

    //在改变了参数后对需要计算的变量从新计算
    virtual void recalc();

protected:
    //通过已有参数计算pannel的高度
    //    int calcPannelHeight(SARibbonPannel::PannelLayoutMode lm) const;
    //计算ribbon的高度
    int calcMainbarHeight(SARibbonBar::RibbonStyle s) const;

private:
    //初始化
    void init();
    //计算pannel的高度
    void updateMainbarHeight();

private:
    int m_tabBarHeight;                        ///< ribbon tab 的高度
    int m_titleBarHeight;                      ///< 标题栏高度
    int m_ribbonbarHeightOfficeStyleThreeRow;  ///< office样式的3行高度
    int m_ribbonbarHeightWPSStyleThreeRow;     ///< wps样式3行的高度
    int m_ribbonbarHeightWPSStyleTwoRow;       ///< wps样式2行的高度
    int m_ribbonbarHeightOfficeStyleTwoRow;    ///< office样式2行的高度
};

SA_RIBBON_EXPORT QDebug operator<<(QDebug debug, const SARibbonStyleOption& c);
#endif  // SARIBBONSTYLEOPTION_H

/*** End of inlined file: SARibbonStyleOption.h ***/

/*** Start of inlined file: SARibbonElementFactory.h ***/
#ifndef SARIBBONELEMENTFACTORY_H
#define SARIBBONELEMENTFACTORY_H

#include <QColor>
#include <QMargins>
#include <QSize>
#include <QScopedPointer>

class QWidget;
class SARibbonBar;
class SARibbonTabBar;
class SARibbonApplicationButton;
class SARibbonCategory;
class SARibbonContextCategory;
class SARibbonPannel;
class SARibbonSeparatorWidget;
class SARibbonGallery;
class SARibbonGalleryGroup;
class SARibbonToolButton;
class SARibbonControlButton;
class SARibbonButtonGroupWidget;
class SARibbonStackedWidget;
class SARibbonQuickAccessBar;
class SARibbonPannelOptionButton;

///
/// \brief SARibbon的子元素创建的工厂，SARibbon内部创建子元素都通过SARibbonElementFactory来创建
///
/// 由于SARibbonBar是一个复合控件，很多子窗口组合而成，有些部件的创建如果想继承，那么就需要这个工厂类来处理
/// 如SARibbonCategory，可以重载此类的createRibbonCategory,返回重载的类的实例
///
class SA_RIBBON_EXPORT SARibbonElementFactory
{
public:
    SARibbonElementFactory();
    virtual ~SARibbonElementFactory();
    virtual SARibbonTabBar* createRibbonTabBar(QWidget* parent);
    virtual SARibbonApplicationButton* createRibbonApplicationButton(QWidget* parent);
    virtual SARibbonCategory* createRibbonCategory(QWidget* parent);
    virtual SARibbonContextCategory* createRibbonContextCategory(QWidget* parent);
    virtual SARibbonPannel* createRibbonPannel(QWidget* parent);
    virtual SARibbonSeparatorWidget* createRibbonSeparatorWidget(int value, QWidget* parent);
    virtual SARibbonSeparatorWidget* createRibbonSeparatorWidget(QWidget* parent);
    virtual SARibbonGallery* createRibbonGallery(QWidget* parent);
    virtual SARibbonGalleryGroup* createRibbonGalleryGroup(QWidget* parent);
    virtual SARibbonToolButton* createRibbonToolButton(QWidget* parent);
    virtual SARibbonStackedWidget* createRibbonStackedWidget(SARibbonBar* parent);

    //创建隐藏ribbon的按钮代理函数
    virtual SARibbonControlButton* createHidePannelButton(SARibbonBar* parent);
    virtual SARibbonButtonGroupWidget* craeteButtonGroupWidget(QWidget* parent);
    virtual SARibbonQuickAccessBar* createQuickAccessBar(QWidget* parent);

    // SARibbonStyleOption可以进行继承，此函数无需设置为虚函数
    SARibbonStyleOption& getRibbonStyleOption();
    void setRibbonStyleOption(SARibbonStyleOption* opt);

    //创建SARibbonPannelOptionButton
    virtual SARibbonPannelOptionButton* createRibbonPannelOptionButton(SARibbonPannel* pannel);

private:
    QScopedPointer< SARibbonStyleOption > mStyleOption;
};

#endif  // SARIBBONELEMENTCREATEDELEGATE_H

/*** End of inlined file: SARibbonElementFactory.h ***/

/*** Start of inlined file: SARibbonElementManager.h ***/
#ifndef SARIBBONELEMENTMANAGER_H
#define SARIBBONELEMENTMANAGER_H

///
/// \brief 此类是一个全局单例，用于管理SARibbonElementCreateDelegate
///
class SA_RIBBON_EXPORT SARibbonElementManager
{
protected:
    SARibbonElementManager();

public:
    virtual ~SARibbonElementManager();
    static SARibbonElementManager* instance();
    SARibbonElementFactory* factory();
    void setupFactory(SARibbonElementFactory* fac);

private:
    QScopedPointer< SARibbonElementFactory > mFactory;
};
#ifndef RibbonSubElementMgr
#define RibbonSubElementMgr SARibbonElementManager::instance()
#endif
#ifndef RibbonSubElementDelegate
#define RibbonSubElementDelegate SARibbonElementManager::instance()->factory()
#endif
#ifndef RibbonSubElementStyleOpt
#define RibbonSubElementStyleOpt SARibbonElementManager::instance()->factory()->getRibbonStyleOption()
#endif
#endif  // SARIBBONELEMENTMANAGER_H

/*** End of inlined file: SARibbonElementManager.h ***/

/*** Start of inlined file: SARibbonCustomizeData.h ***/
#ifndef SARIBBONCUSTOMIZEDATA_H
#define SARIBBONCUSTOMIZEDATA_H

#include <QList>

class SARibbonMainWindow;

/**
 * @brief 记录所有自定义操作的数据类
 * @note 此数据依赖于@ref SARibbonActionsManager 要在SARibbonActionsManager之后使用此类
 */
class SA_RIBBON_EXPORT SARibbonCustomizeData
{
public:
    enum ActionType
    {
        UnknowActionType = 0,           ///< 未知操作
        AddCategoryActionType,          ///< 添加category操作(1)
        AddPannelActionType,            ///< 添加pannel操作(2)
        AddActionActionType,            ///< 添加action操作(3)
        RemoveCategoryActionType,       ///< 删除category操作(4)
        RemovePannelActionType,         ///< 删除pannel操作(5)
        RemoveActionActionType,         ///< 删除action操作(6)
        ChangeCategoryOrderActionType,  ///< 改变category顺序的操作(7)
        ChangePannelOrderActionType,    ///< 改变pannel顺序的操作(8)
        ChangeActionOrderActionType,    ///< 改变action顺序的操作(9)
        RenameCategoryActionType,       ///< 对category更名操作(10)
        RenamePannelActionType,         ///< 对Pannel更名操作(11)
        VisibleCategoryActionType       ///< 对category执行隐藏/显示操作(12)
    };
    SARibbonCustomizeData();
    SARibbonCustomizeData(ActionType type, SARibbonActionsManager* mgr = nullptr);
    //获取CustomizeData的action type
    ActionType actionType() const;

    //设置CustomizeData的action type
    void setActionType(ActionType a);

    //判断是否是一个正常的CustomizeData
    bool isValid() const;

    //应用SARibbonCustomizeData
    bool apply(SARibbonMainWindow* m) const;

    //获取actionmanager指针
    SARibbonActionsManager* actionManager();

    //设置ActionsManager
    void setActionsManager(SARibbonActionsManager* mgr);

    //对应AddCategoryActionType
    static SARibbonCustomizeData makeAddCategoryCustomizeData(const QString& title, int index, const QString& objName);

    //对应AddPannelActionType
    static SARibbonCustomizeData makeAddPannelCustomizeData(const QString& title, int index, const QString& categoryobjName, const QString& objName);

    //对应AddActionActionType
    static SARibbonCustomizeData makeAddActionCustomizeData(const QString& key,
                                                            SARibbonActionsManager* mgr,
                                                            SARibbonPannelItem::RowProportion rp,
                                                            const QString& categoryObjName,
                                                            const QString& pannelObjName);

    //对应RenameCategoryActionType
    static SARibbonCustomizeData makeRenameCategoryCustomizeData(const QString& newname, const QString& categoryobjName);

    //对应RenamePannelActionType
    static SARibbonCustomizeData makeRenamePannelCustomizeData(const QString& newname,
                                                               const QString& categoryobjName,
                                                               const QString& pannelObjName);

    //对应RemoveCategoryActionType
    static SARibbonCustomizeData makeRemoveCategoryCustomizeData(const QString& categoryobjName);

    //对应ChangeCategoryOrderActionType
    static SARibbonCustomizeData makeChangeCategoryOrderCustomizeData(const QString& categoryobjName, int moveindex);

    //对应ChangePannelOrderActionType
    static SARibbonCustomizeData makeChangePannelOrderCustomizeData(const QString& categoryobjName,
                                                                    const QString& pannelObjName,
                                                                    int moveindex);

    //对应ChangeActionOrderActionType
    static SARibbonCustomizeData makeChangeActionOrderCustomizeData(const QString& categoryobjName,
                                                                    const QString& pannelObjName,
                                                                    const QString& key,
                                                                    SARibbonActionsManager* mgr,
                                                                    int moveindex);

    //对应RemovePannelActionType
    static SARibbonCustomizeData makeRemovePannelCustomizeData(const QString& categoryobjName, const QString& pannelObjName);

    //对应RemoveActionActionType
    static SARibbonCustomizeData makeRemoveActionCustomizeData(const QString& categoryobjName,
                                                               const QString& pannelObjName,
                                                               const QString& key,
                                                               SARibbonActionsManager* mgr);

    //对应VisibleCategoryActionType
    static SARibbonCustomizeData makeVisibleCategoryCustomizeData(const QString& categoryobjName, bool isShow);

    //判断是否可以自定义,如果某个action不想被编辑，可以通过此函数设置
    static bool isCanCustomize(QObject* obj);
    static void setCanCustomize(QObject* obj, bool canbe = true);

    //对QList<SARibbonCustomizeData>进行简化
    static QList< SARibbonCustomizeData > simplify(const QList< SARibbonCustomizeData >& csd);

public:
    /**
     * @brief 记录顺序的参数
     *
     * 在actionType==AddCategoryActionType时，此参数记录Category的insert位置,
     * 在actionType==AddPannelActionType时，此参数记录pannel的insert位置,
     * 在actionType==AddActionActionType时，此参数记录pannel的insert位置
     */
    int indexValue;

    /**
     * @brief 记录标题、索引等参数
     *
     * 在actionType==AddCategoryActionType时，key为category标题，
     * 在actionType==AddPannelActionType时，key为pannel标题，
     * 在actionType==AddActionActionType时，key为action的查询依据，基于SARibbonActionsManager::action查询
     */
    QString keyValue;

    /**
     * @brief 记录categoryObjName，用于定位Category
     */
    QString categoryObjNameValue;

    /**
     * @brief 记录pannelObjName，saribbon的Customize索引大部分基于objname
     */
    QString pannelObjNameValue;

    SARibbonPannelItem::RowProportion actionRowProportionValue;  ///< 行的占比，ribbon中有large，media和small三种占比,见@ref RowProportion
private:
    ActionType m_type;  ///< 标记这个data是category还是pannel亦或是action
    SARibbonActionsManager* m_actionsManagerPointer;
};
Q_DECLARE_METATYPE(SARibbonCustomizeData)

typedef QList< SARibbonCustomizeData > SARibbonCustomizeDataList;

#endif  // SARIBBONCUSTOMIZEDATA_H

/*** End of inlined file: SARibbonCustomizeData.h ***/

/*** Start of inlined file: SARibbonCustomizeWidget.h ***/
#ifndef SARIBBONCUSTOMIZEWIDGET_H
#define SARIBBONCUSTOMIZEWIDGET_H

#include <QWidget>

// SARibbonCustomizeWidget 特有
class SARibbonCustomizeWidgetUi;
class SARibbonMainWindow;
class QStandardItemModel;
class QStandardItem;
class QAbstractButton;
//
class QXmlStreamWriter;
class QXmlStreamReader;

/**
 * @brief 自定义界面窗口
 *
 * @note SARibbon的自定义是基于步骤的，如果在窗口生成前调用了@ref sa_apply_customize_from_xml_file 类似函数
 * 那么在对话框生成前为了保证同步需要调用@ref SARibbonCustomizeWidget::fromXml 同步配置文件，这样再次修改后的配置文件就一致
 */
class SA_RIBBON_EXPORT SARibbonCustomizeWidget : public QWidget
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonCustomizeWidget)
public:
    SARibbonCustomizeWidget(SARibbonMainWindow* ribbonWindow, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~SARibbonCustomizeWidget();

    /**
     * @brief 定义ribbon树的显示类型
     */
    enum RibbonTreeShowType
    {
        ShowAllCategory,  ///< 显示所有Category，包括contextcategory
        ShowMainCategory  ///< 显示主要的category，不包含上下文
    };

    /**
     * @brief QStandardItem对应的role
     */
    enum ItemRole
    {
        LevelRole            = Qt::UserRole + 1,  ///< 代表这是层级，有0：category 1：pannel 2：item
        PointerRole          = Qt::UserRole + 2,  ///< 代表这是存放指针。根据LevelRole来进行转
        CanCustomizeRole     = Qt::UserRole + 3,  ///< 代表个item是可以自定义的.bool
        CustomizeRole        = Qt::UserRole + 4,  ///< 代表这个是自定义的item,bool,主要用于那些自己添加的标签和pannel，有此角色必有CanCustomizeRole
        CustomizeObjNameRole = Qt::UserRole + 5   ///< 记录了临时的自定义内容的obj名 QString
    };

    //设置action管理器
    void setupActionsManager(SARibbonActionsManager* mgr);

    //判断用户是否有改动内容
    bool isChanged() const;

    //获取model
    const QStandardItemModel* model() const;

    //根据当前的radiobutton选项来更新model
    void updateModel();

    //更新model
    void updateModel(RibbonTreeShowType type);

    //应用所有的设定
    bool applys();

    //转换为xml
    bool toXml(QXmlStreamWriter* xml) const;
    bool toXml(const QString& xmlpath) const;

    //从xml中加载QList<SARibbonCustomizeData>，对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
    void fromXml(QXmlStreamReader* xml);
    void fromXml(const QString& xmlpath);

    //应用xml配置，可以结合customize_datas_from_xml和customize_datas_apply函数
    static bool fromXml(QXmlStreamReader* xml, SARibbonMainWindow* w, SARibbonActionsManager* mgr);

    //清除所有动作，在执行applys函数后，如果要继续调用，应该clear，否则会导致异常
    void clear();

protected:
    //把QList<SARibbonCustomizeData>进行裁剪,把一些动作合并
    void simplify();

    SARibbonPannelItem::RowProportion selectedRowProportion() const;

    QAction* selectedAction() const;
    QAction* itemToAction(QStandardItem* item) const;

    QStandardItem* selectedItem() const;

    //获取选中的ribbon tree 的level
    int selectedRibbonLevel() const;

    //根据选中的item判断
    int itemLevel(QStandardItem* item) const;

    //设置某个item被选中
    void setSelectItem(QStandardItem* item, bool ensureVisible = true);

    //判断itemn能否改动，可以改动返回true
    bool isItemCanCustomize(QStandardItem* item) const;
    bool isSelectedItemCanCustomize() const;

    //判断item是否是自定义的item
    bool isCustomizeItem(QStandardItem* item) const;
    bool isSelectedItemIsCustomize() const;

    //删除一个item
    void removeItem(QStandardItem* item);

private slots:
    void onComboBoxActionIndexCurrentIndexChanged(int index);
    void onRadioButtonGroupButtonClicked(QAbstractButton* b);
    void onPushButtonNewCategoryClicked();
    void onPushButtonNewPannelClicked();
    void onPushButtonRenameClicked();
    void onPushButtonAddClicked();
    void onPushButtonDeleteClicked();
    void onListViewSelectClicked(const QModelIndex& index);
    void onTreeViewResultClicked(const QModelIndex& index);
    void onToolButtonUpClicked();
    void onToolButtonDownClicked();
    void onItemChanged(QStandardItem* item);
    void onLineEditSearchActionTextEdited(const QString& text);
    void onPushButtonResetClicked();

private:
    void initConnection();

private:
    SARibbonCustomizeWidgetUi* ui;
};

/**
 * @brief 转换为xml
 *
 * 此函数仅会写element，不会写document相关内容，因此如果需要写document，
 * 需要在此函数前调用QXmlStreamWriter::writeStartDocument(),在此函数后调用QXmlStreamWriter::writeEndDocument()
 * @param xml QXmlStreamWriter指针
 * @note 注意，在传入QXmlStreamWriter之前，需要设置编码为utf-8:xml->setCodec("utf-8");
 * @note 由于QXmlStreamWriter在QString作为io时，是不支持编码的，而此又无法保证自定义过程不出现中文字符，
 * 因此，QXmlStreamWriter不应该通过QString进行构造，如果需要用到string，也需要通过QByteArray构造，如：
 * @param cds 基于QList<SARibbonCustomizeData>生成的步骤
 * @return 如果出现异常，返回false,如果没有自定义数据也会返回false
 */
bool SA_RIBBON_EXPORT sa_customize_datas_to_xml(QXmlStreamWriter* xml, const QList< SARibbonCustomizeData >& cds);

/**
 * @brief 通过xml获取QList<SARibbonCustomizeData>
 * @param xml
 * @return QList<SARibbonCustomizeData>
 */
QList< SARibbonCustomizeData > SA_RIBBON_EXPORT sa_customize_datas_from_xml(QXmlStreamReader* xml, SARibbonActionsManager* mgr);

/**
 * @brief 应用QList<SARibbonCustomizeData>
 * @param cds
 * @param w SARibbonMainWindow指针
 * @return 成功应用的个数
 */
int SA_RIBBON_EXPORT sa_customize_datas_apply(const QList< SARibbonCustomizeData >& cds, SARibbonMainWindow* w);

/**
 * @brief 直接加载xml自定义ribbon配置文件用于ribbon的自定义显示
 * @param filePath xml配置文件
 * @param w 主窗体
 * @param mgr action管理器
 * @return 成功返回true
 * @note 重复加载一个配置文件会发生异常，为了避免此类事件发生，一般通过一个变量保证只加载一次，如：
 * @code
 * static bool has_call = false;
 * if (!has_call) {
 *     has_call = sa_apply_customize_from_xml_file("customize.xml", this, m_actMgr);
 * }
 * @endcode
 */
bool SA_RIBBON_EXPORT sa_apply_customize_from_xml_file(const QString& filePath, SARibbonMainWindow* w, SARibbonActionsManager* mgr);

#endif  // SARIBBONCUSTOMIZEWIDGET_H

/*** End of inlined file: SARibbonCustomizeWidget.h ***/

/*** Start of inlined file: SARibbonCustomizeDialog.h ***/
#ifndef SARIBBONCUSTOMIZEDIALOG_H
#define SARIBBONCUSTOMIZEDIALOG_H

#include <QDialog>

class SARibbonActionsManager;
class SARibbonCustomizeDialogUi;
class QXmlStreamWriter;

/**
 * @brief SARibbonCustomizeWidget的对话框封装
 *
 * 此功能依赖于@ref SARibbonActionsManager ，SARibbonActionsManager建议作为mianwindow的成员变量，
 * SARibbonActionsManager可以快速绑定所有QAction，详细见SARibbonActionsManager的说明
 *
 * @note SARibbon的自定义是基于步骤的，如果在窗口生成前调用了@ref sa_apply_customize_from_xml_file 类似函数
 * 那么在对话框生成前为了保证同步需要调用@ref SARibbonCustomizeDialog::fromXml 同步配置文件，这样再次修改后的配置文件就一致
 */
class SA_RIBBON_EXPORT SARibbonCustomizeDialog : public QDialog
{
    Q_OBJECT
public:
    SARibbonCustomizeDialog(SARibbonMainWindow* ribbonWindow, QWidget* p = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    //设置action管理器
    void setupActionsManager(SARibbonActionsManager* mgr);

    //应用所有的设定
    bool applys();

    //清除所有动作
    void clear();

    //转换为xml
    bool toXml(QXmlStreamWriter* xml) const;
    bool toXml(const QString& xmlpath) const;

    //从xml中加载QList<SARibbonCustomizeData>，对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
    void fromXml(QXmlStreamReader* xml);
    void fromXml(const QString& xmlpath);

    //返回SARibbonCustomizeWidget窗口指针
    SARibbonCustomizeWidget* customizeWidget() const;

private:
    void initConnection();

    SARibbonCustomizeDialogUi* ui;
};

#endif  // SARIBBONCUSTOMIZEDIALOG_H

/*** End of inlined file: SARibbonCustomizeDialog.h ***/

/*** Start of inlined file: SARibbonMainWindow.h ***/
#ifndef SARIBBONMAINWINDOW_H
#define SARIBBONMAINWINDOW_H

#include <QMainWindow>
class SARibbonBar;
class SAFramelessHelper;

/**
 * @brief 如果要使用SARibbonBar，必须使用此类代替QMainWindow
 *
 * 由于ribbon的风格和传统的Toolbar风格差异较大，
 * SARibbonBar使用需要把原有的QMainWindow替换为SARibbonMainWindow,
 * SARibbonMainWindow是个无边框窗体，继承自QMainWindow，其构造函数的参数useRibbon
 * 用于指定是否使用ribbon风格，默认为true
 *
 * @code
 * SARibbonMainWindow(QWidget* parent = nullptr,bool useRibbon = true);
 * @endcode
 *
 * 如果想换回非ribbon风格，只需要把useRibbon设置为false即可,
 * 成员函数isUseRibbon用于判断当前是否为ribbon模式，这个函数在兼容传统Toolbar风格和ribbon风格时非常有用。
 *
 * @code
 * bool isUseRibbon() const;
 * @endcode
 *
 * @ref SARibbonMainWindow 提供了几种常用的ribbon样式，样式可见@ref RibbonTheme
 * 通过@ref setRibbonTheme 可改变ribbon的样式，用户也可通过qss自己定义自己的样式
 *
 */
class SA_RIBBON_EXPORT SARibbonMainWindow : public QMainWindow
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonMainWindow)
    Q_PROPERTY(RibbonTheme ribbonTheme READ ribbonTheme WRITE setRibbonTheme)
public:
    /**
     * @brief Ribbon主题，可以通过qss定制ribbon的主题，定制方法可参看源码中office2013.qss
     *
     * 注意，由于有些qss的尺寸，在C++代码中无法获取到，因此针对用户自定义的qss主题，有些尺寸是需要手动设置进去的
     *
     * 例如ribbon tab的margin信息，在QTabBar是无法获取到，而这个影响了SARibbonContextCategory的绘制，
     * 因此，在设置qss后需要针对margin信息重新设置进SARibbonTabBar中
     */
    enum RibbonTheme
    {
        NormalTheme,  ///< 普通主题
        Office2013    ///< office2013主题
    };
    Q_ENUM(RibbonTheme)
public:
    SARibbonMainWindow(QWidget* parent = nullptr, bool useRibbon = true);
    ~SARibbonMainWindow() Q_DECL_OVERRIDE;
    //返回SARibbonBar
    const SARibbonBar* ribbonBar() const;
    SARibbonBar* ribbonBar();

    //返回SAFramelessHelper
    SAFramelessHelper* framelessHelper();

    void setRibbonTheme(RibbonTheme theme);
    RibbonTheme ribbonTheme() const;

    //判断当前是否使用ribbon模式
    bool isUseRibbon() const;

    //此函数仅用于控制最小最大化和关闭按钮的显示
    void updateWindowFlag(Qt::WindowFlags flags);

    //获取系统按钮的状态
    Qt::WindowFlags windowButtonFlags() const;

    //覆写setMenuWidget
    void setMenuWidget(QWidget* menubar);

    //覆写setMenuBar
    void setMenuBar(QMenuBar* menuBar);

protected:
    //创建ribbonbar的工厂函数
    SARibbonBar* createRibbonBar();
    void loadTheme(const QString& themeFile);
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject* obj, QEvent* e) Q_DECL_OVERRIDE;
    virtual bool event(QEvent* e) Q_DECL_OVERRIDE;
};

#endif  // SARIBBONMAINWINDOW_H

/*** End of inlined file: SARibbonMainWindow.h ***/

/*** End of inlined file: SARibbonAmalgamTemplatePublicHeaders.h ***/

#endif

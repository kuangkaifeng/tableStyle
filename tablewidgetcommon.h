#ifndef MYTABLEWIDGET_H
#define MYTABLEWIDGET_H
#include <QTableWidget>
#include <QDebug>
#include <QHeaderView>
#include <QProxyStyle>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QWidget>
#include <QScrollBar>
#include "customtableheader.h"

//自定义表格滚动条
class RoundScrollBarStyle : public QProxyStyle {
public:
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override {
        if (element == CE_ScrollBarSlider) {
            const QStyleOptionSlider *sliderOpt = qstyleoption_cast<const QStyleOptionSlider *>(option);
            if (sliderOpt) {
                // 滚动条滑块颜色设置为灰色
                QColor handleColor = sliderOpt->state & State_MouseOver ? QColor(169, 169, 169) : QColor(128, 128, 128);  // 深灰色和浅灰色

                // 创建圆角矩形
                QRectF rect = sliderOpt->rect;
                painter->setRenderHint(QPainter::Antialiasing);  // 开启抗锯齿
                painter->setBrush(handleColor);
                painter->setPen(Qt::NoPen);
                painter->drawRoundedRect(rect, rect.width() / 2, rect.width() / 2); // 圆角半径 = 宽度一半
                return;
            }
        }

        // 处理 QTableWidget 或 QTableView 的单元格绘制
        if (element == CE_ItemViewItem) {
            const QStyleOptionViewItem *viewItemOpt = qstyleoption_cast<const QStyleOptionViewItem *>(option);
            if (viewItemOpt) {
                QRect itemRect = viewItemOpt->rect;
                painter->setRenderHint(QPainter::Antialiasing);  // 开启抗锯齿

                // 绘制单元格背景
                painter->setPen(Qt::NoPen); // 不绘制单元格的边框
                painter->setBrush(viewItemOpt->backgroundBrush);
                painter->drawRect(itemRect);

                // 设定横向的分割线颜色和线条样式
                painter->setPen(QPen(QColor(229, 230, 235), 1));  // 浅灰色的横向分割线

                // 每个单元格的底部绘制一条分割线
                painter->drawLine(itemRect.bottomLeft(), itemRect.bottomRight()); // 绘制底部横线
                return;
            }
        }

        // 其他控件使用默认绘制
        QProxyStyle::drawControl(element, option, painter, widget);
    }
};
//自定义表格文本框内容
class CustomTableStyle : public QProxyStyle {
public:
    void drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override {
        if (element == QStyle::CE_ItemViewItem) {
            const QStyleOptionViewItem *viewItemOpt = qstyleoption_cast<const QStyleOptionViewItem *>(option);
            if (viewItemOpt) {
                QRect itemRect = viewItemOpt->rect;
                painter->setRenderHint(QPainter::Antialiasing);  // 开启抗锯齿

                // 确保 itemRect 的高度和行高一致
                int rowHeight = viewItemOpt->rect.height();
                if (itemRect.height() != rowHeight) {
                    itemRect.setHeight(rowHeight);
                }

                // 绘制单元格背景
                painter->setPen(Qt::NoPen);
                painter->setBrush(viewItemOpt->backgroundBrush);
                painter->drawRect(itemRect);  // 确保背景填充整个单元格区域

                // 绘制横向的分割线（只保留底部边框）
                painter->setPen(QPen(QColor(229, 230, 235), 1));  // 浅灰色的横向分割线
                painter->drawLine(itemRect.bottomLeft(), itemRect.bottomRight());  // 绘制底部横线
            }

            // 绘制单元格内容（文本、图标等）
            QProxyStyle::drawControl(element, option, painter, widget);  // 确保绘制默认内容
            return;
        }

        // 默认绘制其他控件
        QProxyStyle::drawControl(element, option, painter, widget);
    }
};

//文本内容超长问题代理
class ElideTextDelegate : public QStyledItemDelegate
{
public:
    explicit ElideTextDelegate(int maxPixelWidth = 100, QObject *parent = nullptr)
        : QStyledItemDelegate(parent), maxPixelWidth(maxPixelWidth) {}

    void setMaxPixelWidth(int width) {
        maxPixelWidth = width;
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // 对于复选框所在的第一列，不做处理
        if (index.column() == 0) {
            QStyledItemDelegate::paint(painter, opt, index);
            return;
        }

        // 直接根据像素宽度对文本进行省略处理
        QFontMetrics fm(opt.font);
        // 使用指定的最大像素宽度进行文本省略
        opt.text = fm.elidedText(opt.text, Qt::ElideRight, maxPixelWidth);

        // 只调用一次基类绘制方法，这样背景、复选框、文本等都能一次绘制完成，不会重影
        QStyledItemDelegate::paint(painter, opt, index);
    }

private:
    int maxPixelWidth; // 允许的最大文本显示宽度，单位为像素
};

class TableWidgetCommon : public QTableWidget
{
    Q_OBJECT
public:
    explicit TableWidgetCommon(bool checkBoxFlag=false,QTableWidget *parent = nullptr);
    void InitTable(bool checkBoxFlag=false);
    void showTable(QList<QStringList> data,int start,int end,bool flag=false);

    void setTableColwidth(bool checkBoxFlag);
    void setTableHeadAglign(Qt::Alignment alignment);
    void setTableAglignAndColor(int colAglign,int colColor=0);
public slots:
//    void checkAllItems();
    void setAllCheckBoxstate(bool state);


signals:
private:
    bool m_checkBoxFlag;
protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        // 获取鼠标点击的位置
        QPoint pos = event->pos();

        // 检查是否点击了表格内的单元格
        QTableWidgetItem *item = itemAt(pos);

        if (!item) {
            // 如果没有点击到单元格，取消所有选中状态
            clearSelection();
        } else {
            // 如果点击了单元格，调用父类的 mousePressEvent
            QTableWidget::mousePressEvent(event);
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        // 调用父类的 mouseReleaseEvent
        QTableWidget::mouseReleaseEvent(event);
    }

};

#endif // MYTABLEWIDGET_H

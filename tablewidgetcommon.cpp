#include "tablewidgetcommon.h"





TableWidgetCommon::TableWidgetCommon(bool checkBoxFlag,QTableWidget *parent) : QTableWidget(parent),m_checkBoxFlag(checkBoxFlag)
{
    InitTable(checkBoxFlag);
    connect(this, &QTableWidget::cellClicked, this, [=](int row, int column){
        // 只处理第0列
        if (column != 0) return;

        QTableWidgetItem *item = this->item(row, column);
        if (!item) return;

        // 获取当前点击位置
        QPoint cursorPos = this->viewport()->mapFromGlobal(QCursor::pos());
        QRect itemRect = this->visualItemRect(item);

        // 计算复选框的大致区域
        QRect checkBoxRect(itemRect.left() + 5, itemRect.top() + 5, 20, 20);

        // 如果点击的是非复选框区域，我们就手动切换勾选状态
        if (!checkBoxRect.contains(cursorPos)) {
            // 只有非复选框区域点击我们才改变
            if (item->checkState() == Qt::Checked) {
                item->setCheckState(Qt::Unchecked);
            } else {
                item->setCheckState(Qt::Checked);
            }
        }
    });
}

void TableWidgetCommon::InitTable(bool checkBoxFlag)
{
    this->setStyleSheet(
           "QTableWidget {"
           "    border: none;"  // 无边框
           "    gridline-color: transparent;"  // 去除网格线
           "}"
           "QScrollBar:vertical {"
           "    border: none;"
           "    background: #f0f0f0;" /* 更浅的灰色背景 */
           "    width: 8px;" /* 滚动条宽度 */
           "    margin: 0px 0px 0px 0px;"
           "}"
           "QScrollBar::handle:vertical {"
           "    background: #888888;" /* 浅灰色 */
           "    border-radius: 4px;" /* 圆角 */
           "    min-height: 20px;"
           "}"
           "QScrollBar::handle:vertical:hover {"
           "    background: #aaaaaa;" /* 悬停时变亮 */
           "    border-radius: 4px;"
           "}"
           "QScrollBar::handle:vertical:pressed {"
           "    background: #cccccc;" /* 按下时更亮 */
           "    border-radius: 4px;"
           "}"
           "QScrollBar::add-line:vertical, "
           "QScrollBar::sub-line:vertical {"
           "    background: none;"
           "    height: 0px;"
           "}"
           "QScrollBar:horizontal {"
           "    border: none;"
           "    background: #f0f0f0;" /* 更浅的灰色背景 */
           "    height: 8px;"
           "    margin: 0px 0px 0px 0px;"
           "}"
           "QScrollBar::handle:horizontal {"
           "    background: #888888;" /* 浅灰色 */
           "    border-radius: 4px;"
           "    min-width: 20px;"
           "}"
           "QScrollBar::handle:horizontal:hover {"
           "    background: #aaaaaa;" /* 悬停时变亮 */
           "    border-radius: 4px;"
           "}"
           "QScrollBar::handle:horizontal:pressed {"
           "    background: #cccccc;" /* 按下时更亮 */
           "    border-radius: 4px;"
           "}"
           "QScrollBar::add-line:horizontal, "
           "QScrollBar::sub-line:horizontal {"
           "    background: none;"
           "    width: 0px;"
           "}"
       );


    //禁用所有编辑操作：
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->verticalHeader()->setVisible(false);
    // 隐藏水平滚动条
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 去除列分割线，只保留行分割线
    this->setShowGrid(true);  // 显示网格线
    this->setGridStyle(Qt::NoPen);  // 去除所有的分割线


    // 设置自定义滚动条样式
    RoundScrollBarStyle *scrollBarStyle = new RoundScrollBarStyle();
    this->verticalScrollBar()->setStyle(scrollBarStyle);
    this->horizontalScrollBar()->setStyle(scrollBarStyle);

    this->setStyle(new CustomTableStyle);
    // 设置选中整行
//    this->setSelectionBehavior(QAbstractItemView::SelectRows);

//    this->horizontalHeader()->setFixedHeight(30);

    if(checkBoxFlag)
    {
        // 设置表头为 CheckBoxHeader
        CCheckBoxHeader *header = new CCheckBoxHeader(Qt::Horizontal, this);
        this->setHorizontalHeader(header);

        // 连接复选框状态变化的信号
        QObject::connect(header, &CCheckBoxHeader::toggleAll, this,&TableWidgetCommon::setAllCheckBoxstate);
    }
    this->horizontalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "    background-color: #FAFBFC;"  // 表头背景色
        "    border: none;"               // 去除边框
        "    padding: 4px;"               // 内边距
        "    font-weight: bold;"          // 字体加粗
        "}"
    );
    // 强制刷新 UI 以应用样式
    this->update();
    this->repaint();
}

void TableWidgetCommon::showTable(QList<QStringList> data,int start,int end,bool flag)
{
    // 清空所有数据，但保留表头
    this->clearContents();

    // 设置表头
    this->setRowCount(end - start);

    // 填充数据并设置工具提示
    for (int row = start,row1=0; row < end; ++row,++row1)
    {
        int col=0;
        if(flag)
        {
            QTableWidgetItem* itemCheckBox = new QTableWidgetItem();
            itemCheckBox->setFlags(itemCheckBox->flags() | Qt::ItemIsUserCheckable);  // 添加复选框标志
            itemCheckBox->setCheckState(Qt::Unchecked);  // 初始状态：未选中
            itemCheckBox->setTextAlignment(Qt::AlignCenter);
            this->setItem(row1,0,itemCheckBox);
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(row1 + 1));
            // 设置单元格内容居中
            itemCheckBox->setTextAlignment(Qt::AlignCenter);
            this->setItem(row1, 1, item);
            col = 2;
        }
        else
        {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(row1 + 1));
            item->setTextAlignment(Qt::AlignCenter);
            // 设置单元格内容居中
            this->setItem(row1, 0, item);
            col = 1;
        }
        // 填充数据并设置工具提示
        for (int i = 0; i < data[row].size(); ++i)
        {
            QString dataText = data[row][i];

            QTableWidgetItem* item = new QTableWidgetItem(dataText);


            // 设置单元格内容居中

            // 设置工具提示为完整文本
            item->setToolTip(dataText);
            this->setItem(row1, col, item);
            col++;
        }
    }


}

void TableWidgetCommon::setAllCheckBoxstate(bool state)
{

    for (int row = 0; row < this->rowCount(); row++)  // 修改了循环条件
        {
            QTableWidgetItem *item = this->item(row, 0);
            if (item)
            {
                if (state)
                {
                    item->setCheckState(Qt::Checked);
                }
                else
                {
                    item->setCheckState(Qt::Unchecked);
                }
            }
        }

}

void TableWidgetCommon::setTableColwidth(bool checkBoxFlag)
{
    for (int i = 0; i < this->columnCount(); i++)
    {
            if (checkBoxFlag)
            {
                if (i < 2)
                {
                    this->setColumnWidth(i, 20);  // 第一列固定宽度
                    this->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);
                } else
                {
                    this->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);  // 其他列自适应
                }
            }
            else
            {
                if (i == 0)
                {
                    this->setColumnWidth(i, 20);  // 第一列固定宽度
                    this->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);
                }
                else
                {
                    this->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);  // 其他列自适应
                }
            }
    }

}

void TableWidgetCommon::setTableHeadAglign(Qt::Alignment alignment)
{
    this->horizontalHeader()->setDefaultAlignment(alignment);
}

void TableWidgetCommon::setTableAglignAndColor(int colAglign,int colColor)
{
    for (int row = 0; row < this->rowCount(); ++row)
    {
        QTableWidgetItem* item = this->item(row, colAglign);
        if (item)
        {
            item->setTextAlignment(Qt::AlignCenter);
        }
        QTableWidgetItem* itemColor = this->item(row, colColor);

        if(itemColor&&itemColor!=0&&itemColor->text()=="未读")
        {

            itemColor->setForeground(QBrush(Qt::red));
        }
        else if(itemColor&&itemColor!=0&&itemColor->text()=="已读")
        {
            itemColor->setForeground(QBrush(Qt::green));
        }
    }
}

//void TableWidgetCommon::checkAllItems()
//{
//    for (int row = 0; row < this->rowCount(); ++row)
//    {
//        QTableWidgetItem *item = this->item(row, 0);  // 假设复选框在第 0 列
//        if (item) {
//            Qt::CheckState state = item->checkState();
//            item->setCheckState(state == Qt::Checked ? Qt::Unchecked : Qt::Checked);
//        }
//    }
//}



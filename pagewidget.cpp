#include "pagewidget.h"
#include <QIntValidator>
#include <QStyle>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

#ifndef SAFE_DELETE
#define SAFE_DELETE(pObj) {if(pObj != Q_NULLPTR) {delete pObj; pObj = Q_NULLPTR;}}
#endif

PageWidget::PageWidget(QList<quint32> list, QWidget* parent) : QWidget(parent),m_itemCountPerPage(list)
{
    QString qss = QString("")
        + QString("QPushButton{border:1px solid #eeeeee;background:#ffffff;border-radius:5px;font-family:\"Microsoft YaHei\";font-size:13px;}")
        + QString("QPushButton:hover{background: #13efa5;color:#ffffff}")
        + QString("QPushButton[currentPage=\"true\"]{background: #e3fbf3;color:#386487;border-radius:5px;}")
        + QString("QLabel{font-family:\"Microsoft YaHei\";font-size:13px;}")
        + QString("QComboBox{font-size:13px;border: 1px solid #ADD8E6;}")
        + QString("QLineEdit{background-color:white;border-radius:5px;border:1px solid #eeeeee;font-family:\"Microsoft YaHei\";font-size:13px;}");
    this->setStyleSheet(qss);
    m_pPageHLayout = Q_NULLPTR;
    m_pTotalLabel = Q_NULLPTR;
    m_pPageBtnHLayout = Q_NULLPTR;
    m_pPrePageBtn = Q_NULLPTR;
    m_pNextPageBtn = Q_NULLPTR;
    m_pFirstPageBtn = Q_NULLPTR;           //首页按钮
    m_pLastPageBtn = Q_NULLPTR;            //最后一页按钮
    m_pLeftMoreBtn = Q_NULLPTR;            //左边省略号按钮
    m_pRightMoreBtn = Q_NULLPTR;           //右边省略号按钮
    //跳转元素控件
    m_pJumpHLayout = Q_NULLPTR;
    m_pGoToLabel = Q_NULLPTR;
    m_pPageLineEdit = Q_NULLPTR;
    m_pGotoBtn = Q_NULLPTR;
    m_pPageUnitLabel = Q_NULLPTR;

    m_nPageNum = 1;

    if(m_itemCountPerPage.count() > 0)
        m_nPageSize = m_itemCountPerPage.at(0);
    else
        m_nPageSize = 10;

    m_pPageHLayout = new QHBoxLayout();
    m_pPageHLayout->setSpacing(8);
    m_pPageHLayout->setMargin(0);
    this->setLayout(m_pPageHLayout);
    m_pPageHLayout->addStretch(1);
    m_pTotalLabel = new QLabel(this);
    m_pTotalLabel->setStyleSheet("QLabel {"
                            "  border: none;"
                            "}");


    m_itemCountPerPageCombo = new QComboBox(this);
    m_itemCountPerPageCombo->setFixedHeight(35);
    m_itemCountPerPageCombo->setFixedWidth(100);
    m_itemCountPerPageCombo->setStyleSheet("QComboBox::drop-down { border: 0px; image: none; }");
    for(quint32 itemCount : m_itemCountPerPage)
    {
        QString text = QString("%1条/页").arg(itemCount);
        m_itemCountPerPageCombo->addItem(text,itemCount);
    }

    connect(m_itemCountPerPageCombo,QOverload<int>::of(&QComboBox::currentIndexChanged),this,[this](int index){
        quint32 pageSize = this->m_itemCountPerPageCombo->itemData(index).toUInt();
        this->m_nPageSize = pageSize;
        emit pageSizeChanged(this->m_nPageSize);
    });
    m_pPageHLayout->addWidget(m_pTotalLabel);
    m_pPageHLayout->addWidget(m_itemCountPerPageCombo);
    initPageBtnControl();
    initJumpControl();
    qDebug()<<"pageend";
}

/**
 * @brief PageControl::initPage 初始化
 * @param total 总数据条数
 * @param pageNum 当前页码
 * @param pageSize 单页大小
 */
void PageWidget::initPage(int total, int pageNum)
{

    m_nPageCount = (total + m_nPageSize - 1) / m_nPageSize;
    if(m_nPageCount == 0)
        m_nPageCount = 1;
    m_pTotalLabel->setText(QString("共%1条").arg(total));
    if (m_nPageCount > 1)
    {
        m_pLastPageBtn->setVisible(true);
        QString maxPageNum = QString::number(m_nPageCount);
        m_pLastPageBtn->setText(maxPageNum);
        m_pLastPageBtn->setFixedWidth(35 + (maxPageNum.length() < 3 ? 0 : maxPageNum.length() - 3) * 12);
    }
    else
    {
        m_pLastPageBtn->setVisible(false);
    }
    if (m_nPageNum > m_nPageCount)
    {
        m_nPageNum = m_nPageCount;
    }
    setCurrentPageInternal(pageNum);
}

/**
 * @brief setCurrentPage 设置当前页码
 * @param pageNum 页码
 */
void PageWidget::setCurrentPageInternal(int pageNum)
{
    m_nPageNum = pageNum;
    if (pageNum >= m_nPageCount)
    {
        m_nPageNum = m_nPageCount;
        pageNum = m_nPageNum;
        m_pLastPageBtn->setProperty("currentPage", "true");

        m_pLastPageBtn->style()->unpolish(m_pLastPageBtn);
        m_pLastPageBtn->style()->polish(m_pLastPageBtn);
        m_pLastPageBtn->update();
        //        m_pLastPageBtn->setStyleSheet("/**/");
        m_pNextPageBtn->setCheckable(false);
        m_pNextPageBtn->setDisabled(true);
    }
    else
    {
        m_pLastPageBtn->setProperty("currentPage", "false");
        //        m_pLastPageBtn->setStyleSheet("/**/");
        m_pLastPageBtn->style()->unpolish(m_pLastPageBtn);
        m_pLastPageBtn->style()->polish(m_pLastPageBtn);
        m_pLastPageBtn->update();
        m_pNextPageBtn->setCheckable(true);
        m_pNextPageBtn->setDisabled(false);
        m_pNextPageBtn->setCursor(Qt::PointingHandCursor);
    }
    if (pageNum <= 1)
    {
        m_nPageNum = 1;
        m_pFirstPageBtn->setProperty("currentPage", "true");
        //        m_pFirstPageBtn->setStyleSheet("/**/");
        m_pFirstPageBtn->style()->unpolish(m_pFirstPageBtn);
        m_pFirstPageBtn->style()->polish(m_pFirstPageBtn);
        m_pFirstPageBtn->update();
        m_pPrePageBtn->setCheckable(false);
        m_pPrePageBtn->setDisabled(true);
        //m_pPrePageBtn->setCursor(Qt::ForbiddenCursor);
    }
    else
    {
        m_pFirstPageBtn->setProperty("currentPage", "false");
        //        m_pFirstPageBtn->setStyleSheet("/**/");
        m_pFirstPageBtn->style()->unpolish(m_pFirstPageBtn);
        m_pFirstPageBtn->style()->polish(m_pFirstPageBtn);
        m_pFirstPageBtn->update();
        m_pPrePageBtn->setCheckable(true);
        m_pPrePageBtn->setDisabled(false);
        m_pPrePageBtn->setCursor(Qt::PointingHandCursor);
    }
    updatePageBtn();
}

/**
 * @brief setCurrentPage 设置当前页码
 * @param pageNum 页码
 */
void PageWidget::setCurrentPage(int pageNum)
{
    if (pageNum >= m_nPageCount)
        pageNum = m_nPageCount;
    if (pageNum <=0 )
        pageNum = 1;

    emit pageChanged(pageNum);
    setCurrentPageInternal(pageNum);
}

int PageWidget::currentPageIndex()
{
    return m_nPageNum;
}

quint32 PageWidget::pageSize()
{
    return m_nPageSize;
}

void PageWidget::setTotal(int total)
{
    m_pTotalLabel->setText(QString("共%1条").arg(total));
    int pageCount = (total + m_nPageSize - 1) / m_nPageSize;
    if (pageCount != m_nPageCount)
    {
        m_nPageCount = pageCount;
        if (m_nPageNum >= m_nPageCount)
        {
            m_nPageNum = m_nPageCount;
            m_pLastPageBtn->setProperty("currentPage", "true");
            m_pLastPageBtn->style()->unpolish(m_pLastPageBtn);
            m_pLastPageBtn->style()->polish(m_pLastPageBtn);
            m_pLastPageBtn->update();
            //            m_pLastPageBtn->setStyleSheet("/**/");
            m_pNextPageBtn->setCheckable(false);
            m_pNextPageBtn->setDisabled(true);
        }
        else
        {
            m_pLastPageBtn->setProperty("currentPage", "false");
            //            m_pLastPageBtn->setStyleSheet("/**/");
            m_pLastPageBtn->style()->unpolish(m_pLastPageBtn);
            m_pLastPageBtn->style()->polish(m_pLastPageBtn);
            m_pLastPageBtn->update();
            m_pNextPageBtn->setCheckable(true);
            m_pNextPageBtn->setDisabled(false);
            m_pNextPageBtn->setCursor(Qt::PointingHandCursor);
        }
        if (m_nPageCount > 1)
        {
            m_pLastPageBtn->setVisible(true);
            QString maxPageNum = QString::number(m_nPageCount);
            m_pLastPageBtn->setText(maxPageNum);
            m_pLastPageBtn->setFixedWidth(35 + (maxPageNum.length() < 3 ? 0 : maxPageNum.length() - 3) * 12);
        }
        else
        {
            m_pLastPageBtn->setVisible(false);
        }
        updatePageBtn();
    }
}

/**
 * @brief PageControl::initPageBtnControl 初始化页码按钮部分控件
 */
void PageWidget::initPageBtnControl()
{
    m_pPageBtnHLayout = new QHBoxLayout();
    m_pPageBtnHLayout->setSpacing(8);
    m_pPageBtnHLayout->setMargin(0);
    m_pPrePageBtn = initPushButton();
    m_pPrePageBtn->setText("<");
    m_pPageBtnHLayout->addWidget(m_pPrePageBtn);

    m_pFirstPageBtn = initPushButton();
    m_pFirstPageBtn->setText("1");
    m_pPageBtnHLayout->addWidget(m_pFirstPageBtn);

    m_pLeftMoreBtn = initPushButton();
    m_pLeftMoreBtn->setText("...");
    m_pPageBtnHLayout->addWidget(m_pLeftMoreBtn);

    for (int i = 0; i < 5; i++)
    {
        QPushButton* btn = initPushButton();
        m_pPageBtnHLayout->addWidget(btn);
        btn->setCursor(Qt::PointingHandCursor);
        m_MorePageBtnList << btn;
    }

    m_pRightMoreBtn = initPushButton();
    m_pRightMoreBtn->setText("...");
    m_pPageBtnHLayout->addWidget(m_pRightMoreBtn);

    m_pLastPageBtn = initPushButton();
    m_pPageBtnHLayout->addWidget(m_pLastPageBtn);

    m_pNextPageBtn = initPushButton();
    m_pNextPageBtn->setText(">");
    m_pPageBtnHLayout->addWidget(m_pNextPageBtn);

    m_pPageHLayout->addLayout(m_pPageBtnHLayout);
}

/**
 * @brief PageControl::initJumpControl 初始化跳转部分控件
 */
void PageWidget::initJumpControl()
{
    m_pJumpHLayout = new QHBoxLayout();
    m_pJumpHLayout->setSpacing(5);
    m_pJumpHLayout->setMargin(0);
    m_pGoToLabel = new QLabel(this);
    m_pGoToLabel->setText(" 前往");
    m_pGoToLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_pGoToLabel->setFixedHeight(35);
    m_pGoToLabel->setStyleSheet("QLabel {"
                            "  border: none;"
                            "}");
    m_pJumpHLayout->addWidget(m_pGoToLabel);

    m_pPageLineEdit = new QLineEdit(this);
    m_pPageLineEdit->setFixedSize(50, 35);
    m_pPageLineEdit->setAlignment(Qt::AlignHCenter);
    m_pPageLineEdit->setValidator(new QIntValidator(1, 10000000, this));
    m_pPageLineEdit->installEventFilter(this);
    m_pJumpHLayout->addWidget(m_pPageLineEdit);

    m_pPageUnitLabel = new QLabel(this);
    m_pPageUnitLabel->setText("页     ");
    m_pPageUnitLabel->setStyleSheet("QLabel {"
                            "  border: none;"
                            "}");
    m_pPageUnitLabel->setFixedHeight(35);
    m_pJumpHLayout->addWidget(m_pPageUnitLabel);

    m_pPageHLayout->addLayout(m_pJumpHLayout);
}


/**
 * @brief PageControl::updatePageBtn 设置中间数字按钮
 * @param showLeftMore 是否显示左边更多
 * @param showRightMore 是否显示右边更多
 */
void PageWidget::updatePageBtn()
{
    m_bShowLeftMoreBtn = false;
    m_bShowRightMoreBtn = false;
    int beginPageNum = 2;
    if (m_nPageCount > 7)
    {
        if (m_nPageNum - 1 < m_nPageCount - m_nPageNum)
        {
            int rightAddIndex = 1;
            if (m_nPageNum > 4)
            {
                m_bShowLeftMoreBtn = true;
                beginPageNum = m_nPageNum - 1;
            }
            else
            {
                rightAddIndex += (4 - m_nPageNum);
                beginPageNum = 2;
            }
            m_bShowRightMoreBtn = true;
        }
        else
        {
            int leftSubIndex = 1;
            if (m_nPageCount - m_nPageNum > 3)
            {
                m_bShowRightMoreBtn = true;
            }
            else
            {
                leftSubIndex += (3 - (m_nPageCount - m_nPageNum));
            }
            m_bShowLeftMoreBtn = true;
            beginPageNum = m_nPageNum - leftSubIndex;
        }
    }
    m_pLeftMoreBtn->setVisible(m_bShowLeftMoreBtn);
    m_pRightMoreBtn->setVisible(m_bShowRightMoreBtn);
    int showBtnSize = m_MorePageBtnList.size() - m_bShowLeftMoreBtn - m_bShowRightMoreBtn;
    for (int i = 0; i < m_MorePageBtnList.size(); i++)
    {
        if (i < showBtnSize && beginPageNum + i < m_nPageCount)
        {
            QString pageNum = QString::number(beginPageNum + i);
            m_MorePageBtnList[i]->setText(pageNum);
            m_MorePageBtnList[i]->setVisible(true);
            m_MorePageBtnList[i]->setFixedWidth(35 + (pageNum.length() < 3 ? 0 : pageNum.length() - 3) * 12);
            if (beginPageNum + i == m_nPageNum)
            {
                m_MorePageBtnList[i]->setProperty("currentPage", "true");
            }
            else
            {
                m_MorePageBtnList[i]->setProperty("currentPage", "false");
            }
            //            m_MorePageBtnList[i]->setStyleSheet("/**/");
            m_MorePageBtnList[i]->style()->unpolish(m_MorePageBtnList[i]);
            m_MorePageBtnList[i]->style()->polish(m_MorePageBtnList[i]);
            m_MorePageBtnList[i]->update();
        }
        else
        {
            m_MorePageBtnList[i]->setVisible(false);
        }
    }
}

QPushButton* PageWidget::initPushButton()
{
    QPushButton* pushButton = new QPushButton(this);
    pushButton->setFixedSize(35, 35);
    pushButton->setCursor(Qt::PointingHandCursor);
    pushButton->installEventFilter(this);
    return pushButton;
}

/**
 * @brief PageControl::eventFilter
 * @param watched
 * @param e
 * @return
 */
bool PageWidget::eventFilter(QObject* watched, QEvent* e)
{
    if (e->type() == QEvent::MouseButtonRelease)
    {
        //上一页按钮被点击
        if (watched == m_pPrePageBtn && m_pPrePageBtn->isCheckable())
        {
            setCurrentPage(m_nPageNum - 1);
        }
        //下一页按钮被点击
        else if (watched == m_pNextPageBtn && m_pNextPageBtn->isCheckable())
        {
            setCurrentPage(m_nPageNum + 1);
        }
        //左边省略号按钮被点击
        else if (watched == m_pLeftMoreBtn)
        {
            setCurrentPage(m_MorePageBtnList.at(1)->text().toInt() - 2);
        }
        //右边省略号按钮被点击
        else if (watched == m_pRightMoreBtn)
        {
            setCurrentPage(m_MorePageBtnList.at(2)->text().toInt() + 2);
        }
        else if (watched == m_pFirstPageBtn)
        {
            setCurrentPage(1);
        }
        else if (watched == m_pLastPageBtn)
        {
            setCurrentPage(m_nPageCount);
        }
        else
        {
            for (int i = 0; i < m_MorePageBtnList.size(); ++i)
            {
                if (watched == m_MorePageBtnList.at(i))
                {
                    setCurrentPage(m_MorePageBtnList.at(i)->text().toInt());
                    break;
                }
            }
        }
    }
    //跳转页敲击回车事件
    if (watched == m_pPageLineEdit && e->type() == QEvent::KeyRelease)
    {
        QKeyEvent* ke = reinterpret_cast<QKeyEvent*>(e);
        if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return)
        {
            setCurrentPage(m_pPageLineEdit->text().toInt());
            return true;
        }
    }
    return QWidget::eventFilter(watched, e);
}

/**
 * @brief PageControl::~PageControl 析构函数
 */
PageWidget::~PageWidget()
{
    SAFE_DELETE(m_pTotalLabel);
    SAFE_DELETE(m_pPrePageBtn);
    SAFE_DELETE(m_pNextPageBtn);
    SAFE_DELETE(m_pFirstPageBtn);
    SAFE_DELETE(m_pLastPageBtn);
    SAFE_DELETE(m_pLeftMoreBtn);
    SAFE_DELETE(m_pRightMoreBtn);
    foreach(QPushButton * pBtn, m_MorePageBtnList)
    {
        SAFE_DELETE(pBtn);
    }
    SAFE_DELETE(m_pPageBtnHLayout);

    //跳转元素控件
    SAFE_DELETE(m_pGoToLabel);
    SAFE_DELETE(m_pPageLineEdit);
    SAFE_DELETE(m_pGotoBtn);
    SAFE_DELETE(m_pPageUnitLabel);
    SAFE_DELETE(m_pJumpHLayout);

    SAFE_DELETE(m_pPageHLayout);
}

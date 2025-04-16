#ifndef PAGECONTROL_H
#define PAGECONTROL_H
#include <QList>
#include <QLabel>
#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>


class PageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PageWidget(QList<quint32> list, QWidget* parent = 0);
    ~PageWidget();
    void initPage(int total, int pageNum = 1);
    void setTotal(int total);
    void setCurrentPage(int pageNum = 1);
    int currentPageIndex();
    quint32 pageSize();
protected:
    virtual bool eventFilter(QObject* watched, QEvent* e);
    void setCurrentPageInternal(int pageNum = 1);
Q_SIGNALS:
    //页码切换消息
    void pageChanged(int currentPage);
    void pageSizeChanged(int pageSize);

private:
    void initJumpControl();
    void initPageBtnControl();
    void updatePageBtn();
    QPushButton* initPushButton();

private:
    QHBoxLayout* m_pPageHLayout;
    QLabel* m_pTotalLabel;                  //显示总数据条数
    QComboBox* m_itemCountPerPageCombo = nullptr;
    QHBoxLayout* m_pPageBtnHLayout;
    QPushButton* m_pPrePageBtn;             //上一页
    QPushButton* m_pNextPageBtn;            //下一页
    QPushButton* m_pFirstPageBtn;           //首页按钮
    QPushButton* m_pLastPageBtn;            //最后一页按钮
    QPushButton* m_pLeftMoreBtn;            //左边省略号按钮
    bool m_bShowLeftMoreBtn;
    QPushButton* m_pRightMoreBtn;           //右边省略号按钮
    bool m_bShowRightMoreBtn;
    QList<QPushButton*> m_MorePageBtnList;  //中间数字按钮
    //跳转元素控件
    QHBoxLayout* m_pJumpHLayout;
    QLabel* m_pGoToLabel;
    QLineEdit* m_pPageLineEdit;
    QPushButton* m_pGotoBtn;
    QLabel* m_pPageUnitLabel;
    QList<quint32> m_itemCountPerPage;

    int m_nPageNum;
    int m_nPageSize;
    int m_nPageCount;
    int m_nTotal;
    int m_nMidBtnNum;
};
#endif // PAGECONTROL_H

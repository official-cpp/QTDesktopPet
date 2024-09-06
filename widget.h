#ifndef WIDGET_H
#define WIDGET_H

#include <QMouseEvent>
#include <QWidget>
#include <QDebug>
#include <QMovie>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>

#include "lResources.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    QMovie *m_movie = nullptr;
    bool m_movie_lock = false;
    //鼠标触碰检测
    void MoveCurPos(bool _enter);
    //置系统托盘
    bool ShowTraymenu(const QString& _icon);
protected:
    QPoint m_moveWinodw={};
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    //标签事件
    bool eventFilter(QObject *watched, QEvent *event) override;
    //滚轮事件，控制人物大小
    void wheelEvent(QWheelEvent *event) override;
private:
    //锁定对话
    void MsgLock(bool _lock);
    //判断对话锁定状态
    bool IsMsgLock();
private:
    ENUM_PET_STATUS m_petStatus = ENUM_PET_STATUS::_DAIJI;
    void SetPetStatus(ENUM_PET_STATUS _status);
    ENUM_PET_STATUS GetPetStatus();
    void SwirchPetStatus();//可以主动控制
    void MovieLock(bool _look);
    bool IsMovieLook();
    bool MovieReStart(const QString& _gif,bool _lock = false);
    QTimer m_msg_timer;
    void ShowMsg(const QString& _msg,int _timeOut,bool _lock = false/*当前消息超时后自动解锁*/,bool _breakLock = false);
    int GetQStringListAlign(const QStringList& _list)const;
    int m_shakingCount = 0;//晃动次数临时记录
    const int m_shakingCountMAX = 6;//最大晃动次数
    bool m_msg_lock = false;//消息锁
};
#endif // WIDGET_H

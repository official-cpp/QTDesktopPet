#include "widget.h"
#include "ui_widget.h"
#include <QDateTime>
#include <QRandomGenerator>
#include "namespace.h"
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setLayout(ui->gridLayout);

    this->setFixedSize(QSize(300,300));
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint); //取消任务栏和边框
    this->setWindowFlags (windowFlags () | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    // 设置QLabel自动缩放内容以适应其大小
    ui->label->setScaledContents(true);

    // 连接定时器的timeout信号到槽函数
    QObject::connect(&m_msg_timer, &QTimer::timeout, [&]() {
        if(m_msg_timer.isActive())
            m_msg_timer.stop();
        this->MsgLock(false);
        this->m_shakingCount = 0;
        ui->label_msg->hide();
    });
    //显示启动语录
    this->ShowMsg(lResources::P()->GetStartMsg(),lResources::P()->GetSleepTimeLength(5000,8000));

    //初始化待机状态
    this->SetPetStatus(ENUM_PET_STATUS::_DAIJI);
    m_movie = new QMovie(lResources::P()->GetResDirPath() + "/daiji/daiji_1.gif", QByteArray(), this);
    ui->label->setMovie(m_movie);
     connect(m_movie, &QMovie::frameChanged, this, [this](int frameNumber) {
         if (frameNumber == (m_movie->frameCount() - 1)) {
             //解除动作锁
             this->MovieLock(false);
             //触发动作选项
             this->SwirchPetStatus();
         }
     }); m_movie->start();

     //安装窗口事件
    //this->setMouseTracking(true); // 启用鼠标跟踪.
    ui->label->installEventFilter(this);

    //置托盘
    this->ShowTraymenu(":/model/logo.ico");

    //移动宠物到右下角
    QRect rect = qAPI::_system::GetScreenWH();
    this->move(rect.width() - this->width(),rect.height() - this->height());
}

Widget::~Widget()
{
    delete ui;
}

void Widget::MoveCurPos(bool _enter)
{
    if(_enter){
        this->m_shakingCount++;//牛逼 晃到越界
    }
    //摸摸脚丫才转向
    QPoint pos = this->cursor().pos() - this->pos();
       if(pos.y() > this->height() - this->height() / 3){
           if(pos.x() > this->width() / 2){
               bool ok = this->MovieReStart(lResources::P()->GetResDirPath() + "/follow/to_right.gif");//左右操作固定
               if(ok)this->SetPetStatus(ENUM_PET_STATUS::_CHANGE_DIRECT_right);
           }else{
               bool ok = this->MovieReStart(lResources::P()->GetResDirPath() + "/follow/to_left.gif");//左右操作固定
               if(ok)this->SetPetStatus(ENUM_PET_STATUS::_CHANGE_DIRECT_left);
           }

           //检测摸摸次数
           if(this->m_shakingCount > this->m_shakingCountMAX && this->m_shakingCount <= 18){
               this->ShowMsg(u8"摸我脚丫丫干嘛？",2000);
           }else if(this->m_shakingCount > 18 && this->m_shakingCount <= 30){
               this->ShowMsg(u8"你再摸  我不理你了~~~",5000);
           }else if(this->m_shakingCount > 30 && this->m_shakingCount <= 50){
               this->ShowMsg(u8"别摸了。。。冷静10秒",10000,true);
           }
           else if(this->m_shakingCount > 50 && this->m_shakingCount < 100){
               this->ShowMsg(u8"臭猪人，我yao跟你 en短一jo.",10000);
           }
           else if(this->m_shakingCount > 100){
               this->ShowMsg(u8"一天不要理你\nbye-bye~~~",20000,true);//锁住20秒，此消息会一直显示
               //锁住动作
               this->MovieLock(true);//最后一个动作后自动解锁
           }
       }else{
           //上半身雄触发
           if(!this->IsMovieLook()){
               this->MovieReStart(lResources::P()->GetResDirPath() + "/chupeng/chupeng_0.gif",true);
               this->ShowMsg(u8"走开 ! ! !",3000,true);
           }
       }

}

bool Widget::ShowTraymenu(const QString &_icon)
{
        // 创建系统托盘图标
         QSystemTrayIcon* m_trayIcon = new QSystemTrayIcon(this);
         m_trayIcon->setIcon(QIcon(_icon)); // 设置图标路径
         // 创建菜单
         QMenu *trayMenu = new QMenu(this);

         // 添加动作
         //QAction *viewWindow = new QAction("ShowWindow", this);
         QAction *quitAction = new QAction("Quit", this);

         //connect(viewWindow, &QAction::triggered, this, &Widget::MinWindow);
         connect(quitAction, &QAction::triggered, this, [=](){
             this->close();
             QCoreApplication::exit(0);//强制退出
         });

         //trayMenu->addAction(viewWindow);
         trayMenu->addAction(quitAction);

         // 设置菜单
         m_trayIcon->setContextMenu(trayMenu);

         // 显示托盘图标
         m_trayIcon->show();

         return true;
    }

void Widget::mousePressEvent(QMouseEvent *event)
{
    static int pressCount = 0;
    static qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
    static qint64 timeEnd = timeStart;
    timeEnd = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if((timeEnd - 5000) < timeStart){
        pressCount++;
        if(pressCount > 8){
            this->ShowMsg(u8"晕了...",3000, true,true);
            pressCount = 0;
            timeStart = timeEnd;
        }
    }else{
        pressCount = 0;
        timeStart = timeEnd;
    }
    m_moveWinodw = event->globalPos() - this->pos();
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    m_moveWinodw = QPoint();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{

    this->ShowMsg(u8"要带我去哪里?",5000);

    this->move(event->globalPos() - this->m_moveWinodw);
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
       //qDebug() << event->type();
       if(watched->objectName() == "label"){
           if (event->type() == QEvent::Enter) {
               //qDebug() << "Enter";
               this->MoveCurPos(true);
               return false;

           }else if (event->type() == QEvent::Leave){
                //qDebug() << "Leave";
                this->MoveCurPos(false);
               return false;
           }

       }

       // 对于不感兴趣的事件，调用基类的eventFilter方法
       return QObject::eventFilter(watched, event);
   }

void Widget::wheelEvent(QWheelEvent *event)
{
    // 输出滚轮事件的详细信息
    //qDebug() << "Mouse wheel event detected at:" << event->pos() << "with delta:" << event->delta();
    //event->delta() == 120
    static int width = 300;
    static int height = 300;
    if(event->delta() > 0){
        //放大,默认最大600
        if(width >= 700) return;
        width+=10;height+=10;
        this->ShowMsg("大大大大大大...",1000,false,true);
    }else{
        if(width <= 300) return;
       width-=10;height-=10;
       this->ShowMsg("小小小小小小...",1000,false,true);
    }
    this->setFixedSize(QSize(width,height));
    //QWidget::wheelEvent(event);
}

void Widget::MsgLock(bool _lock)
{
    this->m_msg_lock = _lock;
}

bool Widget::IsMsgLock()
{
    return this->m_msg_lock;
}

void Widget::SetPetStatus(ENUM_PET_STATUS _status)
{
    this->m_petStatus = _status;
}
ENUM_PET_STATUS Widget::GetPetStatus()
{
    return this->m_petStatus;
}

void Widget::SwirchPetStatus()
{
    if(this->m_shakingCount > this->m_shakingCountMAX){
        m_movie->setFileName(lResources::P()->GetGif(ENUM_PET_STATUS::_DAIJI));
        return;
    }
    switch (this->GetPetStatus()) {
    case ENUM_PET_STATUS::_DAIJI:{
        //m_movie->setFileName(lResources::P()->GetGif(ENUM_PET_STATUS::_DAIJI));

        this->MovieReStart(lResources::P()->GetGif(ENUM_PET_STATUS::_DAIJI));
        this->ShowMsg(lResources::P()->GetMsg_daiji(),lResources::P()->GetSleepTimeLength(5000,10000));

        //this->SetPetStatus(ENUM_PET_STATUS::_DAIJI);//测试待机

         this->SetPetStatus(((lResources::P()->GetSleepTimeLength(0,100) % 3) ? ENUM_PET_STATUS::_DONGZUO : ENUM_PET_STATUS::_DAIJI));//待机过后随机

        break;
    }
    case ENUM_PET_STATUS::_DONGZUO:{
         this->SetPetStatus(ENUM_PET_STATUS::_DAIJI);//动作过后必然待机

        this->MovieReStart(lResources::P()->GetGif(ENUM_PET_STATUS::_DONGZUO));

        this->ShowMsg(lResources::P()->GetMsg_dongzuo(),lResources::P()->GetSleepTimeLength(10000,25000));
        break;
    }
    case ENUM_PET_STATUS::_CHANGE_DIRECT_left:
        this->SetPetStatus(ENUM_PET_STATUS::_DAIJI);

        this->ShowMsg(u8"一二一...!",3000);
        this->MovieReStart(lResources::P()->GetResDirPath() + "/follow/left_to_just.gif");
        break;
    case ENUM_PET_STATUS::_CHANGE_DIRECT_right:
        this->SetPetStatus(ENUM_PET_STATUS::_DAIJI);

        this->ShowMsg(u8"看我华丽的转身!",3000);
        this->MovieReStart(lResources::P()->GetResDirPath() + "/follow/right_to_just.gif");
        break;
    default:{

        break;
    }
    }

}

void Widget::MovieLock(bool _look)
{
    this->m_movie_lock = _look;
}

bool Widget::IsMovieLook()
{
    return this->m_movie_lock;
}

bool Widget::MovieReStart(const QString &_gif,bool _lock)
{
    if(IsMovieLook()){
        return false;
    }
    this->m_movie_lock = _lock;

    if(this->m_movie->isValid()){
        this->m_movie->stop();
    }
    this->m_movie->setFileName(_gif);

    this->m_movie->start();
    return true;
}

void Widget::ShowMsg(const QString &_msg,int _timeOut,bool _lock, bool _breakLock)
{
    if(this->IsMsgLock()){
        if(_breakLock == false){
             return;
        }

    }
    this->MsgLock(_lock);

    //定位显示消息
    ui->label_msg->move(this->width() / 2 - ui->label_msg->width() / 2, 0);
    //显示消息
    ui->label_msg->setText(_msg);
    //停止消息定时
    if(m_msg_timer.isActive()){
        m_msg_timer.stop();
    }
    ui->label_msg->show();
    // 启动定时器
    m_msg_timer.start();
    // 设置定时器的超时时间为5秒
    m_msg_timer.setInterval(_timeOut);

}

int Widget::GetQStringListAlign(const QStringList &_list) const
{
    int randNum = QRandomGenerator::global()->bounded(_list.size());
    return (randNum % 2) ? randNum - 1 : randNum;

}

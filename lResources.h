#ifndef LRESOURCES_H
#define LRESOURCES_H

#include <QString>
#include <QDir>
#include <QStringList>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QDebug>
enum class ENUM_PET_STATUS
{
    _DAIJI,_DONGZUO,_GENSUI,_CHANGE_DIRECT_left,_CHANGE_DIRECT_right
};

class lResources
{
    lResources(){
        this->Init_Gif();
        qDebug() <<"Init_Gif().Ok";

    }
private:
    const QString m_dirPath = "./model";                        //资源路径
public:
    static lResources* P();                                     //静态实例
    const QString& GetResDirPath();                             //获取资源路径
    void getGifFilesFromDirectory(const QString &directoryPath,const QString &_filters, QStringList &_buffer);  //gif文件过滤器
    void Init_Gif();                                            //初始化gif数据
    QString GetGif(ENUM_PET_STATUS _type,int _index = -1);      //获取gif下标
    int GetSleepTimeLength(int _min = 3000,int _max = 17000);   //随机时间
    const QString& GetStartMsg();                               //启动语录
    const QString& GetMsg_daiji();                              //待机语录
    const QString& GetMsg_shaking();                            //晃动、抚摸
    const QString& GetMsg_dongzuo();                            //动作
private:
    QStringList m_gif_daiji;                                    //待机动作列表
    QStringList m_gif_dongzuo;                                  //跳舞等其他动作列表

private:

};

#endif // LRESOURCES_H

#ifndef NAMESPACE_H
#define NAMESPACE_H

#include <QBuffer>
#include <QSharedMemory>
#include <QString>

namespace qAPI {
    namespace _system {
        //设置程序开机自启动
        bool SetStartup(const QString &applicationName, bool startup, const QString& _executablePath = "", const QString & _cmd = "");
        //判断是否已经设置为开启启动
        bool IsStartupRegistered(const QString &applicationName);
        //设置当前程序开机启动
        bool SetCurrentAutoStart(const QString& _appName, bool _start = false,const QString& _cmd = "");
        //判断当前程序是否已经运行
        bool IsRun(const QString& _appName);
        //获取屏幕宽高
        QRect GetScreenWH();
    }
    namespace _file {
        //获取目录下所有指定后辍的文件
        void GetFilesFromDirectory(const QString &directoryPath,const QString &_filters, QStringList &_buffer);
    }
        //共享内存类
    namespace _memory{

        class SharedMenory{
            public:
                SharedMenory();
                ~SharedMenory();
            public:
                bool Init(const QString& _name,int _size);
                bool SetData(const QByteArray &_data,int _size);
                bool GetData(QByteArray &_data,int _size);
                static bool GetUserData(const QString& _name, QByteArray &_data,int _size);
            private:
                QSharedMemory* m_sharedMemory = nullptr;
        };
    }
}
#endif // NAMESPACE_H

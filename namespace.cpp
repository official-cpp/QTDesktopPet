#include "namespace.h"

#include <QCoreApplication>
#include <QDir>
#include <QGuiApplication>
#include <QMessageBox>
#include <QScreen>
#include <QSettings>
#include <QSharedMemory>

namespace qAPI {
    namespace _system {
        bool SetStartup(const QString &applicationName, bool startup, const QString & _executablePath, const QString & _cmd) {
            QSettings settings("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
            //Software\\Microsoft\\Windows\\CurrentVersion\\Run"
            if (startup) {
                settings.setValue(applicationName, "\"" + _executablePath + "\" " + _cmd);
                return qAPI::_system::IsStartupRegistered(applicationName);
            } else {
                settings.remove(applicationName);
                return !qAPI::_system::IsStartupRegistered(applicationName);
            }
        }
        bool IsStartupRegistered(const QString &applicationName) {
            QSettings settings("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
            return settings.contains(applicationName);
        }
        bool SetCurrentAutoStart(const QString& _appName, bool _start,const QString& _cmd){
			if (!_start) {
                qAPI::_system::SetStartup(_appName,false);
                return !qAPI::_system::IsStartupRegistered(_appName);
			} else {
				QString path = QCoreApplication::applicationFilePath();
				path.replace("/","\\");
                qAPI::_system::SetStartup(_appName,true,path,_cmd);
                return qAPI::_system::IsStartupRegistered(_appName);
			}
		}
        bool IsRun(const QString& _appName){
            QSharedMemory sharedMemory;
            sharedMemory.setKey(_appName);
            if (!sharedMemory.create(1)) {
                //QMessageBox::information(nullptr, "Warning", "The application is already running.");
                return true; // Exit the application
            }
            return false;
        }
        QRect GetScreenWH(){
            // 获取主屏幕
            QScreen *screen = QGuiApplication::primaryScreen();
            if (screen) {
                return screen->geometry();
            }
            return QRect();
        }
    }
    namespace _file {
        void GetFilesFromDirectory(const QString &directoryPath,const QString &_filters, QStringList &_buffer) {
                QDir directory(directoryPath);
                // 按文件名排序
                directory.setSorting(QDir::Name);
                // 设置过滤器，只选择文件且扩展名为.gif
                directory.setNameFilters(QStringList() << ("*" + _filters));
                directory.setFilter(QDir::Files);
                // 遍历目录中的所有文件
                foreach (QFileInfo fileInfo, directory.entryInfoList()) {
                    _buffer << fileInfo.absoluteFilePath(); // 将文件的绝对路径添加到QStringList中
                }
            }
    }
    namespace _memory {
        SharedMenory::SharedMenory(){

        }
        SharedMenory::~SharedMenory(){
            if(this->m_sharedMemory != nullptr)
            // 释放共享内存
            this->m_sharedMemory->detach();
            this->m_sharedMemory = nullptr;
        }
        bool SharedMenory::Init(const QString &_name, int _size)
        {
            if(_size < 1 || _name.isEmpty()){
                return false;
            }
            if(this->m_sharedMemory == nullptr){
                this->m_sharedMemory = new QSharedMemory(_name);
                if(this->m_sharedMemory == nullptr){
                    return false;
                }
                return this->m_sharedMemory->create(_size);
            }
            return false;
        }
        bool SharedMenory::SetData(const QByteArray &_data,int _size){
            QBuffer buffer;
            if(buffer.open(QBuffer::ReadWrite) == false){
                return false;
            }
            QDataStream out(&buffer);
            out << _data;
            int size = _size;//buffer.size();
            if(size < 1){
                size = buffer.size();
            }
            this->m_sharedMemory->lock();
            char *to = (char*)this->m_sharedMemory->data();
            memcpy(to, buffer.data().data(), size);
           this->m_sharedMemory->unlock();
        }
        bool SharedMenory::GetData(QByteArray &_data,int _size){
            this->m_sharedMemory->lock();
            if(this->m_sharedMemory->size() >= _size){
                _data.setRawData((char*)this->m_sharedMemory->constData(), _size);
            }else{
                this->m_sharedMemory->unlock();
                return false;
            }
            return this->m_sharedMemory->unlock();
        }

        bool SharedMenory::GetUserData(const QString& _name, QByteArray &_data,int _size){
            QSharedMemory sharedMemory(_name);
                if (!sharedMemory.attach()) {
                    //qDebug() << "Unable to attach to shared memory:" << sharedMemory.errorString();
                    return false;
                }

                // 从共享内存读取数据
                QBuffer buffer;
                QDataStream in(&buffer);
                if(sharedMemory.lock()){
                    buffer.setData((char*)sharedMemory.constData(), sharedMemory.size());
                    buffer.open(QBuffer::ReadOnly);
                    in >> _data;
                    return sharedMemory.unlock();
                }
                return false;
        }
    }


}

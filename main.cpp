/******************************************************************************************
**  Copyright 2023 Hangzhou Zhicun (Witmem) Technology Co., Ltd.  All rights reserved.   **
**                                                                                       **
**  This program is free software: you can redistribute it and/or modify                 **
**  it under the terms of the GNU General Public License as published by                 **
**  the Free Software Foundation, either version 3 of the License, or                    **
**  (at your option) any later version.                                                  **
**                                                                                       **
**  This program is distributed in the hope that it will be useful,                      **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of                       **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                        **
**  GNU General Public License for more details.                                         **
**                                                                                       **
**  You should have received a copy of the GNU General Public License                    **
**  along with this program.  If not, see http://www.gnu.org/licenses/.                  **
*******************************************************************************************/

#include "HearingAidvMainWindow.h"
#include <QApplication>
#include <QFile>
#include<QTranslator>
#include<QDateTime>
#include<QMutex>
#include<QDir>
#include<QTextStream>
#include <QTextCodec>

#define LOG_FILE     qApp->applicationDirPath()+"/logs"
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();
    QString text;
    switch (type) {
    case QtDebugMsg:
        text = QString("Debug:");
        break;
    case QtWarningMsg:
        text = QString("Warning:");
        break;
    case QtInfoMsg:
        text = QString("Info:");
        break;
    case QtCriticalMsg:
        text = QString("Critical:");
        break;
    case QtFatalMsg:
        text = QString("Fatal:");
    }
    if(type != QtWarningMsg){
        QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
        QString current_date = QString("[%1]").arg(current_date_time);
        QString current_Info = QString("Info:[%1]").arg(msg);
        QString message = QString("%1\t%2\t%4").arg(current_date).arg(text).arg(current_Info);
        QDir logDir(LOG_FILE);
        if (!logDir.exists()) {
            logDir.mkpath(LOG_FILE);
        }
        QString logPath = LOG_FILE +"/"+QString(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh").append("_log.txt"));
        QFile file(logPath);
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream text_stream(&file);
        text_stream << message << "\r\n";
        file.flush();
        file.close();
    }
    mutex.unlock();
}

void initUniversalErrorCode(){
    UniversalErrorCode.insert(0x01,"暂未定义");
    UniversalErrorCode.insert(0x02,"暂未定义");
    UniversalErrorCode.insert(0x03,"校验错误");
    UniversalErrorCode.insert(0x04,"超时错误");
    UniversalErrorCode.insert(0x05,"不支持此命令");
    UniversalErrorCode.insert(0x06,"参数超出范围");
    UniversalErrorCode.insert(0x07,"无相应权限");
    UniversalErrorCode.insert(0x08,"命令帧数据载荷长度异常");
}
void initUpdaSpecialErrorCode(){
    UpdaSpecialErrorCode =
    {
        {INST_CMD_0x00,
         {{0XF0, "密码错误"},
          {0XF1, "目标权限未设置密码"}}},
        {INST_CMD_0x01,
         {{0XF0, "旧密码错误"}}},
        {INST_CMD_0x02,
         {{0XF1, "目标权限未设置密码"}}},
        {INST_CMD_0x03,
         {{0XF2, "目标权限已设置初始密码"},
          {0XF0, "未解除写保护"}}},
        {INST_CMD_0x20,
         {{0XF0, "未解除写保护"}}},
        {INST_CMD_0x21,
         {{0XF0, "未解除写保护"}}},
        {INST_CMD_0x25,
         {{0XF0, "未解除写保护"}}},
    };
}
void initCtrlCmdSpecialErrorCode(){

    CtrlCmdSpecialErrorCode =
    {
        {INST_CMD_0x00,
         {{0XF0, "在本次调节之前已经达到最大值"}}},
        {INST_CMD_0x01,
         {{0XF0, "在本次调节之前已经达到最小值"}}},
        {INST_CMD_0x02,
         {{0XF0, "指定的音量值超出配置范围"}}},
    };
}
void initCfgCmdSpecialErrorCode(){

}
void initAlgeriaCmdSpecialErrorCode(){

}
void  initMuteStatus(){

    MuteStatus.insert(0x00,"已静音");
    MuteStatus.insert(0x01,"未静音");

}

void initAlgFileTypeMap(){
    algFileTypeMap.insert(0x00,"模式1");
    algFileTypeMap.insert(0x01,"模式2");
    algFileTypeMap.insert(0x02,"模式3");
    algFileTypeMap.insert(0x03,"模式4");
}

void initCurrentWorkingMode(){
    WorkingMode.insert(0x00,"助听模式");
    WorkingMode.insert(0x01,"通话模式");
    WorkingMode.insert(0x02,"音乐模式");
    WorkingMode.insert(0x03,"音乐+助听模式");
}

void initFileTypeMap(){
   FileTypeMap.insert(0x00,"固件");
   FileTypeMap.insert(0x01,"模式");
   FileTypeMap.insert(0x02,"配置");
   FileTypeMap.insert(0x03,"记忆");
}
void Setstyle(const QString &styleName)
{
    QFile file(QString(":/Resource/qss/%1").arg(styleName));
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
}


int main(int argc, char *argv[])
{
    qInstallMessageHandler(outputMessage);
    initUniversalErrorCode();
    initUpdaSpecialErrorCode();
    initCtrlCmdSpecialErrorCode();
    initMuteStatus();
    initCurrentWorkingMode();
    initFileTypeMap();
    initAlgFileTypeMap();

    QApplication a(argc, argv);
    CHearingAidvMainWindow w;
    Setstyle("CustomStyle.qss");
    QFont font;
    font.setPixelSize(12);
    qApp->setFont(font);
    w.setWindowTitle(QString("知存助听器控制台_v%1").arg(APP_VERSION));
    w.setWindowIcon(QIcon(":/Resource/icon/logo.png"));
    w.show();
    return a.exec();
}

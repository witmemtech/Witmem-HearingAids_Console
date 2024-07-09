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

#include "SerialWork.h"
#include <QWaitCondition>

SerialWork::SerialWork()
{
    if(m_pSerialIo == nullptr){
        m_pSerialIo = new QSerialPort();
        connect(m_pSerialIo,&QSerialPort::readyRead,this,[=]() {
            QByteArray data = m_pSerialIo->readAll();
            if(data.isEmpty()){
                emit signaShowMesgbox("数据为空");
                return;
            }
            emit signaShowMegDevStatus("已连接");
            parseData(data);
        });
        connect(m_pSerialIo,&QSerialPort::errorOccurred,[=](QSerialPort::SerialPortError error){
            if(error == QSerialPort::PermissionError){
                m_pSerialIo->close();
                emit signaShowMesgbox("串口被拔出");
                emit signaSerialportStateChanged(false);
            }
        });
    }
    qRegisterMetaType<QVector<uchar>>("QVector<uchar>&");

    if(nullptr == m_pTimeOutTimer){
        m_pTimeOutTimer = new QTimer();
        connect(m_pTimeOutTimer, &QTimer::timeout, this,[=](){
            emit signaShowMesgbox("设备无响应");
            emit signaShowMegDevStatus("已掉线");
            m_bWaitingForResponse = false;
            m_qCommandQueue.clear();
        });
    }
}

SerialWork::~SerialWork()
{
    if(m_pSerialIo){
        if(m_pSerialIo->isOpen()){
            m_pSerialIo->clear();
            m_pSerialIo->close();
        }

        delete m_pSerialIo;
        m_pSerialIo= nullptr;
    }
    if(m_pTimeOutTimer){
        m_pTimeOutTimer->stop();
        m_pTimeOutTimer->deleteLater();
    }
}
bool SerialWork::getSerialIsoOpen()
{
    return m_pSerialIo ->isOpen();
}

void SerialWork::startWriteTimer()
{
    m_pTimeOutTimer->setSingleShot(true);
    m_pTimeOutTimer->start(3000);
}

void SerialWork::stopWriteTimer()
{
    if(m_pTimeOutTimer->isActive()){
        m_pTimeOutTimer->stop();
    }
}
void SerialWork::slotInitSerial(QString port, int baudRate)
{
    slotCloseSerial();
    m_pSerialIo->setPortName(port);
    if(!m_pSerialIo->open(QIODevice::ReadWrite)){
        emit signaSerialportStateChanged(false);
        emit signaShowMesgbox("串口打开失败");
        return;
    }
    emit signaSerialportStateChanged(true);

    m_pSerialIo->setBaudRate(baudRate);
    m_pSerialIo->setDataBits(QSerialPort::Data8);
    m_pSerialIo->setParity(QSerialPort::NoParity);
    m_pSerialIo->setStopBits(QSerialPort::TwoStop);
    m_pSerialIo->setFlowControl(QSerialPort::NoFlowControl);
    emit signalInitUIData(m_pSerialIo->isOpen());
}

void SerialWork::slotCloseSerial()
{
    if (m_pSerialIo->isOpen()){
        m_pSerialIo->clear();
        m_pSerialIo->close();
        emit signaSerialportStateChanged(false);
    }
}

void SerialWork::parseData(QByteArray data)
{
    int length = data.length();
    int i = 0;
    if(length <= 0){
        return;
    }
    while(length--){
        unsigned char ch = data.at(i++);
        switch(m_parseBuf.ParseType){
        case StateIdle:
            if(ch == '*'){
                m_parseBuf.ParseType = StateRecvLen;
                m_parseBuf.bodyLen = 0;
                m_parseBuf.headLen = 0;
                m_parseBuf.crcLen = 0;
                m_parseBuf.msgidLen = 0;
                m_parseBuf.extLen = 0;
                m_parseBuf.extBufLen = 0;
                m_parseBuf.allLen = 0;
                m_parseBuf.bufBody.clear();
                m_parseBuf.bufExt.clear();
            }
            break;

        case StateRecvLen:
            m_parseBuf.bufHead[m_parseBuf.headLen++] = ch;
            if(m_parseBuf.headLen == 2){
                m_parseBuf.allLen = (m_parseBuf.bufHead[1]<<8) + m_parseBuf.bufHead[0];
                if(m_parseBuf.allLen <= 11){
                    m_parseBuf.ParseType = StateIdle;
                    break;
                }else{
                    m_parseBuf.ParseType = StateRecvExtFlag;
                }
            }
            break;

        case StateRecvExtFlag:
            m_parseBuf.extFlag = ch;
            if(ch == 0x00){
                m_parseBuf.ParseType = StateRecvCMD;
            }
            if(ch == 0x01){
                m_parseBuf.ParseType = StateRecvExtension;
            }
            break;

        case StateRecvExtension:
            m_parseBuf.bufExt.append(ch);
            m_parseBuf.extLen = (m_parseBuf.bufExt[1]<<8) + m_parseBuf.bufExt[0];
            if(m_parseBuf.extLen == m_parseBuf.extBufLen){
                m_parseBuf.ParseType = StateRecvCMD;
            }
            break;

        case StateRecvCMD:
            m_parseBuf.cmd = ch;
            m_parseBuf.ParseType = StateRecvSubCMD;
            break;
        case StateRecvSubCMD:
            m_parseBuf.subCmd = ch;
            m_parseBuf.ParseType = StateRecvMsgid;
            break;
        case StateRecvMsgid:
            m_parseBuf.bufMsgid[m_parseBuf.msgidLen++] = ch;
            if(m_parseBuf.msgidLen == 2){
                m_parseBuf.ParseType = StateRecvBody;
                m_sSerialReturnMsgID = (m_parseBuf.bufMsgid[1]<<8) + m_parseBuf.bufMsgid[0];
                if(m_parseBuf.allLen - m_parseBuf.extBufLen - 11 == 0){
                    m_parseBuf.ParseType = StateRecvCRC;
                }
            }
            break;

        case StateRecvBody:
            m_parseBuf.bufBody.append(ch);
            m_parseBuf.bodyLen = m_parseBuf.bufBody.length();
            if(m_parseBuf.bodyLen == m_parseBuf.allLen - m_parseBuf.extLen - 11){
                m_parseBuf.ParseType = StateRecvCRC;
            }
            break;

        case StateRecvCRC:
            if(m_parseBuf.crcLen != 2){
                m_parseBuf.bufCRC[m_parseBuf.crcLen++] = ch;
            }
            if(m_parseBuf.crcLen == 2){
                m_parseBuf.ParseType = StateRecvEnd;
            }
            break;

        case StateRecvEnd:
            if(ch == '#'){
                ushort RecvCRC = (m_parseBuf.bufCRC[1]<<8) + m_parseBuf.bufCRC[0];
                QByteArray arr;
                arr.append(0x2a);
                arr.append(reinterpret_cast<const char *>(m_parseBuf.bufHead), m_parseBuf.headLen);
                arr.append(m_parseBuf.extFlag);
                if(m_parseBuf.extFlag == 1){
                    arr.append(reinterpret_cast<const char *>(m_parseBuf.bufExt.data()), m_parseBuf.extLen);
                }
                arr.append(m_parseBuf.cmd);
                arr.append(m_parseBuf.subCmd);
                arr.append(reinterpret_cast<const char *>(m_parseBuf.bufMsgid), m_parseBuf.msgidLen);
                arr.append(reinterpret_cast<const char *>(m_parseBuf.bufBody.data()), m_parseBuf.bodyLen);
                ushort calcCRC;
                if(m_parseBuf.extFlag == 0){
                    calcCRC = crc16(arr.data(), m_parseBuf.bodyLen + 8);
                }else{
                    calcCRC = crc16(arr.data(), m_parseBuf.bodyLen + 8 + m_parseBuf.extLen);
                }
                if(calcCRC == RecvCRC){
                    m_parseBuf.ParseType = StateIdle;
                    saveData(m_parseBuf,m_sSerialReturnMsgID);
                }
            }
            break;

        default:
            break;
        }
    }
}

QString SerialWork::numToString(QVector<uchar> &vec,int nSpaceType)
{
    QString strUiDiaplyText = "";
    for (int i = 1; i < vec.size(); ++i) {
        uchar byte = vec[i];
        QString byteStr = QString("%1"). arg(byte,2,16,QLatin1Char('0')).toUpper();
        strUiDiaplyText += byteStr;
        if(nSpaceType == DEFAULT_ADD_SPACE){
            strUiDiaplyText += ' ';
        }
    }
    return strUiDiaplyText;
}

QByteArray SerialWork::intToArry16(int nData)
{
    int16_t dataSize = (nData -1) & 0xFFFF;
    int16_t *p = &dataSize;
    qToBigEndian(dataSize, (uchar*)p);
    QByteArray sendFileSize = QByteArray::fromRawData(reinterpret_cast<const char *>(p),2);
    QByteArray RawDataArry;
    RawDataArry.append(sendFileSize);
    return RawDataArry;
}

void SerialWork::enqueueCommand(const QByteArray &command)
{
    m_qCommandQueue.enqueue(command);
    if (!m_bWaitingForResponse){
        sendNextCommand();
    }
}

void SerialWork::sendNextCommand()
{
    if (!m_qCommandQueue.isEmpty() && m_pSerialIo->isOpen()){
        m_arraySendCommand = m_qCommandQueue.dequeue();
        m_bWaitingForResponse = true;
        uchar byte1 = m_arraySendCommand.at(6);
        uchar byte2 = m_arraySendCommand.at(7);
        m_sSendMsgID = (byte2 << 8) | byte1;
        m_pSerialIo->write(m_arraySendCommand);
        emit signaleStartTimer();
    }else {
        emit signalCMDAllFinshed();
        m_bWaitingForResponse = false;
        return;
    }
}

void SerialWork::saveData(ParseBuffer m_parseBuf, ushort recvMsgid)
{
    if(recvMsgid != m_sSendMsgID + 1){
        emit signaShowMesgbox("id不符");
        m_bWaitingForResponse = false;
        m_qCommandQueue.clear();
        emit signaleStopTimer();
        return;
    }
    if(m_pTimeOutTimer->isActive()){
        emit signaleStopTimer();
    }
    uchar retChar = m_parseBuf.bufBody[0];
    if(retChar != 0){
        if(m_parseBuf.cmd == CMD_ALG_0x02 && m_parseBuf.subCmd == INST_CMD_0x01){
            emit  signalShowChannelCountDefault();
        }else{
            m_qCommandQueue.clear();
            m_bWaitingForResponse = false;
        }
    }
    QString strBoyData;
    QByteArray FileCrcArray;
    emit signalOperationResult(m_parseBuf.cmd,m_parseBuf.subCmd,retChar);
    if(0 == retChar){
        if(m_parseBuf.cmd == CMD_UPDATE_0x03){
            switch(m_parseBuf.subCmd){
            case INST_CMD_0x00:
                emit signalFlashData(strBoyData,PASSWORD_PERMISSION);
                break;
            case INST_CMD_0x02:
                emit signalFlashData(strBoyData,CLEAR_PASSWORD);
                break;
            case INST_CMD_0x21:
                emit signalCMDWriteFinshed(m_arraySendCommand.size() -19,WRITE_DATA);
                break;
            case INST_CMD_0x22:
                strBoyData = numToString(m_parseBuf.bufBody,DEFAULT_ADD_SPACE);
                if(1 == m_flashReadToUiOrFile){
                    emit signalFlashData(strBoyData,READ_DATA_UI);
                }else if(2 == m_flashReadToUiOrFile){
                    emit signalFlashData(strBoyData,READ_DATA_FILE);
                }
                emit signalCMDWriteFinshed(m_parseBuf.bufBody.size() -1,READ_DATA);
                break;
            case INST_CMD_0x24:
                for(int  i = 1; i < m_parseBuf.bufBody.size();i++){
                    FileCrcArray.append(m_parseBuf.bufBody.at(i));
                }

                std::reverse(FileCrcArray.begin(), FileCrcArray.end());
                strBoyData= "结果： 0x" + numToString(m_parseBuf.bufBody,NONE_SPACE);
                emit signalFlashData(strBoyData,GET_CHECK_RESULT);
                break;
            case INST_CMD_0x11:
                emit signalShowUpdateCmdReadData(FIRMWARE_FILE_HEAD,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x13:
                emit signalCMDWriteFinshed(m_arraySendCommand.size() -19,WRITE_DATA);
                break;
            case INST_CMD_0x15:
                emit signalCMDWriteFinshed(m_arraySendCommand.size() -19,WRITE_DATA);
                break;
            case INST_CMD_0x16:
                emit signalShowUpdateCmdReadData(CFG_FILE_HEAD,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x17:
                emit signalShowUpdateCmdReadData(CFG_FILE_READ_DATA,m_parseBuf.bufBody);
                emit signalCMDWriteFinshed(m_parseBuf.bufBody.size() -1,READ_DATA);
                break;
            case INST_CMD_0x19:
                emit signalCMDWriteFinshed(m_arraySendCommand.size() -19,WRITE_DATA);
                break;
            case INST_CMD_0x1A:
                emit signalShowUpdateCmdReadData(ALG_ARG_FILE_HEAD,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x1B:
                emit signalShowUpdateCmdReadData(ALG_ARG_FILE_READ_DATA,m_parseBuf.bufBody);
                emit signalCMDWriteFinshed(m_parseBuf.bufBody.size() -1,READ_DATA);
                break;
            }
        }else if(m_parseBuf.cmd == CMD_ALG_0x02){
            switch(m_parseBuf.subCmd){
            case INST_CMD_0x01:
                emit signalShowAlgCmdReadData(ALG_GET_WDRC_CHANNEL_COUNT,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x11:
                emit signalShowAlgCmdReadData(ALG_GET_WDRC_ARG,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x13:
                emit signalShowAlgCmdReadData(ALG_GET_WDRC_CT,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x15:
                emit signalShowAlgCmdReadData(ALG_GET_WDRC_CR,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x17:
                emit signalShowAlgCmdReadData(ALG_GET_WDRC_GAIN,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x19:
                emit signalShowAlgCmdReadData(ALG_GET_WDRC_MPO,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x21:
                emit signalShowAlgCmdReadData(ALG_GET_DENOISE_ARG,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x23:
                emit signalShowAlgCmdReadData(ALG_GET_DENOISE_LEVEL,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x31:
                emit signalShowAlgCmdReadData(ALG_GET_EQ_ARG,m_parseBuf.bufBody);
                break;
            }
        }else if(m_parseBuf.cmd == CMD_CTRL_0x01){
            uchar firstArg = 0xFF;
            uchar secondArg = 0xFF;
            switch(m_parseBuf.subCmd){
            case INST_CMD_0x03:
                firstArg  = m_parseBuf.bufBody[1] ;
                secondArg = m_parseBuf.bufBody[2];
                emit signalShowVolumeCurrNum(firstArg,secondArg,CURRENT_VOLUME_NUM);
                break;
            case INST_CMD_0x05:
                firstArg  = m_parseBuf.bufBody[1] & 0b1;
                secondArg = m_parseBuf.bufBody[1] >> 1 & 0b1;
                emit signalShowVolumeCurrNum(firstArg,secondArg,CURRENT_MUTE_TATUS);
                break;
            case INST_CMD_0x12:
                firstArg = m_parseBuf.bufBody[1];
                emit signalShowVolumeCurrNum(firstArg,secondArg,CURRENT_ALG_ARG_TYPE);
                break;

            case INST_CMD_0x14:
                firstArg = m_parseBuf.bufBody[1];
                emit signalShowVolumeCurrNum(firstArg,secondArg,CURRENT_WORKING_MODE);
                break;
            case INST_CMD_0x16:
                firstArg = m_parseBuf.bufBody[1];
                secondArg= m_parseBuf.bufBody[2];
                emit signalShowVolumeCurrNum(firstArg,secondArg,CURRENT_ALG_SWITCHBTN_STATUS);
                break;
            }
        }else if(m_parseBuf.cmd == CMD_CFG_0x00){
            switch(m_parseBuf.subCmd){
            case INST_CMD_0x01:
                emit signalShowCfgCmdReadData(CFG_VOLUME_MODE_VOLUMETYPE,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x03:
                emit signalShowCfgCmdReadData(CFG_VOLUME_KEY_DATA,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x05:
                emit  signalShowCfgCmdReadData(CFG_VOLUME_KNOB_DATA,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x15:
                emit signalShowCfgCmdReadData(CFG_PICKUP_ARG_MODE,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x17:
                emit  signalShowCfgCmdReadData(CFG_PLAYBACK_ARG_MODE,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x21:
                emit signalShowCfgCmdReadData(CFG_DEVIVE_NUM_MODE,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x23:
                emit signalShowCfgCmdReadData(CFG_ALG_ARG_COUNT,m_parseBuf.bufBody);
                break;
            case INST_CMD_0x25:
                emit signalShowCfgCmdReadData(CFG_AUDIO_COMPENSATION,m_parseBuf.bufBody);
                break;
            }
        }
    }
}

int SerialWork::processData(uchar command, uchar subCommand, QByteArray srcData)
{
    if(m_pSerialIo->isOpen() == false){
        emit signaShowMesgboxDialog("请先连接助听器");
        return -1;
    }
    emit signalShowNoOper();
    if(65534 == m_msgid)  m_msgid = 0;
    else    m_msgid += 2;
    uchar start = DATA_HEAD;
    ushort len = 0;
    uchar flag = 0;
    QByteArray extData;
    ushort CRC = 0;
    uchar uTail = DATA_TAIL;
    QByteArray packet;

    if(flag == 0){
        len = srcData.length() + 11;
    }

    QByteArray crcArr;
    crcArr.append(start);
    crcArr.append(QByteArray::fromRawData((char*)&len,2));
    crcArr.append(flag);
    if (flag == 1) {
        crcArr.append(extData);
    }
    crcArr.append(command);
    crcArr.append(subCommand);

    QByteArray byteArray = QByteArray::fromRawData(reinterpret_cast<const char*>(&m_msgid),2);
    crcArr.append(byteArray);
    if(!srcData.isNull()){
        crcArr.append(srcData);
    }
    CRC = crc16(crcArr.data(),crcArr.length());
    packet.append(crcArr);
    packet.append(QByteArray::fromRawData((char*)&CRC,2));
    packet.append(uTail);
    if (m_pSerialIo->isOpen()){
        enqueueCommand(packet);
    }
    return 0;
}

void SerialWork::slotFlashReadToUiOrFile(int flag)
{
    m_flashReadToUiOrFile = flag;
}

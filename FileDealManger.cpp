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

#include "FileDealManger.h"
#include <QFileInfo>
#include <QtEndian>
#include <QDebug>

FileDealManger::FileDealManger(const QString &strBinFilePath)
{
    m_strFilePath = strBinFilePath;
    getFileSourceData(m_strFilePath,&pSourceArry, m_nFileSizeTotal);
}

FileDealManger::FileDealManger()
{

}

FileDealManger::~FileDealManger()
{
    if(pSourceArry){
        delete pSourceArry;
        pSourceArry = nullptr;
    }
}

int FileDealManger::getFileSizeTotal()
{
    return m_nFileSizeTotal;
}

void FileDealManger::getFileSourceData(const QString &strBinFilePath, uchar **pData, int &nSize)
{
    if(strBinFilePath.isEmpty())
        return;
    QFile objectFile(strBinFilePath);
    if(!objectFile.exists()){
        return;
    }
    if(!objectFile.open(QIODevice::ReadOnly)){
        return;
    }
    QFileInfo fileInfo;
    QByteArray DataArry;
    QByteArray tailArry;
    fileInfo.setFile(strBinFilePath);

    uchar* pFileData = new uchar[fileInfo.size()];
    *pData = pFileData;
    if(fileInfo.suffix() == "bin"){
        objectFile.read((char *)pFileData, fileInfo.size());
        DataArry  =  QByteArray::fromRawData(reinterpret_cast<const char *>(pFileData),fileInfo.size());
    }else{
        QTextStream stream(&objectFile);
        QString text = stream.readAll();
        QStringList numbers = text.split(" ");
        for (const auto& number : numbers) {
            bool ok;
            int value = number.toInt(&ok,16);
            if (ok) {
                uchar byte = static_cast<uchar>(value);
                DataArry.append(byte);
            }
        }
    }

    tailArry.append(DataArry);
    memcpy(pFileData, tailArry,  tailArry.size());

    nSize =  fileInfo.size();
    objectFile.close();
}

void FileDealManger::getFilePiecewiseArry(int nFinalDataSize, int count, int tailCount, uchar **arry, int &nFullSize)
{
    if(nFinalDataSize > 0 && (count + 1)  == tailCount){
        memcpy(*arry,pSourceArry + count *WRITE_DATA_SIZE,nFinalDataSize);
        nFullSize = nFinalDataSize;
    }else{
        memcpy(*arry,pSourceArry + count *WRITE_DATA_SIZE,WRITE_DATA_SIZE);
        nFullSize = WRITE_DATA_SIZE;
    }
}

void FileDealManger::int32To8Byte(int nStartAddress, uchar *pStartAddressArry)
{
    pStartAddressArry[0] = (uchar)(nStartAddress >> 24);
    pStartAddressArry[1] = (uchar)(nStartAddress >> 16);
    pStartAddressArry[2] = (uchar)(nStartAddress >> 8);
    pStartAddressArry[3] = (uchar)(nStartAddress);
}

QByteArray FileDealManger::intToArry32(int nData)
{
    int *p  = &nData;
    QByteArray Arry = QByteArray::fromRawData(reinterpret_cast<const char *>(p),4);
    QByteArray RawDataArry;
    RawDataArry.append(Arry);
    return RawDataArry;
}

QByteArray FileDealManger::intToArry16(int nData)
{
    int16_t dataSize = (nData -1) & 0xFFFF;
    int16_t *p = &dataSize;
    QByteArray sendFileSize = QByteArray::fromRawData(reinterpret_cast<const char *>(p),2);
    QByteArray RawDataArry;
    RawDataArry.append(sendFileSize);
    return RawDataArry;
}

void FileDealManger::setDataSourceUi(uchar *pSourceDataArry)
{
    pSourceArry = pSourceDataArry;
}

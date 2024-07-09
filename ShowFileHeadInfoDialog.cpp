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

#include "ShowFileHeadInfoDialog.h"
#include "ui_ShowFileHeadInfoDialog.h"
#include <QDebug>
#include  "DatadefineHeader.h"

ShowFileHeadInfoDialog::ShowFileHeadInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowFileHeadInfoDialog)
{
    ui->setupUi(this);
    setWindowTitle("版本信息");
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setFixedSize(this->size().width(),this->size().height());
}

ShowFileHeadInfoDialog::~ShowFileHeadInfoDialog()
{
    delete ui;
}

void ShowFileHeadInfoDialog::setFileHeadInfoText(FileInfoHeadData InfoData)
{
    if(InfoData.nFileType == FIRMWARE_FILE_HEAD){
        ui->FirmWareFileCodeLabel->setText(InfoData.strFileCode);
        ui->FirmWareFileVersionLabel->setText(InfoData.strFileVersion);
        ui->FirmWareFileTimeLabel->setText(InfoData.strWriteTime);
    }else if(InfoData.nFileType == CFG_FILE_HEAD){
        ui->CfgFileCodeLabel->setText(InfoData.strFileCode);
        ui->CfgFileVersionLabel->setText(InfoData.strFileVersion);
        ui->CfgFileTimeLabel->setText(InfoData.strWriteTime);
    }else if(InfoData.nFileType == ALG_ARG_FILE_HEAD){
        if(InfoData.nFileIndex == 1){
            ui->Mode1FileCodeLabel->setText(InfoData.strFileCode);
            ui->Mode1FileVersionLabel->setText(InfoData.strFileVersion);
            ui->Mode1FileTimeLabel->setText(InfoData.strWriteTime);
        }else if(InfoData.nFileIndex == 2){
            ui->Mode2FileCodeLabel->setText(InfoData.strFileCode);
            ui->Mode2FileVersionLabel->setText(InfoData.strFileVersion);
            ui->Mode2FileTimeLabel->setText(InfoData.strWriteTime);
        }else if(InfoData.nFileIndex == 3){
            ui->Mode3FileCodeLabel->setText(InfoData.strFileCode);
            ui->Mode3FileVersionLabel->setText(InfoData.strFileVersion);
            ui->Mode3FileTimeLabel->setText(InfoData.strWriteTime);
        }else if(InfoData.nFileIndex == 4){
            ui->Mode4FileCodeLabel->setText(InfoData.strFileCode);
            ui->Mode4FileVersionLabel->setText(InfoData.strFileVersion);
            ui->Mode4FileTimeLabel->setText(InfoData.strWriteTime);
        }
    }
}

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

#include "FileHeadInfoDialog.h"
#include "ui_FileHeadInfoDialog.h"
#include <QDebug>

FileHeadInfoDialog::FileHeadInfoDialog(int nDialogType ,QWidget *parent) :QDialog(parent),
    ui(new Ui::FileHeadInfoDialog)
{
    ui->setupUi(this);
    m_nDialogType  = nDialogType;
    initUi();
}

FileHeadInfoDialog::~FileHeadInfoDialog()
{
    delete ui;
}

QString FileHeadInfoDialog::getFileVersionText()
{
    return ui->FileVersionLineEdit->text().trimmed();
}

QString FileHeadInfoDialog::getFileIdentificationCodeText()
{
    return ui->FileIdentificationCodeLineEdit->text().trimmed();
}

void FileHeadInfoDialog::setFileInitInfoText(QString FileVersion, QString FileCode)
{
    ui->FileVersionLineEdit->setText(FileVersion);
    ui->FileIdentificationCodeLineEdit->setText(FileCode);
}

void FileHeadInfoDialog::initUi()
{
    setWindowTitle("设置生产封包参数");
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->FileVersionLineEdit->setMaxLength(7);
    ui->FileIdentificationCodeLineEdit->setMaxLength(7);

    ui->FileVersionLineEdit->setPlaceholderText("格式：0.00.00");
    ui->errorShowLabel->setStyleSheet("color:red;");
    connect(ui->FileVersionLineEdit,&QLineEdit::textChanged,
            this,&FileHeadInfoDialog::on_FileIdentificationCodeLineEdit_textChanged);
    ui->FileVersionLineEdit->setText("0.00.00");
    ui->FileVersionLineEdit->setCursor(QCursor(Qt::IBeamCursor));
    if(m_nDialogType == FIRMWARE_FILE_FLAG){
        ui->FileVersionLineEdit->setDisabled(true);
    }else{
        ui->FileVersionLineEdit->setDisabled(false);
    }
}

void FileHeadInfoDialog::on_ConfirmBtn_clicked()
{
    if(ui->FileVersionLineEdit->text().isEmpty()||
            ui->FileIdentificationCodeLineEdit->text().isEmpty()){
        ui->errorShowLabel->setText("参数不得为空");
        return;
    }
    if(ui->FileVersionLineEdit->text().length() < 7
            || ui->FileIdentificationCodeLineEdit->text().length() < 7){
        ui->errorShowLabel->setText("数据长度不可小于7");
        if(ui->FileVersionLineEdit->text().length() < 7){

            ui->FileVersionLineEdit->setCursorPosition(ui->FileVersionLineEdit->text().length() + 1);
            ui->FileVersionLineEdit->setFocus();
        }else if(ui->FileIdentificationCodeLineEdit->text().length() < 7){
            ui->FileIdentificationCodeLineEdit->setCursorPosition(ui->FileIdentificationCodeLineEdit->text().length() + 1);
            ui->FileIdentificationCodeLineEdit->setFocus();
        }
        return;
    }
    emit signalSaveData(m_nDialogType,ui->FileVersionLineEdit->text(),ui->FileIdentificationCodeLineEdit->text());
    done(1);
}


void FileHeadInfoDialog::on_FileIdentificationCodeLineEdit_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->errorShowLabel->text().isEmpty()){
        ui->errorShowLabel->clear();
    }
}


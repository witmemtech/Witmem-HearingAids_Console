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

#ifndef FILEHEADINFODIALOG_H
#define FILEHEADINFODIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class FileHeadInfoDialog;
}
enum FileHeadType{
    FIRMWARE_FILE_FLAG = 0,
    ALG_FILE_FLAG,
    CFG_FILE_FLAG
};

class FileHeadInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileHeadInfoDialog(int nDialogType ,QWidget *parent = nullptr);
    ~FileHeadInfoDialog();
public:
    QString getFileVersionText();
    QString getFileIdentificationCodeText();
    void setFileInitInfoText(QString FileVersion, QString FileCode);
private:
    void  initUi();
signals:
    void signalSaveData(int  type,QString FileVersion, QString FileCode);
private slots:
    void on_ConfirmBtn_clicked();
    void on_FileIdentificationCodeLineEdit_textChanged(const QString &arg1);

private:
    Ui::FileHeadInfoDialog *ui;
    int m_nDialogType = 0;
};

#endif // FILEHEADINFODIALOG_H

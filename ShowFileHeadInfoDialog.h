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

#ifndef SHOWFILEHEADINFODIALOG_H
#define SHOWFILEHEADINFODIALOG_H
#include <QDialog>

namespace Ui {
class ShowFileHeadInfoDialog;
}

struct FileInfoHeadData{
    QString strFileCode;
    QString strFileVersion;
    int nFileIndex;
    int nFileType;
    QString  strWriteTime;

};

class ShowFileHeadInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShowFileHeadInfoDialog(QWidget *parent = nullptr);
    ~ShowFileHeadInfoDialog();
    void setFileHeadInfoText(FileInfoHeadData InfoData);
private:
    Ui::ShowFileHeadInfoDialog *ui;
};

#endif // SHOWFILEHEADINFODIALOG_H

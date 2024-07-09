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

#ifndef WDRCITEMWIDGET_H
#define WDRCITEMWIDGET_H
#include  <QFile>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QMessageBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QWidget>

namespace Ui {
class CWDRCItemWidget;
}

struct WDRCItemData
{
    QComboBox *pGainComBox;
    QComboBox *pCTComBox;
    QComboBox* pMPOCoBox;
    QDoubleSpinBox *pCRSpinBox;
    WDRCItemData() {
        pGainComBox = nullptr;
        pCTComBox = nullptr;
        pMPOCoBox = nullptr;
        pCRSpinBox = nullptr;
    }
};

class CWDRCItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CWDRCItemWidget(QWidget *parent = nullptr);
    ~CWDRCItemWidget();
    void setHeadWidgetHide(bool status);
    void setChecBoxLabelValue(QString str);
    void setObjName(int i);
    WDRCItemData getWDRCData();
    QCheckBox * getChannelSingleCheckBox();
private slots:
    void on_ChannelSelectcheckBox_clicked(bool checked);

public slots:
    void slotSetisSelectAllCheckBox(bool isChecked);
    void on_ChannelSelectcheckBox_stateChanged(int arg1);
signals:
    void signal_isSelectAllCheckBox(bool status);

private:
    void initUi();
private:
    Ui::CWDRCItemWidget *ui;
    WDRCItemData m_sWDRCData;
};

#endif // WDRCITEMWIDGET_H

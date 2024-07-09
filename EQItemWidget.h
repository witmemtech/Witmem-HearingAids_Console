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

#ifndef EQITEMWIDGET_H
#define EQITEMWIDGET_H
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QWidget>

struct EqItemData
{
    QLabel *pdBLabelValue;
    QSlider *pdBSliderValue;
    QCheckBox* pdBCheckBox;
    EqItemData() {
        pdBLabelValue = nullptr;
        pdBLabelValue = nullptr;
        pdBCheckBox = nullptr;
    }
};

namespace Ui {
class CEQItemWidget;
}

class CEQItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CEQItemWidget(QWidget *parent = nullptr);
    ~CEQItemWidget();
    void  initUi();
    void setHeadWidgetHide(bool status);
    void setHeadCheckBoxHide(bool status);
    void setChecBoxLabelString(QString str);
    EqItemData getEqData();
    QSlider* getSlider();
    void setEqLabelText(int value);
    QCheckBox *getSignalCheckBox();
    void setSliderObjName(int i);

private slots:
    void on_EQAllChannelSelectCheckBox_clicked(bool checked);
    void on_EQVerticalSlider_valueChanged(int value);
    void on_EQVerticalSlider_sliderReleased();
    void on_EQVerticalSlider_sliderPressed();
    void on_EQAllChannelSelectCheckBox_stateChanged(int arg1);

public slots:
    void slotSetEQIsSelectAllCheckBox(bool checked);
signals:
    void signalIsSelectAllCheckBox(bool status);
    void signalSendData();
    void signalSliderValue(int oldValue ,int newValue,QString name);
private:
    Ui::CEQItemWidget *ui;
    EqItemData m_EqData;
    int m_nOldSliderValue = 0;
    QString m_strSliderName;
};

#endif // EQITEMWIDGET_H

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

#include "EQItemWidget.h"
#include "ui_EQItemWidget.h"
#include <QDebug>

CEQItemWidget::CEQItemWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CEQItemWidget)
{
    ui->setupUi(this);
    initUi();
    m_EqData.pdBCheckBox = ui->EQChannelSinglecheckBox;
    m_EqData.pdBLabelValue = ui->EQVerticalSliderValueLabel;
    m_EqData.pdBSliderValue = ui->EQVerticalSlider;
    m_nOldSliderValue = ui->EQVerticalSlider->value();
}

CEQItemWidget::~CEQItemWidget()
{
    delete ui;
}

void CEQItemWidget::initUi()
{
    ui->EQChannelSinglecheckBox->setObjectName("EQChannelCheckBox");
    ui->HzLabel->setVisible(false);
    ui->dBLabel->setVisible(false);
    ui->labelAll->setVisible(false);
    ui->widgetAllCheckBoxWidget->setVisible(false);
    ui->EQVerticalSliderValueLabel->setText(QString("%1").arg(ui->EQVerticalSlider->value()));
    ui->EQVerticalSlider->setMinimum(-20);
    ui->EQVerticalSlider->setMaximum(0);
    ui->EQVerticalSlider->setSingleStep(1);
    ui->labelAll->setAlignment(Qt::AlignTop);
    QString strAllDesc = QString("全") + "\n" +  QString("选");
    ui->labelAll->setText(strAllDesc);
}

void CEQItemWidget::setHeadWidgetHide(bool status)
{
    ui->HzLabel->setVisible(status);
    ui->dBLabel->setVisible(status);
}

void CEQItemWidget::setHeadCheckBoxHide(bool status)
{
    ui->widgetAllCheckBoxWidget->setVisible(status);
    ui->labelAll->setVisible(status);
}

void CEQItemWidget::setChecBoxLabelString(QString str)
{
    ui->EQChannelNumLabel->setText(str);
}

EqItemData CEQItemWidget::getEqData()
{
    return m_EqData;
}

QSlider *CEQItemWidget::getSlider()
{
    return ui->EQVerticalSlider;
}

void CEQItemWidget::setEqLabelText(int value)
{
    ui->EQVerticalSliderValueLabel->setText(QString("%1").arg(value));
}

QCheckBox *CEQItemWidget::getSignalCheckBox()
{
    return ui->EQChannelSinglecheckBox;
}

void CEQItemWidget::setSliderObjName(int i)
{
    ui->EQVerticalSlider->setObjectName(QString("EQSlider_%1").arg(i));
}

void CEQItemWidget::on_EQAllChannelSelectCheckBox_clicked(bool checked)
{
    emit signalIsSelectAllCheckBox(checked);
}

void CEQItemWidget::slotSetEQIsSelectAllCheckBox(bool checked)
{
    ui->EQAllChannelSelectCheckBox->setChecked(checked);
}

void CEQItemWidget::on_EQVerticalSlider_valueChanged(int value)
{
    emit  signalSliderValue(m_nOldSliderValue, value,m_strSliderName);
    ui->EQVerticalSliderValueLabel->setText(QString("%1").arg(value));
    m_nOldSliderValue = value;
}

void CEQItemWidget::on_EQVerticalSlider_sliderReleased()
{

}

void CEQItemWidget::on_EQVerticalSlider_sliderPressed()
{
    m_strSliderName.clear();
    m_strSliderName = ui->EQVerticalSlider->objectName();
}

void CEQItemWidget::on_EQAllChannelSelectCheckBox_stateChanged(int arg1)
{
    QString strAllDesc = "";
    if(arg1 == 2){
        strAllDesc = QString("取") + "\n" + QString("消") + "\n" +
                QString("全") + "\n" + QString("选");
    }
    if(arg1 == 0){
        strAllDesc = QString("全") + "\n" +  QString("选");
    }
    ui->labelAll->setText(strAllDesc);
}


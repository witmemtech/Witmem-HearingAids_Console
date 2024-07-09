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

#include "WDRCItemWidget.h"
#include "ui_WDRCItemWidget.h"
#include <QDebug>
CWDRCItemWidget::CWDRCItemWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWDRCItemWidget)
{
    ui->setupUi(this);
    initUi();
}

CWDRCItemWidget::~CWDRCItemWidget()
{
    delete ui;
}

void CWDRCItemWidget::setHeadWidgetHide(bool status)
{
    ui->widget->setVisible(status);
    ui->label_CH->setVisible(status);
    ui->label_CT->setVisible(status);
    ui->label_CR->setVisible(status);
    ui->label_Gain->setVisible(status);
    ui->label_MPO->setVisible(status);
}

void CWDRCItemWidget::setChecBoxLabelValue(QString str)
{
    ui->checkbox_label->setText(str);
}

void CWDRCItemWidget::setObjName(int i)
{
    ui->WDRCGainBox->setObjectName(QString("GainComBox_%1").arg(i));
    ui->WDRC_CTBox->setObjectName(QString("CTComBox_%1").arg(i));
    ui->WDRC_MPOBox->setObjectName(QString("MPOComBox_%1").arg(i));
    ui->WDRC_CRSpinBox->setObjectName(QString("CRSpinBox_%1").arg(i));
}

WDRCItemData CWDRCItemWidget::getWDRCData()
{
    return m_sWDRCData;
}

QCheckBox *CWDRCItemWidget::getChannelSingleCheckBox()
{
    return ui->ChannelSinglecheckBox;
}

void CWDRCItemWidget::initUi()
{
    ui->ChannelSinglecheckBox->setObjectName("CHCheckBox");
    ui->widget->setVisible(false);
    ui->label_MPO->setVisible(false);
    ui->label_CH->setVisible(false);
    ui->label_CT->setVisible(false);
    ui->label_CR->setVisible(false);
    ui->label_Gain->setVisible(false);
    QStringList list;
    list.clear();

    for(int i = 60; i >= 0; i = i-1){
        QString str = QString::number(i,10);
        list.append(str);
    }
    ui->WDRCGainBox->addItems(list);
    list.clear();
    for(int i = 120; i >= 40; i = i-1){
        QString str = QString::number(i,10);
        list.append(str);
    }
    ui->WDRC_CTBox->addItems(list);
    list.clear();
    for(int i = 140; i >= 70; i = i-1){
        QString str = QString::number(i,10);
        list.append(str);
    }
    ui->WDRC_MPOBox->addItems(list);

    ui->WDRC_MPOBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    ui->WDRC_MPOBox->setMinimumContentsLength(10);
    ui->WDRCGainBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    ui->WDRCGainBox->setMinimumContentsLength(10);
    ui->WDRC_CTBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    ui->WDRC_CTBox->setMinimumContentsLength(10);

    ui->WDRCGainBox->setCurrentText("30");
    ui->WDRC_CTBox->setCurrentText("60");
    ui->WDRC_MPOBox->setCurrentText("100");

    m_sWDRCData.pGainComBox = ui->WDRCGainBox;
    m_sWDRCData.pCTComBox = ui->WDRC_CTBox;
    m_sWDRCData.pMPOCoBox = ui->WDRC_MPOBox;
    m_sWDRCData.pCRSpinBox = ui->WDRC_CRSpinBox;
}

void CWDRCItemWidget::on_ChannelSelectcheckBox_clicked(bool checked)
{
    emit  signal_isSelectAllCheckBox(checked);
}

void CWDRCItemWidget::slotSetisSelectAllCheckBox(bool isChecked)
{
    ui->ChannelSelectcheckBox->setChecked(isChecked);
}

void CWDRCItemWidget::on_ChannelSelectcheckBox_stateChanged(int arg1)
{
    if(arg1 == 2){
       ui->ChannelSelectcheckBox->setText("取消全选");
    }
    if(arg1 == 0){
       ui->ChannelSelectcheckBox->setText("全选");
    }
}


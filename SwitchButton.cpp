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


#include "switchbutton.h"

#include <QDebug>
#include <QPainter>
#include <QApplication>

SwitchButton::SwitchButton(QWidget *parent) :
    QWidget(parent)
{
    this->setFixedSize(QSize(44, 20));
    m_bIsChecked = false;
    m_strTextOn = "开";
    m_strTextOff = "关";

    m_borderColorOff = QColor(190,190,190);
    m_bgColorOff = QColor(190,190,190);
    m_bgColorOn = QColor(55,144,250,255);
    m_bgColorDisEnableOff = QColor(190,190,190,50);
    m_bgColorDisEnableOn =QColor(55,144,250,50);

    m_sliderColorOff = QColor("#ffffff");
    m_sliderColorOn = QColor("#ffffff");
    m_nSpace = 4;
    m_nStep = width() / 40;
    m_nStartX = 0;
    m_nEndX = 0;

    m_pTimer = new QTimer(this);
    m_pTimer->setInterval(5);

    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(updatevalue()));
}

SwitchButton::~SwitchButton()
{

}

void SwitchButton::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    drawBg(&painter);
    drawSlider(&painter);
}

void SwitchButton::drawBg(QPainter *painter){
    painter->save();
    if (!m_bIsChecked){
        painter->setPen(Qt::NoPen);
        if(this->isEnabled() == false){
            painter->setBrush(m_bgColorDisEnableOff);
        }else{
            painter->setBrush(m_bgColorOff);
        }
    }else{
        painter->setPen(Qt::NoPen);
        if(this->isEnabled() == false){
            painter->setBrush(m_bgColorDisEnableOn);
        }else{
           painter->setBrush(m_bgColorOn);
        }
    }
    QRect rect(0, 0, width(), height());
    int radius = rect.height() / 2;
    int circleWidth = rect.height();

    QPainterPath path;
    path.moveTo(radius, rect.left());
    path.arcTo(QRectF(rect.left(), rect.top(), circleWidth, circleWidth), 90, 180);
    path.lineTo(rect.width() - radius, rect.height());
    path.arcTo(QRectF(rect.width() - rect.height(), rect.top(), circleWidth, circleWidth), 270, 180);
    path.lineTo(radius, rect.top());

    painter->drawPath(path);

    int sliderWidth = qMin(height(), width()) - m_nSpace * 2 - 5;
    if (m_bIsChecked){
        QRect textRect(0, 0, width() - sliderWidth, height());
        painter->setPen(QPen(m_sliderColorOn));
        painter->drawText(textRect, Qt::AlignCenter, m_strTextOn);
    }else{
        QRect textRect(sliderWidth, 0, width() - sliderWidth, height());
        painter->setPen(QPen(m_sliderColorOn));
        painter->drawText(textRect, Qt::AlignCenter, m_strTextOff);
    }

    painter->restore();
}

void SwitchButton::drawSlider(QPainter *painter){
    painter->save();
    painter->setPen(Qt::NoPen);

    if (!m_bIsChecked){
        painter->setBrush(m_sliderColorOff);
    }else
        painter->setBrush(m_sliderColorOn);
    QRect rect(0, 0, width(), height());
    int sliderWidth = rect.height() - m_nSpace * 2;
    QRect sliderRect(m_nStartX + m_nSpace, m_nSpace, sliderWidth, sliderWidth);
    painter->drawEllipse(sliderRect);

    painter->restore();
}
void SwitchButton::resizeEvent(QResizeEvent *){
    m_nStep = width() / 40;

    if (m_bIsChecked){
        m_nStartX = width() - height();
    }else
        m_nStartX = 0;

    update();
}

void SwitchButton::updatevalue(){
    if (m_bIsChecked)
        if (m_nStartX < m_nEndX){
            m_nStartX = m_nStartX + m_nStep;
        }else{
            m_nStartX = m_nEndX;
            m_pTimer->stop();
        }else{
        if (m_nStartX > m_nEndX){
            m_nStartX = m_nStartX - m_nStep;
        }else{
            m_nStartX = m_nEndX;
            m_pTimer->stop();
        }
    }
    update();
}

void SwitchButton::setChecked(bool checked){
    if (this->m_bIsChecked != checked){
        this->m_bIsChecked = checked;
        update();
    }

    m_nStep = width() / 40;
    if (checked){
        m_nEndX = width() - height();
    }else{
        m_nEndX = 0;
    }
    m_pTimer->start();
}

bool SwitchButton::isChecked()
{
    return this->m_bIsChecked;
}

void SwitchButton::mouseReleaseEvent(QMouseEvent *event)
{
    (void)event;
    if (m_pTimer->isActive())
        return;

    m_bIsChecked = !m_bIsChecked;
    m_nStep = width() / 40;
    if (m_bIsChecked){
        m_nEndX = width() - height();
    }else{
        m_nEndX = 0;
    }
    emit checkedChanged(m_bIsChecked);
    m_pTimer->start();
}



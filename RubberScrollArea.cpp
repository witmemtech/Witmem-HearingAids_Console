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

#include "RubberScrollArea.h"

#include <QDebug>
#include <QScrollBar>
#include <QCheckBox>

RubberScrollArea::RubberScrollArea(QWidget* parent)
    :QScrollArea(parent)
{
    m_pRubberBand = nullptr;
}

RubberScrollArea::~RubberScrollArea()
{
    if (nullptr != m_pRubberBand) delete m_pRubberBand;
}

void RubberScrollArea::mousePressEvent(QMouseEvent* e)
{
    QPoint  origin = e->pos();

    if (!m_pRubberBand)
        m_pRubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    QRect rc = QRect(origin, QSize());
    m_pRubberBand->setGeometry(rc);
    m_pRubberBand->show();
    QWidget* pw = this->widget();
    QPoint ptScreen = this->mapToGlobal(e->pos());
    m_qOriginWidget = pw->mapFromGlobal(ptScreen);
}


void RubberScrollArea::moveRubberRange(QMouseEvent* e)
{
    int height_srcollArea = this->geometry().height();
    int deta = e->pos().y() - height_srcollArea;
    if (deta > 0){
        QWidget* pw = this->widget();
        QPoint ptScreen;
        ptScreen = pw->mapToGlobal(m_qOriginWidget);

        QPoint  origin = this->mapFromGlobal(ptScreen);
        QRect rc = QRect(origin, e->pos()).normalized();
        int widget_h = pw->height();
        int shit_pos = deta * widget_h / height_srcollArea;
        QPoint ptTL = rc.topLeft();
        ptScreen = this->mapToGlobal(ptTL);
        ptTL = pw->mapFromGlobal(ptTL);
        ptTL.setY(ptTL.y() - shit_pos);

        ptScreen = pw->mapToGlobal(ptTL);
        ptTL = this->mapFromGlobal(ptScreen);
        rc.setTopLeft(ptTL);
        m_pRubberBand->setGeometry(rc.normalized());
    }
}

void RubberScrollArea::mouseMoveEvent(QMouseEvent* e)
{
    if (m_pRubberBand){
        int max_scroll = this->verticalScrollBar()->maximum();
        QWidget* pw = this->widget();
        QPoint ptScreen;
        ptScreen = pw->mapToGlobal(m_qOriginWidget);
        QPoint  origin = this->mapFromGlobal(ptScreen);
        QRect rc = QRect(origin, e->pos()).normalized();
        int height_srcollArea = this->geometry().height();
        int deta = e->pos().y() - height_srcollArea;
        if (deta > 0){
            int shit_scroll_pos = deta * max_scroll / height_srcollArea;
            int curpos = this->verticalScrollBar()->sliderPosition();
            int target_pos = curpos + shit_scroll_pos;
            if (target_pos >= max_scroll){
                target_pos = max_scroll;
            }
            this->verticalScrollBar()->setSliderPosition(target_pos);
        }else if (e->pos().y() < 0){
            int shit_scroll_pos = e->pos().y() * max_scroll / height_srcollArea;
            int curpos = this->verticalScrollBar()->sliderPosition();
            int target_pos = curpos + shit_scroll_pos;
            if (target_pos <= 0){
                target_pos = 0;
            }
            this->verticalScrollBar()->setSliderPosition(target_pos);
        }
        m_pRubberBand->setGeometry(rc.normalized());
    }
}

void RubberScrollArea::mouseReleaseEvent(QMouseEvent* e)
{
    QWidget* pw = this->widget();
    QPoint pt1, pt2;
    pt1 = pw->mapToGlobal(m_qOriginWidget);
    pt2 = mapToGlobal(e->pos());
    QRect rct = QRect(pt1, pt2).normalized();
    bool is_in = false;
    for (auto p : m_checkList){
        QRect pbox_rc = p->geometry();
        QPoint pt_box_tf, pt_box_br;
        pt_box_tf = p->mapToGlobal(pbox_rc.topLeft());
        pt_box_br = p->mapToGlobal(pbox_rc.bottomRight());
        pbox_rc.setTopLeft(pt_box_tf);
        pbox_rc.setBottomRight(pt_box_br);
        if (rct.contains(pbox_rc)){
            is_in = true;
            break;
        }
    }

    if(is_in == false){
        m_pRubberBand->hide();
        return;
    }

    int  count = 0;
    for (auto p : m_checkList){
        QRect pbox_rc = p->geometry();
        QPoint pt_box_tf, pt_box_br;
        pt_box_tf = p->mapToGlobal(pbox_rc.topLeft());
        pt_box_br = p->mapToGlobal(pbox_rc.bottomRight());
        pbox_rc.setTopLeft(pt_box_tf);
        pbox_rc.setBottomRight(pt_box_br);
        if (rct.contains(pbox_rc)){
            p->setCheckState(Qt::Checked);
            count++;
        }else{
            p->setCheckState(Qt::Unchecked);
        }
    }

    if(count == m_checkList.size()){
        emit signalSelectAllcheckbox(true);
    }else{
        emit signalSelectAllcheckbox(false);
    }

    if (m_pRubberBand)
        m_pRubberBand->hide();
}

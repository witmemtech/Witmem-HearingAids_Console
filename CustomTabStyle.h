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

#ifndef CUSTOMTABSTYLE_H
#define CUSTOMTABSTYLE_H

#include <QtGui>
#include<QProxyStyle>
#include<QStyleOptionTab>
class CustomTabStyle : public QProxyStyle
{
public:
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                           const QSize &size, const QWidget *widget) const{
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == QStyle::CT_TabBarTab){
            s.transpose();
            s.rwidth() = 90;
            s.rheight() = 30;
        }
        return s;
    }
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        if (element == CE_TabBarTabLabel){
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)){
                QRect allRect = tab->rect;
                allRect.setWidth(allRect.width()/* - 5*/);
                allRect.setHeight(allRect.height() /*- 2*/);
                if (tab->state & QStyle::State_Selected){
                    painter->save();
                    painter->setPen(QColor("#3790FA"));
                    painter->setBrush(QBrush(0x00AEFF));
                    painter->drawRect(allRect.adjusted(0, 0, 0, 0));
                    QTextOption option;
                    option.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                    painter->setPen(QColor("#FFFFFF"));
                    painter->setBrush(QBrush("#3790FA"));

                    painter->drawText(QRect(allRect.x()+20,allRect.y(),allRect.width()-5,allRect.height()), tab->text, option);
                    painter->setBrush(QBrush("#3790FA"));
                    painter->restore();
                }else if(tab->state & QStyle::State_MouseOver){
                    painter->save();
                    painter->setBrush(QBrush(0xBEBEBE));
                    painter->setPen(Qt::NoPen);
                    painter->drawRect(allRect);
                    QTextOption option;
                    option.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                    painter->setPen(QColor("#000000"));
                    painter->drawText(QRect(allRect.x()+20,allRect.y(),allRect.width()-5,allRect.height()), tab->text, option);
                    painter->restore();
                }else{
                    painter->save();
                    painter->setBrush(QBrush(0xFFFFFF));
                    painter->setPen(Qt::NoPen);
                    painter->drawRect(allRect);
                    QTextOption option;
                    option.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                    painter->setPen(QColor("#000000"));
                    painter->drawText(QRect(allRect.x()+20,allRect.y(),allRect.width()-5,allRect.height()), tab->text, option);
                    painter->restore();
                }
                return;
            }
        }
        if (element == CE_TabBarTab){
            QProxyStyle::drawControl(element, option, painter, widget);
        }
    }
};
#endif // CUSTOMTABSTYLE_H

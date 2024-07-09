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


#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QPainterPath>
#include <QEvent>

class SwitchButton : public QWidget
{
    Q_OBJECT

public:
    SwitchButton(QWidget *parent = 0);
    ~SwitchButton();

    void setChecked(bool checked);
    bool isChecked();
protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);
    void drawBg(QPainter * painter);
    void drawSlider(QPainter * painter);
private:
    bool m_bIsChecked;
    QColor m_borderColorOff;
    QColor m_bgColorOff;
    QColor m_bgColorOn;
    QColor m_bgColorDisEnableOff;
    QColor m_bgColorDisEnableOn;
    QColor m_sliderColorOff;
    QColor m_sliderColorOn;

    int m_nSpace;
    int m_nRectRadius;

    int m_nStep;
    int m_nStartX;
    int m_nEndX;

    QTimer *m_pTimer;
    QString m_strTextOn;
    QString m_strTextOff;

private slots:
    void updatevalue();

Q_SIGNALS:
    void checkedChanged(bool checked);

};

#endif // SWITCHBUTTON_H

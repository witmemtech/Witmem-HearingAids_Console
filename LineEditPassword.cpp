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

#include "LineEditPassword.h"
LineEditPassword::LineEditPassword(QWidget *parent):QLineEdit(parent)
{
    this->setEchoMode(EchoMode::Password);
    this->setStyleSheet("QLineEdit{lineedit-password-character: 42 }");
    m_pPwdIsShowAct = new QAction(QIcon(":/Resource/icon/password_hide.png"), "",this);
    this->addAction(m_pPwdIsShowAct, QLineEdit::TrailingPosition);
    connect(m_pPwdIsShowAct, SIGNAL(triggered(bool)), this, SLOT(slotPwdIsShowActTriggered(bool)));
}

void LineEditPassword::slotPwdIsShowActTriggered(bool)
{
    if(this->echoMode() == EchoMode::Password){
        this->setEchoMode(EchoMode::Normal);
        m_pPwdIsShowAct->setIcon(QIcon(":/Resource/icon/password_show.png"));
    }else if (this->echoMode() == EchoMode::Normal){
        this->setEchoMode(EchoMode::Password);
        m_pPwdIsShowAct->setIcon(QIcon(":/Resource/icon/password_hide.png"));
    }
}

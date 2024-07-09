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

#include "HexTextEdit.h"
#include <QDebug>

HexTextEdit::HexTextEdit(QWidget *parent):QTextEdit(parent)
{
    setAttribute(Qt::WA_InputMethodEnabled, false);
}


void HexTextEdit::keyPressEvent(QKeyEvent *event)
{
    QString input = event->text().toUpper();
    if (input.contains(QRegExp("[0-9A-F]"))) {
        QTextCursor cursor = textCursor();
        QTextCharFormat charFormat;
        charFormat.setForeground(Qt::black);
        setCurrentCharFormat(charFormat);

        int   row = cursor.blockNumber();
        QString str = this->document()->findBlock(row).text();
        if(str.length() < 3 && str.length() == 2){
            insertPlainText(" ");
        }else if(str.length() > 3 && str.length() % 3 == 2){
            insertPlainText(" ");
        }
        insertPlainText(event->text().toUpper());

    }else if(event->key() == Qt::Key_Backspace){
        QTextCursor cursor = textCursor();
        cursor.deletePreviousChar();
        QString text = toPlainText();
        if (text.endsWith(" ")) {
            cursor.deletePreviousChar();
        }
    }else if(event->key() == Qt::Key_C && event->modifiers() == Qt::CTRL){
        QTextCursor cursor = this->textCursor();
        QString selectedText = cursor.selectedText();
        QApplication::clipboard()->setText(selectedText);
    }else if(event->key() == Qt::Key_V && event->modifiers() == Qt::CTRL){
        QString clipboardText = QApplication::clipboard()->text().trimmed();
        QRegularExpression hexRegex("^[0-9A-Fa-f]*$");
        QStringList strTextList = clipboardText.split(" ");
        for(int  i = 0; i < strTextList.size(); i++){
            if (hexRegex.match(strTextList.at(i)).hasMatch() != true) {
               return;
            }
        }
        QTextCursor cursor = this->textCursor();
        int   row = cursor.blockNumber();
        QString str = this->document()->findBlock(row).text();
        if(str.length() < 3 && str.length() == 2){
            insertPlainText(" ");
        }else if(str.length() > 3 && str.length() % 3 == 2){
            insertPlainText(" ");
        }
        cursor.insertText(clipboardText);
    }else if(event->key() == Qt::Key_A && event->modifiers() == Qt::CTRL){
        selectAll();
    }else if(event->key() == Qt::Key_Delete){
        this->clear();
    }
}



QT       += core gui charts
QT       += serialport

VERSION = 0.0.31
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

#for Developer Version, if need normal version commented out below line
#DEFINES += DEVELOPER_VERSION

TARGET = HearingAids_Console
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ChangeTabOrder.cpp \
    ComboBoxCustom.cpp \
    CustomTabStyle.cpp \
    DatadefineHeader.cpp \
    EQItemWidget.cpp \
    FileDealManger.cpp \
    FileHeadInfoDialog.cpp \
    HearingAidvMainWindow.cpp \
    HexTextEdit.cpp \
    LineEditPassword.cpp \
    LineEditToolTip.cpp \
    RubberScrollArea.cpp \
    ShowFileHeadInfoDialog.cpp \
    WDRCItemWidget.cpp \
    main.cpp \
    SerialWork.cpp \
    SwitchButton.cpp

HEADERS += \
    ChangeTabOrder.h \
    ComboBoxCustom.h \
    CustomTabStyle.h \
    DatadefineHeader.h \
    EQItemWidget.h \
    FileDealManger.h \
    FileHeadInfoDialog.h \
    HearingAidvMainWindow.h \
    HexTextEdit.h \
    LineEditPassword.h \
    LineEditToolTip.h \
    RubberScrollArea.h \
    ShowFileHeadInfoDialog.h \
    WDRCItemWidget.h \
    SerialWork.h \
    SwitchButton.h

FORMS += \
    EQItemWidget.ui \
    FileHeadInfoDialog.ui \
    HearingAidvMainWindow.ui \
    ShowFileHeadInfoDialog.ui \
    WDRCItemWidget.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc

DISTFILES +=

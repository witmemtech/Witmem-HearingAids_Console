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

#include "HearingAidvMainWindow.h"
#include "ui_HearingAidvMainWindow.h"
#include "CustomTabStyle.h"
#include "WDRCItemWidget.h"
#include "EQItemWidget.h"
#include "ChangeTabOrder.h"

#include <QMetaType>
#include <QDebug>

CHearingAidvMainWindow::CHearingAidvMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CHearingAidvMainWindow)
{
    ui->setupUi(this);
    m_pSerialWork = SerialWork::instance();

    ui->DeviceModeNumLeftMicBox->setObjectName("MicLeft");
    ui->DeviceModeNumRightMicBox->setObjectName("Right");

    initConnects();
    initWDRCUi(WDRC_CHANNGL_COUNT);
    initCharts();
    initEQUi();
    initUi();
    initNoiseUi();
    initOther();
    initSerialDialog();
    on_IsFileFullSizecheckBox_clicked(false);
    initCalibrationValue();
    initUiReadJsonData();
    initTabFocusOrder();

    ui->WDRCwriteArg->hide();
    ui->WDRCReadArg->hide();
    ui->RestoreSerialBtn->hide();
    ui->tabWidget->setCurrentIndex(0);
    ui->errorMsgShowLabel->setText("无操作");
    ui->errorMsgShowLabel->setStyleSheet("");
    ui->isSuccessStatusShowWidget->setStyleSheet("border-radius:8px;background-color:gray;");
    setVolumeKeyModeShow();
    m_btnStatusMap.insert(ui->modeTypeBtn_0,ui->modeTypeBtn_0->isChecked());
    m_btnStatusMap.insert(ui->modeTypeBtn_1,ui->modeTypeBtn_1->isChecked());
    m_btnStatusMap.insert(ui->modeTypeBtn_2,ui->modeTypeBtn_2->isChecked());
    m_btnStatusMap.insert(ui->modeTypeBtn_3,ui->modeTypeBtn_3->isChecked());
    ui->comboBoxFre->hide();

#ifndef DEVELOPER_VERSION
    int index = ui->tabWidget->indexOf(ui->tab_Flash);
    if (index != -1) {
        ui->tabWidget->removeTab(index);
    }
#endif
}

CHearingAidvMainWindow::~CHearingAidvMainWindow()
{
    if(m_pArrySectionalData256){
        delete m_pArrySectionalData256;
        m_pArrySectionalData256 = nullptr;
    }
    delete ui;
}

void CHearingAidvMainWindow::initUi()
{
    initBtnStatus();
    m_pArrySectionalData256= new uchar[1024];
    initProcessProgressBar();

    ui->FlashReadFileNameLE->hide();
    ui->FlashFileReadBtn->hide();
    ui->modeTypeBtn_3->hide();

    ui->tabWidget->setTabPosition(QTabWidget::West);
    ui->tabWidget->tabBar()->setStyle(new CustomTabStyle);

    ui->FlashReadAddressLE->setMaxLength(11);
    ui->FlashFromAddressLE->setMaxLength(11);
    ui->FlashEraseAddressLE->setMaxLength(11);
    ui->FLashCopySrcAddressLE->setMaxLength(11);
    ui->FlashVerifyAddressLE->setMaxLength(11);
    ui->FlashCopyDestAddrLE->setMaxLength(11);

    ui->soundPresLE->setMaxLength(3);
    ui->soundFrequencyLE->setMaxLength(5);
    ui->soundNumsLE->setMaxLength(3);
    ui->soundContinueLE->setMaxLength(6);
    ui->soundCycleLE->setMaxLength(6);

    QRegExp regx("^0|[1-9][01]?|^[1-9]{2}$|^[1][0-9]{2}$|^[2][0-4][0-9]$|^[2][5][0-5]$");
    ui->soundNumsLE->setValidator(new QRegExpValidator(regx));
    ui->configFilePathEdit->setValidator(new QRegExpValidator(regx));

    QRegExp regx140("^(0|140|[1-9][0-3]?[0-9]?|[1-9]?)$");
    ui->soundPresLE->setValidator(new QRegExpValidator(regx140));

    QRegExp regxNext("^(20000|[2-9][0-9]{1,3}|1[0-9]{1,4})$");
    ui->soundFrequencyLE->setValidator(new QRegExpValidator(regxNext));

    QRegExp regxTail("^(0|[1-9]\\d{0,3}|[1-5]\\d{4}|6[0-4]\\d{3}|655[0-3][0-5])$");
    ui->soundContinueLE->setValidator(new QRegExpValidator(regxTail));
    ui->soundCycleLE->setValidator(new QRegExpValidator(regxTail));
    ui->WDRCRTLineEdit->setValidator(new QRegExpValidator(regxTail));
    ui->WDRCATLineEdit->setValidator(new QRegExpValidator(regxTail));

    QRegExp regArg("^-128|0|-?([1-9]|[1-9]\\d|1[01]\\d|12[0-7])$");
    ui->pickupLeftChannelDGAEdit->setValidator(new QRegExpValidator(regArg));
    ui->pickupRightChannelDGAEdit->setValidator(new QRegExpValidator(regArg));
    ui->pickupLeftChannelPGAEdit->setValidator(new QRegExpValidator(regArg));
    ui->pickupRightChannelPGAEdit->setValidator(new QRegExpValidator(regArg));
    ui->playbackArgLeftChannelDGAEdilt->setValidator(new QRegExpValidator(regArg));
    ui->playbackArgRightChannelDGAEdilt->setValidator(new QRegExpValidator(regArg));
    ui->NRNLICalibrationLeftLineEdit->setValidator(new QRegExpValidator(regArg));
    ui->NRNLICalibrationRightLineEdit->setValidator(new QRegExpValidator(regArg));

    QRegExp regxNum("^(?!0)\\d{1,9}$|^[1-3]\\d{9}$|^4([0-1]\\d{8}|2([0-8]\\d{7}|9([0-4]\\d{6}|95([0-4]\\d{5}|5([0-2]\\d{4}|3([0-5]\\d{3}|6([0-6]\\d{2}|7([0-2]\\d|3[0-5]))))))))$");
    ui->FlashReadLenthLE->setValidator(new QRegExpValidator(regxNum));
    ui->FlashFromLenthLE->setValidator(new QRegExpValidator(regxNum));
    ui->FlashVerifyLenthLE->setValidator(new QRegExpValidator(regxNum));
    ui->FlashCopyLenthLE->setValidator(new QRegExpValidator(regxNum));

    ui->FlashReadLenthLE->setMaxLength(10);
    ui->FlashFromLenthLE->setMaxLength(10);
    ui->FlashVerifyLenthLE->setMaxLength(10);
    ui->FlashCopyLenthLE->setMaxLength(10);

    QString strWriteDesc = "请选择一个文件";
    ui->FlashFromFileNameLE->setPlaceholderText(strWriteDesc);
    ui->configFilePathEdit->setPlaceholderText(strWriteDesc);
    ui->AlgArgUpdataFilePathlineEdit_1->setPlaceholderText(strWriteDesc);
    ui->AlgArgUpdataFilePathlineEdit_2->setPlaceholderText(strWriteDesc);
    ui->AlgArgUpdataFilePathlineEdit_3->setPlaceholderText(strWriteDesc);
    ui->AlgArgUpdataFilePathlineEdit_4->setPlaceholderText(strWriteDesc);
    ui->firmwareUpdataFilePathlineEdit->setPlaceholderText(strWriteDesc);

    for(int  i = 1; i <= 32; i++){
        if(i <= 16){
            ui->VolumeKeyStepComBox->addItem(QString::number(i));
        }
        ui->VolumeKeyStallNumComBox->addItem(QString::number(i));
    }
    QStringList comBoxList;
    comBoxList <<"不使能循环" <<"使能循环";
    ui->VolumeKeyCyCleComBox->addItems(comBoxList);
    comBoxList.clear();

    comBoxList <<"顺时针" <<"逆时针";
    ui->VolumeKnobDirectionComBox->addItems(comBoxList);
    comBoxList.clear();

    for(int  i = 0; i <= 255; i++){
        ui-> VolumeKnobRangeComBox->addItem(QString::number(i));
    }
    QString str65535Desc = "请输入[0,65535]之间的数";
    QString str255Desc = "请输入[0,255]之间的数";
    ui->soundPresLE->setPlaceholderText("请输入[0,140]之间的数");
    ui->soundFrequencyLE->setPlaceholderText("请输入[20,20000]之间的数");
    ui->soundCycleLE->setPlaceholderText(str65535Desc);
    ui->soundContinueLE->setPlaceholderText(str65535Desc);
    ui->soundNumsLE->setPlaceholderText(str255Desc);
    ALGMODELDTAT algDataStruct;
    algDataStruct.pAlgGroupBox = ui->algGroupBox_1;
    algDataStruct.pAlgFilePath = ui->AlgArgUpdataFilePathlineEdit_1;
    m_AlgModelList.append(algDataStruct);

    algDataStruct.pAlgGroupBox = ui->algGroupBox_2;
    algDataStruct.pAlgFilePath = ui->AlgArgUpdataFilePathlineEdit_2;
    m_AlgModelList.append(algDataStruct);

    algDataStruct.pAlgGroupBox = ui->algGroupBox_3;
    algDataStruct.pAlgFilePath = ui->AlgArgUpdataFilePathlineEdit_3;
    m_AlgModelList.append(algDataStruct);

    algDataStruct.pAlgGroupBox = ui->algGroupBox_4;
    algDataStruct.pAlgFilePath = ui->AlgArgUpdataFilePathlineEdit_4;
    m_AlgModelList.append(algDataStruct);
    ui->exportArgCountBox->setCurrentText("3");
}

void CHearingAidvMainWindow::initWDRCUi(int nChannelCount)
{
    m_wdrcCTStatus.resize(nChannelCount);
    m_wdrcMPOStatus.resize(nChannelCount);
    m_wdrcGainStatus.resize(nChannelCount);
    m_spinboxStatus.resize(nChannelCount);
    m_wdrcCTStatus.fill(0);
    m_wdrcMPOStatus.fill(0);
    m_wdrcGainStatus.fill(0);
    m_spinboxStatus.fill(0);
    ui->FrequencyBandCoBoxFirst->clear();

    QString path = QCoreApplication::applicationDirPath();
    QString ChannelRangeFilePath = path + QString("/JsonFile/ChannelRange_%1.json").arg(nChannelCount);
    QFile file(ChannelRangeFilePath);
    QString msg;
    if(!file.exists()){
        msg = "WDRC频段数据文件不存在";
        slotShowMsgBox(msg);
        return;
    }

    if(file.size() == 0){
        msg = "WDRC频段数据文件为空";
        slotShowMsgBox(msg);
        return;
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        msg = "WDRC频段数据文件无法打开";
        slotShowMsgBox(msg);
        return;
    }
    QString str = file.readAll();
    file.close();

    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(str.toUtf8(), &parseJsonErr);
    if (!(parseJsonErr.error == QJsonParseError::NoError)) {
        slotShowMsgBox("WDRC频段配置文件错误！");
        return;
    }

    QJsonObject jsonObject = document.object();
    if (jsonObject.contains(QStringLiteral("WDRC Channel Range"))) {
        QJsonValue arrayValue = jsonObject.value(QStringLiteral("WDRC Channel Range"));
        if (arrayValue.isArray()) {
            QJsonArray array = arrayValue.toArray();
            for (int i = 1; i < nChannelCount +1; i++) {
                QString strChannel = QString("%1-%2").arg(array.at(i - 1).toString()).arg(array.at(i).toString());
                ui->FrequencyBandCoBoxFirst->addItem(strChannel);
                CWDRCItemWidget * pWidget = new CWDRCItemWidget(this);
                pWidget->setObjName(i - 1);
                pWidget->setObjectName("enableWidget");
                connect(pWidget,&CWDRCItemWidget::signal_isSelectAllCheckBox,
                        this,&CHearingAidvMainWindow::slotIsSelectAllCheckBox);
                connect(this,&CHearingAidvMainWindow::signalWDRCIsSelectAll,
                        pWidget,&CWDRCItemWidget::slotSetisSelectAllCheckBox);
                connect(pWidget->getChannelSingleCheckBox(),&QCheckBox::clicked,
                        this,&CHearingAidvMainWindow::slotWDRCSigalClicked);
                connect(ui->scrollArea,&RubberScrollArea::signalSelectAllcheckbox,
                        pWidget,&CWDRCItemWidget::slotSetisSelectAllCheckBox);

                connect(pWidget->getWDRCData().pGainComBox, SIGNAL(activated(int)),
                        this, SLOT(slotSetWDRCComBoxIndexSynchronization(int)));
                connect(pWidget->getWDRCData().pCTComBox, SIGNAL(activated(int)),
                        this, SLOT(slotSetWDRCComBoxIndexSynchronization(int)));
                connect(pWidget->getWDRCData().pMPOCoBox, SIGNAL(activated(int)),
                        this, SLOT(slotSetWDRCComBoxIndexSynchronization(int)));
                connect(pWidget->getWDRCData().pCRSpinBox, SIGNAL(valueChanged(double)),
                        this, SLOT(slotSetWDRCSpinBoxValueSynchronization(double)));

                pWidget->setChecBoxLabelValue(strChannel);
                if(i ==1){
                    pWidget->setHeadWidgetHide(true);
                }
                ui->verticalLayout_WDRC->addWidget(pWidget);

                m_wdrcGainStatus[i - 1] = pWidget->getWDRCData().pGainComBox->currentIndex();
                m_wdrcCTStatus[i - 1]  = pWidget->getWDRCData().pCTComBox->currentIndex();
                m_wdrcMPOStatus[i - 1] = pWidget->getWDRCData().pMPOCoBox->currentIndex();
                m_spinboxStatus[i - 1] = pWidget->getWDRCData().pCRSpinBox->value();
            }
        }
    }
    on_WDRCChannelCounBox_currentTextChanged("4");
}

void CHearingAidvMainWindow::initProcessProgressBar()
{
    ui->showProcessProgressBar->setMinimum(0);
    ui->showProcessProgressBar->setValue(0);
    double dProgress = (ui->showProcessProgressBar->value() - ui->showProcessProgressBar->minimum()) * 100.0
            / (ui->showProcessProgressBar->maximum() - ui->showProcessProgressBar->minimum());
    ui->showProcessProgressBar->setFormat(QString("读/写进度为: %1%").arg(QString::number(dProgress, 'f', 2)));
}

void CHearingAidvMainWindow::initCharts()
{
    QValueAxis* pAxisXFirst = new QValueAxis(this);
    QValueAxis* pAxisYFirst = new QValueAxis(this);

    m_pLineSeriesFirst = new QLineSeries(this);
    QChart *pChartFirst = new QChart();

    pChartFirst->addAxis(pAxisXFirst, Qt::AlignBottom);
    pChartFirst->addAxis(pAxisYFirst, Qt::AlignLeft);
    pChartFirst->addSeries(m_pLineSeriesFirst);
    m_pLineSeriesFirst->attachAxis(pAxisXFirst);
    m_pLineSeriesFirst->attachAxis(pAxisYFirst);

    ui->graphicsViewFirst->setRenderHint(QPainter::Antialiasing);
    ui->graphicsViewFirst->setChart(pChartFirst);
    pChartFirst->layout()->setContentsMargins(0, 0,0, 0);
    pChartFirst->setMargins(QMargins(5, 5, 5,5));
    pAxisXFirst->setTitleText("In(dB spl)");
    pAxisYFirst->setTitleText("Out(dB spl)");
    pAxisXFirst->setMin(20);
    pAxisYFirst->setMin(20);
    pAxisXFirst->setMax(WDRC_AXISX_RANGE);
    pAxisYFirst->setMax(WDRC_AXISY_RANGE);
    pAxisXFirst->setTickCount(11);
    pAxisYFirst->setTickCount(7);
    pAxisXFirst->setLineVisible(true);
    pAxisYFirst->setLineVisible(true);
    pAxisYFirst->setLabelFormat("%d");
    pAxisXFirst->setLabelFormat("%d");
    m_pLineSeriesFirst->clear();
    QLegend *legend = pChartFirst->legend();
    if(legend){
        legend->setVisible(false);
    }
}

void CHearingAidvMainWindow::drawCurves(float threshold,float ratio,float gain,float mpo,int type)
{
    float yy = WDRC_AXISY_RANGE <= mpo ? WDRC_AXISY_RANGE : mpo;
    QPointF first, second, third, forth;
    first.setX(20);
    first.setY(20 + gain);
    if (threshold > yy){
        second.setX(mpo - gain);
        second.setY(mpo);
        third.setX(WDRC_AXISX_RANGE);
        third.setY(mpo);
        forth = third;
    }else{
        second.setX(threshold - gain);
        second.setY(threshold);
        third.setX(threshold - gain + (yy - threshold)*ratio);
        third.setY(yy);
        forth.setX(WDRC_AXISX_RANGE);
        forth.setY(yy);
    }

    if(type == LineSeriesFirst){
        m_pLineSeriesFirst->clear();
        m_pLineSeriesFirst->append(first);
        m_pLineSeriesFirst->append(second);
        m_pLineSeriesFirst->append(third);
        m_pLineSeriesFirst->append(forth);
    }
    return;
}

void CHearingAidvMainWindow::initNoiseUi()
{
    ui->NoiseNumberShowLabel->setText(QString("%1").arg(ui->NoiseHorizontalSlider->value()));
}

void CHearingAidvMainWindow::applyLineEditFormat(QLineEdit *lineEdit,int rex) {

    QRegExp rxNormal("[A-Fa-f\\d\\s]+");
    QRegExp rxNumber("[0-9]+$");
    if(rex == HEX_NORMAL){
        lineEdit->setValidator(new QRegExpValidator(rxNormal, lineEdit));
    }else{
        lineEdit->setValidator(new QRegExpValidator(rxNumber, lineEdit));
    }

    connect(lineEdit, &QLineEdit::textEdited, [lineEdit](const QString &input) {
        QString hexStr = input.simplified().remove(" ");
        QString formattedStr;
        for (int i = 0; i < hexStr.length(); i += 2) {
            formattedStr += hexStr.mid(i, 2) + " ";
        }
        formattedStr = formattedStr.trimmed();
        lineEdit->setText(formattedStr.toUpper());
    });
}

QByteArray CHearingAidvMainWindow::intToArry(int nData)
{
    int *p  = &nData;
    QByteArray Arry = QByteArray::fromRawData(reinterpret_cast<const char *>(p),4);
    QByteArray RawDataArry;
    RawDataArry.append(Arry);
    return RawDataArry;
}

void CHearingAidvMainWindow::initOther()
{
    m_pSerialThread = new QThread;
    m_pSerialWork->moveToThread(m_pSerialThread);
    connect(m_pSerialThread, &QThread::finished,m_pSerialWork, &SerialWork::deleteLater);
    m_pSerialThread->start();

    QString strPWDest = "请输入8位密码";
    ui->permPasswdLE->setMaxLength(PASSWORD_LENTH);
    ui->permPasswdLE->setPlaceholderText(strPWDest);
    ui->changeOldpaswdLE->setMaxLength(PASSWORD_LENTH);
    ui->changeOldpaswdLE->setPlaceholderText(strPWDest);
    ui->changeNewpaswdLE->setMaxLength(PASSWORD_LENTH);
    ui->changeNewpaswdLE->setPlaceholderText(strPWDest);
    ui->changeConfirmPaswdLE->setMaxLength(PASSWORD_LENTH);
    ui->changeConfirmPaswdLE->setPlaceholderText(strPWDest);
    ui->changeConfirmPaswdLE->setMaxLength(PASSWORD_LENTH);
    ui->changeConfirmPaswdLE->setPlaceholderText(strPWDest);
    ui->clearPaswdLE->setMaxLength(PASSWORD_LENTH);
    ui->clearPaswdLE->setPlaceholderText(strPWDest);
    ui->InitPasswordEdit->setMaxLength(PASSWORD_LENTH);
    ui->InitPasswordEdit->setPlaceholderText(strPWDest);

    QRegExp rxNumber("[a-zA-Z0-9]+$");
    ui->permPasswdLE->setValidator(new QRegExpValidator(rxNumber));
    ui->changeOldpaswdLE->setValidator(new QRegExpValidator(rxNumber));
    ui->changeNewpaswdLE->setValidator(new QRegExpValidator(rxNumber));
    ui->changeConfirmPaswdLE->setValidator(new QRegExpValidator(rxNumber));
    ui->clearPaswdLE->setValidator(new QRegExpValidator(rxNumber));
    ui->InitPasswordEdit->setValidator(new QRegExpValidator(rxNumber));

    applyLineEditFormat(ui->FlashCopyDestAddrLE,HEX_NORMAL);
    applyLineEditFormat(ui->FLashCopySrcAddressLE,HEX_NORMAL);
    applyLineEditFormat(ui->FlashEraseAddressLE,HEX_NORMAL);
    applyLineEditFormat(ui->FlashReadAddressLE,HEX_NORMAL);
    applyLineEditFormat(ui->FlashVerifyAddressLE,HEX_NORMAL);
    applyLineEditFormat(ui->FlashFromAddressLE,HEX_NORMAL);
}

void CHearingAidvMainWindow::initSerialDialog()
{
    ui->SerialNameBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    ui->SerialNameBox->setMinimumContentsLength(5);

    slotSerialNameBoxRefresh();
    connect(m_pSerialWork, &SerialWork::signaSerialportStateChanged, this, &CHearingAidvMainWindow::slotSerialPortStateChanged);
    connect(this, &CHearingAidvMainWindow::signalSendAccept, m_pSerialWork, &SerialWork::slotInitSerial);
    connect(this, &CHearingAidvMainWindow::signalSendClose, m_pSerialWork, &SerialWork::slotCloseSerial);
}

void CHearingAidvMainWindow::initUiReadJsonData()
{
    QString strCfgFilePath;
    strCfgFilePath = QCoreApplication::applicationDirPath() +"/JsonFile/cfgFile.json";

    QFile cfgFile(strCfgFilePath);
    if (cfgFile.open(QIODevice::ReadOnly)) {
        QByteArray data = cfgFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = doc.object();
        QJsonObject FlashObj = jsonObj["FlashData"].toObject();
        ui->FlashReadAddressLE->setText(FlashObj["ReadAdd"].toString());
        ui->FlashReadLenthLE->setText(FlashObj["ReadLen"].toString());
        ui->FlashFromAddressLE->setText(FlashObj["WriteAdd"].toString());
        ui->FlashFromLenthLE->setText(FlashObj["WriteLen"].toString());
        ui->FlashEraseAddressLE->setText(FlashObj["EraseAdd"].toString());
        ui->FlashEraseCombox->setCurrentIndex(FlashObj["EraseType"].toInt());
        ui->FlashVerifyAddressLE->setText(FlashObj["CheckAdd"].toString());
        ui->FlashVerifyLenthLE->setText(FlashObj["CheckLen"].toString());
        ui->FLashCopySrcAddressLE->setText(FlashObj["BackSrceAdd"].toString());
        ui->FlashCopyDestAddrLE->setText(FlashObj["BackDesAdd"].toString());
        ui->FlashCopyLenthLE->setText(FlashObj["BackLen"].toString());

        QJsonObject PureToneObj = jsonObj["PureToneData"].toObject();
        ui->soundPresLE->setText(PureToneObj["SPL"].toString());
        ui->soundFrequencyLE->setText(PureToneObj["Frequency"].toString());
        ui->soundCycleLE->setText(PureToneObj["Cycle"].toString());
        ui->soundContinueLE->setText(PureToneObj["Sound"].toString());
        ui->soundNumsLE->setText(PureToneObj["SoundNum"].toString());

        QJsonObject PassWdObj = jsonObj["PassWordData"].toObject();
        ui->InitPasswordLevelCombox->setCurrentIndex(PassWdObj["InitPerm"].toInt());
        ui->getPermLevelCombox->setCurrentIndex(PassWdObj["InputPerm"].toInt());
        ui->changePaswdLevelCombox->setCurrentIndex(PassWdObj["ModifyPerm"].toInt());
        ui->clearPaswdLevelCombox->setCurrentIndex(PassWdObj["ClearPerm"].toInt());

        QJsonObject WDRCObj = jsonObj["WDRCData"].toObject();
        ui->WDRCATLineEdit->setText(WDRCObj["AT"].toString());
        ui->WDRCRTLineEdit->setText(WDRCObj["RT"].toString());
        ui->WDRCChannelCounBox->setCurrentText(WDRCObj["ChannelCount"].toString());
        QString WdrcCount = WDRCObj["ChannelCount"].toString();
        if(WdrcCount.isEmpty()){
            WdrcCount = "4";
        }
        on_WDRCChannelCounBox_currentTextChanged(WdrcCount);

        QJsonObject NRNLIObj = jsonObj["NRNLIData"].toObject();
        ui->NRNLICalibrationLeftLineEdit->setText(NRNLIObj["LeftValue"].toString());
        ui->NRNLICalibrationRightLineEdit->setText(NRNLIObj["RightValue"].toString());

        QJsonObject DirObj = jsonObj["fileDialogDirData"].toObject();
        QString strFlashRead = DirObj["FlashReadDir"].toString();
        QString strFlashWrite = DirObj["FlashWriteDir"].toString();
        QString strOTAModelPath = DirObj["OtaModelDir"].toString();

        QJsonObject otaModelFilePathObj = jsonObj["otaModelFilePath"].toObject();
        QString strFirmwareFilePath =  otaModelFilePathObj["firmwareFilePath"].toString();
        QString strCfgFilePath = otaModelFilePathObj["cfgFilePath"].toString();
        QString strAlg1FilePath = otaModelFilePathObj["alg1FilePath"].toString();
        QString strAlg2FilePath = otaModelFilePathObj["alg2FilePath"].toString();
        QString strAlg3FilePath = otaModelFilePathObj["alg3FilePath"].toString();
        QString strAlg4FilePath = otaModelFilePathObj["alg4FilePath"].toString();

        ui->firmwareUpdataFilePathlineEdit->setText(strFirmwareFilePath);
        ui->configFilePathEdit->setText(strCfgFilePath);
        ui->AlgArgUpdataFilePathlineEdit_1->setText(strAlg1FilePath);
        ui->AlgArgUpdataFilePathlineEdit_2->setText(strAlg2FilePath);
        ui->AlgArgUpdataFilePathlineEdit_3->setText(strAlg3FilePath);
        ui->AlgArgUpdataFilePathlineEdit_4->setText(strAlg4FilePath);
        ui->exportArgCountBox->setCurrentIndex(otaModelFilePathObj["algFileCount"].toInt());

        if(!strFlashRead.isEmpty()){
            m_strFalshReadPath = strFlashRead;
        }
        if(!strFlashWrite.isEmpty()){
            m_strFalshWritePath = strFlashWrite;
        }
        if(!strOTAModelPath.isEmpty()){
            m_strOTAModelPath = strOTAModelPath;
        }

        QJsonObject SerialObj = jsonObj["serialPortData"].toObject();
        ui->SerialNameBox->setCurrentText(SerialObj["SerialName"].toString());
        ui->comboBoxFre->setCurrentText(SerialObj["SerialFre"].toString());
        cfgFile.close();
    }
}

void CHearingAidvMainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F1) {
        on_userGuiBtn_clicked();
    }
    QWidget::keyPressEvent(event);
}

void CHearingAidvMainWindow::showStatusBarlightBtn(QString msg, int ret)
{
    if(ret == 0){
        ui->isSuccessStatusShowWidget->setStyleSheet("border-radius:8px;background-color:green;");
        ui->errorMsgShowLabel->setStyleSheet("color:green;");
    }else if(ret == 99){
        ui->isSuccessStatusShowWidget->setStyleSheet("border-radius:8px;background-color:gray;");
        ui->errorMsgShowLabel->setStyleSheet("color:black;");
    }else {
        ui->isSuccessStatusShowWidget->setStyleSheet("border-radius:8px;background-color:red;");
        ui->errorMsgShowLabel->setStyleSheet("color:red;");
    }
    ui->errorMsgShowLabel->setText(msg);
}

void CHearingAidvMainWindow::initCalibrationValue()
{
    ui->DeviceModeNumLeftMicBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    ui->DeviceModeNumLeftMicBox->setMinimumContentsLength(5);
    ui->DeviceModeNumRightMicBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    ui->DeviceModeNumRightMicBox->setMinimumContentsLength(5);
    ui->DeviceModeNumLeftReceiverBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    ui->DeviceModeNumLeftReceiverBox->setMinimumContentsLength(5);
    ui->DeviceModeNumRightReceiverBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    ui->DeviceModeNumRightReceiverBox->setMinimumContentsLength(5);

    ui->cbCutLowFreq->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    ui->cbCutLowFreq->setMinimumContentsLength(5);
    ui->cbCutLowFreq->clear();
    ui->cbCutLowFreq->addItem("关");
    ui->cbCutLowFreq->addItem("开");
    ui->cbCutLowFreq->setCurrentIndex(0);

    slotMicDeviceNumRefresh();
    slotMicRightDeviceNumRefresh();
    slotReceiverDeviceNumRefresh();
    slotReceiverRightDeviceNumRefresh();
}

void CHearingAidvMainWindow::initBtnStatus()
{
    ui->ArgNumWidget->hide();
    ui->DeviceTypeWidget->hide();
    ui->VloumeWidget->hide();
    ui->BackUpWidget->hide();
    ui->PickupArgWidget->hide();
}

void CHearingAidvMainWindow::initTabFocusOrder()
{
    QWidget* arr[] = {ui->NoiseHorizontalSlider,ui->NRARGWriteBtn,
                      ui->NRNLICalibrationLeftLineEdit,ui->NRNLIDataWriteBtn,
                      ui->InitPasswordLevelCombox,ui->clearPasswdBt,
                      ui->soundPresLE,ui->voiceCtrlBt,
                      ui->firmwareUpdataFilePathlineEdit,ui->OTADataSplicingBtn,
                      ui->VolumeKeyModeRadioBtn,ui->CfgCmdWriteAllBtn

                     };
    int len = sizeof(arr)/sizeof(QWidget*);
    int count = len/2;
    for (int i=0;i<count;i++){
        arr[2*i+1]->installEventFilter(new ChangeTabOrder(arr[2*i]));
    }

    QList<QWidget*>arrWDRCList;
    arrWDRCList.append(ui->FrequencyBandCoBoxFirst);
    arrWDRCList.append(ui->WDRCChannelCounBox);

    int nOffset = 2;
    CWDRCItemWidget *pWidget = NULL;
    QList<CWDRCItemWidget*>WdrcitemList = ui->scrollAreaWidgetContents->findChildren<CWDRCItemWidget*>();
    int nCount = ui->WDRCChannelCounBox->currentText().toInt();

    for (int k=0;k<nCount * 4;k++){
        arrWDRCList.append(nullptr);
    }

    for(int  i = 0; i < nCount;i++){
        if(WdrcitemList.size() > 0){
            pWidget = WdrcitemList.at(i);
        }
        if(pWidget){
            arrWDRCList[nOffset + i] = pWidget->getWDRCData().pGainComBox;
            arrWDRCList[nOffset + nCount + i] = pWidget->getWDRCData().pCTComBox;
            arrWDRCList[nOffset + 2 * nCount + i] =pWidget->getWDRCData().pCRSpinBox;
            arrWDRCList[nOffset + 3 * nCount + i] =pWidget->getWDRCData().pMPOCoBox;
        }
    }

    arrWDRCList.append(ui->WDRCATLineEdit);
    arrWDRCList.append(ui->WDRCRTLineEdit);
    arrWDRCList.append(ui->WDRCReadArgTogether);
    arrWDRCList.append(ui->WDRCwriteArgTogether);

    int ctlCount = arrWDRCList.count();
    for (int i=0;i<ctlCount;i++){
        if (i == ctlCount - 1){
            arrWDRCList[ctlCount - 1]->installEventFilter(new ChangeTabOrder(arrWDRCList[0]));
        }else{
            QWidget::setTabOrder(arrWDRCList[i],arrWDRCList[i+1]);
        }
    }

    QList<QWidget*>arrEQList;
    CEQItemWidget *pEQWidget = NULL;
    QList<CEQItemWidget*>EqitemList = ui->EQScrollAreaWidgetContents->findChildren<CEQItemWidget*>();
    for(int i =0; i < EqitemList.size(); i++){
        pEQWidget = EqitemList.at(i);
        if(pEQWidget){
            arrEQList.append(pEQWidget->getSlider());
        }
    }
    arrEQList.append(ui->EQArgRead64ChannelBtn);
    arrEQList.append(ui->EQArgWrite64ChannelBtn);

    int EQctlCount = arrEQList.count();
    for (int i=0;i<EQctlCount;i++){
        if (i == EQctlCount - 1){
            arrEQList[EQctlCount - 1]->installEventFilter(new ChangeTabOrder(arrEQList[0]));
        }else{
            QWidget::setTabOrder(arrEQList[i],arrEQList[i+1]);
        }
    }
}

QString CHearingAidvMainWindow::parseTimeShowUi(QString time)
{
    QDateTime dateTime;
    QString formattedTime;
    if(time.length() == 8){
        dateTime = QDateTime::fromString(time, "yyyyMMdd");
        formattedTime = dateTime.toString("yyyy/MM/dd");
    }else{
        dateTime = QDateTime::fromString(time, "yyyyMMddhhmmss");
        formattedTime = dateTime.toString("yyyy/MM/dd\nhh:mm:ss");
    }
    return formattedTime;
}

void CHearingAidvMainWindow::getDirFileAll(QString pathDir, QFileInfoList &fileList)
{
    QDir dir(pathDir);
    if(!dir.exists()){
        return;
    }

    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setNameFilters(QStringList() << "*.txt");
    fileList = dir.entryInfoList();
}

void CHearingAidvMainWindow::getFileCalibrationData(int type,QString strFilePath, QByteArray &fileDataArry)
{
    QString strTailFilePath;
    if(type == DEVICE_MIC){
        strTailFilePath = QCoreApplication::applicationDirPath() +
                "/MicCalibrationValue/" +QString("%1.txt").arg(strFilePath);
    }else if(type == DEVICE_RECEIVER){
        strTailFilePath = QCoreApplication::applicationDirPath() +
                "/ReceiverCalibrationValue/" +QString("%1.txt").arg(strFilePath);
    }

    QFile file(strTailFilePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList values = line.split(' ');
            foreach (const QString &value, values) {
                qint8 Calibration = static_cast<qint8>(value.toInt());
                fileDataArry.append(Calibration);
            }
        }
        file.close();
    }
}

void CHearingAidvMainWindow::initConnects()
{
    connect(m_pSerialWork, &SerialWork::signaleStartTimer,this, &CHearingAidvMainWindow::slotStartTimer);
    connect(m_pSerialWork, &SerialWork::signaleStopTimer,this, &CHearingAidvMainWindow::slotStopTimer);
    connect(m_pSerialWork, &SerialWork::signalShowChannelCountDefault,this, &CHearingAidvMainWindow::slotShowChannelCountDefault);
    connect(m_pSerialWork, &SerialWork::signalInitUIData,this, &CHearingAidvMainWindow::slotInitUiData);
    connect(this, &CHearingAidvMainWindow::signalFlashToUiOrFile,m_pSerialWork, &SerialWork::slotFlashReadToUiOrFile);
    connect(m_pSerialWork, &SerialWork::signalFlashData,this, &CHearingAidvMainWindow::slotFlashDataUpdata);
    connect(m_pSerialWork, &SerialWork::signalShowUpdateCmdReadData,this, &CHearingAidvMainWindow::slotShowUpdataCmdReadData);

    connect(m_pSerialWork, &SerialWork::signaShowMesgbox,this, &CHearingAidvMainWindow::slotShowMsgBox);
    connect(m_pSerialWork, &SerialWork::signaShowMesgboxDialog,this, &CHearingAidvMainWindow::slotShowMsgBoxdialog);
    connect(m_pSerialWork, &SerialWork::signaShowMegDevStatus,this, &CHearingAidvMainWindow::slotShowMsgDevStatus);
    connect(m_pSerialWork, &SerialWork::signalCMDWriteFinshed,this, &CHearingAidvMainWindow::slotCmdWriteFinshed);
    connect(m_pSerialWork, &SerialWork::signalCMDAllFinshed,this, &CHearingAidvMainWindow::slotAllCmdFinshed);
    connect(m_pSerialWork, &SerialWork::signalShowNoOper,this, &CHearingAidvMainWindow::slotStausBarNoOper);

    connect(m_pSerialWork, &SerialWork::signalOperationResult,this, &CHearingAidvMainWindow::slotShowAnswerResult);
    connect(m_pSerialWork, &SerialWork::signalShowVolumeCurrNum,this, &CHearingAidvMainWindow::slotShowReadCurrCtrlCMDNum);
    connect(m_pSerialWork, &SerialWork::signalShowCfgCmdReadData,this, &CHearingAidvMainWindow::slotShowCfgCmdReadData);
    connect(m_pSerialWork, &SerialWork::signalShowAlgCmdReadData,this, &CHearingAidvMainWindow::slotShowAlgeriaCmdReadData);

    connect(ui->flashSwitchBtn, &SwitchButton::checkedChanged, this ,&CHearingAidvMainWindow::slotFlashSwitchBtnClicked);
    connect(ui->changeNewpaswdLE,&LineEditPassword::textChanged,this,&CHearingAidvMainWindow::on_changeOldpaswdLE_textChanged);
    connect(ui->VolumeMuteLeftSwithBtn, &SwitchButton::checkedChanged, this ,&CHearingAidvMainWindow::slotVolumeMuteSwithBtnClicked);
    connect(ui->VolumeMuteRightSwithBtn, &SwitchButton::checkedChanged, this ,&CHearingAidvMainWindow::slotVolumeMuteSwithBtnClicked);

    connect(ui->soundContinueLE,&QLineEdit::textChanged,this,&CHearingAidvMainWindow::on_soundPresLE_textChanged);
    connect(ui->soundCycleLE,&QLineEdit::textChanged,this,&CHearingAidvMainWindow::on_soundPresLE_textChanged);
    connect(ui->soundFrequencyLE,&QLineEdit::textChanged,this,&CHearingAidvMainWindow::on_soundPresLE_textChanged);
    connect(ui->soundNumsLE,&QLineEdit::textChanged,this,&CHearingAidvMainWindow::on_soundPresLE_textChanged);
    connect(ui->soundPresLE,&QLineEdit::textChanged,this,&CHearingAidvMainWindow::on_soundPresLE_textChanged);

    connect(ui->AlgARGBtn_1,&QPushButton::clicked,this,&CHearingAidvMainWindow::slotChangeAlgArgBtnClick);
    connect(ui->AlgARGBtn_2,&QPushButton::clicked,this,&CHearingAidvMainWindow::slotChangeAlgArgBtnClick);
    connect(ui->AlgARGBtn_3,&QPushButton::clicked,this,&CHearingAidvMainWindow::slotChangeAlgArgBtnClick);
    connect(ui->AlgARGBtn_4,&QPushButton::clicked,this,&CHearingAidvMainWindow::slotChangeAlgArgBtnClick);

    connect(ui->modeTypeBtn_0,&QRadioButton::clicked,this,&CHearingAidvMainWindow::slotChangeModeTypeBtnClick);
    connect(ui->modeTypeBtn_1,&QRadioButton::clicked,this,&CHearingAidvMainWindow::slotChangeModeTypeBtnClick);
    connect(ui->modeTypeBtn_2,&QRadioButton::clicked,this,&CHearingAidvMainWindow::slotChangeModeTypeBtnClick);
    connect(ui->modeTypeBtn_3,&QRadioButton::clicked,this,&CHearingAidvMainWindow::slotChangeModeTypeBtnClick);

    connect(ui->algBypassSwitch,&SwitchButton::checkedChanged,this,&CHearingAidvMainWindow::slotAlgSwitchBtnClicked);
    connect(ui->algNLISwitchBtn,&SwitchButton::checkedChanged,this,&CHearingAidvMainWindow::slotAlgSwitchBtnClicked);
    connect(ui->algNRSwitchBtn,&SwitchButton::checkedChanged,this,&CHearingAidvMainWindow::slotAlgSwitchBtnClicked);
    connect(ui->algWDRCSwitchBtn,&SwitchButton::checkedChanged,this,&CHearingAidvMainWindow::slotAlgSwitchBtnClicked);
    connect(ui->algEQSwitchBtn,&SwitchButton::checkedChanged,this,&CHearingAidvMainWindow::slotAlgSwitchBtnClicked);
    connect(ui->algHSSwitchBtn,&SwitchButton::checkedChanged,this,&CHearingAidvMainWindow::slotAlgSwitchBtnClicked);
    connect(ui->algFFTSwitchBtn,&SwitchButton::checkedChanged,this,&CHearingAidvMainWindow::slotAlgSwitchBtnClicked);

    connect(ui->pickupLeftChannelPGAEdit,&QLineEdit::textChanged,this,&CHearingAidvMainWindow::on_pickupLeftChannelDGAEdit_textChanged);
    connect(ui->pickupRightChannelDGAEdit,&QLineEdit::textChanged,this,&CHearingAidvMainWindow::on_pickupLeftChannelDGAEdit_textChanged);
    connect(ui->pickupRightChannelPGAEdit,&QLineEdit::textChanged,this,&CHearingAidvMainWindow::on_pickupLeftChannelDGAEdit_textChanged);
    connect(ui->playbackArgRightChannelDGAEdilt,&QLineEdit::textChanged,this,&CHearingAidvMainWindow::on_playbackArgLeftChannelDGAEdilt_textChanged);
    connect(ui->NRNLICalibrationRightLineEdit,&QLineEdit::textChanged,this,&CHearingAidvMainWindow::on_NRNLICalibrationLeftLineEdit_textChanged);
    connect(ui->WDRCRTLineEdit,&QLineEdit::textChanged,this,&CHearingAidvMainWindow::on_WDRCATLineEdit_textChanged);
    connect(ui->SerialNameBox,&ComboBoxCustom::signalComboBoxClicked,this,&CHearingAidvMainWindow::slotSerialNameBoxRefresh);
    connect(ui->DeviceModeNumLeftMicBox,&ComboBoxCustom::signalComboBoxClicked,this,&CHearingAidvMainWindow::slotMicDeviceNumRefresh);
    connect(ui->DeviceModeNumRightMicBox,&ComboBoxCustom::signalComboBoxClicked,this,&CHearingAidvMainWindow::slotMicRightDeviceNumRefresh);
    connect(ui->DeviceModeNumLeftReceiverBox,&ComboBoxCustom::signalComboBoxClicked,this,&CHearingAidvMainWindow::slotReceiverDeviceNumRefresh);
    connect(ui->DeviceModeNumRightReceiverBox,&ComboBoxCustom::signalComboBoxClicked,this,&CHearingAidvMainWindow::slotReceiverRightDeviceNumRefresh);

    connect(ui->channelLeftCtrlBtn,&QCheckBox::clicked,this,&CHearingAidvMainWindow::slotVolumeChannelSelectClicked);
    connect(ui->channelRightCtrlBtn,&QCheckBox::clicked,this,&CHearingAidvMainWindow::slotVolumeChannelSelectClicked);
}

void CHearingAidvMainWindow::slotIsSelectAllCheckBox(bool isChecked)
{
    for(auto checkBox: ui->scrollArea->m_checkList){
        checkBox->setChecked(isChecked);
    }
}

void CHearingAidvMainWindow::slotEQArgIsSelectAllCheckBox(bool isChecked)
{
    for(auto checkBox: ui->EQArgscrollArea->m_checkList){
        checkBox->setChecked(isChecked);
    }
}

void CHearingAidvMainWindow::initEQUi()
{
    m_eq_status.resize(64);
    m_eq_status.fill(0);

    QString path = QCoreApplication::applicationDirPath();
    QFile file(path + "/JsonFile/EQChannelRange.json");
    QString msg;
    if(!file.exists()){
        msg = "EQ频点数据文件不存在";
        slotShowMsgBox(msg);
        return;
    }

    if(file.size() == 0){
        msg = "EQ频点数据文件为空";
        slotShowMsgBox(msg);
        return;
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        msg = "EQ频点数据文件无法打开";
        slotShowMsgBox(msg);
        return;
    }
    QString str = file.readAll();
    file.close();

    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(str.toUtf8(), &parseJsonErr);
    if (!(parseJsonErr.error == QJsonParseError::NoError)) {
        slotShowMsgBox("EQ频点数据文件错误");
        return;
    }

    QJsonObject jsonObject = document.object();
    if (jsonObject.contains(QStringLiteral("EQ Channel Range"))) {
        QJsonValue arrayValue = jsonObject.value(QStringLiteral("EQ Channel Range"));
        if (arrayValue.isArray()) {
            QJsonArray array = arrayValue.toArray();
            int size = 0;
            size =  array.size();
            for(int i = 0; i < size ; i++){
                QString strChannel = QString("%1").arg(array.at(i).toString());
                CEQItemWidget * pWidget = new CEQItemWidget(this);
                pWidget->setChecBoxLabelString(strChannel);
                pWidget->setSliderObjName(i);

                connect(pWidget,&CEQItemWidget::signalIsSelectAllCheckBox,
                        this,&CHearingAidvMainWindow::slotEQArgIsSelectAllCheckBox);
                connect(pWidget->getSlider(),&QSlider::valueChanged,
                        this,&CHearingAidvMainWindow::slotSetEqSliderPosSynchronization);
                connect(this,&CHearingAidvMainWindow::signalEQIsSelectAll,
                        pWidget,&CEQItemWidget::slotSetEQIsSelectAllCheckBox);
                connect(pWidget->getSignalCheckBox(),&QCheckBox::clicked,
                        this,&CHearingAidvMainWindow::slotEqSigalClicked);

                connect(ui->EQArgscrollArea,&RubberScrollArea::signalSelectAllcheckbox,
                        pWidget,&CEQItemWidget::slotSetEQIsSelectAllCheckBox);

                if(i < size / 4){
                    ui->HLEQ_1->addWidget(pWidget);
                }else if(i < size / 2){
                    ui->HLEQ_2->addWidget(pWidget);
                }else if(i < size / 4 *3){
                    ui->HLEQ_3->addWidget(pWidget);
                }else if(i < size){
                    ui->HLEQ_4->addWidget(pWidget);
                }

                if(i == (size / 4 *1) -1 || i == (size /4 *2) -1 ||
                        i == (size / 4 *3) -1 || i == size -1){
                    pWidget->setHeadWidgetHide(true);
                }
                if(i == 0){
                    pWidget->setHeadCheckBoxHide(true);
                }

                m_eq_status[i]= pWidget->getSlider()->sliderPosition();
            }

        }
    }
    ui->EQArgscrollArea->m_checkList= this->findChildren<QCheckBox*>("EQChannelCheckBox");
}

QByteArray CHearingAidvMainWindow::getPaswd(QLineEdit *le)
{
    QByteArray array;
    array = le->text().trimmed().toUtf8();
    return array;
}

void CHearingAidvMainWindow::on_getPermissionBt_clicked()
{
    if(ui->permPasswdLE->text().length() != PASSWORD_LENTH){
        slotShowMsgBox("获取密码不得少于8位");
        return;
    }
    QByteArray array = getPaswd(ui->permPasswdLE);
    uchar level = ui->getPermLevelCombox->currentText().toInt();
    array.append(level);
    int ret = m_pSerialWork->processData(CMD_UPDATE_0x03, INST_CMD_0x00, array);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::on_changePasswdBt_clicked()
{
    if(ui->changeOldpaswdLE->text().length() != PASSWORD_LENTH ||
            ui->changeNewpaswdLE->text().length() != PASSWORD_LENTH ||
            ui->changeConfirmPaswdLE->text().length() != PASSWORD_LENTH){
        slotShowMsgBox("更改密码不得少于8位");
        return;
    }
    QByteArray array = getPaswd(ui->changeOldpaswdLE);
    if(!(ui->changeNewpaswdLE->text() == ui->changeConfirmPaswdLE->text())){
        slotShowMsgBox("新密码和确认密码不一致");
        return;
    }
    array.append(getPaswd(ui->changeNewpaswdLE));
    uchar level = ui->changePaswdLevelCombox->currentText().toInt();
    array.append(level);
    int ret = m_pSerialWork->processData(CMD_UPDATE_0x03, INST_CMD_0x01, array);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::on_clearPasswdBt_clicked()
{
    if(ui->clearPaswdLE->text().length() != PASSWORD_LENTH){
        slotShowMsgBox("清除密码不得少于8位");
        return;
    }
    QByteArray array = getPaswd(ui->clearPaswdLE);
    uchar level = ui->clearPaswdLevelCombox->currentText().toInt();
    array.append(level);
    int ret = m_pSerialWork->processData(CMD_UPDATE_0x03, INST_CMD_0x02, array);
    if(ret != 0)
        return;
}

QByteArray CHearingAidvMainWindow::getHexAddr(QLineEdit *le)
{
    bool ok;
    QString strResAddress = le->text().trimmed().replace(" ","");
    int dec = strResAddress.toInt(&ok,16);
    QByteArray array = intToArry(dec);
    return array;
}

void CHearingAidvMainWindow::on_FlashReadToUiBt_clicked()
{
    m_nReadFileDataTailSize = 0;
    ui->FlashReadTE->clear();
    if(ui->FlashReadLenthLE->text().isEmpty()){
        slotShowMsgBox("请先输入Flash读取的长度");
        return;
    }

    emit signalFlashToUiOrFile(CODE_FLAG_1);
    m_nReadFileDataTailSize = ui->FlashReadLenthLE->text().toInt();

    initProcessProgressBar();
    uartReadBytes512();
}

void CHearingAidvMainWindow::on_FlashFileReadBtn_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("读取到文件"), QDir::currentPath(), tr("Bin Files (*.bin);;Text Files (*.txt)"));
    if (!filePath.isEmpty()) {
        m_flashReadFile.setFile(filePath);
    }
}
int  CHearingAidvMainWindow::uartWriteFileBytes(int transmissionsCount,QByteArray fileDataArray)
{
    FileDealManger *m_pFileOperation = nullptr;
    m_pFileOperation = new  FileDealManger();
    m_pFileOperation->setDataSourceUi(reinterpret_cast<unsigned char*>(fileDataArray.data()));

    m_nWriteFullSize = 0;
    QByteArray sendData;
    sendData.clear();
    bool ok;
    QString strResAddress = ui->FlashFromAddressLE->text().trimmed().replace(" ","");
    int dec = strResAddress.toInt(&ok,16);
    int address =dec + transmissionsCount*WRITE_DATA_SIZE;
    QByteArray startAddress =  m_pFileOperation->intToArry32(address);

    m_pFileOperation->getFilePiecewiseArry(m_nLastPackDataSize, transmissionsCount,m_nSendCount,&m_pArrySectionalData256,m_nWriteFullSize);
    QByteArray sendFileSizeArry = m_pFileOperation->intToArry32(m_nWriteFullSize);
    QByteArray fileDataArry = QByteArray::fromRawData(reinterpret_cast<const char *>(m_pArrySectionalData256), m_nWriteFullSize);

    sendData.append(startAddress);
    sendData.append(sendFileSizeArry);
    sendData.append(fileDataArry);
    int ret = m_pSerialWork->processData(CMD_UPDATE_0x03, INST_CMD_0x21,sendData);
    if(ret != 0)
        return ret;
    return 0;
}

void CHearingAidvMainWindow::otaUartWriteFileBytes512(int transmissionsCount,uchar cmd, uchar sunCmd,QByteArray fileDataArray)
{
    FileDealManger *m_pFileOperation = nullptr;
    m_pFileOperation = new  FileDealManger();
    m_nWriteFullSize = 0;
    QByteArray sendData;
    sendData.clear();
    m_pFileOperation->setDataSourceUi(reinterpret_cast<unsigned char*>(fileDataArray.data()));
    m_pFileOperation->getFilePiecewiseArry(m_nLastPackDataSize, transmissionsCount,m_nSendCount,&m_pArrySectionalData256,m_nWriteFullSize);
    QByteArray fileDataArry = QByteArray::fromRawData(reinterpret_cast<const char *>(m_pArrySectionalData256), m_nWriteFullSize);

    sendData.append(intToArry(transmissionsCount));
    sendData.append(intToArry(fileDataArry.size()));
    sendData.append(fileDataArry);
    int ret = m_pSerialWork->processData(cmd, sunCmd,sendData);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::otaUartReadFileBytes512(uchar cmd, uchar sunCmd)
{
    QByteArray array;
    int ret =  -1;
    int inputLen = m_nReadFileDataTailSize;

    int nSendCount = inputLen / WRITE_DATA_SIZE;
    int n512Count = nSendCount;
    int nLastPackDataSize = inputLen % WRITE_DATA_SIZE;
    if(nLastPackDataSize > 0){
        nSendCount += 1;
    }
    for(int i = 0; i < nSendCount; i++){
        array.clear();
        array.append(intToArry(i));
        if(n512Count > 0 && i < n512Count){
            array.append(intToArry(WRITE_DATA_SIZE));
        }else{
            array.append(intToArry(nLastPackDataSize));
        }
        ret = m_pSerialWork->processData(cmd, sunCmd, array);
        if(ret != 0)
            return;
    }
}

void CHearingAidvMainWindow::parsingFileHeader(QVector<uchar> boyData, QByteArray &FileHeadInfoArray, QString &time)
{
    uint32_t value = 0;
    FileHeadInfoArray.clear();
    QString strfileHeadIdentification = "WITMEM";
    for(int i = 1; i < boyData.size();i++){
        if(i <= 64){
            FileHeadInfoArray.append(boyData.at(i));
        }
        if(i == 6){
            if(strfileHeadIdentification != FileHeadInfoArray){
                QString msg = "未找到文件头标识";
                slotShowMsgBox(msg);
                return;
            }
        }
    }
    for (int i = 24; i < 27; i += 4) {
        for (int j = 3; j > 0; j--) {
            if (i + j < boyData.size()) {
                uint8_t  Calibration = static_cast<uint8_t >(boyData[i + j]);
                value = (value << 8) | Calibration;
            }
        }
    }
    m_nReadFileDataTailSize = value;
    QByteArray timeArray;
    for (int i = 65; i < 79; i++) {
        timeArray.append(boyData[i]);
    }
    time = QString(timeArray);
}

void CHearingAidvMainWindow::parsingFileHeaderInfoShow(QByteArray FileHeadInfoArray,QString time, int FileIndex, int HeadType)
{
    QByteArray FileCodeArray;
    QByteArray FileHeadArray;
    QByteArray FileVersion;

    for(int i = 0; i < FileHeadInfoArray.size(); i++){
        if(i < 8){
            FileHeadArray.append(FileHeadInfoArray.at(i));
        }else if(i <= 15){
            FileCodeArray.append(FileHeadInfoArray.at(i));
        }else if(i <= 23){
            FileVersion.append(FileHeadInfoArray.at(i));
        }
    }


    FileInfoHeadData infoData;
    infoData.nFileIndex = FileIndex;
    infoData.nFileType = HeadType;
    infoData.strWriteTime = parseTimeShowUi(time);;
    infoData.strFileCode = QString(FileCodeArray);
    infoData.strFileVersion =  QString(FileVersion);
    m_pShowFileHeadDialog->setFileHeadInfoText(infoData);
}

int CHearingAidvMainWindow::parsingOtaFileHeaderInfo(QByteArray FileHeadInfoArray,int Index, int HeadType,
                                                     FileInfoHeadData &infoData)
{
    QByteArray FileCodeArray;
    QByteArray FileHeadArray;
    QByteArray FileVersion;

    for(int i = 0; i < FileHeadInfoArray.size(); i++){
        if(i < 8){
            FileHeadArray.append(FileHeadInfoArray.at(i));
        }else if(i <= 15){
            FileCodeArray.append(FileHeadInfoArray.at(i));
        }else if(i <= 23){
            FileVersion.append(FileHeadInfoArray.at(i));
        }
    }

    infoData.nFileIndex = Index;
    infoData.nFileType = HeadType;
    infoData.strFileCode = QString(FileCodeArray);
    infoData.strFileVersion =  QString(FileVersion);
    return 0;
}

void CHearingAidvMainWindow::uartReadBytes512()
{
    QByteArray array;
    int inputLen = ui->FlashReadLenthLE->text().toInt();

    bool  ok;
    QString strResAddress = ui->FlashReadAddressLE->text().trimmed().replace(" ","");
    int dec = strResAddress.toInt(&ok,16);

    int nSendCount = inputLen / WRITE_DATA_SIZE;
    int n512Count = nSendCount;
    int nLastPackDataSize = inputLen % WRITE_DATA_SIZE;
    if(nLastPackDataSize > 0){
        nSendCount += 1;
    }
    for(int i = 0; i < nSendCount; i++){
        array.clear();
        int address = dec + i*WRITE_DATA_SIZE;
        QByteArray startAddress = intToArry(address);
        array.append(startAddress);
        if(n512Count > 0 && i < n512Count){
            array.append(intToArry(WRITE_DATA_SIZE));
        }else{
            array.append(intToArry(nLastPackDataSize));
        }
        int ret = m_pSerialWork->processData(CMD_UPDATE_0x03, INST_CMD_0x22, array);
        if(ret != 0)
            return;
    }
}

void CHearingAidvMainWindow::slotFlashDataUpdata(QString flashData,int flag)
{
    if(READ_DATA_UI == flag){
        ui->FlashReadTE->insertPlainText(flashData);
        ui->FlashReadTE->moveCursor(QTextCursor::End);
    }else if(READ_DATA_FILE == flag){
        QFile file(m_flashReadFile.filePath());
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)){
            if(m_flashReadFile.suffix() == "txt"){
                QByteArray byteArray = flashData.toLocal8Bit();
                QString strWriteData = byteArray;
                QTextStream stream(&file);
                stream << strWriteData;
            }else if(m_flashReadFile.suffix() == "bin"){
                flashData.replace(" ","");
                QByteArray binaryData = QByteArray::fromHex(flashData.toUtf8());
                QDataStream out(&file);
                out.writeRawData(binaryData.data(), binaryData.size());
            }
        }
        file.close();
    }else if(GET_CHECK_RESULT == flag){
        ui->flashVerifyResultLb->setText(flashData);
    }else if(PASSWORD_PERMISSION == flag){
        QString strPermissions = ui->getPermLevelCombox->currentText();
        ui->statusBarShowIsPermissionsLabel->setText(strPermissions);
    }else if(CLEAR_PASSWORD == flag){
        QString strPermissions = "—";
        ui->statusBarShowIsPermissionsLabel->setText(strPermissions);
    }
}

void CHearingAidvMainWindow::slotShowUpdataCmdReadData(int nReadType, QVector<uchar> &boyData)
{
    QByteArray fileHeadInfoArray;
    QByteArray readFileDataArray;
    readFileDataArray.clear();
    QFile file;
    QString timeWrite = 0;
    if(ALG_ARG_FILE_HEAD == nReadType && m_isViewVersion == true){
        m_nArgCountHead ++;
    }
    if(CFG_FILE_HEAD == nReadType || ALG_ARG_FILE_HEAD == nReadType || FIRMWARE_FILE_HEAD == nReadType){
        parsingFileHeader(boyData,fileHeadInfoArray,timeWrite);
        if(FIRMWARE_FILE_HEAD == nReadType && m_isViewVersion == false){
            QByteArray FileVersion = fileHeadInfoArray.mid(16,8);
            m_strFirmWareVersion = QString(FileVersion);
            noAddressWriteFile512(FIRMWARE_FILE_FLAG,FILE_INDEX_0x00,CMD_UPDATE_0x03,
                                  INST_CMD_0x12,INST_CMD_0x13,m_strFirmWareFilePath);
        }
        if(m_isViewVersion == true ){
            parsingFileHeaderInfoShow(fileHeadInfoArray,timeWrite,m_nArgCountHead,nReadType);
        }
        if(m_nArgCountHead == m_nArgCountRead && m_isViewVersion == true){
            m_pShowFileHeadDialog->exec();
            m_nArgCountHead = 0;
            m_isViewVersion = false;
        }else if (m_isViewVersion == false && FIRMWARE_FILE_HEAD != nReadType){
            otaUartReadFileBytes512(m_cCmd, m_cSubCmdSecond);
        }

        if(FIRMWARE_FILE_HEAD == nReadType){
            if(static_cast<int>(fileHeadInfoArray[6]) != static_cast<int>(FIRMWARE_FILE_FLAG)){
                fileHeadInfoArray[6] = static_cast<unsigned char>(FIRMWARE_FILE_FLAG);
            }
        }else if(ALG_ARG_FILE_HEAD == nReadType){
            if(static_cast<int>(fileHeadInfoArray[6]) != static_cast<int>(ALG_FILE_FLAG)){
                fileHeadInfoArray[6] = static_cast<unsigned char>(ALG_FILE_FLAG);
            }
        }else if(CFG_FILE_HEAD == nReadType){
            if(static_cast<int>(fileHeadInfoArray[6]) != static_cast<int>(CFG_FILE_FLAG)){
                fileHeadInfoArray[6] = static_cast<unsigned char>(CFG_FILE_FLAG);
            }
        }
        m_fileHeadInfoArray = fileHeadInfoArray;
    }else{
        if(CFG_FILE_READ_DATA == nReadType || ALG_ARG_FILE_READ_DATA == nReadType){
            file.setFileName(OTAModelInfo.filePath());
            int  wdrcDefault = 32;
            QByteArray tempArray;
            tempArray.append(wdrcDefault);
            readFileDataArray.append(m_fileHeadInfoArray);
            m_fileHeadInfoArray.clear();
            for(int  i = 1; i < boyData.size(); i++){
                readFileDataArray.append(boyData.at(i));
            }

            if (file.open(QIODevice::WriteOnly | QIODevice::Append)){
                QDataStream out(&file);
                out.writeRawData(readFileDataArray.data(), readFileDataArray.size());
            }
            file.close();
        }
    }
}


void CHearingAidvMainWindow::on_FlashReadToFileBt_clicked()
{
    m_nReadFileDataTailSize = 0;

    if(ui->FlashReadLenthLE->text().isEmpty()){
        QString msg = "请先输入Flash读取的长度";
        slotShowMsgBox(msg);
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, tr("读取到文件"),m_strFalshReadPath,
                                                    tr("Bin Files (*.bin);;Text Files (*.txt)"));
    if(filePath.isEmpty()){
        return;
    }

    m_nReadFileDataTailSize = ui->FlashReadLenthLE->text().toInt();
    m_flashReadFile.setFile(filePath);
    m_strFalshReadPath = m_flashReadFile.absolutePath();

    QFile file(filePath);
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    file.close();

    initProcessProgressBar();
    emit signalFlashToUiOrFile(CODE_FLAG_2);
    uartReadBytes512();
}

void CHearingAidvMainWindow::on_FlashFromUiBt_clicked()
{
    m_nTransmissionsCount = 0;
    m_nFileDataTailSize = 0;
    m_bIsUiWrite = true;
    m_UIDataSouce.clear();

    if(ui->FlashFromAddressLE->text().size() != HEX_DATA_LENTH){
        QString msg = "请输入正确的地址参数";
        slotShowMsgBox(msg);
        return;
    }
    if(ui->FlashFromLenthLE->text().isEmpty()){
        QString msg = "请先输入长度";
        showStatusBarlightBtn(msg,-1);
        return;
    }
    if(ui->FlashFromTE->toPlainText().isEmpty()){
        QString msg = "请先输入烧写的数据";
        slotShowMsgBox(msg);
        return;
    }

    QString text = ui->FlashFromTE->toPlainText().trimmed();
    QStringList numbers = text.split(" ");
    int inputLen = ui->FlashFromLenthLE->text().toInt();
    int lenWrite = inputLen < numbers.size() ? inputLen : numbers.size();

    m_nFileDataTailSize = lenWrite;
    m_nSendCount = m_nFileDataTailSize / WRITE_DATA_SIZE;
    m_nLastPackDataSize = m_nFileDataTailSize % WRITE_DATA_SIZE;
    if(m_nLastPackDataSize > 0){
        m_nSendCount += 1;
    }

    for (const auto& number : numbers) {
        bool ok;
        if(number.length() != 2){
            QString msg = QString("数据长度不等于2！错误数据为:%1").arg(number);
            slotShowMsgBox(msg);
            return;
        }
        int value = number.toInt(&ok,16);
        if (ok) {
            uchar byte = static_cast<uchar>(value);
            m_UIDataSouce.append(byte);
        }
    }
    initProcessProgressBar();
    for(int i = 0 ; i < m_nSendCount; i++){
        int ret = uartWriteFileBytes(i,m_UIDataSouce);
        if(ret != 0){
            return;
        }
    }
}


void CHearingAidvMainWindow::on_flashOpenFIleBt_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择烧录的文件"), m_strFalshWritePath, tr("Bin Files (*.bin);;Text Files (*.txt)"));
    if (!filePath.isEmpty()) {
        ui->FlashFromFileNameLE->setText(filePath);
    }
}

QByteArray CHearingAidvMainWindow::currentSysTime()
{
    QDateTime timeCurrent = QDateTime::currentDateTime();
    QString dateString = timeCurrent.toString("yyyyMMddhhmmss");
    QByteArray currData = dateString.toUtf8();
    return currData;
}

int CHearingAidvMainWindow::getFileHeadInfo(int type, QByteArray cfgFileData,QByteArray &fileHeadArray)
{
    QString strFileHeadPath = QCoreApplication::applicationDirPath() +"/JsonFile/FileHeadInfo.json";
    FileHeadInfoDialog *dialog =  new FileHeadInfoDialog(type,this);
    connect(dialog,&FileHeadInfoDialog::signalSaveData,this,&CHearingAidvMainWindow::slotSaveFileHeadInfo);
    QFile FileHead(strFileHeadPath);
    QString strFileV,strFileC;
    if (FileHead.open(QIODevice::ReadOnly)) {
        QByteArray data = FileHead.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = doc.object();
        QJsonObject FileHeadObj = jsonObj["fileHeadInfo"].toObject();
        if(type == CFG_FILE_FLAG){
            strFileV = FileHeadObj["CfgFileVersion"].toString();
            strFileC = FileHeadObj["CfgFileCode"].toString();
        }else if(type == ALG_FILE_FLAG){
            strFileV = FileHeadObj["AlgFileVersion"].toString();
            strFileC = FileHeadObj["AlgFileCode"].toString();
        }else if(type == FIRMWARE_FILE_FLAG){
            if(m_strFirmWareVersion.isEmpty()){
                strFileV = FileHeadObj["FirFileVersion"].toString();
            }else{
                strFileV = m_strFirmWareVersion;
            }

            strFileC = FileHeadObj["FirFileCode"].toString();
        }
    }
    if(type == FIRMWARE_FILE_FLAG){
        strFileV = m_strFirmWareVersion;
    }

    FileHead.close();
    dialog->setFileInitInfoText(strFileV,strFileC);

    QString strFileVersion,strFileCode;
    if(dialog->exec() == 1){
        strFileVersion = dialog->getFileVersionText();
        strFileCode = dialog->getFileIdentificationCodeText();
    }else{
        return -1;
    }

    QString strHeadIdentification = "WITMEM";
    uchar ch = 0x00;
    fileHeadArray.append(strHeadIdentification.toUtf8());
    fileHeadArray.append(type);
    for(int  i = fileHeadArray.size();i < 8;i++){
        fileHeadArray.append(ch);
    }
    fileHeadArray.append(strFileCode.toUtf8());
    for(int  i = strFileCode.length();i < 8;i++){
        fileHeadArray.append(ch);
    }

    fileHeadArray.append(strFileVersion.toUtf8());
    for(int  i = strFileVersion.length();i < 8;i++){
        fileHeadArray.append(ch);
    }
    fileHeadArray.append(intToArry(cfgFileData.size()));

    ushort CRCFile = m_pSerialWork->crc16(cfgFileData.data(),cfgFileData.size());
    fileHeadArray.append(QByteArray::fromRawData((char*)&CRCFile,2));
    for(int i = 0; i < 2;i++){
        fileHeadArray.append(ch);
    }

    for(int  i = fileHeadArray.size();i < 64;i++){
        fileHeadArray.append(ch);
    }
    return 0;
}

void CHearingAidvMainWindow::setVolumeKeyModeShow()
{
    int nKeyRange = ui->VolumeKeyStepComBox->currentText().toInt() * (ui->VolumeKeyStallNumComBox->currentText().toInt() -1);
    QString strRange;
    if(nKeyRange == 0){
        strRange = QString("[%1,0]").arg(QString::number(nKeyRange));
    }else{
        strRange = QString("[-%1,0]").arg(QString::number(nKeyRange));
    }

    ui->VolumeKeyModeShowRangeLabel->setText(strRange);
}

QByteArray CHearingAidvMainWindow::getPickUpArgdata()
{
    QByteArray arrayPickUpData;
    int nLeftPickUpArgDigitalGain = ui->pickupLeftChannelDGAEdit->text().toInt();
    int nLeftPickUpArgAnalogGain = ui->pickupLeftChannelPGAEdit->text().toInt();
    int nRightPickUpArgDigitalGain = ui->pickupRightChannelDGAEdit->text().toInt();
    int nRightPickUpArgAnalogGain = ui->pickupRightChannelPGAEdit->text().toInt();

    arrayPickUpData.append(nLeftPickUpArgDigitalGain);
    arrayPickUpData.append(nLeftPickUpArgAnalogGain);
    arrayPickUpData.append(nRightPickUpArgDigitalGain);
    arrayPickUpData.append(nRightPickUpArgAnalogGain);
    return  arrayPickUpData;
}

QByteArray CHearingAidvMainWindow::getAudioCompensationValueData()
{
    uchar ch = 0x00;
    QByteArray arrayAudioCompensationData;
    int nLeftAudioCompensationGain = ui->LeftChannelPreGainBox->currentText().toInt();
    int nRightAudioCompensationGain = ui->RightChannelPreGainBox->currentText().toInt();
    int nCutLowFreq = ui->cbCutLowFreq->currentIndex();

    arrayAudioCompensationData.append(nLeftAudioCompensationGain);
    arrayAudioCompensationData.append(nRightAudioCompensationGain);
    arrayAudioCompensationData.append(nCutLowFreq);

    for(int i = arrayAudioCompensationData.size(); i < 16; i++){
        arrayAudioCompensationData.append(ch);
    }
    return  arrayAudioCompensationData;
}

QByteArray CHearingAidvMainWindow::getVolumeModeldata()
{
    QByteArray arrayVolumeModelData;
    uchar VolumeTypeKey = 0x01;
    uchar VolumeTypeBtn = 0x00;

    int nVolumeStall = ui->VolumeKeyStallNumComBox->currentText().toInt();
    int nVolumeStep = ui->VolumeKeyStepComBox->currentText().toInt();
    int nVolumeCycle = ui->VolumeKeyCyCleComBox->currentIndex();

    int nVolumeDirect = ui->VolumeKnobDirectionComBox->currentIndex();
    int nVolumeRange = ui->VolumeKnobRangeComBox->currentText().toInt();
    if(ui->VolumeKeyModeRadioBtn->isChecked()){
        arrayVolumeModelData.append(VolumeTypeKey);
    }else if(ui->VolumeKnobModeRadioBtn->isChecked()){
        arrayVolumeModelData.append(VolumeTypeBtn);
    }
    arrayVolumeModelData.append(nVolumeStall);
    arrayVolumeModelData.append(nVolumeStep);
    arrayVolumeModelData.append(nVolumeCycle);
    arrayVolumeModelData.append(nVolumeRange);
    arrayVolumeModelData.append(nVolumeDirect);
    return arrayVolumeModelData;
}

QByteArray CHearingAidvMainWindow::getBackUpArgdata()
{
    QByteArray arrayBackUpData;
    int nLeftPalyBackArg = ui->playbackArgLeftChannelDGAEdilt->text().toInt();
    int nRightPalyBackArg = ui->playbackArgRightChannelDGAEdilt->text().toInt();
    arrayBackUpData.append(nLeftPalyBackArg);
    arrayBackUpData.append(nRightPalyBackArg);
    return arrayBackUpData;
}

QByteArray CHearingAidvMainWindow::getDeviceNumdata()
{
    QByteArray sendArryData;
    QByteArray sendFileData;
    sendArryData.clear();
    sendFileData.clear();

    QString strMicLeft = ui->DeviceModeNumLeftMicBox->currentText();
    QString strMicRight = ui->DeviceModeNumRightMicBox->currentText();
    QString strReceiverLeft = ui->DeviceModeNumLeftReceiverBox->currentText();
    QString strReceiverRight = ui->DeviceModeNumRightReceiverBox->currentText();

    uchar ModeType = 0x00;
    QByteArray arryModeNumMicLeft = strMicLeft.toUtf8();
    for(int  i = arryModeNumMicLeft.size();i < MODE_NUM_LENTH;i++){
        arryModeNumMicLeft.append(ModeType);
    }

    QByteArray arryModeNumMicRight = strMicRight.toUtf8();
    for(int  i = arryModeNumMicRight.size();i < MODE_NUM_LENTH;i++){
        arryModeNumMicRight.append(ModeType);
    }

    QByteArray arryModeNumReceiverLeft = strReceiverLeft.toUtf8();
    for(int  i = arryModeNumReceiverLeft.size();i < MODE_NUM_LENTH;i++){
        arryModeNumReceiverLeft.append(ModeType);
    }

    QByteArray arryModeNumReceiverRight = strReceiverRight.toUtf8();
    for(int  i = arryModeNumReceiverRight.size();i < MODE_NUM_LENTH;i++){
        arryModeNumReceiverRight.append(ModeType);
    }

    sendArryData.append(arryModeNumMicLeft);
    getFileCalibrationData(DEVICE_MIC,strMicLeft,sendFileData);
    sendArryData.append(sendFileData);
    sendFileData.clear();

    sendArryData.append(arryModeNumMicRight);
    getFileCalibrationData(DEVICE_MIC,strMicRight,sendFileData);
    sendArryData.append(sendFileData);
    sendFileData.clear();

    sendArryData.append(arryModeNumReceiverLeft);
    getFileCalibrationData(DEVICE_RECEIVER,strReceiverLeft,sendFileData);
    sendArryData.append(sendFileData);
    sendFileData.clear();

    sendArryData.append(arryModeNumReceiverRight);
    getFileCalibrationData(DEVICE_RECEIVER,strReceiverRight,sendFileData);
    sendArryData.append(sendFileData);

    return sendArryData;
}

QByteArray CHearingAidvMainWindow::getAlgArgCountdata()
{
    QByteArray algArgCountArray;
    int nAlgArgCount = ui->AlgArgCountBox->currentText().toInt();
    algArgCountArray.append(nAlgArgCount);
    return algArgCountArray;
}

QByteArray CHearingAidvMainWindow::getWDRCData()
{
    QByteArray GainArray;
    QByteArray MPOArray;
    QByteArray CTArray;
    QByteArray CRArray;
    QByteArray sendArray;
    uchar ch = 0x00;
    float chCR = 0;
    int nATArg = ui->WDRCATLineEdit->text().trimmed().toUInt();
    int nRTArg = ui->WDRCRTLineEdit->text().trimmed().toUInt();

    CWDRCItemWidget *pWidget = NULL;
    QList<CWDRCItemWidget*>WdrcitemList = ui->scrollAreaWidgetContents->findChildren<CWDRCItemWidget*>("enableWidget");
    for(int  i = 0; i < WdrcitemList.size();i++){
        pWidget = WdrcitemList.at(i);
        if(pWidget){
            GainArray .append(pWidget->getWDRCData().pGainComBox->currentText().toUInt());
            MPOArray .append(pWidget->getWDRCData().pMPOCoBox->currentText().toUInt());
            CTArray .append(pWidget->getWDRCData().pCTComBox->currentText().toUInt());
            float CR = pWidget->getWDRCData().pCRSpinBox->text().toFloat();
            CRArray .append(QByteArray::fromRawData((char*)&CR, 4));
        }
    }

    for(int i = GainArray.size(); i < WDRC_CHANNGL_COUNT; i++){
        GainArray.append(ch);
        MPOArray.append(ch);
        CTArray.append(ch);
        CRArray .append(QByteArray::fromRawData((char*)&chCR, 4));
    }

    sendArray.append(GainArray);
    sendArray.append(MPOArray);
    sendArray.append(CTArray);
    sendArray.append(CRArray);
    sendArray.append(intToArry(nATArg));
    sendArray.append(intToArry(nRTArg));
    return sendArray;
}

QByteArray CHearingAidvMainWindow::getEQData()
{
    CEQItemWidget *pWidget = NULL;
    QByteArray arrySendData;
    QList<CEQItemWidget*>EqitemList = ui->EQScrollAreaWidgetContents->findChildren<CEQItemWidget*>();
    for(int i =0; i < EqitemList.size(); i++){
        pWidget = EqitemList.at(i);
        if(pWidget){
            int ndBNum = pWidget->getEqData().pdBLabelValue->text().toInt();
            arrySendData.append(ndBNum);
        }
    }
    return arrySendData;
}

QByteArray CHearingAidvMainWindow::getNRData()
{
    QByteArray arrayNrData;
    uchar ch = 0;
    int nNrArgNum = ui->NoiseHorizontalSlider->value();
    arrayNrData.append(nNrArgNum);
    for(int i = arrayNrData.size(); i < 8; i++){
        arrayNrData.append(ch);
    }
    return arrayNrData;
}

QByteArray CHearingAidvMainWindow::getNLIData()
{
    QByteArray sendArray;
    uchar ch = 0x00;
    int nLeftNRNLICalibration =  ui->NRNLICalibrationLeftLineEdit->text().toInt();
    int nRightNRNLICalibration =  ui->NRNLICalibrationRightLineEdit->text().toInt();
    sendArray.append(ch);
    sendArray.append(nLeftNRNLICalibration);
    sendArray.append(nRightNRNLICalibration);
    return sendArray;
}

int CHearingAidvMainWindow::getBinFileData(uchar index,int fileType, QString strFilePath, QByteArray &FileDataArray,QByteArray &FileDataHeadArray)
{
    QString msg;
    QFile file(strFilePath);
    if(!file.exists()){
        if(fileType == ALG_FILE_FLAG){
            msg = QString("%1文件不存在").arg(algFileTypeMap.value(index));
        }else{
            msg = QString("%1文件不存在").arg(FileTypeMap.value(fileType));
        }
        slotShowMsgBox(msg);
        return -1;
    }

    if(file.size() == 0){
        if(fileType == ALG_FILE_FLAG){
            msg = QString("%1文件为空").arg(algFileTypeMap.value(index));
        }else{
            msg = QString("%1文件为空").arg(FileTypeMap.value(fileType));
        }
        slotShowMsgBox(msg);
        return -1;
    }
    if(!file.open(QIODevice::ReadOnly)){
        QString msg = "文件无法打开";
        slotShowMsgBox(msg);
        return -1;
    }

    FileDataArray = file.readAll();

    if(fileType != 99){
        QString strfileHeadIdentification = "WITMEM";
        QByteArray fileArrayTemp;
        FileDataHeadArray = FileDataArray.mid(0,64);
        for(int i = 0; i < FileDataHeadArray.size();i++){
            fileArrayTemp.append(FileDataHeadArray.at(i));
            if(i == 5){
                if(strfileHeadIdentification != fileArrayTemp){
                    if(fileType == ALG_FILE_FLAG){
                        msg = QString("%1文件不含文件头，请重新选择").arg(algFileTypeMap.value(index));
                    }else{
                        msg = QString("%1文件不含文件头，请重新选择").arg(FileTypeMap.value(fileType));
                    }
                    slotShowMsgBox(msg);
                    return -1;
                }
            }else if(i == 6){
                if(fileArrayTemp.at(6) != fileType){
                    if(fileType == ALG_FILE_FLAG){
                        msg = QString("%1文件不合法，请重新选择").arg(algFileTypeMap.value(index));
                    }else{
                        msg = QString("%1文件不合法，请重新选择").arg(FileTypeMap.value(fileType));
                    }
                    slotShowMsgBox(msg);
                    slotShowMsgBoxdialog(msg);
                    return -1;
                }
            }
        }
        FileDataArray = FileDataArray.mid(64,FileDataArray.size()-1);

        uint8_t left = FileDataHeadArray.at(29);
        uint8_t right = FileDataHeadArray.at(28);
        ushort RecvCRC = (left << 8) | right;
        ushort CRCAlgFile = m_pSerialWork->crc16(FileDataArray.data(),FileDataArray.size());
        if(RecvCRC != CRCAlgFile){
            if(fileType == ALG_FILE_FLAG){
                msg = QString("%1文件已篡改，请重新选择").arg(algFileTypeMap.value(index));
            }else{
                msg = QString("%1文件已篡改，请重新选择").arg(FileTypeMap.value(fileType));
            }
            slotShowMsgBox(msg);
            slotShowMsgBoxdialog(msg);
            return -1;
        }
    }
    return 0;
}

int CHearingAidvMainWindow::get128ParameterFileData(uchar index,int type, QString strFilePath,QByteArray &FileDataArrayTail)
{
    uchar ch = 0x00;
    QByteArray algArrayData;
    QByteArray algArrayHeadData;
    int ret = getBinFileData(index,type,strFilePath,algArrayData,algArrayHeadData);
    FileDataArrayTail.append(algArrayHeadData);
    QByteArray time = currentSysTime();
    FileDataArrayTail.append(time);
    for(int i = FileDataArrayTail.size(); i < 128; i++){
        FileDataArrayTail.append(ch);
    }
    FileDataArrayTail.append(algArrayData);
    for(int i = FileDataArrayTail.size(); i < 4*OTA_DATA_SIZE_K; i++){
        FileDataArrayTail.append(ch);
    }
    return ret;
}

void CHearingAidvMainWindow::getMemoryAreaData(QByteArray &MemoryAreaDataArray)
{
    QByteArray MemoryAreaDataArrayInit;
    QString strHeadIdentification = "WITMEM";
    uchar ch = 0x00;
    uchar type = 0x03;

    for(int i = 0; i < 4; i++){
        MemoryAreaDataArrayInit.append(ch);
    }
    MemoryAreaDataArray.append(strHeadIdentification.toUtf8());
    MemoryAreaDataArray.append(type);
    for(int  i = MemoryAreaDataArray.size();i < 24;i++){
        MemoryAreaDataArray.append(ch);
    }
    MemoryAreaDataArray.append(intToArry(MemoryAreaDataArrayInit.size()));

    ushort CRCFile = m_pSerialWork->crc16(MemoryAreaDataArrayInit.data(),MemoryAreaDataArrayInit.size());
    MemoryAreaDataArray.append(QByteArray::fromRawData((char*)&CRCFile,2));
    for(int  i = MemoryAreaDataArray.size();i < 64;i++){
        MemoryAreaDataArray.append(ch);
    }
    MemoryAreaDataArray.append(MemoryAreaDataArrayInit);
    for(int  i = MemoryAreaDataArray.size();i < 4*OTA_DATA_SIZE_K;i++){
        MemoryAreaDataArray.append(ch);
    }
}

int CHearingAidvMainWindow::getOtaModelPathIsEmpty()
{
    QString msg;
    if(ui->firmwareUpdataFilePathlineEdit->text().isEmpty()){
        msg = "请选择一个固件文件";
        slotShowMsgBox(msg);
        return -1;
    }
    if(ui->configFilePathEdit->text().isEmpty()){
        msg = "请选择一个配置文件";
        slotShowMsgBox(msg);
        return -1;
    }

    for(int  i = 0; i < ui->exportArgCountBox->currentText().toInt(); i++){
        if(m_AlgModelList.at(i).pAlgFilePath->text().isEmpty()){
            msg = QString("请选择模式%1文件").arg(i+1);
            slotShowMsgBox(msg);
            return -1;
        }
    }
    return 0;
}

void CHearingAidvMainWindow::saveInputData()
{
    QString strCfgFilePath = QCoreApplication::applicationDirPath() +"/JsonFile/cfgFile.json";

    QJsonObject json;
    QJsonObject FlashData;
    QJsonObject PureToneData;
    QJsonObject PassWordData;
    QJsonObject WDRCData;
    QJsonObject NRNLIData;
    QJsonObject fileDialogDirData;
    QJsonObject serialPortData;
    QJsonObject otaModelFilePath;

    serialPortData["SerialName"] =  ui->SerialNameBox->currentText();
    serialPortData["SerialFre"] =   ui->FreLabel->text();

    FlashData["ReadAdd"] = ui->FlashReadAddressLE->text().trimmed();
    FlashData["ReadLen"] = ui->FlashReadLenthLE->text().trimmed();
    FlashData["WriteAdd"] = ui->FlashFromAddressLE->text().trimmed();
    FlashData["WriteLen"] = ui->FlashFromLenthLE->text().trimmed();
    FlashData["EraseAdd"] = ui->FlashEraseAddressLE->text().trimmed();
    FlashData["EraseType"] = ui->FlashEraseCombox->currentIndex();
    FlashData["CheckAdd"] = ui->FlashVerifyAddressLE->text().trimmed();
    FlashData["CheckLen"] = ui->FlashVerifyLenthLE->text().trimmed();
    FlashData["BackSrceAdd"] = ui->FLashCopySrcAddressLE->text().trimmed();
    FlashData["BackDesAdd"] = ui->FlashCopyDestAddrLE->text().trimmed();
    FlashData["BackLen"] = ui->FlashCopyLenthLE->text().trimmed();

    PureToneData["SPL"] = ui->soundPresLE->text().trimmed();
    PureToneData["Frequency"] = ui->soundFrequencyLE->text().trimmed();
    PureToneData["Cycle"] = ui->soundCycleLE->text().trimmed();
    PureToneData["Sound"] = ui->soundContinueLE->text().trimmed();
    PureToneData["SoundNum"] = ui->soundNumsLE->text().trimmed();

    PassWordData["InitPerm"] = ui->InitPasswordLevelCombox->currentIndex();
    PassWordData["InputPerm"] = ui->getPermLevelCombox->currentIndex();
    PassWordData["ModifyPerm"] = ui->changePaswdLevelCombox->currentIndex();
    PassWordData["ClearPerm"] = ui->clearPaswdLevelCombox->currentIndex();

    WDRCData["AT"] = ui->WDRCATLineEdit->text().trimmed();
    WDRCData["ChannelCount"] = ui->WDRCChannelCounBox->currentText();
    WDRCData["RT"] = ui->WDRCRTLineEdit->text().trimmed();

    NRNLIData["LeftValue"] = ui->NRNLICalibrationLeftLineEdit->text().trimmed();
    NRNLIData["RightValue"] = ui->NRNLICalibrationRightLineEdit->text().trimmed();

    fileDialogDirData["FlashReadDir"] = m_strFalshReadPath;
    fileDialogDirData["FlashWriteDir"] = m_strFalshWritePath;
    fileDialogDirData["OtaModelDir"] = m_strOTAModelPath;

    otaModelFilePath["firmwareFilePath"] = ui->firmwareUpdataFilePathlineEdit->text().trimmed();
    otaModelFilePath["cfgFilePath"] = ui->configFilePathEdit->text().trimmed();
    otaModelFilePath["alg1FilePath"] = ui->AlgArgUpdataFilePathlineEdit_1->text().trimmed();
    otaModelFilePath["alg2FilePath"] = ui->AlgArgUpdataFilePathlineEdit_2->text().trimmed();
    otaModelFilePath["alg3FilePath"] = ui->AlgArgUpdataFilePathlineEdit_3->text().trimmed();
    otaModelFilePath["alg4FilePath"] = ui->AlgArgUpdataFilePathlineEdit_4->text().trimmed();
    otaModelFilePath["algFileCount"] = ui->exportArgCountBox->currentIndex();

    json["FlashData"] = FlashData;
    json["PureToneData"] = PureToneData;
    json["PassWordData"] = PassWordData;
    json["WDRCData"] = WDRCData;
    json["NRNLIData"] = NRNLIData;
    json["fileDialogDirData"] = fileDialogDirData;
    json["serialPortData"] = serialPortData;
    json["otaModelFilePath"] = otaModelFilePath;

    QJsonDocument doc(json);
    QFile cfgFile(strCfgFilePath);
    if (cfgFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        cfgFile.write(doc.toJson());
    }
    cfgFile.close();
}

void CHearingAidvMainWindow::closeEvent(QCloseEvent *event)
{
    saveInputData();
    QMainWindow::closeEvent(event);
}

void CHearingAidvMainWindow::slotEqSigalClicked()
{
    CEQItemWidget *pWidget = NULL;
    bool isSelectAll = false;
    QList<CEQItemWidget*>EqitemList = ui->EQScrollAreaWidgetContents->findChildren<CEQItemWidget*>();
    for(int i = 0; i < EqitemList.size();i++){
        pWidget = EqitemList.at(i);
        if(pWidget){
            isSelectAll = pWidget->getSignalCheckBox()->isChecked();
            if(isSelectAll ==  false){
                break;
            }
        }
    }

    emit signalEQIsSelectAll(isSelectAll);
}

void CHearingAidvMainWindow::slotWDRCSigalClicked()
{
    CWDRCItemWidget *pWidget = NULL;
    bool isSelectAll = false;
    QList<CWDRCItemWidget*>WdrcitemList = ui->scrollAreaWidgetContents->findChildren<CWDRCItemWidget*>("enableWidget");
    for(int  i = 0; i < WdrcitemList.size();i++){
        pWidget = WdrcitemList.at(i);
        if(pWidget){
            isSelectAll = pWidget->getChannelSingleCheckBox()->isChecked();
            if(isSelectAll ==  false){
                break;
            }
        }
    }
    emit signalWDRCIsSelectAll(isSelectAll);
}

void CHearingAidvMainWindow::on_FlashFromFileBt_clicked()
{
    m_nTransmissionsCount = 0;
    m_nFileDataTailSize = 0;
    m_bIsUiWrite = false;
    initProcessProgressBar();
    m_flashFromFile.setFile(ui->FlashFromFileNameLE->text().trimmed());
    m_strFalshWritePath = m_flashFromFile.absolutePath();

    if(ui->FlashFromAddressLE->text().size() != HEX_DATA_LENTH){
        QString msg = "请输入正确的地址参数";
        slotShowMsgBox(msg);
        return;
    }

    if(ui->FlashFromFileNameLE->text().isEmpty()){
        QString msg = "请先选择待烧写的文件";
        slotShowMsgBox(msg);
        return;
    }

    QFile file(m_flashFromFile.filePath());
    if(!file.exists()){
        QString msg = QString("%1文件不存在").arg("Flash");
        slotShowMsgBox(msg);
        return;
    }

    if(file.size() == 0){
        QString msg =  QString("%1文件为空").arg("Flash");
        slotShowMsgBox(msg);
        return;
    }

    QByteArray array;
    array = getHexAddr(ui->FlashFromAddressLE);

    int lenWrite;
    int inputLen = ui->FlashFromLenthLE->text().toInt();
    int fileLen = m_flashFromFile.size();

    QByteArray FalshDataArray;
    QFile objectFile(m_flashFromFile.filePath());
    objectFile.open(QIODevice::ReadOnly);
    if(m_flashFromFile.suffix() == "txt"){
        QTextStream stream(&objectFile);
        QString text = stream.readAll();
        QStringList numbers = text.split(" ");
        fileLen = numbers.size();
        for (const auto& number : numbers) {
            bool ok;
            int value = number.toInt(&ok,16);
            if (ok) {
                uchar byte = static_cast<uchar>(value);
                FalshDataArray.append(byte);
            }
        }
    }else{
        FalshDataArray = objectFile.readAll();
    }

    objectFile.close();

    if(ui->IsFileFullSizecheckBox->isChecked() == true){
        lenWrite = fileLen;
    }else{
        if(ui->FlashFromLenthLE->text().isEmpty()){
            QString msg = "请先输入长度";
            slotShowMsgBox(msg);
            return;
        }
        lenWrite = inputLen < fileLen ? inputLen : fileLen;
    }
    m_nFileDataTailSize = lenWrite;
    m_nSendCount = m_nFileDataTailSize / WRITE_DATA_SIZE;
    m_nLastPackDataSize = m_nFileDataTailSize % WRITE_DATA_SIZE;
    if(m_nLastPackDataSize > 0){
        m_nSendCount += 1;
    }
    for(int i = 0 ; i < m_nSendCount; i++){
        int ret = uartWriteFileBytes(i,FalshDataArray);
        if(ret != 0){
            return;
        }
    }
}

void CHearingAidvMainWindow::on_FlashEraseBt_clicked()
{
    if(ui->FlashEraseAddressLE->text().size() != HEX_DATA_LENTH){
        QString msg = "请输入正确的地址参数";
        slotShowMsgBox(msg);
        return;
    }
    QByteArray array;
    if(ui->FlashEraseCombox->currentIndex() == 0){
        array.append(uchar(CODE_FLAG_0));
        array.append(intToArry(CODE_FLAG_0));
    }else if(ui->FlashEraseCombox->currentIndex() == 1){
        array.append(uchar(CODE_FLAG_1));
        array.append(getHexAddr(ui->FlashEraseAddressLE));
    }else{
        array.append(uchar(CODE_FLAG_2));
        array.append(getHexAddr(ui->FlashEraseAddressLE));
    }
    int ret = m_pSerialWork->processData(CMD_UPDATE_0x03, INST_CMD_0x20, array);
    if(ret != 0)
        return;
}


void CHearingAidvMainWindow::on_FlashVerifyBt_clicked()
{
    if(ui->FlashVerifyAddressLE->text().size() != HEX_DATA_LENTH){
        QString msg = "请输入正确的flash校验的地址参数";
        slotShowMsgBox(msg);
        return;
    }
    if(ui->FlashVerifyLenthLE->text().isEmpty()){
        QString msg = "请输入正确的flash校验的长度参数";
        slotShowMsgBox(msg);
        return;
    }
    QByteArray array;
    array = getHexAddr(ui->FlashVerifyAddressLE);
    int inputLen = ui->FlashVerifyLenthLE->text().toInt();
    array.append(QByteArray::fromRawData((char*)&inputLen, 4));
    int ret = m_pSerialWork->processData(CMD_UPDATE_0x03, INST_CMD_0x24, array);
    if(ret != 0)
        return;
}


void CHearingAidvMainWindow::on_FlashCopyBt_clicked()
{
    if(ui->FlashCopyDestAddrLE->text().size() != HEX_DATA_LENTH ||
            ui->FLashCopySrcAddressLE->text().size() != HEX_DATA_LENTH){
        QString msg = "请输入正确的Flash备份的地址参数";
        slotShowMsgBox(msg);
        return;
    }
    if(ui->FlashCopyLenthLE->text().isEmpty()){
        QString msg = "请输入正确的Flash备份的长度参数";
        slotShowMsgBox(msg);
        return;
    }

    QByteArray array;
    array = getHexAddr(ui->FLashCopySrcAddressLE);
    array.append(getHexAddr(ui->FlashCopyDestAddrLE));
    int inputLen = ui->FlashCopyLenthLE->text().toInt();
    array.append(QByteArray::fromRawData((char*)&inputLen, 4));
    int ret =  m_pSerialWork->processData(CMD_UPDATE_0x03, INST_CMD_0x25, array);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::slotShowMsgBox(QString msg)
{
    showStatusBarlightBtn(msg,-1);
    return;
}

void CHearingAidvMainWindow::slotShowMsgBoxdialog(QString msg)
{
    QMessageBox::information(this, tr("提示"), QString("%1").arg(msg));
    return;
}

void CHearingAidvMainWindow::slotShowMsgDevStatus(QString msg)
{
    ui->statusBarShowDeviceStatusLabel->setText(msg);
}

void CHearingAidvMainWindow::slotFlashSwitchBtnClicked(bool checked)
{
    SwitchButton * pSwitchBtn =  qobject_cast<SwitchButton*>(sender());
    if(!m_pSerialWork->getSerialIsoOpen()){
        pSwitchBtn->setChecked(!checked);
        slotShowMsgBoxdialog("请先连接助听器");
        return;
    }

    QByteArray array;
    if(checked){
        array.append(uchar(0));
    }else{
        array.append(uchar(1));
    }

    int ret = m_pSerialWork->processData(CMD_UPDATE_0x03, INST_CMD_0x23, array);
    if(ret != 0){
        ui->flashSwitchBtn->setChecked(!checked);
    }
}

void CHearingAidvMainWindow::on_voiceCtrlBt_clicked()
{
    if(ui->soundContinueLE->text().isEmpty() ||
            ui->soundCycleLE->text().isEmpty() ||
            ui->soundFrequencyLE->text().isEmpty() ||
            ui->soundNumsLE->text().isEmpty() ||
            ui->soundPresLE->text().isEmpty()){
        QString msg = "请输入正确的纯音参数";
        slotShowMsgBox(msg);
        return;
    }
    if(ui->soundFrequencyLE->text().toInt() < 20){
        QString msg = "纯音频率值范围是[20-20000]";
        slotShowMsgBox(msg);
        return;
    }

    QByteArray array;
    uchar pres = ui->soundPresLE->text().toInt();
    ushort freq = ui->soundFrequencyLE->text().toUShort();
    ushort cyc = ui->soundCycleLE->text().toInt();
    ushort cotinue = ui->soundContinueLE->text().toInt();
    uchar num = ui->soundNumsLE->text().toInt();
    if(cyc < cotinue){
        QString msg = "纯音周期时长需大于等于嘀声时长";
        slotShowMsgBox(msg);
        return;
    }
    array.append(pres);
    array.append(QByteArray::fromRawData((char*)&freq, 2));
    array.append(QByteArray::fromRawData((char*)&cyc, 2));
    array.append(QByteArray::fromRawData((char*)&cotinue, 2));
    array.append(num);
    int ret = m_pSerialWork->processData(CMD_CTRL_0x01, INST_CMD_0x20, array);
    if(ret != 0)
        return;
    getCurrentWorkingMode();
}

void CHearingAidvMainWindow::on_permPasswdLE_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->SetPasswdErrorShow->text().isEmpty()){
        ui->SetPasswdErrorShow->clear();
    }
}

void CHearingAidvMainWindow::on_changeOldpaswdLE_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->ModifyPasswdErrorShow->text().isEmpty()){
        ui->ModifyPasswdErrorShow->clear();
    }
}

void CHearingAidvMainWindow::on_clearPaswdLE_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->ClearPasswdErrorShow->text().isEmpty()){
        ui->ClearPasswdErrorShow->clear();
    }
}

void CHearingAidvMainWindow::on_soundPresLE_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->ToneCtrlErrorShow->text().isEmpty()){
        ui->ToneCtrlErrorShow->clear();
    }
}

void CHearingAidvMainWindow::on_getInitPasswordBt_clicked()
{
    if(ui->InitPasswordEdit->text().length() != PASSWORD_LENTH){
        QString msg = "初始化密码不得少于8位";
        slotShowMsgBox(msg);
        return;
    }
    QByteArray array = getPaswd(ui->InitPasswordEdit);
    uchar level = ui->InitPasswordLevelCombox->currentText().toInt();
    array.append(level);
    int ret = m_pSerialWork->processData(CMD_UPDATE_0x03, INST_CMD_0x03, array);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::on_InitPasswordEdit_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->InitPasswordErrorShow->text().isEmpty()){
        ui->InitPasswordErrorShow->clear();
    }
}

void CHearingAidvMainWindow::on_IsFileFullSizecheckBox_clicked(bool checked)
{
    if(checked == true){
        ui->FlashFromLenthLE->setDisabled(true);
    }else{
        ui->FlashFromLenthLE->setDisabled(false);
    }
}

void CHearingAidvMainWindow::showProgressBarStart(int maxValue, int value)
{
    ui->showProcessProgressBar->setVisible(true);
    ui->showProcessProgressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->showProcessProgressBar->setMaximum(maxValue);
    ui->showProcessProgressBar->setValue(ui->showProcessProgressBar->value() + value);
    double dProgress = (ui->showProcessProgressBar->value() - ui->showProcessProgressBar->minimum()) * 100.0
            / (ui->showProcessProgressBar->maximum() - ui->showProcessProgressBar->minimum());
    ui->showProcessProgressBar->setFormat(QString("读/写进度为: %1%").arg(QString::number(dProgress, 'f', 2)));
}

int CHearingAidvMainWindow::noAddressWriteFile512(int fileType,uchar index,uchar cmd,uchar sunCmdFirst,uchar sunCmdSecond, QString path)
{
    initProcessProgressBar();
    m_nTransmissionsCount = 0;
    m_nFileDataTailSize = 0;

    QFileInfo FileInfo;
    FileInfo.setFile(path);

    QFile file(path);
    if(!file.exists()){
        QString msg;
        if(fileType == ALG_FILE_FLAG){
            msg = QString("%1文件不存在").arg(algFileTypeMap.value(index));
        }else{
            msg = QString("%1文件不存在").arg(FileTypeMap.value(fileType));
        }
        slotShowMsgBox(msg);
        return -1;
    }

    if(!file.open(QIODevice::ReadOnly)){
        QString msg = "文件无法打开";
        slotShowMsgBox(msg);
        return -1;
    }

    if(file.size() == 0){
        QString msg ;
        if(fileType == ALG_FILE_FLAG){
            msg = QString("%1文件为空").arg(algFileTypeMap.value(index));
        }else{
            msg = QString("%1文件为空").arg(FileTypeMap.value(fileType));
        }
        slotShowMsgBox(msg);
        return -1;
    }

    QByteArray FileDataArray = file.readAll();
    QByteArray fileHeadArray;
    QByteArray fileFullDataArray;
    QString msg;
    QString strfileHeadIdentification = "WITMEM";
    for(int i = 0 ; i < 64; i++){
        fileHeadArray.append(FileDataArray.at(i));
        if(i == 5){
            if(strfileHeadIdentification != fileHeadArray){
                if(fileType == ALG_FILE_FLAG){
                    msg = QString("%1文件不含文件头，请重新生成").arg(algFileTypeMap.value(index));
                }else{
                    msg = QString("%1文件不含文件头，请重新生成").arg(FileTypeMap.value(fileType));
                }
                slotShowMsgBox(msg);
                if(fileType == FIRMWARE_FILE_FLAG){
                    setFirmwareAddFileHead();
                }
                return -1;
            }
        }else if(i == 6){
            if(fileHeadArray.at(6) != fileType){
                if(fileType == ALG_FILE_FLAG){
                    msg = QString("%1文件不合法，请重新选择").arg(algFileTypeMap.value(index));
                }else{
                    msg = QString("%1文件不合法，请重新选择").arg(FileTypeMap.value(fileType));
                }
                slotShowMsgBox(msg);
                slotShowMsgBoxdialog(msg);
                return -1;
            }
        }
    }
    for(int i = fileHeadArray.size() ; i < FileDataArray.size(); i++){
        fileFullDataArray.append(FileDataArray.at(i));
    }

    uint8_t left = fileHeadArray.at(29);
    uint8_t right = fileHeadArray.at(28);
    ushort RecvCRC = (left << 8) | right;
    ushort CRCAlgFile = m_pSerialWork->crc16(fileFullDataArray.data(),fileFullDataArray.size());
    if(RecvCRC != CRCAlgFile){
        if(fileType == ALG_FILE_FLAG){
            msg = QString("%1文件已篡改，请重新选择").arg(algFileTypeMap.value(index));
        }else{
            msg = QString("%1文件已篡改，请重新选择").arg(FileTypeMap.value(fileType));
        }
        slotShowMsgBox(msg);
        slotShowMsgBoxdialog(msg);
        return -1;
    }

    if(fileType == ALG_FILE_FLAG){
        int WdrcChannelCount = fileFullDataArray[511];
        if(WdrcChannelCount == 0 && m_nWdrcChannelCount == WDRC_CHANNGL_COUNT){
            QMessageBox *msgBox =  new QMessageBox(this);
            QString strErrorMsg = QString("%1文件通道数量不匹配，是否更新文件").arg(algFileTypeMap.value(index));
            msgBox->setText(strErrorMsg);
            msgBox->setWindowTitle("提示");
            QPushButton* noButton = msgBox->addButton(QMessageBox::No);
            noButton->setText("否");
            QPushButton* continueButton = msgBox->addButton("更新", QMessageBox::AcceptRole);
            msgBox->exec();
            if (msgBox->clickedButton() == noButton) {
                strErrorMsg = QString("%1文件通道数量不匹配，已停止导入").arg(algFileTypeMap.value(index));
                slotShowMsgBox(strErrorMsg);
                return -1;
            }
            if (msgBox->clickedButton() == continueButton) {
                QString strAlgFilePath = QFileDialog::getSaveFileName(this, tr("更新模式文件"),m_strOTAModelPath,
                                                                      tr("Bin Files (*.bin);"));
                if(strAlgFilePath.isEmpty()){
                    return -1;
                }
                fileFullDataArray[511] = WDRC_CHANNGL_COUNT;

                QByteArray updateAlgFileArray;
                QByteArray updateAlgFileHeadArray;
                ushort CRCAlgFile = m_pSerialWork->crc16(fileFullDataArray.data(),fileFullDataArray.size());
                updateAlgFileHeadArray.append(QByteArray::fromRawData((char*)&CRCAlgFile,2));
                fileHeadArray.replace(28,updateAlgFileHeadArray.size(),updateAlgFileHeadArray);
                updateAlgFileArray.append(fileHeadArray);
                updateAlgFileArray.append(fileFullDataArray);

                QFile updateAlgeriaFile(strAlgFilePath);
                if (updateAlgeriaFile.open(QIODevice::WriteOnly | QIODevice::Truncate)){
                    QDataStream out(&updateAlgeriaFile);
                    out.writeRawData(updateAlgFileArray.data(), updateAlgFileArray.size());
                }
                updateAlgeriaFile.close();

                QString objName = QString("AlgArgUpdataFilePathlineEdit_%1").arg(index + 1);
                QLineEdit *pAlgFilePathEdit = this->findChild<QLineEdit*>(objName);
                if(pAlgFilePathEdit){
                    pAlgFilePathEdit->setText(strAlgFilePath);
                }
            }
        }else if(WdrcChannelCount != m_nWdrcChannelCount){
            msg = QString("%1文件通道数量不匹配，请重新选择").arg(algFileTypeMap.value(index));
            slotShowMsgBox(msg);
            slotShowMsgBoxdialog(msg);
            return -1;
        }
    }

    m_nFileDataTailSize = fileFullDataArray.size();
    QByteArray firstCmdArrayData;
    firstCmdArrayData.append(index);
    firstCmdArrayData.append(fileHeadArray);
    QByteArray timeArray =currentSysTime();
    firstCmdArrayData.append(timeArray);
    uchar ch = 0x00;
    for(int i = firstCmdArrayData.size();i < 128; i++){
        firstCmdArrayData.append(ch);
    }

    int ret = m_pSerialWork->processData(cmd, sunCmdFirst, firstCmdArrayData);
    m_nSendCount = m_nFileDataTailSize / WRITE_DATA_SIZE;
    m_nLastPackDataSize = m_nFileDataTailSize % WRITE_DATA_SIZE;
    if(m_nLastPackDataSize > 0){
        m_nSendCount += 1;
    }

    if(ret == 0){
        for(int i = 0 ; i < m_nSendCount; i++){
            otaUartWriteFileBytes512(i,cmd, sunCmdSecond, fileFullDataArray);
        }
        getWDRCChannelCount();
    }

    file.close();
    return ret;
}

void CHearingAidvMainWindow::noAddressReadFile512(uchar index,uchar cmd,uchar sunCmdFirst,uchar sunCmdSecond)
{
    m_cCmd = cmd;
    m_cSubCmdSecond = sunCmdSecond;

    m_nReadFileDataTailSize = 0;
    initProcessProgressBar();

    QByteArray sendArray;
    sendArray.append(index);
    int ret = m_pSerialWork->processData(cmd, sunCmdFirst, sendArray);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::on_VolumeAdd_clicked()
{
    uchar enable = 0x01;
    uchar disable = 0x00;
    uchar isDisable = 0x00;
    QByteArray arraySendData;

    if(ui->channelRightCtrlBtn->isChecked() && ui->channelLeftCtrlBtn->isChecked()){
        isDisable |= (enable << disable);
        isDisable |= (enable << enable);
    }else if(ui->channelLeftCtrlBtn->isChecked()){
        isDisable |= (enable << disable);
        isDisable &= ~(enable << enable);
    }else if (ui->channelRightCtrlBtn->isChecked()){
        isDisable &= ~(enable << disable);
        isDisable |= (enable << enable);
    }

    arraySendData.append(isDisable);
    int ret = m_pSerialWork->processData(CMD_CTRL_0x01, INST_CMD_0x00, arraySendData);
    if(ret != 0)
        return;
    getVolumeReadCurrent();
}

void CHearingAidvMainWindow::on_VolumeLessen_clicked()
{
    uchar enable = 0x01;
    uchar disable = 0x00;
    uchar isDisable = 0x00;
    QByteArray arraySendData;

    if(ui->channelRightCtrlBtn->isChecked() && ui->channelLeftCtrlBtn->isChecked()){
        isDisable |= (enable << disable);
        isDisable |= (enable << enable);
    }else if (ui->channelRightCtrlBtn->isChecked()){
        isDisable &= ~(enable << disable);
        isDisable |= (enable << enable);
    }else if(ui->channelLeftCtrlBtn->isChecked()){
        isDisable |= (enable << disable);
        isDisable &= ~(enable << enable);
    }
    arraySendData.append(isDisable);
    int ret = m_pSerialWork->processData(CMD_CTRL_0x01, INST_CMD_0x01, arraySendData);
    if(ret != 0)
        return;
    getVolumeReadCurrent();
}

void CHearingAidvMainWindow::slotVolumeMuteSwithBtnClicked(bool  checked)
{
    SwitchButton * pSwitchBtn =  qobject_cast<SwitchButton*>(sender());
    if(!m_pSerialWork->getSerialIsoOpen()){
        pSwitchBtn->setChecked(!checked);
        slotShowMsgBoxdialog("请先连接助听器");
        return;
    }

    uchar enable = 0x01;
    uchar disable = 0x00;
    uchar isDisable = 0x00;
    QByteArray arraySendData;
    if(ui->VolumeMuteLeftSwithBtn->isChecked() && !ui->VolumeMuteRightSwithBtn->isChecked()){
        isDisable &= ~(enable << disable);
        isDisable |= (enable << enable);
    }else if (!ui->VolumeMuteLeftSwithBtn->isChecked() && ui->VolumeMuteRightSwithBtn->isChecked()){
        isDisable |= (enable << disable);
        isDisable &= ~(enable << enable);
    }else if(ui->VolumeMuteLeftSwithBtn->isChecked() && ui->VolumeMuteRightSwithBtn->isChecked()){
        isDisable &= ~(enable << disable);
        isDisable &= ~(enable << enable);
    }else if(!ui->VolumeMuteLeftSwithBtn->isChecked() && !ui->VolumeMuteRightSwithBtn->isChecked()){
        isDisable |= (enable << disable);
        isDisable |= (enable << enable);
    }

    arraySendData.append(isDisable);
    int  ret = m_pSerialWork->processData(CMD_CTRL_0x01, INST_CMD_0x04, arraySendData);
    if(ret != 0){
        ui->VolumeMuteLeftSwithBtn->setChecked(!checked);
        ui->VolumeMuteRightSwithBtn->setChecked(!checked);
        return;
    }
    getVolumeCurrentMuteSattus();
}

void CHearingAidvMainWindow::getVolumeReadCurrent()
{
    QByteArray arraySendData;
    m_pSerialWork->processData(CMD_CTRL_0x01, INST_CMD_0x03, arraySendData);
}

void CHearingAidvMainWindow::getVolumeCurrentMuteSattus()
{
    QByteArray arraySendData;
    m_pSerialWork->processData(CMD_CTRL_0x01, INST_CMD_0x05, arraySendData);
}

void CHearingAidvMainWindow::slotShowReadCurrCtrlCMDNum(uchar firstArg, uchar secondArg,int nType)
{
    if(nType == CURRENT_VOLUME_NUM){
        qint8 nfirstArg = static_cast<qint8>(firstArg);
        qint8 nsecondArg = static_cast<qint8>(secondArg);
        ui->ReadCurrVolumeRightChannel->setText(QString("%1 dB").arg(QString::number(nsecondArg)));
        ui->ReadCurrVolumeLeftChannel->setText(QString("%1 dB").arg(QString::number(nfirstArg)));
    }else if(nType == CURRENT_MUTE_TATUS){
        ui->VolumeMuteLeftSwithBtn->setChecked(!firstArg);
        ui->VolumeMuteRightSwithBtn->setChecked(!secondArg);
    }else if(nType == CURRENT_ALG_ARG_TYPE){
        QList<QPushButton*> algArgBtnList;
        for(int  i = 1 ; i <= 4; i++){
            QString algArgBtnObj = QString("AlgARGBtn_%1").arg(i);
            QPushButton * btn = ui->groupBoxAlgArg->findChild<QPushButton*>(algArgBtnObj);
            algArgBtnList.append(btn);
        }
        for(int  i = 0; i <algArgBtnList.size(); i++){
            if(algArgBtnList.at(i)->objectName() == QString("AlgARGBtn_%1").arg(firstArg + 1)){
                algArgBtnList.at(i)->setStyleSheet("color:white;background:#3790FA;border-radius: 6px;");
            }else{
                algArgBtnList.at(i)->setStyleSheet("");
            }
        }
    }else if(nType == CURRENT_WORKING_MODE){
        QString radioBtnObj = QString("modeTypeBtn_%1").arg(firstArg);
        QRadioButton * btn = ui->groupBoxModeType->findChild<QRadioButton*>(radioBtnObj);
        btn->setChecked(true);
    }else if(nType == CURRENT_ALG_SWITCHBTN_STATUS){
        m_uFirstArg = firstArg;
        m_uSecondArg = secondArg;
        bool BypassSwitchstatus = m_uFirstArg & 0b1;
        bool FFTSwitchBtnStatus = m_uFirstArg >> 1 & 0b1;

        ui->algWDRCDescLabel->setEnabled(!BypassSwitchstatus && FFTSwitchBtnStatus);
        ui->algWDRCSwitchBtn->setEnabled(!BypassSwitchstatus && FFTSwitchBtnStatus);
        ui->algEQSwitchBtn->setEnabled(!BypassSwitchstatus && FFTSwitchBtnStatus);
        ui->algEQDescLabel->setEnabled(!BypassSwitchstatus && FFTSwitchBtnStatus);
        ui->algNRDescLabel->setEnabled(!BypassSwitchstatus && FFTSwitchBtnStatus);
        ui->algNRSwitchBtn->setEnabled(!BypassSwitchstatus && FFTSwitchBtnStatus);

        ui->algNliDescLabel->setEnabled(!BypassSwitchstatus && FFTSwitchBtnStatus);
        ui->algNLISwitchBtn->setEnabled(!BypassSwitchstatus && FFTSwitchBtnStatus);
        ui->algHSSwitchBtn->setEnabled(!BypassSwitchstatus);
        ui->algHSDescLabel->setEnabled(!BypassSwitchstatus);
        ui->algFFTSwitchBtn->setEnabled(!BypassSwitchstatus);
        ui->algFFTModeLabel->setEnabled(!BypassSwitchstatus);

        ui->algBypassSwitch->setChecked(m_uFirstArg & 0b1);
        ui->algFFTSwitchBtn->setChecked(m_uFirstArg >> 1 & 0b1);
        ui->algHSSwitchBtn->setChecked(m_uSecondArg & 0b1);
        ui->algNRSwitchBtn->setChecked(m_uSecondArg >>1 & 0b1);
        ui->algWDRCSwitchBtn->setChecked(m_uSecondArg >>2 & 0b1);
        ui->algEQSwitchBtn->setChecked(m_uSecondArg >>3 & 0b1);
        ui->algNLISwitchBtn->setChecked(m_uSecondArg >>4 & 0b1);
    }
}

void CHearingAidvMainWindow::slotCmdWriteFinshed(int size,int nBarType)
{
    if(nBarType == WRITE_DATA){
        showProgressBarStart(m_nFileDataTailSize,size);
    }else if(nBarType == READ_DATA){
        showProgressBarStart(m_nReadFileDataTailSize,size);
    }
}

void CHearingAidvMainWindow::slotChangeAlgArgBtnClick()
{
    QPushButton* pAlgArgBtn = qobject_cast<QPushButton*>(sender());
    QString strAlgArg =  pAlgArgBtn->objectName().trimmed().replace("AlgARGBtn_","");
    QByteArray arraySendData;
    uchar argNum = strAlgArg.toInt() -1;
    arraySendData.append(argNum);
    int ret =  m_pSerialWork->processData(CMD_CTRL_0x01, INST_CMD_0x11, arraySendData);
    if(ret != 0)
        return;
    getCurrentAlgArgType();
    getAlgDeviceData();
    if(ui->algFFTSwitchBtn->isChecked() == false ||
            ui->algEQSwitchBtn ->isChecked() == false ||
            ui->algNRSwitchBtn ->isChecked() == false ||
            ui->algWDRCSwitchBtn ->isChecked() == false){
        slotShowMsgBoxdialog("请使FFT 、WDRC、EQ、NR开关\n处于打开状态");
    }
}

void CHearingAidvMainWindow::getCurrentAlgArgType()
{
    QByteArray arraySendData;
    int ret = m_pSerialWork->processData(CMD_CTRL_0x01, INST_CMD_0x12, arraySendData);
    if(ret != 0)
        return;
    getWDRCChannelCount();
}

void CHearingAidvMainWindow::getAudioCompensationValueCmd()
{
    QByteArray arraySendData;
    int ret = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x25, arraySendData);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::setAudioCompensationValueCmd()
{
    QByteArray arraySendData = getAudioCompensationValueData();
    int ret = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x24, arraySendData);
    if(ret != 0)
        return;
}
void CHearingAidvMainWindow::slotChangeModeTypeBtnClick()
{
    QRadioButton* pModeTypeBtn = qobject_cast<QRadioButton*>(sender());
    if(!m_pSerialWork->getSerialIsoOpen()){
        QList<QRadioButton*>btnList = ui->groupBoxModeType->findChildren<QRadioButton*>();
        for(int i = 0; i < btnList.size(); i++){
            bool status  = m_btnStatusMap.value(btnList.at(i));
            if(status == true){
                btnList.at(i)->setChecked(true);
            }
        }
        slotShowMsgBoxdialog("请先连接助听器");
        return;
    }

    QString strModeType =  pModeTypeBtn->objectName().trimmed().replace("modeTypeBtn_","");
    QByteArray arraySendData;
    uchar argNum = strModeType.toInt();
    arraySendData.append(argNum);
    int ret = m_pSerialWork->processData(CMD_CTRL_0x01, INST_CMD_0x13, arraySendData);
    if(ret != 0)
        return;
    getCurrentWorkingMode();
    QList<QRadioButton*>btnList = ui->groupBoxModeType->findChildren<QRadioButton*>();
    for(int i = 0; i < btnList.size(); i++){
        if(btnList.at(i) == pModeTypeBtn){
            m_btnStatusMap.insert(pModeTypeBtn,pModeTypeBtn->isChecked());
        }else{
            m_btnStatusMap.insert(btnList.at(i),!pModeTypeBtn->isChecked());
        }
    }
}

void CHearingAidvMainWindow::getCurrentWorkingMode()
{
    QByteArray arraySendData;
    int ret = m_pSerialWork->processData(CMD_CTRL_0x01, INST_CMD_0x14, arraySendData);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::slotAlgSwitchBtnClicked(bool  checked)
{
    QByteArray arraySendData;
    SwitchButton * pSwitchBtn =  qobject_cast<SwitchButton*>(sender());
    if(!m_pSerialWork->getSerialIsoOpen()){
        pSwitchBtn->setChecked(!checked);
        slotShowMsgBoxdialog("请先连接助听器");
        return;
    }
    if(pSwitchBtn->objectName() == "algBypassSwitch"){
        if(ui->algBypassSwitch->isChecked() == true){
            m_uFirstArg |= (1 << 0);
        }else{
            m_uFirstArg &= ~(1 << 0);
        }
    }
    if(pSwitchBtn->objectName() == "algFFTSwitchBtn"){
        if(ui->algFFTSwitchBtn->isChecked() == true){
            m_uFirstArg |= (1 << 1);
            m_uFirstArg &= ~(1 << 0);
        }else{
            m_uFirstArg &= ~(1 << 1);
        }
    }
    if(pSwitchBtn->objectName() == "algHSSwitchBtn"){
        if(ui->algHSSwitchBtn->isChecked() == true)
            m_uSecondArg |= (1 << 0);
        else
            m_uSecondArg &= ~(1 << 0);
    }
    if(pSwitchBtn->objectName() == "algNRSwitchBtn"){
        if(ui->algNRSwitchBtn->isChecked() == true)
            m_uSecondArg |= (1 << 1);
        else
            m_uSecondArg &= ~(1 << 1);
    }
    if(pSwitchBtn->objectName() == "algWDRCSwitchBtn"){
        if(ui->algWDRCSwitchBtn->isChecked() ==true)
            m_uSecondArg |= (1 << 2);
        else
            m_uSecondArg &= ~(1 << 2);
    }
    if(pSwitchBtn->objectName() == "algEQSwitchBtn"){
        if(ui->algEQSwitchBtn->isChecked() == true)
            m_uSecondArg |= (1 << 3);
        else
            m_uSecondArg &= ~(1 << 3);
    }
    if(pSwitchBtn->objectName() == "algNLISwitchBtn"){
        if(ui->algNLISwitchBtn->isChecked() == true)
            m_uSecondArg |= (1 << 4);
        else
            m_uSecondArg &= ~(1 << 4);
    }

    arraySendData.append(m_uFirstArg);
    arraySendData.append(m_uSecondArg);
    int ret = m_pSerialWork->processData(CMD_CTRL_0x01, INST_CMD_0x15, arraySendData);
    getCurrentAlgSwitchBtnStatus();
    if(ret != 0){
        if(ui->algBypassSwitch->isChecked()){
            ui->algBypassSwitch->setChecked(!checked);
        }else if( ui->algNLISwitchBtn->isChecked()){
            ui->algNLISwitchBtn->setChecked(!checked);
        }else if(ui->algWDRCSwitchBtn->isChecked()){
            ui->algWDRCSwitchBtn->setChecked(!checked);
        }else if(ui->algNRSwitchBtn->isChecked()){
            ui->algNRSwitchBtn->setChecked(!checked);
        }
        else if(ui->algHSSwitchBtn->isChecked()){
            ui->algHSSwitchBtn->setChecked(!checked);
        }
        else if(ui->algFFTSwitchBtn->isChecked()){
            ui->algFFTSwitchBtn->setChecked(!checked);
        }
        return;
    }else{
        if(pSwitchBtn->objectName() == "algBypassSwitch"){
            ui->algWDRCDescLabel->setEnabled(!checked);
            ui->algEQDescLabel->setEnabled(!checked);
            ui->algNRDescLabel->setEnabled(!checked);
            ui->algNliDescLabel->setEnabled(!checked);
            ui->algHSDescLabel->setEnabled(!checked);
            ui->algFFTModeLabel->setEnabled(!checked);
            ui->algFFTSwitchBtn->setEnabled(!checked);
            ui->algNLISwitchBtn->setEnabled(!checked);
            ui->algHSSwitchBtn->setEnabled(!checked);
            ui->algNRSwitchBtn->setEnabled(!checked);
            ui->algWDRCSwitchBtn->setEnabled(!checked);
            ui->algEQSwitchBtn->setEnabled(!checked);
        }
        if(ui->algFFTSwitchBtn->isChecked() ==false){
            ui->algWDRCDescLabel->setEnabled(false);
            ui->algWDRCSwitchBtn->setEnabled(false);
            ui->algEQSwitchBtn->setEnabled(false);
            ui->algEQDescLabel->setEnabled(false);
            ui->algNRDescLabel->setEnabled(false);
            ui->algNRSwitchBtn->setEnabled(false);
            ui->algNLISwitchBtn->setEnabled(false);
            ui->algNliDescLabel->setEnabled(false);
        }

        if(pSwitchBtn->objectName() == "algFFTSwitchBtn"){
            ui->algWDRCDescLabel->setEnabled(checked);
            ui->algWDRCSwitchBtn->setEnabled(checked);
            ui->algEQSwitchBtn->setEnabled(checked);
            ui->algEQDescLabel->setEnabled(checked);
            ui->algNRDescLabel->setEnabled(checked);
            ui->algNRSwitchBtn->setEnabled(checked);
            ui->algNLISwitchBtn->setEnabled(checked);
            ui->algNliDescLabel->setEnabled(checked);
        }

        ui->algBypassSwitch->setChecked(m_uFirstArg & 0b1);
        ui->algFFTSwitchBtn->setChecked(m_uFirstArg >> 1 & 0b1);

        ui->algHSSwitchBtn->setChecked(m_uSecondArg & 0b1);
        ui->algNRSwitchBtn->setChecked(m_uSecondArg >>1 & 0b1);
        ui->algWDRCSwitchBtn->setChecked(m_uSecondArg >>2 & 0b1);
        ui->algEQSwitchBtn->setChecked(m_uSecondArg >>3 & 0b1);
        ui->algNLISwitchBtn->setChecked(m_uSecondArg >>4 & 0b1);
    }
}

void CHearingAidvMainWindow::slotShowCfgCmdReadData(int nReadType, QVector<uchar> &boyData)
{
    if(nReadType == CFG_DEVIVE_NUM_MODE){
        QByteArray arryLeftMicModeNum;
        QByteArray arryRightMicModeNum;
        QByteArray arryLeftMicCalibrationNum;
        QByteArray arryRightMicCalibrationNum;

        QByteArray arryLeftReceiverModeNum;
        QByteArray arryRightReceiverModeNum;
        QByteArray arryLeftReceiverCalibrationNum;
        QByteArray arryRightReceiverCalibrationNum;

        QByteArray arryMicLeftFileData;
        QByteArray arryMicRightFileData;
        QByteArray arryReceiverLeftFileData;
        QByteArray arryReceiverRightFileData;

        for(int  i = 1; i < boyData.size();i++){
            if(i <= MODE_NUM_LENTH){
                uchar a = boyData.at(i);
                arryLeftMicModeNum.append(a);
            }else if(i <= (MODE_NUM_LENTH + MODE_NUM_FFT_SIZE)){
                qint8 Calibration = static_cast<qint8>(boyData.at(i));
                arryLeftMicCalibrationNum.append(Calibration);
            }else if(i <= (2*MODE_NUM_LENTH + MODE_NUM_FFT_SIZE)){
                uchar a = boyData.at(i);
                arryRightMicModeNum.append(a);
            }else if(i <= 2*(MODE_NUM_LENTH + MODE_NUM_FFT_SIZE)){
                qint8 Calibration = static_cast<qint8>(boyData.at(i));
                arryRightMicCalibrationNum.append(Calibration);
            }else if(i <= (3*MODE_NUM_LENTH + 2*MODE_NUM_FFT_SIZE)){
                uchar a = boyData.at(i);
                arryLeftReceiverModeNum.append(a);
            }else if(i <= 3*(MODE_NUM_LENTH + MODE_NUM_FFT_SIZE)){
                qint8 Calibration = static_cast<qint8>(boyData.at(i));
                arryLeftReceiverCalibrationNum.append(Calibration);
            }else if(i <= (4*MODE_NUM_LENTH + 3*MODE_NUM_FFT_SIZE)){
                uchar a = boyData.at(i);
                arryRightReceiverModeNum.append(a);
            }else if(i <= 4*(MODE_NUM_LENTH + MODE_NUM_FFT_SIZE)){
                qint8 Calibration = static_cast<qint8>(boyData.at(i));
                arryRightReceiverCalibrationNum.append(Calibration);
            }
        }

        ui->DeviceModeNumLeftMicBox->setCurrentText(QString(arryLeftMicModeNum));
        ui->DeviceModeNumRightMicBox->setCurrentText(QString(arryRightMicModeNum));
        ui->DeviceModeNumLeftReceiverBox->setCurrentText(QString(arryLeftReceiverModeNum));
        ui->DeviceModeNumRightReceiverBox->setCurrentText(QString(arryRightReceiverModeNum));
        if(!m_micFileList.contains(QString(arryLeftMicModeNum))){
            ui->DeviceModeNumLeftMicBox->setCurrentIndex(-1);
        }

        if(!m_micFileList.contains(QString(arryRightMicModeNum))){
            ui->DeviceModeNumRightMicBox->setCurrentIndex(-1);
        }

        if(!m_resFileList.contains(QString(arryLeftReceiverModeNum))){
            ui->DeviceModeNumLeftReceiverBox->setCurrentIndex(-1);
        }

        if(!m_resFileList.contains(QString(arryRightReceiverModeNum))){
            ui->DeviceModeNumRightReceiverBox->setCurrentIndex(-1);
        }

        if(ui->DeviceModeNumLeftMicBox->currentIndex() != -1){
            getFileCalibrationData(DEVICE_MIC,arryLeftMicModeNum,arryMicLeftFileData);
            if(arryLeftMicCalibrationNum != arryMicLeftFileData){
                slotShowMsgBox("左麦克风校验值有误!");
            }
        }
        if(ui->DeviceModeNumRightMicBox->currentIndex() != -1){
            getFileCalibrationData(DEVICE_MIC,arryRightMicModeNum,arryMicRightFileData);
            if(arryRightMicCalibrationNum != arryMicRightFileData){
                slotShowMsgBox("右麦克风校验值有误!");
            }
        }

        if(ui->DeviceModeNumLeftReceiverBox->currentIndex() != -1){
            getFileCalibrationData(DEVICE_RECEIVER,arryLeftReceiverModeNum,arryReceiverLeftFileData);
            if(arryLeftReceiverCalibrationNum != arryReceiverLeftFileData){
                slotShowMsgBox("左受话器校验值有误!");
            }
        }
        if(ui->DeviceModeNumRightReceiverBox->currentIndex() != -1){
            getFileCalibrationData(DEVICE_RECEIVER,arryRightReceiverModeNum,arryReceiverRightFileData);
            if(arryRightReceiverCalibrationNum != arryReceiverRightFileData){
                slotShowMsgBox("右受话器校验值有误!");
            }
        }
    }else if(nReadType == CFG_PLAYBACK_ARG_MODE){
        qint8 nLeftPalyBack = static_cast<qint8>(boyData[1]);
        qint8 nRightPalyBack = static_cast<qint8>(boyData[2]);
        ui->playbackArgLeftChannelDGAEdilt->setText(QString::number(nLeftPalyBack));
        ui->playbackArgRightChannelDGAEdilt->setText(QString::number(nRightPalyBack));
    }else if(nReadType == CFG_VOLUME_MODE_VOLUMETYPE){
        if(boyData[1] == 0){
            ui->VolumeKnobModeRadioBtn->setChecked(true);
        }else{
            ui->VolumeKeyModeRadioBtn->setChecked(true);
        }
        ui->groupBox_Volume->setEnabled(boyData[1]);
    }else if(CFG_VOLUME_KEY_DATA == nReadType){
        ui->VolumeKeyStallNumComBox->setCurrentText(QString::number(boyData[1]));
        ui->VolumeKeyStepComBox->setCurrentText(QString::number(boyData[2]));
        ui->VolumeKeyCyCleComBox->setCurrentIndex(int(boyData[3]));
        setVolumeKeyModeShow();
    }else if(CFG_VOLUME_KNOB_DATA == nReadType){
        ui->VolumeKnobRangeComBox->setCurrentText(QString::number(boyData[1]));
        ui->VolumeKnobDirectionComBox->setCurrentIndex(int(boyData[2]));
    }else if(nReadType == CFG_PICKUP_ARG_MODE){
        qint8 nLeftPickUpArgDigitalGain =  static_cast<qint8>(boyData[1]);
        qint8 nLeftPickUpArgAnalogGain = static_cast<qint8>(boyData[2]);
        qint8 nRightPickUpArgDigitalGain = static_cast<qint8>(boyData[3]);
        qint8 nRightPickUpArgAnalogGain = static_cast<qint8>(boyData[4]);

        ui->pickupLeftChannelPGAEdit->setText(QString::number(nLeftPickUpArgAnalogGain));
        ui->pickupLeftChannelDGAEdit->setText(QString::number(nLeftPickUpArgDigitalGain));
        ui->pickupRightChannelPGAEdit->setText(QString::number(nRightPickUpArgAnalogGain));
        ui->pickupRightChannelDGAEdit->setText(QString::number(nRightPickUpArgDigitalGain));
    }else if(nReadType == CFG_ALG_ARG_COUNT){
        m_nArgCountRead = boyData[1];
        ui->AlgArgCountBox->setCurrentText(QString::number(boyData[1]));
        ui->exportArgCountBox->setCurrentText(QString::number(boyData[1]));
    }else if(nReadType == CFG_AUDIO_COMPENSATION){
        ui->LeftChannelPreGainBox->setCurrentText(QString::number(boyData[1]));
        ui->RightChannelPreGainBox->setCurrentText(QString::number(boyData[2]));
        ui->cbCutLowFreq->setCurrentIndex((boyData[3] == 0)?0:1);
    }
}

void CHearingAidvMainWindow::slotShowAlgeriaCmdReadData(int nReadType, QVector<uchar> &boyData)
{
    CWDRCItemWidget *pWidget = NULL;
    QList<CWDRCItemWidget*>WdrcitemList = ui->scrollAreaWidgetContents->findChildren<CWDRCItemWidget*>("enableWidget");

    if(nReadType == ALG_GET_EQ_ARG){
        CEQItemWidget *pWidget = NULL;
        QList<CEQItemWidget*>EqitemList = ui->EQScrollAreaWidgetContents->findChildren<CEQItemWidget*>();
        for(int i =0; i < EqitemList.size(); i++){
            pWidget = EqitemList.at(i);
            if(pWidget){
                qint8 ndBNum = static_cast<qint8>(boyData.at(i + 1));
                pWidget->getSlider()->blockSignals(true);
                pWidget->getSlider()->setValue(ndBNum);
                pWidget->setEqLabelText(ndBNum);
                m_eq_status[i]= pWidget->getSlider()->sliderPosition();
                pWidget->getSlider()->blockSignals(false);
            }
        }
    }else if(nReadType == ALG_GET_WDRC_ARG){
        QList <float> crList;
        for (int i = 97; i < boyData.size(); i += 4) {
            float valueSpinBox = 0;
            uint32_t value = 0;
            for (int j = 3; j >= 0; j--) {
                if (i + j < boyData.size()) {
                    uint8_t  Calibration = static_cast<uint8_t >(boyData[i + j]);
                    value = (value << 8) | Calibration;
                }
            }

            memcpy(&valueSpinBox, &value, sizeof(value));
            crList.append(valueSpinBox);
        }
        for(int  i = 0; i < WdrcitemList.size();i++){
            pWidget = WdrcitemList.at(i);
            if(pWidget){
                pWidget->getWDRCData().pGainComBox->setCurrentText(QString::number(boyData[i + 1]));
                pWidget->getWDRCData().pMPOCoBox->setCurrentText(QString::number(boyData[i + 1 + WdrcitemList.size()]));
                pWidget->getWDRCData().pCTComBox->setCurrentText(QString::number(boyData[i + 1 + 2*WdrcitemList.size()]));
                pWidget->getWDRCData().pCRSpinBox->blockSignals(true);
                pWidget->getWDRCData().pCRSpinBox->setValue(crList.at(i));
                pWidget->getWDRCData().pCRSpinBox->blockSignals(false);

                m_wdrcGainStatus[i] = pWidget->getWDRCData().pGainComBox->currentIndex();
                m_wdrcCTStatus[i]  = pWidget->getWDRCData().pCTComBox->currentIndex();
                m_wdrcMPOStatus[i] = pWidget->getWDRCData().pMPOCoBox->currentIndex();
                m_spinboxStatus[i] = pWidget->getWDRCData().pCRSpinBox->value();
            }
        }

        uint32_t value = 0;
        QList <uint32_t>list;
        for (int i = 225; i < boyData.size() -1; i += 4) {
            for (int j = 3; j >= 0; j--) {
                if (i + j < boyData.size()) {
                    uint8_t  Calibration = static_cast<uint8_t >(boyData[i + j]);
                    value = (value << 8) | Calibration;
                }
            }
            list.append(value);
        }
        ui->WDRCATLineEdit->setText(QString::number(list.at(0)));
        ui->WDRCRTLineEdit->setText(QString::number(list.at(1)));

    }else if(nReadType == ALG_GET_WDRC_CT){
        for(int  i = 0; i < WdrcitemList.size();i++){
            pWidget = WdrcitemList.at(i);
            pWidget->getWDRCData().pCTComBox->setCurrentText(QString::number(boyData[i + 1]));
        }
    }else if(nReadType == ALG_GET_WDRC_CR){
        QList <float> crList;
        for (int i = 1; i < boyData.size() -1; i += 4) {
            float valueSpinBox = 0;
            uint32_t value = 0;
            for (int j = 3; j >= 0; j--) {
                if (i + j < boyData.size()) {
                    uint8_t  Calibration = static_cast<uint8_t >(boyData[i + j]);
                    value = (value << 8) | Calibration;
                }
            }
            memcpy(&valueSpinBox, &value, sizeof(value));
            crList.append(valueSpinBox);
        }

        for(int  i = 0; i < WdrcitemList.size();i++){
            pWidget = WdrcitemList.at(i);
            pWidget->getWDRCData().pCRSpinBox->setValue(crList.at(i));
        }
    }else if(nReadType == ALG_GET_WDRC_GAIN){
        for(int  i = 0; i < WdrcitemList.size();i++){
            pWidget = WdrcitemList.at(i);
            pWidget->getWDRCData().pGainComBox->setCurrentText(QString::number(boyData[i + 1]));
        }
    }else if(nReadType == ALG_GET_WDRC_MPO){
        for(int  i = 0; i < WdrcitemList.size();i++){
            pWidget = WdrcitemList.at(i);
            pWidget->getWDRCData().pMPOCoBox->setCurrentText(QString::number(boyData[i + 1]));
        }
    }else if(nReadType ==  ALG_GET_WDRC_ATRT){
        uint32_t value = 0;
        QList <uint32_t>list;
        for (int i = 1; i < boyData.size() -1; i += 4) {
            for (int j = 3; j >= 0; j--) {
                if (i + j < boyData.size()) {
                    uint8_t  Calibration = static_cast<uint8_t >(boyData[i + j]);
                    value = (value << 8) | Calibration;
                }
            }
            list.append(value);
        }
        ui->WDRCATLineEdit->setText(QString::number(list.at(0)));
        ui->WDRCRTLineEdit->setText(QString::number(list.at(1)));
    }else if(nReadType == ALG_GET_DENOISE_ARG){
        if(boyData.size() >= 2){
            qint8 nNrArgNum = static_cast<qint8>(boyData.at(1));
            ui->NoiseHorizontalSlider->setValue(nNrArgNum);
        }
    }else if(nReadType == ALG_GET_DENOISE_LEVEL){
        if(boyData.size() >= 2){
            qint8 nSplNum = static_cast<qint8>(boyData.at(1));
            qint8 nLeftNum = static_cast<qint8>(boyData.at(2));
            qint8 nRightNum = static_cast<qint8>(boyData.at(3));
            ui->showNRNLISPLLabel->setText(QString::number(nSplNum));
            ui->NRNLICalibrationLeftLineEdit->setText(QString::number(nLeftNum));
            ui->NRNLICalibrationRightLineEdit->setText(QString::number(nRightNum));
        }
    }else if(nReadType == ALG_GET_WDRC_CHANNEL_COUNT){
        if(boyData.size() >= 2){
            qint8 nWdrcChannleNum = static_cast<qint8>(boyData.at(1));
            if(nWdrcChannleNum == 0){
                nWdrcChannleNum = WDRC_CHANNGL_COUNT;
            }
            m_nWdrcChannelCount = nWdrcChannleNum;
            ui->WDRCChannelCounBox->setCurrentText(QString::number(nWdrcChannleNum));
            ui->WDRCChannelCounBox->setDisabled(true);
        }
    }
}

void CHearingAidvMainWindow::getCurrentAlgSwitchBtnStatus()
{
    QByteArray arraySendData;
    int ret = m_pSerialWork->processData(CMD_CTRL_0x01, INST_CMD_0x16, arraySendData);
    if(ret != 0)
        return;
}

int  CHearingAidvMainWindow::getWDRCChannelCount()
{
    QByteArray arraySendData;
    int ret = m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x01, arraySendData);
    if(ret != 0)
        return ret;
    return 0;
}

void CHearingAidvMainWindow::initUiDataReadDevice()
{
    getWDRCChannelCount();
    getCurrentWorkingMode();
    getCurrentAlgSwitchBtnStatus();
    getVolumeReadCurrent();
    getVolumeCurrentMuteSattus();
    on_NRNLIDataReadBtn_clicked();
    getCurrentAlgArgType();
    on_WDRCReadArgTogether_clicked();
    on_NRARGReadBtn_clicked();
    on_EQArgRead64ChannelBtn_clicked();
    on_CfgCmdReadAllBtn_clicked();
}

void CHearingAidvMainWindow::getWDRCAlgArgSeparate()
{
    QByteArray arraySendData;
    int ret;
    ret= m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x1B, arraySendData);
    if(ret != 0)
        return;

    ret= m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x13, arraySendData);
    if(ret != 0)
        return;
    ret= m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x15, arraySendData);
    if(ret != 0)
        return;
    ret= m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x17, arraySendData);
    if(ret != 0)
        return;
    ret= m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x19, arraySendData);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::setNoiseUiSattus(bool isEnable)
{
    ui->NoiseHorizontalSlider->setEnabled(isEnable);
    ui->NoiseNumberShowLabel->setEnabled(isEnable);
}

void CHearingAidvMainWindow::slotSetEqSliderPosSynchronization(int newValue)
{
    QSlider *slider;
    slider = dynamic_cast<QSlider*>(sender());
    QString sliderName = slider->objectName();

    int num = sliderName.right(sliderName.length()-sliderName.lastIndexOf("_")-1).toInt();
    int before_ptn = m_eq_status[num];
    int increment = newValue - before_ptn;
    m_eq_status[num] = newValue;
    CEQItemWidget *pWidget = NULL;
    QList <int> list;
    QList<CEQItemWidget*>EqitemList = ui->EQScrollAreaWidgetContents->findChildren<CEQItemWidget*>();
    for(int i = 0; i < EqitemList.size(); i++){
        pWidget = EqitemList.at(i);
        if(pWidget){
            if(pWidget->getSignalCheckBox()->isChecked()){
                list.append(i);
            }
        }
    }

    for(int i = 0; i < EqitemList.size(); i++){
        pWidget = EqitemList.at(i);
        if(pWidget){
            if(pWidget->getSignalCheckBox()->isChecked()){
                if(i == num){
                    continue;
                }
                if(list.size() != EqitemList.size()){
                    if(!list.contains(num)) {
                        continue;
                    }
                }
                int ptn = m_eq_status[i] + increment;
                ptn = ptn > 0 ? 0 : ptn;
                ptn = ptn < -20 ? -20 : ptn;
                m_eq_status[i] = ptn;
                pWidget->getSlider()->setSliderPosition(ptn);
            }
        }
    }
}

void CHearingAidvMainWindow::slotSetWDRCComBoxIndexSynchronization(int value)
{
    (void)value;
    QComboBox *pBox;
    pBox = dynamic_cast<QComboBox*>(sender());
    QString comboxName = pBox->objectName();

    QString strComboxType = comboxName.left(comboxName.lastIndexOf("_"));
    int num = comboxName.right(comboxName.length()-comboxName.lastIndexOf("_")-1).toInt();
    ui->FrequencyBandCoBoxFirst->setCurrentIndex(num);
    int index = pBox->currentIndex();
    int before_idx = 0,increasement = 0;
    if(strComboxType == "GainComBox"){
        before_idx = m_wdrcGainStatus[num];
        increasement = index - before_idx;
        m_wdrcGainStatus[num] = index;
    }else if(strComboxType == "CTComBox"){
        before_idx = m_wdrcCTStatus[num];
        increasement = index - before_idx;
        m_wdrcCTStatus[num] = index;
    }else if(strComboxType == "MPOComBox"){
        before_idx = m_wdrcMPOStatus[num];
        increasement = index - before_idx;
        m_wdrcMPOStatus[num] = index;
    }
    CWDRCItemWidget *pWidget = NULL;
    QList <int> list;
    QList<CWDRCItemWidget*>WdrcitemList = ui->scrollAreaWidgetContents->findChildren<CWDRCItemWidget*>("enableWidget");
    for(int i = 0; i < WdrcitemList.size(); i++){
        pWidget = WdrcitemList.at(i);
        if(pWidget){
            if(pWidget->getChannelSingleCheckBox()->isChecked()){
                list.append(i);
            }
        }
    }

    for(int i = 0; i < WdrcitemList.size(); i++){
        pWidget = WdrcitemList.at(i);
        if(pWidget){
            if(pWidget->getChannelSingleCheckBox()->isChecked()){
                if(i == num){
                    continue;
                }
                if(list.size() != WdrcitemList.size()){
                    if(!list.contains(num)) {
                        continue;
                    }
                }
                int new_idx = 0;
                if(strComboxType == "GainComBox"){
                    new_idx = m_wdrcGainStatus[i] + increasement;
                }else if(strComboxType == "CTComBox"){
                    new_idx = m_wdrcCTStatus[i] + increasement;
                }else if(strComboxType == "MPOComBox"){
                    new_idx = m_wdrcMPOStatus[i] + increasement;
                }

                if (new_idx < 0) {
                    new_idx = 0;
                }else if (new_idx > (pBox->count()-1)) {
                    new_idx = pBox->count() - 1;
                }

                if(strComboxType == "GainComBox"){
                    pWidget->getWDRCData().pGainComBox->setCurrentIndex(new_idx);
                    m_wdrcGainStatus[i] = new_idx;
                }else if(strComboxType == "CTComBox"){
                    pWidget->getWDRCData().pCTComBox->setCurrentIndex(new_idx);
                    m_wdrcCTStatus[i] = new_idx;
                }else if(strComboxType == "MPOComBox"){
                    pWidget->getWDRCData().pMPOCoBox->setCurrentIndex(new_idx);
                    m_wdrcMPOStatus[i] = new_idx;
                }
            }
        }
    }
    on_FrequencyBandCoBoxFirst_activated(ui->FrequencyBandCoBoxFirst->currentIndex());
}

void CHearingAidvMainWindow::slotSetWDRCSpinBoxValueSynchronization(double value)
{
    (void)value;
    QDoubleSpinBox *spinbox;
    spinbox = dynamic_cast<QDoubleSpinBox*>(sender());
    QString spinboxName = spinbox->objectName();
    int num = spinboxName.right(spinboxName.length()-spinboxName.lastIndexOf("_")-1).toInt();

    double value_before = m_spinboxStatus[num];
    double value_now = spinbox->value();
    double increment = value_now - value_before;

    m_spinboxStatus[num] = spinbox->value();
    ui->FrequencyBandCoBoxFirst->setCurrentIndex(num);

    CWDRCItemWidget *pWidget = NULL;
    QList <int> list;
    QList<CWDRCItemWidget*>WDRCitemList = ui->scrollAreaWidgetContents->findChildren<CWDRCItemWidget*>();
    for(int i = 0; i < ui->WDRCChannelCounBox->currentText().toInt(); i++){
        pWidget = WDRCitemList.at(i);
        if(pWidget){
            if(pWidget->getChannelSingleCheckBox()->isChecked()){
                list.append(i);
            }
        }
    }

    for(int i = 0; i < WDRCitemList.size(); i++){
        pWidget = WDRCitemList.at(i);
        if(pWidget){
            if(pWidget->getChannelSingleCheckBox()->isChecked()){
                if(i == num)
                    continue;
                if(list.size() != WDRCitemList.size()){
                    if(!list.contains(num))
                        continue;
                }
                double spinboxValue = m_spinboxStatus[i] + increment;
                double spinboxMin = pWidget->getWDRCData().pCRSpinBox->minimum();
                double spinboxMax = pWidget->getWDRCData().pCRSpinBox->maximum();
                spinboxValue = spinboxValue > spinboxMax ?spinboxMax : spinboxValue;
                spinboxValue = spinboxValue < spinboxMin ? spinboxMin : spinboxValue;
                m_spinboxStatus[i] = spinboxValue;
                pWidget->getWDRCData().pCRSpinBox->blockSignals(true);
                pWidget->getWDRCData().pCRSpinBox->setValue(spinboxValue);
                pWidget->getWDRCData().pCRSpinBox->blockSignals(false);
            }
        }
    }
    on_FrequencyBandCoBoxFirst_activated(ui->FrequencyBandCoBoxFirst->currentIndex());
}

void CHearingAidvMainWindow::slotMicReceiverComboBoxClicked()
{

}

void CHearingAidvMainWindow::slotStartTimer()
{
    m_pSerialWork->startWriteTimer();
}

void CHearingAidvMainWindow::slotStopTimer()
{
    m_pSerialWork->stopWriteTimer();
}

void CHearingAidvMainWindow::slotAllCmdFinshed()
{
    showStatusBarlightBtn("操作成功",0);
}

void CHearingAidvMainWindow::slotStausBarNoOper()
{
    showStatusBarlightBtn("等待结果",99);
}

void CHearingAidvMainWindow::slotSaveFileHeadInfo(int  type,QString FileVersion, QString FileCode)
{
    QString strFileHeadPath = QCoreApplication::applicationDirPath() +"/JsonFile/FileHeadInfo.json";
    QJsonObject json;
    QJsonObject fileHeadInfo;
    QJsonObject fileHeadInfoOld;

    QFile FileInfo(strFileHeadPath);
    if (FileInfo.open(QIODevice::ReadOnly)) {
        QByteArray data = FileInfo.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject json = doc.object();
        fileHeadInfoOld = json["fileHeadInfo"].toObject();
    }

    if(type == CFG_FILE_FLAG){
        fileHeadInfo["CfgFileVersion"] = FileVersion;
        fileHeadInfo["CfgFileCode"] = FileCode;
        fileHeadInfo["AlgFileVersion"] = fileHeadInfoOld.value("AlgFileVersion").toString();
        fileHeadInfo["AlgFileCode"] = fileHeadInfoOld.value("AlgFileCode").toString();
        fileHeadInfo["FirFileVersion"] = fileHeadInfoOld.value("FirFileVersion").toString();
        fileHeadInfo["FirFileCode"] = fileHeadInfoOld.value("FirFileCode").toString();
    }else if(type == ALG_FILE_FLAG){
        fileHeadInfo["AlgFileVersion"] = FileVersion;
        fileHeadInfo["AlgFileCode"] = FileCode;
        fileHeadInfo["CfgFileVersion"] = fileHeadInfoOld.value("CfgFileVersion").toString();
        fileHeadInfo["CfgFileCode"] = fileHeadInfoOld.value("CfgFileCode").toString();
        fileHeadInfo["FirFileVersion"] = fileHeadInfoOld.value("FirFileVersion").toString();
        fileHeadInfo["FirFileCode"] = fileHeadInfoOld.value("FirFileCode").toString();
    }else if(type == FIRMWARE_FILE_FLAG){
        fileHeadInfo["FirFileVersion"] = FileVersion;
        fileHeadInfo["FirFileCode"] = FileCode;
        fileHeadInfo["CfgFileVersion"] = fileHeadInfoOld.value("CfgFileVersion").toString();
        fileHeadInfo["CfgFileCode"] = fileHeadInfoOld.value("CfgFileCode").toString();
        fileHeadInfo["AlgFileVersion"] = fileHeadInfoOld.value("AlgFileVersion").toString();
        fileHeadInfo["AlgFileCode"] = fileHeadInfoOld.value("AlgFileCode").toString();
    }

    json["fileHeadInfo"] = fileHeadInfo;

    QJsonDocument doc(json);
    QFile cfgFile(strFileHeadPath);
    if (cfgFile.open(QIODevice::WriteOnly |QIODevice::Truncate)) {
        cfgFile.write(doc.toJson());
    }
    cfgFile.close();
}

void CHearingAidvMainWindow::on_VolumeReadDataBtn_clicked()
{
    QByteArray sendArryData;
    int ret = -1;
    ret =  m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x01, sendArryData);
    if(ret != 0)
        return;
    ret = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x03, sendArryData);
    if(ret != 0)
        return;
    ret = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x05, sendArryData);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::on_VolumeWirteDataBtn_clicked()
{
    QByteArray sendArryData;
    sendArryData.clear();
    uchar VolumeTypeKey = 0x01;
    uchar VolumeTypeBtn = 0x00;

    int nVolumeStall = ui->VolumeKeyStallNumComBox->currentText().toInt();
    int nVolumeStep = ui->VolumeKeyStepComBox->currentText().toInt();
    int nVolumeCycle = ui->VolumeKeyCyCleComBox->currentIndex();

    int nVolumeDirect = ui->VolumeKnobDirectionComBox->currentIndex();
    int nVolumeRange = ui->VolumeKnobRangeComBox->currentText().toInt();
    int ret = -1;
    uchar ch = 0x00;
    if(ui->VolumeKeyModeRadioBtn->isChecked()){
        sendArryData.append(VolumeTypeKey);
        ret = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x00, sendArryData);
        if(ret != 0)
            return;
        sendArryData.clear();
        sendArryData.append(nVolumeStall);
        sendArryData.append(nVolumeStep);
        sendArryData.append(nVolumeCycle);
        for(int  i = sendArryData.size(); i < 8; i++){
            sendArryData.append(ch);
        }
        ret = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x02, sendArryData);
        if(ret != 0)
            return;
    }else if(ui->VolumeKnobModeRadioBtn->isChecked()){
        sendArryData.append(VolumeTypeBtn);
        ret = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x00, sendArryData);
        if(ret != 0)
            return;

        sendArryData.clear();
        sendArryData.append(nVolumeRange);
        sendArryData.append(nVolumeDirect);
        for(int  i = sendArryData.size(); i < 8; i++){
            sendArryData.append(ch);
        }
        ret = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x04, sendArryData);
        if(ret != 0)
            return;
    }
    getVolumeReadCurrent();
}

void CHearingAidvMainWindow::on_pickupArgReadDataBtn_clicked()
{
    QByteArray sendArryData;
    int ret = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x15, sendArryData);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::on_pickupArgWriteDataBtn_clicked()
{
    if(ui->pickupLeftChannelDGAEdit->text().isEmpty()||
            ui->pickupLeftChannelPGAEdit->text().isEmpty() ||
            ui->pickupRightChannelDGAEdit->text().isEmpty() ||
            ui->pickupRightChannelPGAEdit->text().isEmpty()){
        QString msg = "请输入正确的拾音参数";
        slotShowMsgBox(msg);
        return;
    }
    QByteArray sendArryData = getPickUpArgdata();
    int ret  = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x14, sendArryData);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::on_playbackArgReadDataBtn_clicked()
{
    QByteArray sendArryData;
    int ret = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x17, sendArryData);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::on_playbackArgWriteDataBtn_clicked()
{
    if(ui->playbackArgLeftChannelDGAEdilt->text().isEmpty()||
            ui->playbackArgRightChannelDGAEdilt->text().isEmpty()){
        QString msg = "请输入正确的放音参数";
        slotShowMsgBox(msg);
        return;
    }

    QByteArray sendArryData =  getBackUpArgdata();
    int ret = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x16, sendArryData);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::on_DeviceModeNumReadDataBtn_clicked()
{
    QByteArray sendArryData;
    int ret =  m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x21, sendArryData);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::on_DeviceModeNumWriteDataBtn_clicked()
{
    QString strMicLeft = ui->DeviceModeNumLeftMicBox->currentText();
    QString strMicRight = ui->DeviceModeNumRightMicBox->currentText();
    QString strReceiverLeft = ui->DeviceModeNumLeftReceiverBox->currentText();
    QString strReceiverRight = ui->DeviceModeNumRightReceiverBox->currentText();


    if(strMicLeft.isEmpty() || strMicRight.isEmpty()|| strReceiverLeft.isEmpty()|| strReceiverRight.isEmpty()){
        QString msg = "器件型号不得为空";
        slotShowMsgBox(msg);
        return;
    }

    QByteArray sendArryData = getDeviceNumdata();
    int ret = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x20, sendArryData);
    if(ret != 0)
        return;
}


void CHearingAidvMainWindow::on_configReadBtn_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("读取到文件"),m_strOTAModelPath,tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }
    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath = OTAModelInfo.absolutePath();

    QFile file(filePath);
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    file.close();

    noAddressReadFile512(FILE_INDEX_0x00,CMD_UPDATE_0x03,INST_CMD_0x16,INST_CMD_0x17);
}

void CHearingAidvMainWindow::on_confgWriteBtn_clicked()
{
    if(ui->configFilePathEdit->text().isEmpty()){
        QString msg = "请选择一个配置文件";
        slotShowMsgBox(msg);
        return;
    }

    uchar indexCfg = 0x00;
    QString strFilePath = ui->configFilePathEdit->text().trimmed();
    int  ret = noAddressWriteFile512(CFG_FILE_FLAG,indexCfg,CMD_UPDATE_0x03,INST_CMD_0x14,INST_CMD_0x15,strFilePath);
    if(ret == 0){
        on_CfgCmdReadAllBtn_clicked();
    }
}

void CHearingAidvMainWindow::on_configBrowseBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择烧录的文件"), m_strOTAModelPath, tr("Bin Files (*.bin);"));
    if (!filePath.isEmpty()) {
        ui->configFilePathEdit->setText(filePath);
        ui->configFilePathEdit->setToolTip(filePath);

        OTAModelInfo.setFile(filePath);
        m_strOTAModelPath = OTAModelInfo.absolutePath();
    }
}

void CHearingAidvMainWindow::slotShowAnswerResult(uchar cmd, uchar subCmd, uchar result)
{
    if(result == 0 || (cmd == CMD_ALG_0x02 && subCmd == INST_CMD_0x01 )){
        m_pSerialWork->sendNextCommand();
    }else{
        if(result == 5){
            if((cmd == CMD_CFG_0x00 && subCmd == INST_CMD_0x25)
                    ||(cmd == CMD_CFG_0x00 && subCmd == INST_CMD_0x24)){
                if(m_nCMDCount == CMD_COUNT_OVERALL){
                    m_pSerialWork->sendNextCommand();
                    return;
                }
            }
        }
        QString errString;
        errString = UniversalErrorCode.value(result);
        if(errString.isEmpty()){
            if(cmd == CMD_UPDATE_0x03){
                errString = UpdaSpecialErrorCode.value(subCmd).value(result);
            }else if(cmd == CMD_CTRL_0x01){
                errString = CtrlCmdSpecialErrorCode.value(subCmd).value(result);
            }
        }
        QString byteStr = QString("%1"). arg(result,2,16,QLatin1Char('0')).toUpper();
        QString msg = QString("返回码：%1 ").arg(byteStr) + errString;
        slotShowMsgBox(msg);
    }
}

void CHearingAidvMainWindow::on_CfgCmdReadAllBtn_clicked()
{
    if(m_pSerialWork->getSerialIsoOpen()){
        m_nCMDCount = CMD_COUNT_OVERALL;
        on_VolumeReadDataBtn_clicked();
        on_pickupArgReadDataBtn_clicked();
        on_playbackArgReadDataBtn_clicked();
        on_DeviceModeNumReadDataBtn_clicked();
        on_AlgArgCountReadBtn_clicked();
        getAudioCompensationValueCmd();
    }else{
        slotShowMsgBoxdialog("请先连接助听器");
    }
}

void CHearingAidvMainWindow::on_CfgCmdWriteAllBtn_clicked()
{
    if(ui->pickupLeftChannelDGAEdit->text().isEmpty()||
            ui->pickupLeftChannelPGAEdit->text().isEmpty() ||
            ui->pickupRightChannelDGAEdit->text().isEmpty() ||
            ui->pickupRightChannelPGAEdit->text().isEmpty()){
        QString msg = "请输入正确的拾音参数";
        slotShowMsgBox(msg);
        return;
    }

    if(ui->playbackArgLeftChannelDGAEdilt->text().isEmpty()||
            ui->playbackArgRightChannelDGAEdilt->text().isEmpty()){
        QString msg = "请输入正确的放音参数";
        slotShowMsgBox(msg);
        return;
    }

    if(ui->DeviceModeNumLeftMicBox->currentText().isEmpty() || ui->DeviceModeNumRightMicBox->currentText().isEmpty()
            || ui->DeviceModeNumLeftReceiverBox->currentText().isEmpty()
            || ui->DeviceModeNumRightReceiverBox->currentText().isEmpty()){
        QString msg = "器件型号不得为空";
        slotShowMsgBox(msg);
        return;
    }

    if(m_pSerialWork->getSerialIsoOpen()){
        m_nCMDCount = CMD_COUNT_OVERALL;
        on_VolumeWirteDataBtn_clicked();
        on_pickupArgWriteDataBtn_clicked();
        on_playbackArgWriteDataBtn_clicked();
        on_DeviceModeNumWriteDataBtn_clicked();
        on_AlgArgCountWriteBtn_clicked();
        on_AlgArgCountReadBtn_clicked();
        setAudioCompensationValueCmd();
    }else{
        slotShowMsgBoxdialog("请先连接助听器");
    }
}

void CHearingAidvMainWindow::on_NoiseHorizontalSlider_valueChanged(int value)
{
    ui->NoiseNumberShowLabel->setText(QString("%1").arg(value));
}

void CHearingAidvMainWindow::on_EQArgWrite64ChannelBtn_clicked()
{
    QByteArray arrySendData =  getEQData();

    int ret = m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x30, arrySendData);
    if(ret != 0)
        return;
    if(ui->algFFTSwitchBtn->isChecked() == false ||
            ui->algEQSwitchBtn ->isChecked() == false){
        slotShowMsgBoxdialog("请使FFT及EQ开关处于打开状态");
    }
}


void CHearingAidvMainWindow::on_EQArgRead64ChannelBtn_clicked()
{
    QByteArray arrySendData;
    int ret =  m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x31, arrySendData);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::setLineSeriesView(int index, int pLineSeriesType)
{
    float threshold = 0, ratio = 0, gain = 0,mpo = 0;
    QString strCT = QString("CTComBox_%1").arg(index);
    QString strCR = QString("CRSpinBox_%1").arg(index);
    QString strGain = QString("GainComBox_%1").arg(index);
    QString strMpo = QString("MPOComBox_%1").arg(index);

    CWDRCItemWidget *pWidget = NULL;
    QList<CWDRCItemWidget*>WdrcitemList = ui->scrollAreaWidgetContents->findChildren<CWDRCItemWidget*>("enableWidget");
    for(int  i = 0; i < WdrcitemList.size();i++){
        pWidget = WdrcitemList.at(i);
        if(pWidget){
            if(pWidget->getWDRCData().pCTComBox->objectName() == strCT){
                threshold = pWidget->getWDRCData().pCTComBox->currentText().toFloat();
            }
            if(pWidget->getWDRCData().pCRSpinBox->objectName() == strCR){
                ratio = pWidget->getWDRCData().pCRSpinBox->value();
            }
            if(pWidget->getWDRCData().pGainComBox->objectName() == strGain){
                gain = pWidget->getWDRCData().pGainComBox->currentText().toFloat();
            }
            if(pWidget->getWDRCData().pMPOCoBox->objectName() == strMpo){
                mpo = pWidget->getWDRCData().pMPOCoBox->currentText().toFloat();
            }
        }
    }
    drawCurves(threshold,ratio,gain,mpo,pLineSeriesType);
}

void CHearingAidvMainWindow::slotInitUiData(bool isOpen)
{
    if(isOpen == true){
        m_nWdrcChannelCount = 4;
        initUiDataReadDevice();
        on_FrequencyBandCoBoxFirst_activated(0);
        m_nInitIsUiData = false;
    }
}

void CHearingAidvMainWindow::on_FrequencyBandCoBoxFirst_activated(int index)
{
    setLineSeriesView(index,LineSeriesFirst);
}

void CHearingAidvMainWindow::on_WDRCReadArg_clicked()
{
    getWDRCAlgArgSeparate();
}

void CHearingAidvMainWindow::on_WDRCwriteArg_clicked()
{
    if(ui->WDRCATLineEdit->text().isEmpty()||
            ui->WDRCRTLineEdit->text().isEmpty()){
        QString msg = "请输入正确的时间参数";
        slotShowMsgBox(msg);
        return;
    }
    QByteArray GainArray;
    QByteArray MPOArray;
    QByteArray CTArray;
    QByteArray CRArray;
    QByteArray AtRtArray;

    int nATArg = ui->WDRCATLineEdit->text().trimmed().toInt();
    int nRTArg = ui->WDRCRTLineEdit->text().trimmed().toInt();

    CWDRCItemWidget *pWidget = NULL;
    QList<CWDRCItemWidget*>WdrcitemList = ui->scrollAreaWidgetContents->findChildren<CWDRCItemWidget*>("enableWidget");
    for(int  i = 0; i < WdrcitemList.size();i++){
        pWidget = WdrcitemList.at(i);
        if(pWidget){
            GainArray .append(pWidget->getWDRCData().pGainComBox->currentText().toInt());
            MPOArray .append(pWidget->getWDRCData().pMPOCoBox->currentText().toInt());
            CTArray .append(pWidget->getWDRCData().pCTComBox->currentText().toInt());
            float CR = pWidget->getWDRCData().pCRSpinBox->text().toFloat();
            CRArray .append(QByteArray::fromRawData((char*)&CR, 4));
        }
    }

    AtRtArray.append(intToArry(nATArg));
    AtRtArray.append(intToArry(nRTArg));

    int ret;
    ret = m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x1A, AtRtArray);
    if(ret != 0)
        return;
    ret = m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x12, CTArray);
    if(ret != 0)
        return;
    ret = m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x14, CRArray);
    if(ret != 0)
        return;
    ret = m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x16, GainArray);
    if(ret != 0)
        return;
    ret = m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x18, MPOArray);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::on_FlashFromTE_textChanged()
{
    QString text = ui->FlashFromTE->toPlainText().trimmed();
    if(text.isEmpty()){
        ui->FlashFromLenthLE->clear();
    }else{
        QStringList numbers = text.split(" ");
        ui->FlashFromLenthLE->setText(QString::number(numbers.size()));
    }
}

void CHearingAidvMainWindow::on_pickupLeftChannelDGAEdit_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->pickupArgShowErrorLabel->text().isEmpty()){
        ui->pickupArgShowErrorLabel->clear();
    }
}

void CHearingAidvMainWindow::on_playbackArgLeftChannelDGAEdilt_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->playBackArgShowErrorLabel->text().isEmpty()){
        ui->playBackArgShowErrorLabel->clear();
    }
}


void CHearingAidvMainWindow::on_configFilePathEdit_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->cfgFileShowErrorLabel->text().isEmpty()){
        ui->cfgFileShowErrorLabel->clear();
    }
}

void CHearingAidvMainWindow::on_NRARGWriteBtn_clicked()
{
    QByteArray arrySendData = getNRData();
    int ret =  m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x20, arrySendData);
    if(ret != 0)
        return;
    if(ui->algFFTSwitchBtn->isChecked() == false ||
            ui->algNRSwitchBtn ->isChecked() == false){
        slotShowMsgBoxdialog("请使FFT及NR开关处于打开状态");
    }
}


void CHearingAidvMainWindow::on_VolumeKeyStallNumComBox_activated(int index)
{
    (void)index;
    setVolumeKeyModeShow();
}

void CHearingAidvMainWindow::on_VolumeKeyStepComBox_activated(int index)
{
    (void)index;
    setVolumeKeyModeShow();
}


void CHearingAidvMainWindow::on_WDRCwriteArgTogether_clicked()
{
    QString msg;
    int ret;
    if(ui->WDRCATLineEdit->text().isEmpty()||
            ui->WDRCRTLineEdit->text().isEmpty()){
        msg = "请输入正确WDRC的时间参数";
        slotShowMsgBox(msg);
        return;
    }

    if(ui->WDRCChannelCounBox->currentText().toInt() != m_nWdrcChannelCount){
        msg = "通道数量不匹配，已重新获取当前运行参数";
        slotShowMsgBox(msg);
        slotShowMsgBoxdialog(msg);
        ret = getWDRCChannelCount();
        if(ret != 0)
            return;
    }

    QByteArray  sendArray = getWDRCData();
    ret = m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x10, sendArray);
    if(ret != 0)
        return;
    if(ui->algFFTSwitchBtn->isChecked() == false ||
            ui->algWDRCSwitchBtn ->isChecked() == false){
        slotShowMsgBoxdialog("请使FFT及WDRC开关处于打开状态");
    }
}


void CHearingAidvMainWindow::on_WDRCReadArgTogether_clicked()
{
    int ret = getWDRCChannelCount();
    if(ret != 0)
        return;
    QByteArray sendArray;
    ret = m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x11, sendArray);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::on_CfgCreatFileBtn_clicked()
{
    if(ui->playbackArgLeftChannelDGAEdilt->text().isEmpty()||
            ui->playbackArgRightChannelDGAEdilt->text().isEmpty()){
        QString msg = "请输入正确的放音参数";
        slotShowMsgBox(msg);
        return;
    }

    if(ui->pickupLeftChannelDGAEdit->text().isEmpty()||
            ui->pickupLeftChannelPGAEdit->text().isEmpty() ||
            ui->pickupRightChannelDGAEdit->text().isEmpty() ||
            ui->pickupRightChannelPGAEdit->text().isEmpty()){
        QString msg = "请输入正确的拾音参数";
        slotShowMsgBox(msg);
        return;
    }

    QByteArray cfgFileData;
    cfgFileData.append(getVolumeModeldata());
    cfgFileData.append(getPickUpArgdata());
    cfgFileData.append(getBackUpArgdata());
    cfgFileData.append(getDeviceNumdata());
    cfgFileData.append(getAlgArgCountdata());
    cfgFileData.append(getAudioCompensationValueData());

    uchar ch = 0x00;
    for(int i = cfgFileData.size();i < 512; i++){
        cfgFileData.append(ch);
    }
    QByteArray fileHeadArray;
    int ret = getFileHeadInfo(CFG_FILE_FLAG,cfgFileData,fileHeadArray);
    if(ret != 0){
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, tr("生成配置文件"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }
    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath = OTAModelInfo.absolutePath();

    QByteArray containsFileHeadArray;;
    containsFileHeadArray.append(fileHeadArray);
    containsFileHeadArray.append(cfgFileData);

    QFile cfgFile(filePath);
    if (cfgFile.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        QDataStream out(&cfgFile);
        out.writeRawData(containsFileHeadArray.data(), containsFileHeadArray.size());
    }
    cfgFile.close();
    ui->configFilePathEdit->setText(filePath);
    slotAllCmdFinshed();
}

void CHearingAidvMainWindow::on_NRNLIDataWriteBtn_clicked()
{
    if(ui->NRNLICalibrationLeftLineEdit->text().isEmpty()||
            ui->NRNLICalibrationRightLineEdit->text().isEmpty()){
        QString msg = "请输入正确的噪声等级识别的校准值参数";
        slotShowMsgBox(msg);
        return;
    }

    QByteArray sendArray =  getNLIData();
    int ret = m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x22, sendArray);
    if(ret != 0)
        return;
    on_NRNLIDataReadBtn_clicked();
    if(ui->algFFTSwitchBtn->isChecked() == false ||
            ui->algNLISwitchBtn ->isChecked() == false){
        slotShowMsgBoxdialog("请使FFT及NLI开关处于打开状态");
    }
}


void CHearingAidvMainWindow::on_NRNLIDataReadBtn_clicked()
{
    QByteArray sendArray;
    int ret = m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x23, sendArray);
    if(ret != 0)
        return;
}


void CHearingAidvMainWindow::on_NRNLICalibrationLeftLineEdit_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->NRNLIErrorShowLabel->text().isEmpty()){
        ui->NRNLIErrorShowLabel->clear();
    }
}


void CHearingAidvMainWindow::on_AlgArgCountWriteBtn_clicked()
{
    QByteArray sendArray = getAlgArgCountdata();
    int ret = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x22, sendArray);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::on_AlgArgCountReadBtn_clicked()
{
    QByteArray sendArray;
    int ret = m_pSerialWork->processData(CMD_CFG_0x00, INST_CMD_0x23, sendArray);
    if(ret != 0)
        return;
}

void CHearingAidvMainWindow::on_WDRCATLineEdit_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->WDRCErrorInfoShowLabel->text().isEmpty()){
        ui->WDRCErrorInfoShowLabel->clear();
    }
}

void CHearingAidvMainWindow::on_NRARGReadBtn_clicked()
{
    QByteArray arraySendData;
    int ret = m_pSerialWork->processData(CMD_ALG_0x02, INST_CMD_0x21, arraySendData);
    if(ret != 0)
        return;
}


void CHearingAidvMainWindow::getAlgDeviceData()
{
    if(m_pSerialWork->getSerialIsoOpen()){
        on_WDRCReadArgTogether_clicked();
        on_NRARGReadBtn_clicked();
        on_NRNLIDataReadBtn_clicked();
        on_EQArgRead64ChannelBtn_clicked();
        getCurrentAlgArgType();
    }else{
        slotShowMsgBoxdialog("请先连接助听器");
    }
}

void CHearingAidvMainWindow::on_AlgFileDataToUiBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("预览模式文件"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }
    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath  = OTAModelInfo.absolutePath() ;
    QFile algArgFile(filePath);
    if(!algArgFile.exists()){
        QString msg =  QString("%1文件不存在").arg("预览模式");
        slotShowMsgBox(msg);
        return;
    }

    if(!algArgFile.open(QIODevice::ReadOnly)){
        QString msg = "文件打开失败";
        slotShowMsgBox(msg);
        return;
    }

    if(algArgFile.size() == 0){
        QString msg = QString("%1文件为空").arg("预览模式文件");
        slotShowMsgBox(msg);
        return;
    }

    QByteArray AlgFileDataArray = algArgFile.readAll();
    QByteArray ArryWDRC;
    QByteArray ArryNR;
    QByteArray ArryNLI;
    QByteArray ArryEQ;
    QByteArray ArryHS;
    QByteArray ArryFileHead;
    QList<uint8_t> Mpolist;
    QList<uint8_t> CTlist;
    QList<uint8_t> Gainlist;
    QList<int8_t> CRlist;
    QString msg;
    int WdrcChannelCount = 4;

    QString strfileHeadIdentification = "WITMEM";
    for(int  j = 0; j < AlgFileDataArray.size();j++){
        if(j < 64){
            ArryFileHead.append(AlgFileDataArray.at(j));
            if(j == 5){
                if(strfileHeadIdentification != ArryFileHead){
                    msg= "模式文件不含文件头，请重新选择";
                    slotShowMsgBox(msg);
                    return;
                }
            }else if(j == 6){
                if(ArryFileHead.at(6) != ALG_FILE_FLAG){
                    msg = "模式文件不合法，请重新选择";
                    slotShowMsgBox(msg);
                    slotShowMsgBoxdialog(msg);
                    return;
                }
            }
        }
    }

    for(int  j = 64; j < AlgFileDataArray.size();j++){
        if(j < 512 + 64){
            ArryWDRC.append(AlgFileDataArray.at(j));
        }
        if(j < 32 + 64){
            Gainlist.append(AlgFileDataArray.at(j));
        }else if(j < 64 + 32 + 32){
            Mpolist.append(AlgFileDataArray.at(j));
        }else if(j < 64 + 32 + 32 + 32){
            CTlist.append(AlgFileDataArray.at(j));
        }else if(j < 64 + 32 + 32 +32 + 128){
            CRlist.append(AlgFileDataArray.at(j));
        }else if(j < 512 + 64 ){
            if(j == 511 + 64){
                WdrcChannelCount = AlgFileDataArray[575];
                if(WdrcChannelCount == 0){
                    WdrcChannelCount = WDRC_CHANNGL_COUNT;
                }
            }
        }else if(j < 512 + 64 + 64){
            ArryNR.append(AlgFileDataArray.at(j));
        }else if(j < 512 + 64 + 64+ 64){
            ArryNLI.append(AlgFileDataArray.at(j));
        }else if(j < 512 + 64+ 64 + 64 + 128){
            ArryEQ.append(AlgFileDataArray.at(j));
        }else if(j < 512 + 64 +64 + 64 + 128 + 128){
            ArryHS.append(AlgFileDataArray.at(j));
        }
    }

    QByteArray algArgDataArray = AlgFileDataArray.mid(64,AlgFileDataArray.size() - 1);

    uint8_t left = ArryFileHead.at(29);
    uint8_t right = ArryFileHead.at(28);
    ushort RecvCRC = (left << 8) | right;
    ushort CRCAlgFile = m_pSerialWork->crc16(algArgDataArray.data(),algArgDataArray.size());
    if(RecvCRC != CRCAlgFile){
        QMessageBox *msgBox =  new QMessageBox(this);
        QString strErrorMsg = "模式文件已篡改,是否继续预览?";
        msgBox->setText(strErrorMsg);
        msgBox->setWindowTitle("提示");

        QPushButton* noButton = msgBox->addButton(QMessageBox::No);
        noButton->setText("否");
        QPushButton* continueButton = msgBox->addButton("继续预览", QMessageBox::AcceptRole);
        msgBox->exec();
        if (msgBox->clickedButton() == noButton) {
            slotShowMsgBox("模式文件已篡改，已停止预览");
            return;
        }
        if (msgBox->clickedButton() == continueButton) {
        }
    }
    ui->WDRCChannelCounBox->setCurrentText(QString::number(WdrcChannelCount));

    CWDRCItemWidget *pWDRCWidget = NULL;
    QList<CWDRCItemWidget*>WdrcitemList = ui->scrollAreaWidgetContents->findChildren<CWDRCItemWidget*>("enableWidget");
    QList <double> crList;
    for (int i = 96; i < ArryWDRC.size(); i += 4) {
        float valueSpinBox = 0;
        uint32_t value = 0;
        for (int j = 3; j >= 0; j--) {
            if (i + j < ArryWDRC.size()) {
                uint8_t  Calibration = static_cast<uint8_t >(ArryWDRC[i + j]);
                value = (value << 8) | Calibration;
            }
        }

        memcpy(&valueSpinBox, &value, sizeof(value));
        crList.append(valueSpinBox);
    }

    for(int  i = 0; i < WdrcitemList.size();i++){
        pWDRCWidget = WdrcitemList.at(i);
        if(pWDRCWidget){
            pWDRCWidget->getWDRCData().pGainComBox->setCurrentText(QString::number(Gainlist.at(i)));
            pWDRCWidget->getWDRCData().pMPOCoBox->setCurrentText(QString::number(Mpolist.at(i)));
            pWDRCWidget->getWDRCData().pCTComBox->setCurrentText(QString::number(CTlist.at(i)));
            pWDRCWidget->getWDRCData().pCRSpinBox->blockSignals(true);
            pWDRCWidget->getWDRCData().pCRSpinBox->setValue(crList.at(i));
            pWDRCWidget->getWDRCData().pCRSpinBox->blockSignals(false);
            m_wdrcGainStatus[i] = pWDRCWidget->getWDRCData().pGainComBox->currentIndex();
            m_wdrcCTStatus[i]  = pWDRCWidget->getWDRCData().pCTComBox->currentIndex();
            m_wdrcMPOStatus[i] = pWDRCWidget->getWDRCData().pMPOCoBox->currentIndex();
            m_spinboxStatus[i] = pWDRCWidget->getWDRCData().pCRSpinBox->value();
        }
    }

    uint32_t value = 0;
    QList <uint32_t>list;
    for (int i = 224; i < ArryWDRC.size(); i += 4) {
        for (int j = 3; j >= 0; j--) {
            if (i + j < ArryWDRC.size()) {
                uint8_t  Calibration = static_cast<uint8_t >(ArryWDRC[i + j]);
                value = (value << 8) | Calibration;
            }
        }
        list.append(value);
    }
    ui->WDRCATLineEdit->setText(QString::number(list.at(0)));
    ui->WDRCRTLineEdit->setText(QString::number(list.at(1)));

    ui->NoiseHorizontalSlider->setValue(ArryNR.at(0));

    qint8 nSplNum = static_cast<qint8>(ArryNLI.at(0));
    qint8 nLeftNum = static_cast<qint8>(ArryNLI.at(1));
    qint8 nRightNum = static_cast<qint8>(ArryNLI.at(2));
    ui->showNRNLISPLLabel->setText(QString::number(nSplNum));
    ui->NRNLICalibrationLeftLineEdit->setText(QString::number(nLeftNum));
    ui->NRNLICalibrationRightLineEdit->setText(QString::number(nRightNum));

    CEQItemWidget *pEQWidget = NULL;
    QList<CEQItemWidget*>EqitemList = ui->EQScrollAreaWidgetContents->findChildren<CEQItemWidget*>();
    for(int i =0; i < EqitemList.size(); i++){
        pEQWidget = EqitemList.at(i);
        if(pEQWidget){
            qint8 ndBNum = static_cast<qint8>(ArryEQ.at(i));
            pEQWidget->getSlider()->blockSignals(true);
            pEQWidget->getSlider()->setValue(ndBNum);
            pEQWidget->setEqLabelText(ndBNum);
            m_eq_status[i]= pEQWidget->getSlider()->sliderPosition();
            pEQWidget->getSlider()->blockSignals(false);
        }
    }
    slotAllCmdFinshed();
    algArgFile.close();
}

void CHearingAidvMainWindow::on_AlgUIDataToFileBtn_clicked()
{
    if(ui->WDRCATLineEdit->text().isEmpty()||
            ui->WDRCRTLineEdit->text().isEmpty()){
        QString msg = "请输入正确WDRC的时间参数";
        slotShowMsgBox(msg);
        return;
    }

    if(ui->NRNLICalibrationLeftLineEdit->text().isEmpty()||
            ui->NRNLICalibrationRightLineEdit->text().isEmpty()){
        QString msg = "请输入正确的噪声等级识别的校准值参数";
        slotShowMsgBox(msg);
        return;
    }

    QByteArray algFileData;
    uchar reserveData = 0x00;
    algFileData.append(getWDRCData());
    for(int  i = algFileData.size();i < WRITE_DATA_SIZE -1;i++){
        algFileData.append(reserveData);
    }
    algFileData.append(ui->WDRCChannelCounBox->currentText().toInt());

    algFileData.append(getNRData());
    for(int  i = algFileData.size();i < WRITE_DATA_SIZE + 64;i++){
        algFileData.append(reserveData);
    }
    algFileData.append(getNLIData());
    for(int  i = algFileData.size();i < WRITE_DATA_SIZE + 128;i++){
        algFileData.append(reserveData);
    }

    algFileData.append(getEQData());
    for(int  i = algFileData.size();i < WRITE_DATA_SIZE + 128*2;i++){
        algFileData.append(reserveData);
    }
    for(int  i = algFileData.size();i < WRITE_DATA_SIZE*2;i++){
        algFileData.append(reserveData);
    }

    QByteArray fileHeadArray;
    int ret = getFileHeadInfo(ALG_FILE_FLAG,algFileData,fileHeadArray);
    if(ret != 0){
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, tr("生成模式文件"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }

    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath  = OTAModelInfo.absolutePath() ;

    QByteArray containsFileHeadArray;;
    containsFileHeadArray.append(fileHeadArray);
    containsFileHeadArray.append(algFileData);


    QFile cfgFile(filePath);
    if (cfgFile.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        QDataStream out(&cfgFile);
        out.writeRawData(containsFileHeadArray.data(), containsFileHeadArray.size());
    }
    cfgFile.close();
    slotAllCmdFinshed();
}


void CHearingAidvMainWindow::on_AlgArgUpdataBrowesFileBtn_1_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择模式1文件"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }
    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath = OTAModelInfo.absolutePath();

    QFile algArgFile(filePath);
    if(!algArgFile.open(QIODevice::ReadOnly)){
        return;
    }
    ui->AlgArgUpdataFilePathlineEdit_1->setText(filePath);
    ui->AlgArgUpdataFilePathlineEdit_1->setToolTip(filePath);
}


void CHearingAidvMainWindow::on_AlgArgUpdataReadBtn_1_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("回读模式1文件"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }

    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath = OTAModelInfo.absolutePath();

    QFile file(filePath);
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    file.close();

    noAddressReadFile512(FILE_INDEX_0x00,CMD_UPDATE_0x03,INST_CMD_0x1A,INST_CMD_0x1B);
}


void CHearingAidvMainWindow::on_AlgArgUpdataWriteBtn_1_clicked()
{
    if(ui->AlgArgUpdataFilePathlineEdit_1->text().isEmpty()){
        QString msg = "请选择模式1的文件";
        slotShowMsgBox(msg);
        return;
    }

    QString strFilePath = ui->AlgArgUpdataFilePathlineEdit_1->text().trimmed();
    noAddressWriteFile512(ALG_FILE_FLAG,FILE_INDEX_0x00,CMD_UPDATE_0x03,INST_CMD_0x18,INST_CMD_0x19,strFilePath);
}


void CHearingAidvMainWindow::on_AlgArgUpdataFilePathlineEdit_1_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->AlgArgErrorLabel_1->text().isEmpty()){
        ui->AlgArgErrorLabel_1->clear();
    }
}

void CHearingAidvMainWindow::on_AlgArgUpdataBrowesFileBtn_2_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择模式2文件"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }
    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath = OTAModelInfo.absolutePath();

    QFile algArgFile(filePath);
    if(!algArgFile.open(QIODevice::ReadOnly)){
        return;
    }
    ui->AlgArgUpdataFilePathlineEdit_2->setText(filePath);
    ui->AlgArgUpdataFilePathlineEdit_2->setToolTip(filePath);
}


void CHearingAidvMainWindow::on_AlgArgUpdataReadBtn_2_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("回读模式2文件"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }

    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath = OTAModelInfo.absolutePath();

    QFile file(filePath);
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    file.close();

    noAddressReadFile512(FILE_INDEX_0x01,CMD_UPDATE_0x03,INST_CMD_0x1A,INST_CMD_0x1B);
}

void CHearingAidvMainWindow::on_AlgArgUpdataWriteBtn_2_clicked()
{
    if(ui->AlgArgUpdataFilePathlineEdit_2->text().isEmpty()){
        slotShowMsgBox("请选择模式2的文件");
        return;
    }

    QString strFilePath = ui->AlgArgUpdataFilePathlineEdit_2->text().trimmed();
    noAddressWriteFile512(ALG_FILE_FLAG,FILE_INDEX_0x01,CMD_UPDATE_0x03,INST_CMD_0x18,INST_CMD_0x19,strFilePath);
}

void CHearingAidvMainWindow::on_AlgArgUpdataFilePathlineEdit_2_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->AlgArgErrorLabel_2->text().isEmpty()){
        ui->AlgArgErrorLabel_2->clear();
    }
}

void CHearingAidvMainWindow::on_AlgArgUpdataBrowesFileBtn_3_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择模式3文件"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }
    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath = OTAModelInfo.absolutePath();

    QFile algArgFile(filePath);
    if(!algArgFile.open(QIODevice::ReadOnly)){
        return;
    }
    ui->AlgArgUpdataFilePathlineEdit_3->setText(filePath);
    ui->AlgArgUpdataFilePathlineEdit_3->setToolTip(filePath);
}


void CHearingAidvMainWindow::on_AlgArgUpdataReadBtn_3_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("回读模式3文件"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }

    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath = OTAModelInfo.absolutePath();

    QFile file(filePath);
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    file.close();

    noAddressReadFile512(FILE_INDEX_0x02,CMD_UPDATE_0x03,INST_CMD_0x1A,INST_CMD_0x1B);
}


void CHearingAidvMainWindow::on_AlgArgUpdataWriteBtn_3_clicked()
{
    if(ui->AlgArgUpdataFilePathlineEdit_3->text().isEmpty()){
        slotShowMsgBox("请选择模式3的文件");
        return;
    }
    QString strFilePath = ui->AlgArgUpdataFilePathlineEdit_3->text().trimmed();
    noAddressWriteFile512(ALG_FILE_FLAG,FILE_INDEX_0x02,CMD_UPDATE_0x03,INST_CMD_0x18,INST_CMD_0x19,strFilePath);
}


void CHearingAidvMainWindow::on_AlgArgUpdataFilePathlineEdit_3_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->AlgArgErrorLabel_3->text().isEmpty()){
        ui->AlgArgErrorLabel_3->clear();
    }
}

void CHearingAidvMainWindow::on_AlgArgUpdataBrowesFileBtn_4_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择模式4文件"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }
    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath = OTAModelInfo.absolutePath();

    QFile algArgFile(filePath);
    if(!algArgFile.open(QIODevice::ReadOnly)){
        qDebug() <<"文件无法打开";
        return;
    }
    ui->AlgArgUpdataFilePathlineEdit_4->setText(filePath);
    ui->AlgArgUpdataFilePathlineEdit_4->setToolTip(filePath);
}


void CHearingAidvMainWindow::on_AlgArgUpdataReadBtn_4_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("回读模式4文件"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }

    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath = OTAModelInfo.absolutePath();

    QFile file(filePath);
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    file.close();
    noAddressReadFile512(FILE_INDEX_0x03,CMD_UPDATE_0x03,INST_CMD_0x1A,INST_CMD_0x1B);
}

void CHearingAidvMainWindow::on_AlgArgUpdataWriteBtn_4_clicked()
{
    if(ui->AlgArgUpdataFilePathlineEdit_4->text().isEmpty()){
        slotShowMsgBox("请选择模式4的文件");
        return;
    }
    QString strFilePath = ui->AlgArgUpdataFilePathlineEdit_4->text().trimmed();
    noAddressWriteFile512(ALG_FILE_FLAG,FILE_INDEX_0x03,CMD_UPDATE_0x03,INST_CMD_0x18,INST_CMD_0x19,strFilePath);
}

void CHearingAidvMainWindow::on_AlgArgUpdataFilePathlineEdit_4_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->AlgArgErrorLabel_4->text().isEmpty()){
        ui->AlgArgErrorLabel_4->clear();
    }
}


void CHearingAidvMainWindow::on_firmwareUpdataBrowsFileBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择镜像文件"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }
    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath = OTAModelInfo.absolutePath();

    QFile algArgFile(filePath);
    if(!algArgFile.open(QIODevice::ReadOnly)){
        qDebug() <<"文件无法打开";
        return;
    }
    ui->firmwareUpdataFilePathlineEdit->setText(filePath);
    ui->firmwareUpdataFilePathlineEdit->setToolTip(filePath);
}

void CHearingAidvMainWindow::on_firmwareUpdataWriteBtn_clicked()
{
    if(ui->firmwareUpdataFilePathlineEdit->text().isEmpty()){
        slotShowMsgBox("请选择一个固件文件");
        return;
    }
    m_strFirmWareFilePath = ui->firmwareUpdataFilePathlineEdit->text().trimmed();

    QByteArray sendArray;
    uchar index = 0x00;
    sendArray.append(index);
    int ret = m_pSerialWork->processData(CMD_UPDATE_0x03,INST_CMD_0x11, sendArray);
    if(ret != 0)
        return;
}


void CHearingAidvMainWindow::on_firmwareUpdataFilePathlineEdit_textChanged(const QString &arg1)
{
    (void)arg1;
    if(!ui->firmwareUpdataErrorLabel->text().isEmpty()){
        ui->firmwareUpdataErrorLabel->clear();
    }
}

void CHearingAidvMainWindow::on_SerialPortOpenBtn_clicked()
{
    QString strSerialPortName = ui->SerialNameBox->currentText();
    int first = strSerialPortName.indexOf("(");
    int second = strSerialPortName.indexOf(")");
    QString strName = strSerialPortName.mid(first + 1,second - first -1);
    emit signalSendAccept(strName, ui->FreLabel->text().toInt());
}

void CHearingAidvMainWindow::on_RestoreSerialBtn_clicked()
{
    slotSerialNameBoxRefresh();
}
void CHearingAidvMainWindow::slotSerialPortStateChanged(bool state)
{
    if (state) {

    }else {
        slotShowMsgDevStatus("未连接");
        initProcessProgressBar();
        showStatusBarlightBtn("无操作",99);
    }
}

void CHearingAidvMainWindow::on_SerialPortCloseBtn_clicked()
{
    initProcessProgressBar();
    emit signalSendClose();
    ui->WDRCChannelCounBox->setDisabled(false);
}

void CHearingAidvMainWindow::setFirmwareAddFileHead()
{
    if(ui->firmwareUpdataFilePathlineEdit->text().trimmed().isEmpty()){
        QString msg = "请选择一个固件文件";
        slotShowMsgBox(msg);
        return;
    }

    QString sourceFilePath =ui->firmwareUpdataFilePathlineEdit->text().trimmed();

    QFileInfo FileInfo;
    FileInfo.setFile(sourceFilePath);
    QFile file(sourceFilePath);
    if(!file.exists()){
        QString msg =  QString("%1文件不存在").arg("固件");
        slotShowMsgBox(msg);
        return;
    }

    if(!file.open(QIODevice::ReadOnly)){
        QString msg = "文件无法打开";
        slotShowMsgBox(msg);
        return;
    }

    if(file.size() == 0){
        QString msg = QString("%1文件为空").arg("固件");
        slotShowMsgBox(msg);
        return;
    }

    QByteArray FileDataArray = file.readAll();
    QByteArray fileHeadArray;
    QByteArray fileFullDataArray;
    QString strfileHeadIdentification = "WITMEM";
    for(int i = 0 ; i < 64; i++){
        fileHeadArray.append(FileDataArray.at(i));
    }
    if(QString(fileHeadArray).contains(strfileHeadIdentification)){
        for(int i = fileHeadArray.size() ; i < FileDataArray.size(); i++){
            fileFullDataArray.append(FileDataArray.at(i));
        }
    }else{
        fileFullDataArray .append(FileDataArray);
    }
    fileHeadArray.clear();

    int ret = getFileHeadInfo(FIRMWARE_FILE_FLAG,fileFullDataArray,fileHeadArray);
    if(ret != 0){
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, tr("保存生产封包文件"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }
    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath = OTAModelInfo.absolutePath();

    QByteArray containsFileHeadArray;;
    containsFileHeadArray.append(fileHeadArray);
    containsFileHeadArray.append(fileFullDataArray);

    QFile cfgFile(filePath);
    if (cfgFile.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        QDataStream out(&cfgFile);
        out.writeRawData(containsFileHeadArray.data(), containsFileHeadArray.size());
    }
    cfgFile.close();

    ui->firmwareUpdataFilePathlineEdit->setText(filePath);
    ui->firmwareUpdataFilePathlineEdit->setToolTip(filePath);
    slotAllCmdFinshed();
}

int CHearingAidvMainWindow::hexToint10(ulong hexData)
{
    QString num = QString::number(hexData,10);
    int dec = num.toInt();
    return dec;
}

void CHearingAidvMainWindow::slotSerialNameBoxRefresh()
{
    QStringList SerialPortName{};
    int oldSerialIndex = ui->SerialNameBox->currentIndex();
    if(oldSerialIndex < 0){
        oldSerialIndex = 0;
    }
    QList<QSerialPortInfo> serials = QSerialPortInfo::availablePorts();
    foreach (QSerialPortInfo info, serials){
        SerialPortName.append(QString(info.description()+"(%1)").arg(info.portName()));
        m_strConnectedPortname = QString(info.description()+"(%1)").arg(info.portName());
    }

    ui->SerialNameBox->clear();
    ui->SerialNameBox->addItems(SerialPortName);
    ui->SerialNameBox->setCurrentIndex(oldSerialIndex);
}

void CHearingAidvMainWindow::slotReceiverDeviceNumRefresh()
{
    m_resFileList.clear();
    QString strReceiverPath = QCoreApplication::applicationDirPath() + "/ReceiverCalibrationValue";
    QFileInfoList fileList;
    fileList.clear();
    int  DeviceModeNumLeftReceiverOld = ui->DeviceModeNumLeftReceiverBox->currentIndex();
    if(DeviceModeNumLeftReceiverOld < 0){
        DeviceModeNumLeftReceiverOld = 0;
    }
    getDirFileAll(strReceiverPath,fileList);
    for(int i  = 0; i < fileList.size(); i++){
        m_resFileList.append(fileList.at(i).baseName());
    }

    ui->DeviceModeNumLeftReceiverBox->clear();

    foreach(QFileInfo file, fileList){
        ui->DeviceModeNumLeftReceiverBox->addItem(file.baseName());
    }
    ui->DeviceModeNumLeftReceiverBox->setCurrentIndex(DeviceModeNumLeftReceiverOld);
}

void CHearingAidvMainWindow::slotReceiverRightDeviceNumRefresh()
{
    m_resFileList.clear();
    QString strReceiverPath = QCoreApplication::applicationDirPath() + "/ReceiverCalibrationValue";
    QFileInfoList fileList;
    fileList.clear();
    int  DeviceModeNumRightReceiverOld = ui->DeviceModeNumRightReceiverBox->currentIndex();
    if(DeviceModeNumRightReceiverOld < 0){
        DeviceModeNumRightReceiverOld = 0;
    }
    getDirFileAll(strReceiverPath,fileList);
    for(int i  = 0; i < fileList.size(); i++){
        m_resFileList.append(fileList.at(i).baseName());
    }
    ui->DeviceModeNumRightReceiverBox->clear();
    foreach(QFileInfo file, fileList){
        ui-> DeviceModeNumRightReceiverBox->addItem(file.baseName());
    }
    ui->DeviceModeNumRightReceiverBox->setCurrentIndex(DeviceModeNumRightReceiverOld);
}

void CHearingAidvMainWindow::slotVolumeChannelSelectClicked()
{
    if(ui->channelRightCtrlBtn->isChecked() == false
            && ui->channelLeftCtrlBtn->isChecked() == false){
        ui->channelLeftCtrlBtn->setChecked(true);
    }
}

void CHearingAidvMainWindow::slotShowChannelCountDefault()
{
    int  nWdrcChannleNum = WDRC_CHANNGL_COUNT;
    m_nWdrcChannelCount  = nWdrcChannleNum;
    ui->WDRCChannelCounBox->setCurrentText(QString::number(nWdrcChannleNum));
    ui->WDRCChannelCounBox->setDisabled(true);
}

void CHearingAidvMainWindow::slotMicDeviceNumRefresh()
{
    m_micFileList.clear();
    QString strMicPath = QCoreApplication::applicationDirPath() + "/MicCalibrationValue";
    QFileInfoList fileList;
    fileList.clear();

    int  DeviceModeNumLeftMicOld = ui->DeviceModeNumLeftMicBox->currentIndex();
    if(DeviceModeNumLeftMicOld < 0 ){
        DeviceModeNumLeftMicOld = 0;
    }

    getDirFileAll(strMicPath,fileList);
    for(int i  = 0; i < fileList.size(); i++){
        m_micFileList.append(fileList.at(i).baseName());
    }

    ui->DeviceModeNumLeftMicBox->clear();
    foreach(QFileInfo file, fileList){
        ui->DeviceModeNumLeftMicBox->addItem(file.baseName());
    }
    ui->DeviceModeNumLeftMicBox->setCurrentIndex(DeviceModeNumLeftMicOld);
}

void CHearingAidvMainWindow::slotMicRightDeviceNumRefresh()
{
    m_micFileList.clear();
    QString strMicPath = QCoreApplication::applicationDirPath() + "/MicCalibrationValue";
    QFileInfoList fileList;
    fileList.clear();
    int  DeviceModeNumRightMicOld = ui->DeviceModeNumRightMicBox->currentIndex();
    if(DeviceModeNumRightMicOld < 0){
        DeviceModeNumRightMicOld = 0;
    }
    getDirFileAll(strMicPath,fileList);
    for(int i  = 0; i < fileList.size(); i++){
        m_micFileList.append(fileList.at(i).baseName());
    }
    ui->DeviceModeNumRightMicBox->clear();
    foreach(QFileInfo file, fileList){
        ui->DeviceModeNumRightMicBox->addItem(file.baseName());
    }
    ui->DeviceModeNumRightMicBox->setCurrentIndex(DeviceModeNumRightMicOld);
}

void CHearingAidvMainWindow::on_userGuiBtn_clicked()
{
    QString fileName =qApp->applicationDirPath() + "/知存助听器控制台使用说明.pdf";
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}


void CHearingAidvMainWindow::on_PreviewCfgFileBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("预览配置参数"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }

    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath = OTAModelInfo.absolutePath();
    QFile cfgArgFile(filePath);

    if(!cfgArgFile.open(QIODevice::ReadOnly)){
        return;
    }
    QByteArray cfgFileDataArray = cfgArgFile.readAll();

    QByteArray cfgFileHeadArray;
    QByteArray cfgVolumeModuleArray;
    QByteArray cfgDeviceNumModuleArray;
    QByteArray cfgPlayUpModuleArray;
    QByteArray cfgBackUpModuleArray;
    QByteArray cfgAlgCountModuleArray;
    QByteArray cfgAudioeCompensationArray;

    QString strfileHeadIdentification = "WITMEM";
    QString msg;
    for(int i = 0; i < cfgFileDataArray.size();i++){
        if(i < 64){
            cfgFileHeadArray.append(cfgFileDataArray.at(i));
            if(i == 5){
                if(strfileHeadIdentification != cfgFileHeadArray){
                    msg = "配置文件不含文件头，请重新选择";
                    slotShowMsgBox(msg);
                    return;
                }
            }else if(i == 6){
                if(cfgFileHeadArray.at(6) != CFG_FILE_FLAG){
                    msg = "配置文件不合法，请重新选择";
                    slotShowMsgBox(msg);
                    slotShowMsgBoxdialog(msg);
                    return;
                }
            }
        }else if(i <64 + 6){
            cfgVolumeModuleArray.append(cfgFileDataArray.at(i));
        }else if(i <64 + 6 + 4){
            cfgPlayUpModuleArray.append(cfgFileDataArray.at(i));
        }else if(i <64 + 6 + 4 + 2){
            cfgBackUpModuleArray.append(cfgFileDataArray.at(i));
        }else if(i <64 + 6 + 6 + 320){
            cfgDeviceNumModuleArray.append(cfgFileDataArray.at(i));
        }else if(i <64 + 6 + 6 + 320 + 1){
            cfgAlgCountModuleArray.append(cfgFileDataArray.at(i));
        }else if(i <64 + 6 + 6 + 320 + 1 + 16){
            cfgAudioeCompensationArray.append(cfgFileDataArray.at(i));
        }
    }

    QByteArray cfgArgDataArray = cfgFileDataArray.mid(64,cfgFileDataArray.size() - 1);
    uint8_t left = cfgFileHeadArray.at(29);
    uint8_t right = cfgFileHeadArray.at(28);
    ushort RecvCRC = (left << 8) | right;
    ushort CRCAlgFile = m_pSerialWork->crc16(cfgArgDataArray.data(),cfgArgDataArray.size());
    if(RecvCRC != CRCAlgFile){
        QMessageBox *msgBox =  new QMessageBox(this);
        QString strErrorMsg = "配置文件已篡改,是否继续预览?";
        msgBox->setText(strErrorMsg);
        msgBox->setWindowTitle("提示");

        QPushButton* noButton = msgBox->addButton(QMessageBox::No);
        noButton->setText("否");
        QPushButton* continueButton = msgBox->addButton("继续预览", QMessageBox::AcceptRole);
        msgBox->exec();
        if (msgBox->clickedButton() == noButton) {
            slotShowMsgBox("配置文件已篡改，已停止预览");
            return;
        }
        if (msgBox->clickedButton() == continueButton) {

        }
    }

    if(cfgVolumeModuleArray.at(0) == 0){
        ui->VolumeKnobModeRadioBtn->setChecked(true);
    }else{
        ui->VolumeKeyModeRadioBtn->setChecked(true);
    }
    ui->VolumeKeyStallNumComBox->setCurrentText(QString::number(cfgVolumeModuleArray.at(1)));
    ui->VolumeKeyStepComBox->setCurrentText(QString::number(cfgVolumeModuleArray.at(2)));
    ui->VolumeKeyCyCleComBox->setCurrentIndex(cfgVolumeModuleArray.at(3));
    ui->VolumeKnobRangeComBox->setCurrentText(QString::number(cfgVolumeModuleArray.at(4)));
    ui->VolumeKnobDirectionComBox->setCurrentIndex(cfgVolumeModuleArray.at(5));

    ui->pickupLeftChannelDGAEdit->setText(QString::number(cfgPlayUpModuleArray.at(0)));
    ui->pickupLeftChannelPGAEdit->setText(QString::number(cfgPlayUpModuleArray.at(1)));
    ui->pickupRightChannelDGAEdit->setText(QString::number(cfgPlayUpModuleArray.at(2)));
    ui->pickupRightChannelPGAEdit->setText(QString::number(cfgPlayUpModuleArray.at(3)));

    ui->playbackArgLeftChannelDGAEdilt->setText(QString::number(cfgBackUpModuleArray.at(0)));
    ui->playbackArgRightChannelDGAEdilt->setText(QString::number(cfgBackUpModuleArray.at(1)));

    QByteArray arryLeftMicModeNum;
    QByteArray arryRightMicModeNum;
    QByteArray arryLeftReceiverModeNum;
    QByteArray arryRightReceiverModeNum;

    arryLeftMicModeNum = cfgDeviceNumModuleArray.mid(0,16);
    arryRightMicModeNum = cfgDeviceNumModuleArray.mid(80,16);
    arryLeftReceiverModeNum = cfgDeviceNumModuleArray.mid(160,16);
    arryRightReceiverModeNum = cfgDeviceNumModuleArray.mid(240,16);

    ui->DeviceModeNumLeftMicBox->setCurrentText(arryLeftMicModeNum);
    ui->DeviceModeNumRightMicBox->setCurrentText(arryRightMicModeNum);
    ui->DeviceModeNumLeftReceiverBox->setCurrentText(arryLeftReceiverModeNum);
    ui->DeviceModeNumRightReceiverBox->setCurrentText(arryRightReceiverModeNum);

    ui->AlgArgCountBox->setCurrentText(QString::number(cfgAlgCountModuleArray.at(0)));

    ui->LeftChannelPreGainBox->setCurrentText(QString::number(cfgAudioeCompensationArray.at(0)));
    ui->RightChannelPreGainBox->setCurrentText(QString::number(cfgAudioeCompensationArray.at(1)));
    if (cfgAudioeCompensationArray.at(2) == 0){
        ui->cbCutLowFreq->setCurrentIndex(0);
    }else{
        ui->cbCutLowFreq->setCurrentIndex(1);
    }
    slotAllCmdFinshed();
    cfgArgFile.close();
}

void CHearingAidvMainWindow::on_OTADataSplicingBtn_clicked()
{
    int retEmpty = getOtaModelPathIsEmpty();
    if(retEmpty  != 0){
        return;
    }

    QString strOTADataPath = QCoreApplication::applicationDirPath() + "/OTAData";
    QString strDummyFilePath = strOTADataPath +"/"+ QString("%1.bin").arg("dummy");
    QString strRam1FilePath = strOTADataPath +"/"+ QString("%1.bin").arg("ram1");
    QString strBootloaderFilePath = strOTADataPath + "/" + QString("%1.bin").arg("bootloader");
    QString strImgBaseFilePath = strOTADataPath + "/" + QString("%1.bin").arg("img_base");

    QByteArray bootloaderArrayData;
    QByteArray dummyArrayData;
    QByteArray imgBaseArrayData;
    QByteArray ram1ArrayData;
    QByteArray fileArrayHeadData;
    QByteArray reserveArrayData;
    uchar ch  = 0x00;
    for(int i = reserveArrayData.size(); i < 4*OTA_DATA_SIZE_K; i++){
        reserveArrayData.append(ch);
    }

    for(int  i = 0; i < 840*OTA_DATA_SIZE_K; i++){
        imgBaseArrayData.append(ch);
    }

    QFile ImgBaseFile(strImgBaseFilePath);
    if(ImgBaseFile.exists()){
        if(!ImgBaseFile.open(QIODevice::ReadOnly)){
            QString msg = "文件无法打开";
            slotShowMsgBox(msg);
            return;
        }
        QByteArray  ImgBaseFiledata;
        ImgBaseFiledata = ImgBaseFile.readAll();
        imgBaseArrayData.replace(0, ImgBaseFiledata.size(), ImgBaseFiledata);
    }

    int ret = getBinFileData(ch,99,strDummyFilePath,dummyArrayData,fileArrayHeadData);
    if(ret  != 0 ){
        return;
    }
    for(int i = dummyArrayData.size(); i < OTA_DATA_SIZE_K; i++){
        dummyArrayData.append(ch);
    }


    ret = getBinFileData(ch,99,strRam1FilePath,ram1ArrayData,fileArrayHeadData);
    if(ret  != 0 ){
        return;
    }
    for(int i = ram1ArrayData.size(); i < 3*OTA_DATA_SIZE_K; i++){
        ram1ArrayData.append(ch);
    }

    ret = getBinFileData(ch,99,strBootloaderFilePath,bootloaderArrayData,fileArrayHeadData);
    if(ret  != 0 ){
        return;
    }
    for(int i = bootloaderArrayData.size(); i < 28*OTA_DATA_SIZE_K; i++){
        bootloaderArrayData.append(ch);
    }

    QString firmwareFilePath = ui->firmwareUpdataFilePathlineEdit->text();
    QByteArray firmwareArrayData;
    QByteArray firmwareArrayHeadData;
    QByteArray firmwareArrayHeadDataTail;
    ret = getBinFileData(ch,FIRMWARE_FILE_FLAG,firmwareFilePath,firmwareArrayData,firmwareArrayHeadData);
    if(ret  != 0 ){
        return;
    }
    firmwareArrayHeadDataTail.append(firmwareArrayHeadData);
    QByteArray time = currentSysTime();
    firmwareArrayHeadDataTail.append(time);
    for(int i = firmwareArrayHeadDataTail.size(); i < 4*OTA_DATA_SIZE_K; i++){
        firmwareArrayHeadDataTail.append(ch);
    }

    for(int i = firmwareArrayData.size(); i < 252*OTA_DATA_SIZE_K; i++){
        firmwareArrayData.append(ch);
    }

    QByteArray algArrayDataToatl;
    QByteArray algArrayData;
    int nAlgCount = ui->exportArgCountBox->currentText().toInt();
    for(int  i = 0; i < nAlgCount; i++){
        QString stralgFilePath = m_AlgModelList.at(i).pAlgFilePath->text().trimmed();
        algArrayData.clear();
        ret = get128ParameterFileData(i,ALG_FILE_FLAG,stralgFilePath,algArrayData);
        if(ret  != 0 ){
            return;
        }
        algArrayDataToatl.append(algArrayData);
    }

    for(int  i = 0; i < (4 - nAlgCount); i++){
        algArrayDataToatl.append(reserveArrayData);
    }

    QString strcfgFilePath = ui->configFilePathEdit->text().trimmed();
    QByteArray cfgArrayData;
    ret = get128ParameterFileData(ch,CFG_FILE_FLAG,strcfgFilePath,cfgArrayData);
    if(ret  != 0 ){
        return;
    }

    QByteArray MemoryAreaArrayData;
    getMemoryAreaData(MemoryAreaArrayData);

    QString OTADataSplicingPath = QFileDialog::getSaveFileName(this, tr("OTA封包文件"),m_strOTAModelPath,
                                                               tr("Bin Files (*.bin);"));
    if(OTADataSplicingPath.isEmpty()){
        return;
    }

    QByteArray firmwareFileData;
    firmwareFileData.append(dummyArrayData);
    firmwareFileData.append(ram1ArrayData);
    firmwareFileData.append(bootloaderArrayData);
    firmwareFileData.append(reserveArrayData);
    firmwareFileData.append(firmwareArrayHeadDataTail);
    firmwareFileData.append(firmwareArrayData);
    firmwareFileData.append(firmwareArrayHeadDataTail);
    firmwareFileData.append(firmwareArrayData);
    firmwareFileData.append(reserveArrayData);
    firmwareFileData.append(algArrayDataToatl);
    firmwareFileData.append(cfgArrayData);
    firmwareFileData.append(reserveArrayData);
    firmwareFileData.append(firmwareArrayHeadDataTail);
    firmwareFileData.append(firmwareArrayData);
    firmwareFileData.append(reserveArrayData);

    imgBaseArrayData.replace(0, firmwareFileData.size(), firmwareFileData);
    QFile OTADataSplicingFile(OTADataSplicingPath);
    if (OTADataSplicingFile.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        QDataStream out(&OTADataSplicingFile);
        out.writeRawData(imgBaseArrayData.data(), imgBaseArrayData.size());
    }
    OTADataSplicingFile.close();
    slotAllCmdFinshed();
}

void CHearingAidvMainWindow::on_exportArgCountBox_currentTextChanged(const QString &arg1)
{
    int  nCount =  arg1.toInt();
    for(int i = 0; i < m_AlgModelList.size(); i++){
        if(i < nCount){
            m_AlgModelList.at(i).pAlgGroupBox->setDisabled(false);
        }else{
            m_AlgModelList.at(i).pAlgGroupBox->setDisabled(true);
        }
    }
}


void CHearingAidvMainWindow::on_tabWidget_currentChanged(int index)
{
    switch(index){
    case 0:
        ui->SerialNameBox->setFocus();
        break;
    case 1:
        ui->FrequencyBandCoBoxFirst->setFocus();
        break;
    case 2:
        break;
    case 3:
        ui->NoiseHorizontalSlider->setFocus();
        break;
    case 4:
        ui->NRNLICalibrationLeftLineEdit->setFocus();
        break;
    case 5:
        ui->InitPasswordLevelCombox->setFocus();
        break;
    case 6:
        ui->soundPresLE->setFocus();
        break;
    case 7:
        ui->firmwareUpdataFilePathlineEdit->setFocus();
        break;
    case 8:
        ui->VolumeKeyModeRadioBtn->setFocus();
        break;
    }
}

void CHearingAidvMainWindow::on_WDRCChannelCounBox_currentTextChanged(const QString &arg1)
{
    int nChannelCount = arg1.toInt();
    QString path = QCoreApplication::applicationDirPath();
    QString ChannelRangeFilePath = path + QString("/JsonFile/ChannelRange_%1.json").arg(nChannelCount);
    QFile file(ChannelRangeFilePath);
    QString msg;
    if(!file.exists()){
        msg = "WDRC频段文件不存在";
        slotShowMsgBox(msg);
        return;
    }

    if(file.size() == 0){
        msg = "WDRC频段文件为空";
        slotShowMsgBox(msg);
        return;
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        msg = "文件无法打开";
        slotShowMsgBox(msg);
        return;
    }
    QString str = file.readAll();
    file.close();

    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(str.toUtf8(), &parseJsonErr);
    if (!(parseJsonErr.error == QJsonParseError::NoError)) {
        slotShowMsgBox("WDRC频段配置文件错误！");
        return;
    }
    QList<CWDRCItemWidget*>WdrcitemList = ui->scrollAreaWidgetContents->findChildren<CWDRCItemWidget*>();
    if(WdrcitemList.size() == 0){
        return;
    }
    CWDRCItemWidget *pWidget = nullptr;
    ui->scrollArea->m_checkList.clear();

    QJsonObject jsonObject = document.object();
    if (jsonObject.contains(QStringLiteral("WDRC Channel Range"))) {
        QJsonValue arrayValue = jsonObject.value(QStringLiteral("WDRC Channel Range"));
        if (arrayValue.isArray()) {
            QJsonArray array = arrayValue.toArray();
            ui->FrequencyBandCoBoxFirst->clear();
            for (int i = 1; i < nChannelCount +1; i++) {
                QString strChannel = QString("%1-%2").arg(array.at(i - 1).toString()).arg(array.at(i).toString());
                ui->FrequencyBandCoBoxFirst->addItem(strChannel);
                pWidget = WdrcitemList.at(i -1);
                if(pWidget){
                    pWidget->setChecBoxLabelValue(strChannel);
                }
            }
        }
    }

    for(int i = 0; i < WdrcitemList.size();i++){
        pWidget = WdrcitemList.at(i);
        if(i < nChannelCount){
            ui->scrollArea->m_checkList.append(pWidget->getChannelSingleCheckBox());
            pWidget->setVisible(true);
            pWidget->setObjectName("enableWidget");
        }else{
            pWidget->setVisible(false);
            pWidget->getChannelSingleCheckBox()->setChecked(false);
            pWidget->setObjectName("disableWidget");
        }
    }
    slotWDRCSigalClicked();
}

void CHearingAidvMainWindow::on_VersionBtn_clicked()
{
    m_isViewVersion = true;
    m_pShowFileHeadDialog = new  ShowFileHeadInfoDialog(this);

    QByteArray sendArray;
    uchar index = 0x00;
    sendArray.append(index);
    int ret = m_pSerialWork->processData(CMD_UPDATE_0x03,INST_CMD_0x11, sendArray);
    if(ret != 0)
        return;
    ret = m_pSerialWork->processData(CMD_UPDATE_0x03,INST_CMD_0x16, sendArray);
    if(ret != 0)
        return;

    for(int i = 0; i < m_nArgCountRead;i++){
        sendArray.clear();
        sendArray.append(i);
        ret = m_pSerialWork->processData(CMD_UPDATE_0x03,INST_CMD_0x1A, sendArray);
        if(ret != 0)
            return;
    }
}


void CHearingAidvMainWindow::on_OtaModelViewImageBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择镜像文件"),m_strOTAModelPath,
                                                    tr("Bin Files (*.bin);"));
    if(filePath.isEmpty()){
        return;
    }
    OTAModelInfo.setFile(filePath);
    m_strOTAModelPath = OTAModelInfo.absolutePath();

    QFile imageFile(filePath);
    if(!imageFile.open(QIODevice::ReadOnly)){
        return;
    }
    QByteArray ImageArray;
    ImageArray = imageFile.readAll();
    QByteArray firmwareArray;
    QByteArray firmwareTimeArray;
    QByteArray cfgArray;
    QByteArray cfgTimeArray;
    QByteArray arg1Array;
    QByteArray arg1TimeArray;
    QByteArray arg2Array;
    QByteArray arg2TimeArray;
    QByteArray arg3Array;
    QByteArray arg3TimeArray;
    QByteArray arg4Array;
    QByteArray arg4TimeArray;
    firmwareArray = ImageArray.mid(hexToint10(0x009000),FILEHEADLENGTH);
    firmwareTimeArray = ImageArray.mid(hexToint10(0x009040),FILEHEADTIMELENGTH);

    cfgArray  = ImageArray.mid(hexToint10(0x08E000),FILEHEADLENGTH);
    cfgTimeArray = ImageArray.mid(hexToint10(0x08E040),FILEHEADTIMELENGTH);

    arg1Array = ImageArray.mid(hexToint10(0x08A000),FILEHEADLENGTH);
    arg1TimeArray = ImageArray.mid(hexToint10(0x08A040),FILEHEADTIMELENGTH);

    arg2Array = ImageArray.mid(hexToint10(0x08B000),FILEHEADLENGTH);
    arg2TimeArray = ImageArray.mid(hexToint10(0x08B040),FILEHEADTIMELENGTH);

    arg3Array = ImageArray.mid(hexToint10(0x08C000),FILEHEADLENGTH);
    arg3TimeArray = ImageArray.mid(hexToint10(0x08C040),FILEHEADTIMELENGTH);

    arg4Array = ImageArray.mid(hexToint10(0x08D000),FILEHEADLENGTH);
    arg4TimeArray = ImageArray.mid(hexToint10(0x08D040),FILEHEADTIMELENGTH);

    if(firmwareArray.at(6) != 0x00 || cfgArray.at(6) != 0x02
            || arg1Array.at(6) != 0x01){
        slotShowMsgBox("请选择一个正确的镜像文件");
        return;
    }

    FileInfoHeadData infoData;
    ShowFileHeadInfoDialog *pShowFileHeadDialog  = new  ShowFileHeadInfoDialog(this);

    parsingOtaFileHeaderInfo(firmwareArray,0,FIRMWARE_FILE_HEAD,infoData);
    infoData.strWriteTime = parseTimeShowUi(firmwareTimeArray);
    pShowFileHeadDialog->setFileHeadInfoText(infoData);

    parsingOtaFileHeaderInfo(arg1Array,1,ALG_ARG_FILE_HEAD,infoData);
    infoData.strWriteTime = parseTimeShowUi(arg1TimeArray);
    pShowFileHeadDialog->setFileHeadInfoText(infoData);

    parsingOtaFileHeaderInfo(arg2Array,2,ALG_ARG_FILE_HEAD,infoData);
    infoData.strWriteTime = parseTimeShowUi(arg2TimeArray);
    pShowFileHeadDialog->setFileHeadInfoText(infoData);

    parsingOtaFileHeaderInfo(arg3Array,3,ALG_ARG_FILE_HEAD,infoData);
    infoData.strWriteTime = parseTimeShowUi(arg3TimeArray);
    pShowFileHeadDialog->setFileHeadInfoText(infoData);

    parsingOtaFileHeaderInfo(arg4Array,4,ALG_ARG_FILE_HEAD,infoData);
    infoData.strWriteTime = parseTimeShowUi(arg4TimeArray);
    pShowFileHeadDialog->setFileHeadInfoText(infoData);

    parsingOtaFileHeaderInfo(cfgArray,0,CFG_FILE_HEAD,infoData);
    infoData.strWriteTime = parseTimeShowUi(cfgTimeArray);
    pShowFileHeadDialog->setFileHeadInfoText(infoData);

    slotAllCmdFinshed();
    pShowFileHeadDialog->exec();
}


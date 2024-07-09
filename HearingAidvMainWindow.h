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

#ifndef HEARINGAIDVMAINWINDOW_H
#define HEARINGAIDVMAINWINDOW_H
#include <QRubberBand>
#include <QMainWindow>
#include <QCheckBox>
#include <QRadioButton>
#include <QScrollBar>
#include <QThread>
#include <QSlider>
#include <QLineEdit>
#include <QTextEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QFileInfo>
#include <QtEndian>
#include <QRegExp>
#include <QRegExpValidator>
#include <QRegularExpression>
#include <QMenu>
#include <QValueAxis>
#include <QLineSeries>
#include <QChar>
#include <QGraphicsLayout>
#include <QLegend>
#include <QChartView>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include "SerialWork.h"
#include "FileHeadInfoDialog.h"
#include "ShowFileHeadInfoDialog.h"

struct ALGMODELDTAT
{
    QGroupBox* pAlgGroupBox = nullptr;
    QLineEdit *pAlgFilePath =  nullptr;
};


QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui { class CHearingAidvMainWindow; }
QT_END_NAMESPACE

class CHearingAidvMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    CHearingAidvMainWindow(QWidget *parent = nullptr);
    ~CHearingAidvMainWindow();

private:
    void initUi();
    void initEQUi();
    void initWDRCUi(int nChannelCount);
    void initProcessProgressBar();
    void initCharts();
    void drawCurves(float,float,float,float,int);
    void initNoiseUi();
    void initConnects();
    void initOther();
    void initSerialDialog();
    void initCalibrationValue();
    void initBtnStatus();
    void initTabFocusOrder();
    QString parseTimeShowUi(QString time);

    void getDirFileAll(QString pathDir,  QFileInfoList &fileList);
    void getFileCalibrationData(int type ,QString strFilePath, QByteArray &fileDataArry);
    void applyLineEditFormat(QLineEdit *lineEdit,int rex);
    QByteArray getHexAddr(QLineEdit *le);
    QByteArray getPaswd(QLineEdit *le);
    QByteArray intToArry(int nData);
    void uartReadBytes512();
    void getCurrentAlgArgType();
    void getAudioCompensationValueCmd();
    void setAudioCompensationValueCmd();
    void getCurrentWorkingMode();
    void getVolumeCurrentMuteSattus();
    void getVolumeReadCurrent();
    void getCurrentAlgSwitchBtnStatus();
    void initUiDataReadDevice();
    void getWDRCAlgArgSeparate();
    void setNoiseUiSattus(bool isEnable);
    void setLineSeriesView(int index, int pLineSeriesType);
    void setVolumeKeyModeShow();
    void showProgressBarStart(int maxValue, int value);
    void otaUartWriteFileBytes512(int transmissionsCount,uchar cmd, uchar sunCmd,QByteArray fileDataArray);
    void noAddressReadFile512(uchar index,uchar cmd,uchar sunCmdFirst,uchar sunCmdSecond);
    void otaUartReadFileBytes512(uchar cmd, uchar sunCmd);
    void parsingFileHeader(QVector <uchar > boyData,QByteArray &FileHeadInfoArray,QString &time);
    void parsingFileHeaderInfoShow(QByteArray FileHeadInfoArray,QString time, int Index, int HeadType);
    void getMemoryAreaData(QByteArray &MemoryAreaDataArray);
    void getAlgDeviceData();
    void setFirmwareAddFileHead();

    QByteArray currentSysTime();
    QByteArray getPickUpArgdata();
    QByteArray getAudioCompensationValueData();
    QByteArray getVolumeModeldata();
    QByteArray getBackUpArgdata();
    QByteArray getDeviceNumdata();
    QByteArray getAlgArgCountdata();
    QByteArray getWDRCData();
    QByteArray getEQData();
    QByteArray getNRData();
    QByteArray getNLIData();
    int noAddressWriteFile512(int fileType,uchar index,uchar cmd,uchar sunCmdFirst,uchar sunCmdSecond, QString path);
    int uartWriteFileBytes(int transmissionsCount,QByteArray fileDataArray);
    int getWDRCChannelCount();
    int parsingOtaFileHeaderInfo(QByteArray FileHeadInfoArray,int Index, int HeadType,FileInfoHeadData &infoData);
    int getFileHeadInfo(int type, QByteArray cfgFileData,QByteArray &fileHeadArray);
    int getBinFileData(uchar index,int fileType, QString strFilePath, QByteArray &FileDataArray,QByteArray &FileDataHeadArray);
    int get128ParameterFileData(uchar index,int type,QString strFilePath,QByteArray &FileDataArrayTail);
    int hexToint10(ulong hexData);
    int getOtaModelPathIsEmpty();

protected:
    void saveInputData();
    void closeEvent(QCloseEvent *event);
    void initUiReadJsonData();
    void keyPressEvent(QKeyEvent *event);
    void showStatusBarlightBtn(QString msg,int ret);
signals:
    void signalFlashToUiOrFile(int);
    void signalFilePath(const QString& path, const QString& resAddressArry,int nWriteLength);
    void signalEQIsSelectAll(bool isCheck);
    void signalWDRCIsSelectAll(bool isCheck);
    void signalSendClose();
    void signalSendAccept(QString port, int baudRate);

public slots:
    void slotSerialPortStateChanged(bool state);

private slots:
    void slotEqSigalClicked();
    void slotWDRCSigalClicked();
    void slotInitUiData(bool isOpen);
    void slotIsSelectAllCheckBox(bool isChecked);
    void slotEQArgIsSelectAllCheckBox(bool isChecked);
    void slotShowMsgBox(QString);
    void slotShowMsgBoxdialog(QString);
    void slotShowMsgDevStatus(QString);
    void slotFlashSwitchBtnClicked(bool);
    void slotShowAnswerResult(uchar cmd, uchar subCmd,uchar result);
    void slotVolumeMuteSwithBtnClicked(bool  checked);
    void slotCmdWriteFinshed(int size, int nBarType);
    void slotChangeAlgArgBtnClick();
    void slotChangeModeTypeBtnClick();
    void slotAlgSwitchBtnClicked(bool  checked);

    void slotFlashDataUpdata(QString,int);
    void slotShowUpdataCmdReadData(int nReadType,QVector <uchar > &boyData);
    void slotShowReadCurrCtrlCMDNum(uchar leftChannel, uchar rightChhanle,int nType);
    void slotShowCfgCmdReadData(int nReadType,QVector <uchar > &boyData);
    void slotShowAlgeriaCmdReadData(int nReadType,QVector <uchar > &boyData);
    void slotSetEqSliderPosSynchronization(int newValue);
    void slotSetWDRCComBoxIndexSynchronization(int value);
    void slotSetWDRCSpinBoxValueSynchronization(double value);
    void slotMicReceiverComboBoxClicked();
    void slotStartTimer();
    void slotStopTimer();
    void slotAllCmdFinshed();
    void slotStausBarNoOper();
    void slotSaveFileHeadInfo(int  type,QString FileVersion, QString FileCode);
    void slotSerialNameBoxRefresh();
    void slotMicDeviceNumRefresh();
    void slotMicRightDeviceNumRefresh();
    void slotReceiverDeviceNumRefresh();
    void slotReceiverRightDeviceNumRefresh();
    void slotVolumeChannelSelectClicked();
    void slotShowChannelCountDefault();

private slots:
    void on_getInitPasswordBt_clicked();
    void on_getPermissionBt_clicked();
    void on_changePasswdBt_clicked();
    void on_clearPasswdBt_clicked();
    void on_permPasswdLE_textChanged(const QString &arg1);
    void on_changeOldpaswdLE_textChanged(const QString &arg1);
    void on_clearPaswdLE_textChanged(const QString &arg1);
    void on_soundPresLE_textChanged(const QString &arg1);
    void on_InitPasswordEdit_textChanged(const QString &arg1);
    void on_FlashReadToUiBt_clicked();
    void on_FlashFileReadBtn_clicked();
    void on_FlashReadToFileBt_clicked();
    void on_FlashFromUiBt_clicked();
    void on_FlashFromTE_textChanged();
    void on_flashOpenFIleBt_clicked();
    void on_FlashFromFileBt_clicked();
    void on_IsFileFullSizecheckBox_clicked(bool checked);
    void on_FlashEraseBt_clicked();
    void on_FlashVerifyBt_clicked();
    void on_FlashCopyBt_clicked();
    void on_voiceCtrlBt_clicked();
    void on_VolumeAdd_clicked();
    void on_VolumeLessen_clicked();
    void on_VolumeReadDataBtn_clicked();
    void on_VolumeWirteDataBtn_clicked();
    void on_VolumeKeyStallNumComBox_activated(int index);
    void on_VolumeKeyStepComBox_activated(int index);

    void on_pickupArgReadDataBtn_clicked();
    void on_pickupArgWriteDataBtn_clicked();
    void on_playbackArgReadDataBtn_clicked();
    void on_playbackArgWriteDataBtn_clicked();
    void on_pickupLeftChannelDGAEdit_textChanged(const QString &arg1);
    void on_playbackArgLeftChannelDGAEdilt_textChanged(const QString &arg1);
    void on_DeviceModeNumReadDataBtn_clicked();
    void on_DeviceModeNumWriteDataBtn_clicked();
    void on_AlgArgCountWriteBtn_clicked();
    void on_AlgArgCountReadBtn_clicked();
    void on_CfgCmdReadAllBtn_clicked();
    void on_CfgCmdWriteAllBtn_clicked();

    void on_configReadBtn_clicked();
    void on_confgWriteBtn_clicked();
    void on_PreviewCfgFileBtn_clicked();
    void on_CfgCreatFileBtn_clicked();
    void on_configFilePathEdit_textChanged(const QString &arg1);
    void on_configBrowseBtn_clicked();

    void on_EQArgWrite64ChannelBtn_clicked();
    void on_EQArgRead64ChannelBtn_clicked();
    void on_FrequencyBandCoBoxFirst_activated(int index);
    void on_WDRCReadArg_clicked();
    void on_WDRCwriteArg_clicked();
    void on_WDRCReadArgTogether_clicked();
    void on_WDRCwriteArgTogether_clicked();
    void on_WDRCATLineEdit_textChanged(const QString &arg1);

    void on_NoiseHorizontalSlider_valueChanged(int value);
    void on_NRARGWriteBtn_clicked();
    void on_NRARGReadBtn_clicked();
    void on_NRNLIDataWriteBtn_clicked();
    void on_NRNLIDataReadBtn_clicked();
    void on_NRNLICalibrationLeftLineEdit_textChanged(const QString &arg1);
    void on_AlgFileDataToUiBtn_clicked();
    void on_AlgUIDataToFileBtn_clicked();

    void on_AlgArgUpdataBrowesFileBtn_1_clicked();
    void on_AlgArgUpdataReadBtn_1_clicked();
    void on_AlgArgUpdataWriteBtn_1_clicked();
    void on_AlgArgUpdataFilePathlineEdit_1_textChanged(const QString &arg1);

    void on_AlgArgUpdataBrowesFileBtn_2_clicked();
    void on_AlgArgUpdataReadBtn_2_clicked();
    void on_AlgArgUpdataWriteBtn_2_clicked();
    void on_AlgArgUpdataFilePathlineEdit_2_textChanged(const QString &arg1);

    void on_AlgArgUpdataBrowesFileBtn_3_clicked();
    void on_AlgArgUpdataReadBtn_3_clicked();
    void on_AlgArgUpdataWriteBtn_3_clicked();
    void on_AlgArgUpdataFilePathlineEdit_3_textChanged(const QString &arg1);

    void on_AlgArgUpdataBrowesFileBtn_4_clicked();
    void on_AlgArgUpdataReadBtn_4_clicked();
    void on_AlgArgUpdataWriteBtn_4_clicked();
    void on_AlgArgUpdataFilePathlineEdit_4_textChanged(const QString &arg1);

    void on_firmwareUpdataBrowsFileBtn_clicked();
    void on_firmwareUpdataWriteBtn_clicked();
    void on_firmwareUpdataFilePathlineEdit_textChanged(const QString &arg1);

    void on_SerialPortOpenBtn_clicked();
    void on_RestoreSerialBtn_clicked();
    void on_SerialPortCloseBtn_clicked();
    void on_VersionBtn_clicked();
    void on_userGuiBtn_clicked();
    void on_OTADataSplicingBtn_clicked();
    void on_OtaModelViewImageBtn_clicked();
    void on_exportArgCountBox_currentTextChanged(const QString &arg1);

    void on_tabWidget_currentChanged(int index);
    void on_WDRCChannelCounBox_currentTextChanged(const QString &arg1);
private:
    Ui::CHearingAidvMainWindow *ui;
    QThread *m_pSerialThread;
    bool m_nInitIsUiData = true;
    QString m_strConnectedPortname;

    uchar m_cCmd;
    uchar m_cSubCmdSecond;
    QByteArray m_fileHeadInfoArray;
    bool m_bIsFirstFileHead = true;
    QStringList m_micFileList;
    QStringList m_resFileList;

    int m_nArgCountRead = 0;
    bool m_isViewVersion = false;
    int  m_nArgCountHead = 0;
    ShowFileHeadInfoDialog *m_pShowFileHeadDialog = nullptr;
    QString m_strFirmWareVersion= "";
    QString m_strFirmWareFilePath;
private:
    uchar m_uFirstArg = 0;
    uchar m_uSecondArg = 0;
    QList<ALGMODELDTAT>m_AlgModelList;
private:
    QVector<int> m_eq_status;
    QVector<int> m_wdrcCTStatus;
    QVector<int> m_wdrcMPOStatus;
    QVector<int> m_wdrcGainStatus;
    QVector<double> m_spinboxStatus;
    QMap<QRadioButton*,bool>m_btnStatusMap;

private:
    QLineSeries *m_pLineSeriesFirst;

private:
    int m_nSendCount = 0;
    int m_nTransmissionsCount = 0;
    int m_nFileDataTailSize = 0;
    int m_nReadFileDataTailSize = 0;
    int m_nLastPackDataSize = 0;
    int m_nWriteFullSize = 0;
    uchar *m_pArrySectionalData256;
    bool m_bIsUiWrite = true;
    QByteArray m_UIDataSouce;
    QFile m_ReadFile;
    SerialWork* m_pSerialWork;
    int m_nWdrcChannelCount = 4;
private:
    QFileInfo m_flashReadFile,m_flashFromFile;
    QString m_strFalshReadPath = QDir::homePath();
    QString m_strFalshWritePath =QDir::homePath();

    QFileInfo OTAModelInfo;
    QString m_strOTAModelPath = QDir::homePath();
    int m_nCMDCount = CMD_COUNT_ALONE;
};
#endif // HEARINGAIDVMAINWINDOW_H

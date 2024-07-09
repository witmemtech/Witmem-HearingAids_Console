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

#ifndef DATADEFINEHEADER_H
#define DATADEFINEHEADER_H
#include <QMap>

#endif // DATADEFINEHEADER_H
#define CMD_CFG_0x00 0x00
#define CMD_CTRL_0x01 0x01
#define CMD_ALG_0x02   0x02
#define CMD_UPDATE_0x03 0x03

#define INST_CMD_0x00 0x00
#define INST_CMD_0x01 0x01
#define INST_CMD_0x02 0x02
#define INST_CMD_0x03 0x03
#define INST_CMD_0x04 0x04
#define INST_CMD_0x05 0x05
#define INST_CMD_0x06 0x06
#define INST_CMD_0x07 0x07
#define INST_CMD_0x08 0x08
#define INST_CMD_0x09 0x09
#define INST_CMD_0x0A 0x0A
#define INST_CMD_0x0B 0x0B
#define INST_CMD_0x10 0x10
#define INST_CMD_0x11 0x11
#define INST_CMD_0x12 0x12
#define INST_CMD_0x13 0x13
#define INST_CMD_0x14 0x14
#define INST_CMD_0x15 0x15
#define INST_CMD_0x16 0x16
#define INST_CMD_0x17 0x17
#define INST_CMD_0x18 0x18
#define INST_CMD_0x19 0x19
#define INST_CMD_0x1A 0x1A
#define INST_CMD_0x1B 0x1B
#define INST_CMD_0x1C 0x1C
#define INST_CMD_0x1D 0x1D
#define INST_CMD_0x20 0x20
#define INST_CMD_0x21 0x21
#define INST_CMD_0x22 0x22
#define INST_CMD_0x23 0x23
#define INST_CMD_0x24 0x24
#define INST_CMD_0x25 0x25
#define INST_CMD_0x26 0x26
#define INST_CMD_0x27 0x27
#define INST_CMD_0x28 0x28
#define INST_CMD_0x29 0x29
#define INST_CMD_0x30 0x30
#define INST_CMD_0x31 0x31

#define DATA_HEAD 0X2A
#define DATA_TAIL 0X23

#define CODE_FLAG_0 0
#define CODE_FLAG_1 1
#define CODE_FLAG_2 2
#define CODE_FLAG_3 3

#define FILE_INDEX_0x00 0x00
#define FILE_INDEX_0x01 0x01
#define FILE_INDEX_0x02 0x02
#define FILE_INDEX_0x03 0x03
#define FILE_INDEX_0x04 0x04

#define WRITE_DATA_SIZE 512

#define PASSWORD_LENTH 8
#define HEX_DATA_LENTH 11
#define MODE_NUM_LENTH 16
#define MODE_NUM_FFT_SIZE 64

#define WDRC_AXISY_RANGE  140
#define WDRC_AXISX_RANGE  120
#define OTA_DATA_SIZE_K   1024
#define WDRC_CHANNGL_COUNT 32
#define FILEHEADLENGTH 64
#define FILEHEADTIMELENGTH 14

enum Hexadecimal_DisplayType{
    NONE_SPACE = 0,
    DEFAULT_ADD_SPACE
};

enum Hex_Regx{
    HEX_NORMAL = 0,
    HEX_NUMBER
};

enum Progress_Bar_Type{
    READ_DATA = 0,
    WRITE_DATA
};


enum CTRL_CMD{
    CURRENT_VOLUME_NUM = 0,
    CURRENT_MUTE_TATUS,
    CURRENT_ALG_ARG_TYPE,
    CURRENT_WORKING_MODE,
    CURRENT_ALG_SWITCHBTN_STATUS
};
enum Device_Type{
    DEVICE_MIC = 0,
    DEVICE_RECEIVER
};
enum CMD_COUNT{
    CMD_COUNT_OVERALL,
    CMD_COUNT_ALONE
};
enum CFG_CMD{
    CFG_VOLUME_MODE = 0,
    CFG_VOLUME_MODE_VOLUMETYPE,
    CFG_VOLUME_KEY_DATA,
    CFG_VOLUME_KNOB_DATA,
    CFG_PICKUP_ARG_MODE,
    CFG_PLAYBACK_ARG_MODE,
    CFG_DEVIVE_NUM_MODE,
    CFG_ALG_ARG_COUNT,
    CFG_AUDIO_COMPENSATION,
};

enum ALG_CMD{
    ALG_GET_WDRC_ARG = 0,
    ALG_GET_WDRC_CT,
    ALG_GET_WDRC_CR,
    ALG_GET_WDRC_GAIN,
    ALG_GET_WDRC_MPO,
    ALG_GET_WDRC_ATRT,
    ALG_GET_DENOISE_ARG,
    ALG_GET_DENOISE_LEVEL,
    ALG_GET_EQ_ARG,
    ALG_GET_WDRC_CHANNEL_COUNT
};


enum UPDATA_CMD{
    PASSWORD_PERMISSION = 0,
    CLEAR_PASSWORD,
    READ_DATA_UI,
    READ_DATA_FILE,
    GET_CHECK_RESULT,
    WRITE_PROJECT_SWITCHBTN_STATUS,
    CFG_FILE_HEAD,
    CFG_FILE_READ_DATA,
    ALG_ARG_FILE_HEAD,
    ALG_ARG_FILE_READ_DATA,
    FIRMWARE_FILE_HEAD
};
enum WDRC_LINSERS{
    LineSeriesSecond,
    LineSeriesFirst
};

extern QMap<uchar, QString> UniversalErrorCode;
extern QMap<uchar, QMap<uchar ,QString>> UpdaSpecialErrorCode;
extern QMap<uchar, QMap<uchar ,QString>> CtrlCmdSpecialErrorCode;
extern QMap<uchar, QMap<uchar ,QString>> CfgCmdSpecialErrorCode;
extern QMap<uchar, QMap<uchar ,QString>> AlgeriaCmdSpecialErrorCode;
extern QMap<uchar ,QString> MuteStatus;
extern QMap<uchar ,QString> WorkingMode;
extern QMap<uchar ,QString> FileTypeMap;
extern QMap<uchar ,QString> algFileTypeMap;

















// Crossfire protocol is partially documented here
//    https://github.com/betaflight/betaflight/blob/master/src/main/rx/crsf.c
//    https://github.com/iNavFlight/inav/blob/master/src/main/telemetry/crsf.c
//    https://github.com/CapnBry/CRServoF
//  
// Each message has the following structure:
//    <SYNC BYTE> <FRAME LENGTH INCL. TYPE AND CRC> <FRAME TYPE> <PAYLOAD> <CRC>
//
// The CRC is a CRC8 with the polynomial 0XD5. The CRC calculation includes the TYPE byte and PAYLOAD bytes.
// 
// Example - Link statistics message:
//    0xC8, 0xC, 0x14, 0x14, 0x0, 0x64, 0xD, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0xFB
//  
//    0xC8 is the sync byte
//    0xC is decimal 12, which is the length of the message without sync byte and without crc
//    0x14 is the type, which is link statistics in this example
//    0xFB is the CRC
//
// The RC channels are 11bit each, packed into 22 bytes (16 * 11 / 8 = 22). This is identical to SBUS
// packing so we can use some of the SBUS code here as well.
//
// Notes: 
//  - Although the RC channels message is 22 byte (16 channels), only 12 are used.

#ifndef CRSF_H
#define CRSF_H

#include "Arduino.h"

#include "Crc8.h"

#define CRSF_BAUDRATE 420000

#define CRSF_MAX_MESSAGE_LENGTH 64

#define CRSF_SYNC_BYTE 0xC8
#define CRSF_CRC8_POLY 0xD5

#define CRSF_SYNC_BYTE_MESSAGE_INDEX   0
#define CRSF_LENGTH_BYTE_MESSAGE_INDEX 1
#define CRSF_TYPE_BYTE_MESSAGE_INDEX   2

#define CRSF_MIN_MESSAGE_LENGTH 5 // SYNC(1byte) + LENGTH(1byte) + TYPE(1byte) + PAYLOAD(min. 1byte) + CRC (1byte) 

#define CRSF_FRAMETYPE_RC_CHANNELS_PACKED 0x16
#define CRSF_FRAMETYPE_LINK_STATISTICS    0x14
#define CRSF_FRAMETYPE_LINK_STATISTICS_TX 0x1D
#define CRSF_FRAMETYPE_LINK_STATISTICS_RX 0x1C 
#define CRSF_FRAMETYPE_BATTERY_SENSOR     0x08
#define CRSF_FRAMETYPE_GPS_SENSOR         0x02
#define CRSF_FRAMETYPE_ATTITUDE_SENSOR    0x1E 

#define CRSF_NUMBER_OF_CHANNELS 12

#define CRSF_INCOMING_MESSAGE_TIMEOUT_TIME      5
#define CRSF_CONNECTION_LOST_FAILSAFE_TIME    100
#define CRSF_PAUSE_BETWEEN_TELEMETRY_MESSAGES 200

#define CRSF_RAW_CHANNEL_MIN  172
#define CRSF_RAW_CHANNEL_MAX 1812

#define CRSF_CHANNEL_MIN  988
#define CRSF_CHANNEL_MID 1500
#define CRSF_CHANNEL_MAX 2012

#define CRSF_CHANNELS_DIFFS_RATIO 0.62477120195241 // so that 172 is 988 and 1812 is 2012

#define CRSF_DELAY_BETWEEN_DEBUG_INFO_CALCULATION 1000

class Crsf
{
  public:
  
    Crsf();

    void basicInit();

    void init(HardwareSerial* serialPort_);
  
    void update();

    void addByteToMessage(byte byte_);

    void resetMessage();

    void evaluateMessage();
    void evaluateRcChannelsMessage();
    void evaluateLinkStatisticsMessage();
    void evaluateLinkStatisticsTxMessage();
    void evaluateLinkStatisticsRxMessage();

    void printMessage(HardwareSerial* serialPort_);

    void printChannels(HardwareSerial* serialPort_);

    void printLinkStatistics(HardwareSerial* serialPort_);

    void sendBatterySensorTelemetryMessage();
    void sendGpsSensorTelemetryMessage();
    void sendAttitudeSensorTelemetryMessage();

    HardwareSerial* serialPort;

    Crc8 crc8;

    byte incomingByte;

    unsigned long lastTimeByteReceived;
    unsigned long lastTimeDebugInfoCalculated;
    unsigned long lastTimeRcChannelsMessageWasValid;
    
    uint8_t message[CRSF_MAX_MESSAGE_LENGTH];
    
    uint8_t messageIndex;

    uint16_t messageTimeoutErrorCounter;
    uint16_t syncByteErrorCounter;
    uint16_t crcErrorCounter;
    
    uint16_t channels[CRSF_NUMBER_OF_CHANNELS];

    uint16_t validRcChannelsMessagesCounter;
    uint16_t validLinkStatisticsMessagesCounter;
    uint16_t validLinkStatisticsTxMessagesCounter;
    uint16_t validLinkStatisticsRxMessagesCounter;

    uint16_t validRcChannelsMessagesRate;
    uint16_t validLinkStatisticsMessagesRate;
    uint16_t validLinkStatisticsTxMessagesRate;
    uint16_t validLinkStatisticsRxMessagesRate;

    unsigned long lastTimeTelemetrySent;

    bool useBatterySensorTelemetry;
    bool useGpsSensorTelemetry;
    bool useAttitudeSensorTelemetry;

    bool failsafeIsActive;

    // Link statistics telemetry
    // -----------------------------------------------
    uint8_t uplinkRssi1;
    uint8_t uplinkRssi2;
    uint8_t uplinkQuality;
    int8_t  uplinkSnr;
    uint8_t diversityActiveAntenna;
    uint8_t rFMode;
    uint8_t uplinkTxPower;
    uint8_t downlinkRssi;
    uint8_t downlinkQuality;
    int8_t  downlinkSnr;
    
    // Battery telemetry
    // ------------------------------------------------
    uint16_t voltage;       // Voltage in V multiplied by 10
    uint16_t current;       // Current in A multiplied by 10
    uint32_t capacity;      // Capacity drawn in mAh
    uint8_t  battery;       // Battery remaining in %

    // GPS telemetry
    // ------------------------------------------------
    int32_t  latitude;      // latitude in degrees multiplied by 10.000.000
    int32_t  longitude;     // longitude in degrees multiplied by 10.000.000
    uint16_t groundSpeed;   // ground speed in km/h multiplied by 10
    uint16_t groundCourse;  // ground course in degrees multiplied by 100
    uint16_t altitude;      // altitude in m plus 1000m offset
    uint8_t  numberOfSats;  // number of satellites;

    // Attitude telemetry
    // ------------------------------------------------
    int16_t pitchAngle;     // pitch angle in rad multiplied by 10000
    int16_t rollAngle;      // roll angle in rad multiplied by 10000
    int16_t yawAngle;       // yaw angle in rad multiplied by 10000
};

#endif

/**
 * simple CRSF decoder by 
 * CopterFail 2024
*/

#ifndef MAIN_CRSF_TELEMETRIE_H_
#define MAIN_CRSF_TELEMETRIE_H_

#include "gps.h"

#define PACKED __attribute__((packed))

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

#define CRSF_NUMBER_OF_CHANNELS 16

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
private:
    /* GPS data */
    int32_t i32latitude;
    int32_t i32longitude;
    int16_t i16groundspeed;
    int16_t i16groundcourse;
    int16_t i16altitude;
    int8_t  i8numberofsats;
    bool    bGpsUpdate = false;

    /* channel data */
    //int16_t channels[16];
    struct crsf_channels_s
    {
        unsigned ch0 : 11;
        unsigned ch1 : 11;
        unsigned ch2 : 11;
        unsigned ch3 : 11;
        unsigned ch4 : 11;
        unsigned ch5 : 11;
        unsigned ch6 : 11;
        unsigned ch7 : 11;
        unsigned ch8 : 11;
        unsigned ch9 : 11;
        unsigned ch10 : 11;
        unsigned ch11 : 11;
        unsigned ch12 : 11;
        unsigned ch13 : 11;
        unsigned ch14 : 11;
        unsigned ch15 : 11;
    } PACKED; // crsf_channels_t;
    crsf_channels_s channels;

    /* altitude data */






    HardwareSerial* serialPort;

    byte incomingByte;

    unsigned long lastTimeByteReceived;
    unsigned long lastTimeDebugInfoCalculated;
    unsigned long lastTimeRcChannelsMessageWasValid;
    
    uint8_t message[CRSF_MAX_MESSAGE_LENGTH];
    
    uint8_t messageIndex;

    uint16_t messageTimeoutErrorCounter;
    uint16_t syncByteErrorCounter;
    uint16_t crcErrorCounter;
    
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


public:
    Crsf();
    ~Crsf();

    void setup( bool bsimulation, HardwareSerial* serialPort_ );
    bool loop( void );

    void update( void );
    void addByteToMessage(byte byte_);
    void resetMessage();

    static void dumpData( uint8_t* pData, size_t length);

    bool parseData( uint8_t* pData, size_t length, bool isNotify);
    inline bool checkSync( uint8_t* pData, size_t length );
    bool checkAddress( uint8_t* pData, size_t length );
    bool checkCrc( uint8_t* pData, size_t length );

    bool readGps( uint8_t* pData, size_t length );
    bool readBaro( uint8_t* pData, size_t length );
    bool readChannel( uint8_t* pData, size_t length );
    bool readLinkStatistic( uint8_t* pData, size_t length );
    bool readOtxSync( uint8_t* pData, size_t length );
    bool readRadioId( uint8_t* pData, size_t length );
    bool readVario( uint8_t* pData, size_t length );
    bool readBatterie( uint8_t* pData, size_t length );
    bool readAttitude( uint8_t* pData, size_t length );
    bool readFlightMode( uint8_t* pData, size_t length );
    int16_t getChannel( int i ){ return channels.ch15; }
    bool getAngles( int16_t &i16Pan, int16_t &i16Tilt );


    bool getLatestGps( gps &p );
};




#endif
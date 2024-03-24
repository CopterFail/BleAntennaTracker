/**
 * simple CRSF decoder by 
 * CopterFail 2024
*/

#ifndef MAIN_CRSF_TELEMETRIE_H_
#define MAIN_CRSF_TELEMETRIE_H_

#include "gps.h"

class crsf_telemetrie
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
    int16_t channels[16];

    /* altitude data */


public:
    crsf_telemetrie();
    ~crsf_telemetrie();

    static void dumpData( uint8_t* pData, size_t length);

    bool parseData( uint8_t* pData, size_t length, bool isNotify);
    bool checkTelegram( uint8_t* pData, size_t length );

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
    int16_t getChannel( int i ){ return channels[i]; }


    bool getLatestGps( gps &p );
};




#endif
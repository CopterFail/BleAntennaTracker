
/**
 * simple CRSF decoder by 
 * CopterFail 2024
*/

#include <stdio.h>

#include <Arduino.h>
#include "crsf_protocol.h"
#include "crsf_telemetrie.h"
#include "gps.h"


crsf_telemetrie::crsf_telemetrie()
{
}


crsf_telemetrie::~crsf_telemetrie()
{

}

void crsf_telemetrie::dumpData( uint8_t* pData, size_t length)
{
#if 1
    static const char hex_table[] = "0123456789ABCDEF";
    for( int i=0;i<length;i++ ){
      uint8_t c = *(pData+i);
      char c1 =  hex_table[c >> 4];
      char c2 =  hex_table[c & 0x0f];
      Serial.print(c1); 
      Serial.print(c2);
      if(i<(length-1))
        Serial.print("-"); 
    }
    Serial.println(""); 
#endif
}

bool crsf_telemetrie::parseData( uint8_t* pData, size_t length, bool isNotify)
{
    bool result = false;

    if( checkTelegram( pData, length ) == true ){
        switch( pData[CRSF_TELEMETRY_TYPE_INDEX] ){
        case CRSF_FRAMETYPE_GPS:
            result = readGps( pData, length );
            break;
        case CRSF_FRAMETYPE_BARO_ALTITUDE:
            // Baro is not received?
            result = readBaro(pData, length );
            break;
        case CRSF_FRAMETYPE_RC_CHANNELS_PACKED:
            // read rc channels, including ARM?
            result = readChannel(pData, length );
            break;

        case CRSF_FRAMETYPE_LINK_STATISTICS:
            result = readLinkStatistic(pData, length );
            break;
        case CRSF_FRAMETYPE_OPENTX_SYNC:
            result = readOtxSync(pData, length );
            break;
        case CRSF_FRAMETYPE_RADIO_ID:
            result = readRadioId(pData, length );
            break;
        case CRSF_FRAMETYPE_VARIO:
            result = readVario(pData, length );
            break;
        case CRSF_FRAMETYPE_BATTERY_SENSOR:
            result = readBatterie(pData, length );
            break;
        case CRSF_FRAMETYPE_ATTITUDE:
            result = readAttitude(pData, length );
            break;
        case CRSF_FRAMETYPE_FLIGHT_MODE:
            result = readFlightMode(pData, length );
            break;
            /* ignore */
            break;
        }

    }

    if( result == true ){
    }

    return result;
}

bool crsf_telemetrie::checkTelegram( uint8_t* pData, size_t length )
{
    bool result = false;
    if( pData[CRSF_TELEMETRY_ADDRESS_INDEX] == CRSF_ADDRESS_RADIO_TRANSMITTER ){
        result = true;
    }else{
        Serial.println("CRSF_TELEMETRY_ADDRESS_FAIL");
        result = false;
    }

    //todo: crc, length

    return result;
}

bool crsf_telemetrie::readGps( uint8_t* pData, size_t length )
{
    bool result = false;

    if( pData[CRSF_TELEMETRY_LENGTH_INDEX] == CRSF_FRAME_SIZE(CRSF_FRAME_GPS_PAYLOAD_SIZE) )
    {
        result = true;
        bGpsUpdate = true;

        i32latitude = ( int32_t(pData[3]) << 24 ) | ( int32_t(pData[4]) << 16 ) | ( int32_t(pData[5]) << 8 ) | ( int32_t(pData[6]) << 0 );
        i32longitude = ( int32_t(pData[7]) << 24 ) | ( int32_t(pData[8]) << 16 ) | ( int32_t(pData[9]) << 8 ) | ( int32_t(pData[10]) << 0 );
        i16groundspeed = ( int32_t(pData[11]) << 8 ) | ( int32_t(pData[12]) << 0 );
        i16groundcourse = ( int32_t(pData[13]) << 8 ) | ( int32_t(pData[14]) << 0 );
        i16altitude = ( int32_t(pData[15]) << 8 ) | ( int32_t(pData[16]) << 0 );
        i8numberofsats = (int8_t)pData[17];

#if 0
        Serial.print("GPS_FRAME: ");
        Serial.print( i32latitude );
        Serial.print(" ");
        Serial.print( i32longitude );
        Serial.print(" ");
        Serial.print( i16groundspeed );
        Serial.print(" ");
        Serial.print( i16groundcourse );
        Serial.print(" ");
        Serial.println( i8numberofsats );
#endif

    }
    return result;
}

bool crsf_telemetrie::readBaro( uint8_t* pData, size_t length )
{
    bool result = false;
#if 1
    Serial.print( "BARO_FRAME: " );
    dumpData( pData, length );
#endif
    return result;
}

bool crsf_telemetrie::readChannel( uint8_t* pData, size_t length )
{
    bool result = false;

#define CRSF_NUMBER_OF_CHANNELS 12
#define CRSF_RAW_CHANNEL_MIN  172
#define CRSF_RAW_CHANNEL_MAX 1812
#define CRSF_CHANNEL_MIN  988
#define CRSF_CHANNEL_MID 1500
#define CRSF_CHANNEL_MAX 2012
#define CRSF_CHANNELS_DIFFS_RATIO 0.62477120195241 // so that 172 is 988 and 1812 is 2012

    channels[0]  = ((pData[3]       | pData[4]  << 8                     ) & 0x07FF);
    channels[1]  = ((pData[4]  >> 3 | pData[5]  << 5                     ) & 0x07FF);
    channels[2]  = ((pData[5]  >> 6 | pData[6]  << 2 | pData[7] << 10  ) & 0x07FF);
    channels[3]  = ((pData[7]  >> 1 | pData[8]  << 7                     ) & 0x07FF);
    channels[4]  = ((pData[8]  >> 4 | pData[9]  << 4                     ) & 0x07FF);
    channels[5]  = ((pData[9]  >> 7 | pData[10] << 1 | pData[11] << 9  ) & 0x07FF);
    channels[6]  = ((pData[11] >> 2 | pData[12] << 6                     ) & 0x07FF);
    channels[7]  = ((pData[12] >> 5 | pData[13] << 3                     ) & 0x07FF);
    channels[8]  = ((pData[14]      | pData[15] << 8                     ) & 0x07FF);
    channels[9]  = ((pData[15] >> 3 | pData[16] << 5                     ) & 0x07FF);
    channels[10] = ((pData[16] >> 6 | pData[17] << 2 | pData[18] << 10 ) & 0x07FF);
    channels[11] = ((pData[18] >> 1 | pData[19] << 7                     ) & 0x07FF);

    for(uint8_t i=0; i<CRSF_NUMBER_OF_CHANNELS; i++)
    {
        //todo: avoid float
        channels[i] = float(CRSF_CHANNEL_MIN) + float(CRSF_CHANNELS_DIFFS_RATIO) * float(channels[i] - CRSF_RAW_CHANNEL_MIN) + 0.5;

        if(channels[i] < CRSF_CHANNEL_MIN) 
        {
            channels[i] = CRSF_CHANNEL_MIN;
        }

        if(channels[i] > CRSF_CHANNEL_MAX) 
        {
            channels[i] = CRSF_CHANNEL_MAX;
        }
    } 

#if 0
    Serial.print( "CHANNEL_FRAME: " );
    //dumpData( pData, length );
    Serial.print( String(channels[0]) + " " );
    Serial.print( String(channels[1]) + " " );
    Serial.print( String(channels[3]) + " " );
    Serial.println( String(channels[4]) + " " );
#endif
    return result;
}

bool crsf_telemetrie::readLinkStatistic( uint8_t* pData, size_t length )
{
    bool result = true;
#if 0
    Serial.print( "LINK_FRAME: " );
    dumpData( pData, length );
#endif
    return result;
}

bool crsf_telemetrie::readOtxSync( uint8_t* pData, size_t length )
{
    bool result = true;
#if 0
    Serial.print( "TXSYNC_FRAME: " );
    dumpData( pData, length );
#endif
    return result;
}

bool crsf_telemetrie::readRadioId( uint8_t* pData, size_t length )
{
    bool result = false;
#if 1
    Serial.print( "RADIO_FRAME: " );
    dumpData( pData, length );
#endif
    return result;
}

bool crsf_telemetrie::readVario( uint8_t* pData, size_t length )
{
    bool result = false;
    i16altitude = (( pData[3]<<8 | pData[4] ) & 0xFFFF);
#if 0
    Serial.print( "VARIO_FRAME: " );
    Serial.println( i16altitude );
#endif
    return result;
}

bool crsf_telemetrie::readBatterie( uint8_t* pData, size_t length )
{
    bool result = false;
#if 0
    Serial.print( "BATTERIE_FRAME: " );
    dumpData( pData, length );
#endif
    return result;
}

bool crsf_telemetrie::readAttitude( uint8_t* pData, size_t length )
{
    bool result = false;
#if 0
    Serial.print( "ATTITUDE_FRAME: " );
    dumpData( pData, length );
#endif
    return result;
}

bool crsf_telemetrie::readFlightMode( uint8_t* pData, size_t length )
{
    bool result = false;
#if 0
    Serial.print( "FMODE_FRAME: " );
    dumpData( pData, length );
#endif
    return result;
}




bool crsf_telemetrie::getLatestGps( gps &p )
{
    bool result = bGpsUpdate;
    bGpsUpdate = false;
    p.set( i32latitude,i32longitude, i8numberofsats, i16altitude );
    return result;
}

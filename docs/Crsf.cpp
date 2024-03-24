#include "Crsf.h"

Crsf::Crsf()
{
  basicInit();
  
  crc8.init(CRSF_CRC8_POLY);
}

void Crsf::init(HardwareSerial* serialPort_)
{
  serialPort = serialPort_;
}

void Crsf::update()
{
  while(serialPort->available())
  {
    addByteToMessage(serialPort->read());
    
    lastTimeByteReceived = millis();

    if(messageIndex > CRSF_MIN_MESSAGE_LENGTH && messageIndex == message[CRSF_LENGTH_BYTE_MESSAGE_INDEX]+2) {
      evaluateMessage();
      resetMessage();
    }
  }

  if (millis() - lastTimeByteReceived > CRSF_INCOMING_MESSAGE_TIMEOUT_TIME && messageIndex > 0) {
    messageTimeoutErrorCounter++;
    resetMessage();
  }

  if(millis() - lastTimeRcChannelsMessageWasValid > CRSF_CONNECTION_LOST_FAILSAFE_TIME) {
    failsafeIsActive = true;
  }
  else {
    failsafeIsActive = false;
  }

  if(millis() - lastTimeTelemetrySent > CRSF_PAUSE_BETWEEN_TELEMETRY_MESSAGES)
  {
    if(useBatterySensorTelemetry == true) {
      sendBatterySensorTelemetryMessage();
    }

    if(useGpsSensorTelemetry == true) {  
      sendGpsSensorTelemetryMessage();
    }

    if(useAttitudeSensorTelemetry == true) {
      sendAttitudeSensorTelemetryMessage();
    }

    lastTimeTelemetrySent = millis();
  }

  if(millis() - lastTimeDebugInfoCalculated >= CRSF_DELAY_BETWEEN_DEBUG_INFO_CALCULATION)
  {
    float deltaTime = float(millis() - lastTimeDebugInfoCalculated);
    
    validRcChannelsMessagesRate = float(validRcChannelsMessagesCounter) / deltaTime * 1e3;
    validLinkStatisticsMessagesRate = float(validLinkStatisticsMessagesCounter) / deltaTime * 1e3;
    validLinkStatisticsTxMessagesRate = float(validLinkStatisticsTxMessagesCounter) / deltaTime * 1e3;
    validLinkStatisticsRxMessagesRate = float(validLinkStatisticsRxMessagesCounter) / deltaTime * 1e3;
  
    validRcChannelsMessagesCounter = 0;
    validLinkStatisticsMessagesCounter = 0;
    validLinkStatisticsTxMessagesCounter = 0;
    validLinkStatisticsRxMessagesCounter = 0;

    lastTimeDebugInfoCalculated = millis();
  }
}

void Crsf::addByteToMessage(byte byte_)
{
  if (messageIndex < CRSF_MAX_MESSAGE_LENGTH - 1) {
    message[messageIndex] = byte_;
    messageIndex++;
  }
}

void Crsf::resetMessage()
{
  for(uint8_t i=0; i<CRSF_MAX_MESSAGE_LENGTH; i++) {
    message[i] = 0;
  }

  messageIndex = 0;
}

void Crsf::evaluateMessage()
{
  bool syncByteIsOk = false;
  
  bool crcIsOk = false;
  
  if(message[CRSF_SYNC_BYTE_MESSAGE_INDEX] == CRSF_SYNC_BYTE) {
    syncByteIsOk = true;
  }
  else {
    syncByteErrorCounter++;
  }

  if(crc8.calc(&message[CRSF_TYPE_BYTE_MESSAGE_INDEX], message[CRSF_LENGTH_BYTE_MESSAGE_INDEX]-1) == message[message[CRSF_LENGTH_BYTE_MESSAGE_INDEX]+1]) {
    crcIsOk = true;
  }
  else {
    crcErrorCounter++;
  }
  
  if(syncByteIsOk && crcIsOk) {
    switch (message[CRSF_TYPE_BYTE_MESSAGE_INDEX])
    {
      case CRSF_FRAMETYPE_RC_CHANNELS_PACKED:
        evaluateRcChannelsMessage();
        validRcChannelsMessagesCounter++;

        lastTimeRcChannelsMessageWasValid = millis();
      break;
  
      case CRSF_FRAMETYPE_LINK_STATISTICS:
        evaluateLinkStatisticsMessage();
        validLinkStatisticsMessagesCounter++;
      break;

      case CRSF_FRAMETYPE_LINK_STATISTICS_TX:
        evaluateLinkStatisticsTxMessage();
        validLinkStatisticsTxMessagesCounter++;
      break;

      case CRSF_FRAMETYPE_LINK_STATISTICS_RX:
        evaluateLinkStatisticsRxMessage();
        validLinkStatisticsRxMessagesCounter++;
      break;
    }
  }
}

void Crsf::evaluateRcChannelsMessage()
{
  channels[0]  = ((message[3]       | message[4]  << 8                     ) & 0x07FF);
  channels[1]  = ((message[4]  >> 3 | message[5]  << 5                     ) & 0x07FF);
  channels[2]  = ((message[5]  >> 6 | message[6]  << 2 | message[7] << 10  ) & 0x07FF);
  channels[3]  = ((message[7]  >> 1 | message[8]  << 7                     ) & 0x07FF);
  channels[4]  = ((message[8]  >> 4 | message[9]  << 4                     ) & 0x07FF);
  channels[5]  = ((message[9]  >> 7 | message[10] << 1 | message[11] << 9  ) & 0x07FF);
  channels[6]  = ((message[11] >> 2 | message[12] << 6                     ) & 0x07FF);
  channels[7]  = ((message[12] >> 5 | message[13] << 3                     ) & 0x07FF);
  channels[8]  = ((message[14]      | message[15] << 8                     ) & 0x07FF);
  channels[9]  = ((message[15] >> 3 | message[16] << 5                     ) & 0x07FF);
  channels[10] = ((message[16] >> 6 | message[17] << 2 | message[18] << 10 ) & 0x07FF);
  channels[11] = ((message[18] >> 1 | message[19] << 7                     ) & 0x07FF);

  for(uint8_t i=0; i<CRSF_NUMBER_OF_CHANNELS; i++)
  {
    channels[i] = float(CRSF_CHANNEL_MIN) + float(CRSF_CHANNELS_DIFFS_RATIO) * float(channels[i] - CRSF_RAW_CHANNEL_MIN) + 0.5;

    if(channels[i] < CRSF_CHANNEL_MIN) {
      channels[i] = CRSF_CHANNEL_MIN;
    }

    if(channels[i] > CRSF_CHANNEL_MAX) {
      channels[i] = CRSF_CHANNEL_MAX;
    }
  }
}

void Crsf::evaluateLinkStatisticsMessage()
{
  uplinkRssi1            = message[3];
  uplinkRssi2            = message[4];
  uplinkQuality          = message[5];
  uplinkSnr              = message[6];
  diversityActiveAntenna = message[7];
  rFMode                 = message[8];
  uplinkTxPower          = message[9];
  downlinkRssi           = message[10];
  downlinkQuality        = message[11];
  downlinkSnr            = message[12];
}

void Crsf::evaluateLinkStatisticsTxMessage()
{
  // TBD
}

void Crsf::evaluateLinkStatisticsRxMessage()
{
  // TBD
}


void Crsf::printMessage(HardwareSerial* serialPort_)
{
  for (uint8_t i = 0; i <= messageIndex; i++)
  {
    serialPort_->print("0x");
    serialPort_->print(message[i], HEX);
    serialPort_->print(", ");
  }
  
  serialPort_->println();
}

void Crsf::printChannels(HardwareSerial* serialPort_)
{
  for(uint8_t i=0; i<CRSF_NUMBER_OF_CHANNELS; i++)
  {
    serialPort_->print(channels[i]);
    serialPort_->print(", ");
  }

  serialPort_->println();
}

void Crsf::printLinkStatistics(HardwareSerial* serialPort_)
{
  serialPort_->print("RSSI1 = ");
  serialPort_->print(uplinkRssi1);
  serialPort_->print(", ");

  serialPort_->print("RQly = ");
  serialPort_->print(uplinkQuality);
  serialPort_->print(", ");

  serialPort_->print("RSNR = ");
  serialPort_->print(uplinkSnr);
  serialPort_->print(", ");

  serialPort_->println();
}

void Crsf::sendBatterySensorTelemetryMessage()
{
  byte buf[12];

  buf[0]  = CRSF_SYNC_BYTE;
  buf[1]  = 10;
  buf[2]  = CRSF_FRAMETYPE_BATTERY_SENSOR;
  buf[3]  = voltage >> 8;
  buf[4]  = voltage & 0xffff;
  buf[5]  = current >> 8;
  buf[6]  = current & 0xffff;
  buf[7]  = capacity >> 16;
  buf[8]  = capacity >> 8;
  buf[9]  = capacity & 0xffffff;
  buf[10] = battery;
  buf[11] = crc8.calc(&buf[2], 9);
  
  for(uint8_t i=0; i<12; i++)
  {
    serialPort->write(buf[i]);
  }
}

void Crsf::sendGpsSensorTelemetryMessage()
{
  byte buf[19];

  buf[0]  = CRSF_SYNC_BYTE;
  buf[1]  = 17;
  buf[2]  = CRSF_FRAMETYPE_GPS_SENSOR;
  buf[3]  = latitude >> 24;
  buf[4]  = latitude >> 16;
  buf[5]  = latitude >> 8;
  buf[6]  = latitude & 0xffffff;
  buf[7]  = longitude >> 24;
  buf[8]  = longitude >> 16;
  buf[9]  = longitude >> 8;
  buf[10] = longitude & 0xffffff;
  buf[11] = groundSpeed >> 8;
  buf[12] = groundSpeed & 0xffff;
  buf[13] = groundCourse >> 8;
  buf[14] = groundCourse & 0xffff;
  buf[15] = altitude >> 8;
  buf[16] = altitude & 0xffff;
  buf[17] = numberOfSats;
  buf[18] = crc8.calc(&buf[2], 16);

  for(uint8_t i=0; i<19; i++)
  {
    serialPort->write(buf[i]);
  }
}

void Crsf::sendAttitudeSensorTelemetryMessage()
{
  byte buf[10];

  buf[0] = CRSF_SYNC_BYTE;
  buf[1] = 8;
  buf[2] = CRSF_FRAMETYPE_ATTITUDE_SENSOR;
  buf[3] = pitchAngle >> 8;
  buf[4] = pitchAngle & 0xffff;
  buf[5] = rollAngle >> 8;
  buf[6] = rollAngle & 0xffff;
  buf[7] = yawAngle >> 8;
  buf[8] = yawAngle & 0xffff;
  buf[9] = crc8.calc(&buf[2], 7);

  for(uint8_t i=0; i<10; i++)
  {
    serialPort->write(buf[i]);
  }
}

void Crsf::basicInit()
{
  serialPort = NULL;

  incomingByte = 0;

  lastTimeByteReceived = 0;

  resetMessage();

  messageTimeoutErrorCounter = 0;
  syncByteErrorCounter = 0;
  crcErrorCounter = 0;

  validRcChannelsMessagesCounter = 0;
  validLinkStatisticsMessagesCounter = 0;
  validLinkStatisticsTxMessagesCounter = 0;
  validLinkStatisticsRxMessagesCounter = 0;

  validRcChannelsMessagesRate = 0;
  validLinkStatisticsMessagesRate = 0;
  validLinkStatisticsTxMessagesRate = 0;
  validLinkStatisticsRxMessagesRate = 0;
  
  for(uint8_t i=0; i<CRSF_NUMBER_OF_CHANNELS; i++)
  {
    channels[i] = CRSF_CHANNEL_MID;
  }

  lastTimeTelemetrySent = 0;
  lastTimeDebugInfoCalculated = 0;
  lastTimeRcChannelsMessageWasValid = 0;

  useBatterySensorTelemetry = true;
  useGpsSensorTelemetry = true;
  useAttitudeSensorTelemetry = true;

  failsafeIsActive = true;
  
  uplinkRssi1 = 0;
  uplinkRssi2 = 0;
  uplinkQuality = 0;
  uplinkSnr = 0;
  diversityActiveAntenna = 0;
  rFMode = 0;
  uplinkTxPower = 0;
  downlinkRssi = 0;
  downlinkQuality = 0;
  downlinkSnr = 0;
    
  voltage = 0;          
  current = 0;          
  capacity = 0;          
  battery = 0;           

  latitude = 0;       
  longitude = 0;      
  groundSpeed = 0;   
  groundCourse = 0;  
  altitude = 1000;      
  numberOfSats = 0;

  pitchAngle = 0;
  rollAngle = 0;
  yawAngle = 0;
}

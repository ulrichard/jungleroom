/***********************************************************/
//Wrapper class for the Serial MP3 Player by Catalex
#pragma once
#include <Arduino.h>

class Mp3Player
{
public:
   void begin(); 
   void PlayTitle(const uint8_t num);
private:
    void SendCommand(int8_t command, int16_t dat);

    int8_t Send_buf[8];

    static const uint8_t CMD_PLAY_W_INDEX = 0X03;
    static const uint8_t CMD_SET_VOLUME   = 0X06;
    static const uint8_t CMD_SEL_DEV      = 0X09;
    static const uint8_t DEV_TF           = 0X02;
    static const uint8_t CMD_PLAY         = 0X0D;
    static const uint8_t CMD_PAUSE        = 0X0E;
    static const uint8_t CMD_SINGLE_CYCLE = 0X19;
    static const uint8_t SINGLE_CYCLE_ON  = 0X00;
    static const uint8_t SINGLE_CYCLE_OFF = 0X01;
    static const uint8_t CMD_PLAY_W_VOL   = 0X22;
};


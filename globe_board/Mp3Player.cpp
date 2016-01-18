#include "Mp3Player.h"


void Mp3Player::begin() 
{
	Serial.begin(9600);
	delay(500); // Wait chip initialization is complete
    SendCommand(CMD_SEL_DEV, DEV_TF); //select the TF card  
	delay(200);
	SetVolume(0XE0);
}

void Mp3Player::PlayTitle(const uint8_t num)
{
    const uint16_t val = 0x0100 + num;
	SendCommand(CMD_PLAY_FILE, val);
}

void Mp3Player::SetVolume(const uint8_t num)
{
    const uint16_t val = 0x0000 + num;
	SendCommand(CMD_SET_VOLUME, val);
}

void Mp3Player::SendCommand(int8_t command, int16_t dat)
{
    delay(20);
    Send_buf[0] = 0x7e; //starting byte
    Send_buf[1] = 0xff; //version
    Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
    Send_buf[3] = command; //
    Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
    Send_buf[5] = (int8_t)(dat >> 8);//datah
    Send_buf[6] = (int8_t)(dat); //datal
    Send_buf[7] = 0xef; //ending byte

    for(uint8_t i=0; i<8; i++)//
    {
        Serial.write(Send_buf[i]) ;
    }
}


set(CMAKE_TOOLCHAIN_FILE ../../arduino-cmake/cmake/ArduinoToolchain.cmake)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(globe_board)

print_board_list()

SET(AVR_DEVICE     atmega8)
SET(AVRDUDE_PORT   /dev/ttyACM0)

generate_arduino_firmware(globe_board
    SKETCH ../globe_board
	SRCS globe_board.ino Arduino-IRremote/IRremote.cpp
	PROGRAMMER stk500v2
    PORT  ${AVRDUDE_PORT}
    BOARD ${AVR_DEVICE}
)

ADD_CUSTOM_TARGET(flash
			avrdude -P ${AVRDUDE_PORT} -p ${AVR_DEVICE} -c stk500v2 -Uflash:w:globe_board.hex
	DEPENDS globe_board
)

# for fuse settings see http://www.engbedded.com/fusecalc/
SET(LFUSE 0xbf)
SET(HFUSE 0xd9)
ADD_CUSTOM_TARGET(fuses
			avrdude -P ${AVRDUDE_PORT} -p ${AVR_DEVICE} -c stk500v2 -U lfuse:w:${LFUSE}:m -Uhfuse:w:${HFUSE}:m
)

ADD_CUSTOM_TARGET(readchip
			avrdude -P ${AVRDUDE_PORT} -p ${AVR_DEVICE} -c stk500v2 -U flash:r:dumpflash.hex:i -U eeprom:r:dumpee.hex:i -U lfuse:r:dumplfuse.hex:i -U hfuse:r:dumphfuse.hex:i
)



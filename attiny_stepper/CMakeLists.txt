

set(CMAKE_TOOLCHAIN_FILE avr_crosscompile.cmake)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(tinypi)

INCLUDE(avr_crosscompile.cmake)
IF(NOT CMAKE_CROSSCOMPILING)
#	ERROR()
ENDIF()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_EXECUTABLE(tinypi.elf
	attiny_stepper.c
	usiTwiSlave.c
)

ADD_CUSTOM_COMMAND(TARGET tinypi.elf POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom tinypi.elf tinypi.hex
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# for fuse settings see http://www.engbedded.com/fusecalc/
SET(LFUSE 0xe1)
SET(HFUSE 0xdd)
SET(EFUSE 0xff)

ADD_CUSTOM_TARGET(flash
			avrdude -P ${AVRDUDE_PORT} -p ${AVRDUDE_DEVICE} -c stk500v2 -Uflash:w:tinypi.hex -Ulfuse:w:${LFUSE}:m -Uhfuse:w:${HFUSE}:m -Uefuse:w:${EFUSE}:m
	DEPENDS tinypi.elf
)

ADD_CUSTOM_TARGET(simavr
			run_avr -m ${AVR_DEVICE} -f ${AVR_FREQUENCY} tinypi.hex
	DEPENDS tinypi.elf
)


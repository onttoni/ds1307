#pragma once

#include <Arduino.h>


class DS1307
{
public:
	char timeBuf[10];
	enum AM_PM { _AM = 0, _PM = 1, _24H = 2};
	enum Day { Mon = 1, Tue = 2, Wed = 3, Thu = 4, Fri = 5, Sat = 6, Sun = 7 };
	enum Month { Jan = 1, Feb = 2, Mar = 3, Apr = 4, May = 5, Jun = 6, Jul = 7, Aug = 8, Sep = 9, Oct = 10, Nov = 11, Dec = 12 };
	enum SQWF { SQWF_1 = 1, SQWF_4096 = 4096, SQWF_8192 = 8192, SQWF_32768 = 32768 };
	struct hours_t
	{
		uint8_t hours;
		AM_PM mode;
	};
	DS1307(bool start_clock = true);
	char* getTime(void);
	uint8_t getSeconds();
	uint8_t getMinutes();
	hours_t getHours();
	Day getDay();
	uint8_t getDate();
	Month getMonth();
	uint16_t getYear();
	bool setSeconds(uint8_t seconds);
	bool setMinutes(uint8_t minutes);
	bool setHours(hours_t hours);
	bool setDay(Day day);
	bool setDate(uint8_t date);
	bool setMonth(Month month);
	bool setYear(uint16_t year);
	bool resetTime();
	bool resetMemory();
	bool clockIsRunning();
	bool clockStart();
	bool clockStop();
	bool SQWisEnabled();
	bool enableSQW();
	bool enableSQW(SQWF freq);
	bool disableSQW();
	bool setSQWfreq(SQWF freq);
	bool writeMemory(uint8_t* data, uint8_t len, uint8_t addr = 0);
	bool readMemory(uint8_t* data, uint8_t len, uint8_t addr = 0);

protected:

private:
	static const uint8_t address = 0x68;
	bool toggleCH();
	bool toggleSQW();
	uint8_t readRegister(uint8_t reg);
	bool writeRegister(uint8_t reg, uint8_t val);
};


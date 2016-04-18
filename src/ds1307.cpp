#include <Wire.h>
#include "ds1307.h"


DS1307::DS1307(bool start_clock)
{
	Wire.begin();
	writeRegister(7, 0);
	if (start_clock)
		clockStart();

}


bool DS1307::toggleCH()
{
	uint8_t r0 = readRegister(0);
	r0 ^= 0x80;
	Wire.beginTransmission(DS1307::address);
	Wire.write(0);
	Wire.write(r0);
	if (Wire.endTransmission() > 0)
		return false;
	return true;
}


bool DS1307::toggleSQW()
{
	uint8_t r7 = readRegister(7);
	r7 ^= 0x10;
	Wire.beginTransmission(DS1307::address);
	Wire.write(7);
	Wire.write(r7);
	if (Wire.endTransmission() > 0)
		return false;
	return true;
}


bool DS1307::clockIsRunning()
{
	uint8_t r0 = readRegister(0);
	return (r0 & 0x80) == 0;
}


bool DS1307::SQWisEnabled()
{
	uint8_t r7 = readRegister(7);
	return (r7 & 0x10) > 0;
}


bool DS1307::clockStart()
{
	if (clockIsRunning())
		return true;
	return toggleCH();
}


bool DS1307::clockStop()
{
	if (clockIsRunning())
		return toggleCH();
	return true;
}


bool DS1307::enableSQW()
{
	if (SQWisEnabled())
		return true;
	return toggleSQW();
}


bool DS1307::enableSQW(SQWF freq)
{
	bool status = true;

	if (!SQWisEnabled())
		status = toggleSQW();
	return status && setSQWfreq(freq);
}


bool DS1307::disableSQW()
{
	if (SQWisEnabled())
		return toggleSQW();
	return true;
}


bool DS1307::setSQWfreq(SQWF freq)
{
	uint8_t r7 = readRegister(7);

	switch (freq)
	{
	case SQWF_1:
		return writeRegister(7, r7 & ~3);
	case SQWF_4096:
		return writeRegister(7, (r7 & ~3) | 1);
	case SQWF_8192:
		return writeRegister(7, (r7 & ~3) | 2);
	case SQWF_32768:
		return writeRegister(7, r7 | 3);
	}

	return false;
}


uint8_t DS1307::readRegister(uint8_t reg)
{
	Wire.beginTransmission(DS1307::address);
	Wire.write(reg);
	Wire.endTransmission();

	Wire.requestFrom(DS1307::address, (uint8_t) 1);
	uint8_t val = Wire.read();
	Wire.endTransmission();

	return val;
}


bool DS1307::writeRegister(uint8_t reg, uint8_t val)
{
	Wire.beginTransmission(DS1307::address);
	Wire.write(reg);
	Wire.write(val);
	if (Wire.endTransmission() > 0)
		return false;
	return true;
}


char* DS1307::getTime(void)
{
	uint8_t seconds = getSeconds();
	uint8_t minutes = getMinutes();
	hours_t hours = getHours();

	snprintf(timeBuf, 10, "%02d:%02d:%02d", hours.hours, minutes, seconds);

	return timeBuf;
}


uint8_t DS1307::getSeconds()
{
	uint8_t raw_seconds = readRegister(0) & 0x7F;
	return ((raw_seconds & 0x70) >> 4) * 10 + (raw_seconds & 0x0F);
}


uint8_t DS1307::getMinutes()
{
	uint8_t raw_minutes = readRegister(1) & 0x7F;
	return ((raw_minutes & 0x70) >> 4) * 10 + (raw_minutes & 0x0F);
}


DS1307::hours_t DS1307::getHours()
{
	uint8_t raw_hours = readRegister(2);
	hours_t hours;
	if ((raw_hours & 0x40) > 0)
	{
		// 12-hour mode is selected.
		hours.hours = ((raw_hours & 0x10) >> 4) * 10 + (raw_hours & 0x0F);
		hours.mode = ((raw_hours & 0x20) > 0) ? _PM : _AM;
	}
	else
	{
		// 24-hour mode is selected.
		hours.hours = ((raw_hours & 0x30) >> 4) * 10 + (raw_hours & 0x0F);
		hours.mode = _24H;
	}

	return hours;
}


DS1307::Day DS1307::getDay()
{
	uint8_t raw_day = readRegister(3);

	return Day(raw_day);
}


uint8_t DS1307::getDate()
{
	uint8_t raw_date = readRegister(4);

	return ((raw_date & 0x30) >> 4) * 10 + (raw_date & 0x0F);
}


DS1307::Month DS1307::getMonth()
{
	uint8_t raw_month = readRegister(5);

	return Month(((raw_month & 0x10) >> 4) * 10 + (raw_month & 0x0F));
}


uint16_t DS1307::getYear()
{
	uint8_t raw_year = readRegister(6);

	return 2000 + ((raw_year & 0xF0) >> 4) * 10 + (raw_year & 0x0F);
}


bool DS1307::setSeconds(uint8_t seconds)
{
	if (seconds > 59)
		return false;

	uint8_t raw_seconds = (seconds / 10) << 4 | seconds % 10;

	return writeRegister(0, raw_seconds);
}


bool DS1307::setMinutes(uint8_t minutes)
{

	if (minutes > 59)
		return false;

	uint8_t raw_minutes = (minutes / 10) << 4 | minutes % 10;

	return writeRegister(1, raw_minutes);
}


bool DS1307::setHours(hours_t hours)
{
	if (hours.mode == _24H && hours.hours > 23)
		return false;
	else if ((hours.mode == _AM || hours.mode == _PM) && hours.hours > 12)
		return false;

	uint8_t raw_hours = (hours.hours / 10) << 4 | hours.hours % 10;

	if (hours.mode != _24H)
	{
		// 12-hour mode is selected.
		raw_hours |= 0x40;
		raw_hours |= hours.mode == _PM ? 0x20 : 0;
	}

	return writeRegister(2, raw_hours);
}


bool DS1307::setDay(Day day)
{

	if (day > 7)
		return false;

	return writeRegister(3, day);
}


bool DS1307::setDate(uint8_t date)
{
	if (date > 31)
		return false;

	uint8_t raw_date = (date / 10) << 4 | date % 10;

	return writeRegister(4, raw_date);
}


bool DS1307::setMonth(Month month)
{
	if (month > 12)
		return false;

	uint8_t raw_month = (month / 10) << 4 | month % 10;

	return writeRegister(5, raw_month);
}


bool DS1307::setYear(uint16_t year)
{
	if (year < 2000 || year > 2099)
		return false;

	year -= 2000;

	uint8_t raw_year = (year / 10) << 4 | year % 10;

	return writeRegister(6, raw_year);
}


bool DS1307::resetTime()
{
	bool status = true;
	hours_t hours;
	hours.hours = 0;
	hours.mode = _24H;

	status &= setYear(2000);
	status &= setMonth(Jan);
	status &= setDate(1);
	status &= setDay(Sat);
	status &= setHours(hours);
	status &= setMinutes(0);
	status &= setSeconds(0);

	return status;
}


bool DS1307::writeMemory(uint8_t* data, uint8_t len, uint8_t addr)
{
	// Client shouldn't need to care about absolute data position and
	// in this library can specify first writable location as 0.
	if (addr + len > 55)
		// Data will not fit in memory.
		return false;

	// Memory address range is 08h–3Fh.
	addr += 8;

	for (uint8_t i = 0; i < len; i++)
	{
		if (writeRegister(addr + i, *(data + i) == false))
			return false;
	}

	return true;
}


bool DS1307::readMemory(uint8_t* data, uint8_t len, uint8_t addr)
{
	// Client shouldn't need to care about absolute data position and
	// in this library can specify first readable location as 0.
	if (addr + len > 55)
		// Don't have that much data in memory.
		return false;

	// Memory address range is 08h–3Fh.
	addr += 8;

	for (uint8_t i = 0; i < len; i++)
		*(data + i) = readRegister(addr + i);

	return true;
}


bool DS1307::resetMemory()
{
	bool status = true;
	uint8_t zero = 0;

	for (uint8_t i = 0; i < 56; i++)
		status &= writeMemory(&zero, 1, i);

	return status;
}


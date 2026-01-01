#include <string.h>

#include "UtcClock.h"

#include "UniTimeX.h"


UtcClock gUtcClock;


int initUtcClock(void)
{
	memset(&gUtcClock, 0, sizeof(gUtcClock));
	
	gUtcClock.edge = getUniTimeX();
	gUtcClock.width = UNITIMEX_COUNTER_FREQUENCY_HZ;
	
	return 0;
}

int updateUtcClock(void)
{
	uint32_t now;
	
	now = getUniTimeX();
	
	// Count up the UTC second
	if(now - gUtcClock.edge >= gUtcClock.width)
	{
		if(gUtcClock.LSP)
		{
			if(gUtcClock.LS)
			{
				if(gUtcClock.second % 60 == 58)
				{
					gUtcClock.second += 2;
					gUtcClock.LSP = false;
				}
				else
					gUtcClock.second += 1;
			}
			else
			{
				if(gUtcClock.second % 3600 == 0)
				{
					gUtcClock.second += 0;
					gUtcClock.LSP = false;
				}
				else
					gUtcClock.second += 1;
			}
		}
		else
			gUtcClock.second += 1;
		
		gUtcClock.edge += gUtcClock.width;
	}
	
	// Quality reducing
	if(gUtcClock.quality > UtcQualityPoor)
	{
		if(now - gUtcClock.adjustTime > UNITIMEX_COUNTER_FREQUENCY_HZ * 3)
			gUtcClock.quality = UtcQualityPoor;
	}
	else if(gUtcClock.quality > UtcQualityMiss)
	{
		if(now - gUtcClock.adjustTime > UNITIMEX_COUNTER_FREQUENCY_HZ * 10)
			gUtcClock.quality = UtcQualityMiss;
	}
	else if(gUtcClock.quality > UtcQualityLost)
	{
		if(now - gUtcClock.adjustTime > UNITIMEX_COUNTER_FREQUENCY_HZ * 20)
			gUtcClock.quality = UtcQualityLost;
	}
	
	return 0;
}

int getUtcStamp(UtcTimeStamp* timeStamp)
{
	uint32_t now, elapse;
	
	uint32_t second;
	uint32_t edge;
	uint32_t width;
	
	bool LSP, LS, leap;
	UtcQuality quality;
	
	// Interruptable locking
	do {
		second = gUtcClock.second;
		edge = gUtcClock.edge;
		
		LSP = gUtcClock.LSP;
		
		LS = gUtcClock.LS;
		width = gUtcClock.width;
		quality = gUtcClock.quality;
		
	} while((second != gUtcClock.second) || (edge != gUtcClock.edge) || (LSP != gUtcClock.LSP) || (LS != gUtcClock.LS) || (width != gUtcClock.width) || (quality != gUtcClock.quality));
	
	now = getUniTimeX();
	
	// Count up the UTC second
	leap = false; if(now - edge >= width)
	{
		if(LSP)
		{
			if(LS)
			{
				if(second % 60 == 58)
					second += 2;
				else
					second += 1;
			}
			else
			{
				if(second % 3600 == 0)
					second += 0;
				else if(second % 60 == 59)
				{
					second += 1;
					leap = true;
				}
				else
					second += 1;
			}
		}
		else
			second += 1;
		
		elapse = now - edge - width;
	}
	else
	{
		if(LSP && !LS)
		{
			if(second % 3600 == 0)
				leap = true;
		}
		
		elapse = now - edge;
	}
	
	timeStamp->second = second;
	timeStamp->ms = (uint64_t)elapse * 1000 * 1000 / width / 1000;
	timeStamp->us = (uint64_t)elapse * 1000 * 1000 / width % 1000;
	timeStamp->leap = leap;
	timeStamp->quality = quality;
	
	return 0;
}

int toDateStamp(const UtcTimeStamp* timeStamp, UtcDateStamp* dateStamp, signed timeZoneOffsetMinutes)
{
	uint64_t dateSeconds;
	uint32_t febSeconds;
	
	// 总秒数
	dateSeconds = timeStamp->second;
	
	// 时区偏移
	dateSeconds += timeZoneOffsetMinutes * 60;
	
	// 不计闰秒 防止日期进位
	if(timeStamp->leap)
		dateSeconds --;
	
	// 年
	if(dateSeconds >= UTC_SECONDS_AT_2000)
	{
		dateStamp->year = 2000;
		dateSeconds -= UTC_SECONDS_AT_2000;
		
		while(dateSeconds >= UTC_SECONDS_PER_400_YEAR - UTC_SECONDS_PER_1_DAY)
		{
			if(isLeapYear(dateStamp->year))
			{
				if(dateSeconds >= UTC_SECONDS_PER_400_YEAR)
				{
					dateStamp->year += 400;
					dateSeconds -= UTC_SECONDS_PER_400_YEAR;
				}
				else
					break;
			}
			else
			{
				dateStamp->year += 400;
				dateSeconds -= UTC_SECONDS_PER_400_YEAR - UTC_SECONDS_PER_1_DAY;
			}
		}
		
		while(dateSeconds >= UTC_SECONDS_PER_100_YEAR)
		{
			if(isLeapYear(dateStamp->year))
			{
				if(dateSeconds >= UTC_SECONDS_PER_100_YEAR + UTC_SECONDS_PER_1_DAY)
				{
					dateStamp->year += 100;
					dateSeconds -= UTC_SECONDS_PER_100_YEAR + UTC_SECONDS_PER_1_DAY;
				}
				else
					break;
			}
			else
			{
				dateStamp->year += 100;
				dateSeconds -= UTC_SECONDS_PER_100_YEAR;
			}
		}
		
		while(dateSeconds >= UTC_SECONDS_PER_4_YEAR - UTC_SECONDS_PER_1_DAY)
		{
			if(isLeapYear(dateStamp->year))
			{
				if(dateSeconds >= UTC_SECONDS_PER_4_YEAR)
				{
					dateStamp->year += 4;
					dateSeconds -= UTC_SECONDS_PER_4_YEAR;
				}
				else
					break;
			}
			else
			{
				dateStamp->year += 4;
				dateSeconds -= UTC_SECONDS_PER_4_YEAR - UTC_SECONDS_PER_1_DAY;
			}
		}
		
		while(dateSeconds >= UTC_SECONDS_PER_1_DAY * 365)
		{
			if(isLeapYear(dateStamp->year))
			{
				if(dateSeconds >= UTC_SECONDS_PER_1_DAY * 366)
				{
					dateStamp->year ++;
					dateSeconds -= UTC_SECONDS_PER_1_DAY * 366;
				}
				else
					break;
			}
			else
			{
				dateStamp->year ++;
				dateSeconds -= UTC_SECONDS_PER_1_DAY * 365;
			}
		}
	}
	else
	{
		dateStamp->year = 1970;
		
		while(dateSeconds >= UTC_SECONDS_PER_1_DAY * 365)
		{
			if(isLeapYear(dateStamp->year))
			{
				if(dateSeconds >= UTC_SECONDS_PER_1_DAY * 366)
				{
					dateStamp->year ++;
					dateSeconds -= UTC_SECONDS_PER_1_DAY * 366;
				}
				else
					break;
			}
			else
			{
				dateStamp->year ++;
				dateSeconds -= UTC_SECONDS_PER_1_DAY * 365;
			}
		}
	}
	
	// 一月
	dateStamp->month = 1;
	if(dateSeconds >= UTC_SECONDS_PER_1_DAY * 31)
	{
		// 二月
		dateStamp->month ++;
		dateSeconds -= UTC_SECONDS_PER_1_DAY * 31;
		
		// 平年或闰年
		if(isLeapYear(dateStamp->year))
			febSeconds = UTC_SECONDS_PER_1_DAY * 29;
		else
			febSeconds = UTC_SECONDS_PER_1_DAY * 28;
		
		if(dateSeconds >= febSeconds)
		{
			// 三月
			dateStamp->month ++;
			dateSeconds -= febSeconds;
			
			if(dateSeconds >= UTC_SECONDS_PER_1_DAY * 31)
			{
				// 四月
				dateStamp->month ++;
				dateSeconds -= UTC_SECONDS_PER_1_DAY * 31;
				
				if(dateSeconds >= UTC_SECONDS_PER_1_DAY * 30)
				{
					// 五月
					dateStamp->month ++;
					dateSeconds -= UTC_SECONDS_PER_1_DAY * 30;
					
					if(dateSeconds >= UTC_SECONDS_PER_1_DAY * 31)
					{
						// 六月
						dateStamp->month ++;
						dateSeconds -= UTC_SECONDS_PER_1_DAY * 31;
						
						if(dateSeconds >= UTC_SECONDS_PER_1_DAY * 30)
						{
							// 七月
							dateStamp->month ++;
							dateSeconds -= UTC_SECONDS_PER_1_DAY * 30;
							
							if(dateSeconds >= UTC_SECONDS_PER_1_DAY * 31)
							{
								// 八月
								dateStamp->month ++;
								dateSeconds -= UTC_SECONDS_PER_1_DAY * 31;
								
								if(dateSeconds >= UTC_SECONDS_PER_1_DAY * 31)
								{
									// 九月
									dateStamp->month ++;
									dateSeconds -= UTC_SECONDS_PER_1_DAY * 31;
									
									if(dateSeconds >= UTC_SECONDS_PER_1_DAY * 30)
									{
										// 十月
										dateStamp->month ++;
										dateSeconds -= UTC_SECONDS_PER_1_DAY * 30;
										
										if(dateSeconds >= UTC_SECONDS_PER_1_DAY * 31)
										{
											// 十一月
											dateStamp->month ++;
											dateSeconds -= UTC_SECONDS_PER_1_DAY * 31;
											
											if(dateSeconds >= UTC_SECONDS_PER_1_DAY * 30)
											{
												// 十二月
												dateStamp->month ++;
												dateSeconds -= UTC_SECONDS_PER_1_DAY * 30;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	// 日
	dateStamp->day = 1;
	dateStamp->day += dateSeconds / UTC_SECONDS_PER_1_DAY;
	dateSeconds = dateSeconds % UTC_SECONDS_PER_1_DAY;
	
	// 时
	dateStamp->hour = dateSeconds / 3600;
	dateSeconds = dateSeconds % 3600;
	
	// 分
	dateStamp->minute = dateSeconds / 60;
	dateSeconds = dateSeconds % 60;
	
	// 秒
	if(timeStamp->leap)
		dateSeconds ++;
	dateStamp->second = dateSeconds;
	
	// 分秒
	dateStamp->ms = timeStamp->ms;
	dateStamp->us = timeStamp->us;
	
	// 品质
	dateStamp->quality = timeStamp->quality;
	
	return 0;
}

int toTimeStamp(const UtcDateStamp* dateStamp, signed timeZoneOffsetMinutes, UtcTimeStamp* timeStamp)
{
	uint16_t year;
	uint8_t month;
	
	// 年
	if(dateStamp->year >= 2000)
	{
		timeStamp->second = UTC_SECONDS_AT_2000;
		year = 2000;
		
		while(dateStamp->year >= year + 400)
		{
			if(isLeapYear(year))
				timeStamp->second += UTC_SECONDS_PER_400_YEAR;
			else
				timeStamp->second += UTC_SECONDS_PER_400_YEAR - UTC_SECONDS_PER_1_DAY;
			
			year += 400;
		}
		
		while(dateStamp->year >= year + 100)
		{
			if(isLeapYear(year))
				timeStamp->second += UTC_SECONDS_PER_100_YEAR + UTC_SECONDS_PER_1_DAY;
			else
				timeStamp->second += UTC_SECONDS_PER_100_YEAR;
			
			year += 100;
		}
		
		while(dateStamp->year >= year + 4)
		{
			if(isLeapYear(year))
				timeStamp->second += UTC_SECONDS_PER_4_YEAR;
			else
				timeStamp->second += UTC_SECONDS_PER_4_YEAR - UTC_SECONDS_PER_1_DAY;
			
			year += 4;
		}
		
		while(dateStamp->year > year)
		{
			if(isLeapYear(year))
				timeStamp->second += UTC_SECONDS_PER_1_DAY * 366;
			else
				timeStamp->second += UTC_SECONDS_PER_1_DAY * 365;
			
			year += 1;
		}
	}
	else
	{
		timeStamp->second = 0;
		year = 1970;
		
		while(dateStamp->year > year)
		{
			if(isLeapYear(year))
				timeStamp->second += UTC_SECONDS_PER_1_DAY * 366;
			else
				timeStamp->second += UTC_SECONDS_PER_1_DAY * 365;
			
			year += 1;
		}
	}
	
	// 一月
	month = 1;
	if(dateStamp->month > month)
	{
		timeStamp->second += UTC_SECONDS_PER_1_DAY * 31;
		month += 1;
		
		// 二月
		if(dateStamp->month > month)
		{
			if(isLeapYear(dateStamp->year))
				timeStamp->second += UTC_SECONDS_PER_1_DAY * 29;
			else
				timeStamp->second += UTC_SECONDS_PER_1_DAY * 28;
			month += 1;
			
			// 三月
			if(dateStamp->month > month)
			{
				timeStamp->second += UTC_SECONDS_PER_1_DAY * 31;
				month += 1;
				
				// 四月
				if(dateStamp->month > month)
				{
					timeStamp->second += UTC_SECONDS_PER_1_DAY * 30;
					month += 1;
				
					// 五月
					if(dateStamp->month > month)
					{
						timeStamp->second += UTC_SECONDS_PER_1_DAY * 31;
						month += 1;
						
						// 六月
						if(dateStamp->month > month)
						{
							timeStamp->second += UTC_SECONDS_PER_1_DAY * 30;
							month += 1;
							
							// 七月
							if(dateStamp->month > month)
							{
								timeStamp->second += UTC_SECONDS_PER_1_DAY * 31;
								month += 1;
								
								// 八月
								if(dateStamp->month > month)
								{
									timeStamp->second += UTC_SECONDS_PER_1_DAY * 31;
									month += 1;
									
									// 九月
									if(dateStamp->month > month)
									{
										timeStamp->second += UTC_SECONDS_PER_1_DAY * 30;
										month += 1;
										
										// 十月
										if(dateStamp->month > month)
										{
											timeStamp->second += UTC_SECONDS_PER_1_DAY * 31;
											month += 1;
											
											// 十一月
											if(dateStamp->month > month)
												timeStamp->second += UTC_SECONDS_PER_1_DAY * 30;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	// 日
	timeStamp->second += (dateStamp->day - 1) * UTC_SECONDS_PER_1_DAY;
	
	// 时
	timeStamp->second += dateStamp->hour * 3600;
	
	// 分
	timeStamp->second += dateStamp->minute * 60;
	
	// 秒
	timeStamp->second += dateStamp->second;
	
	if(dateStamp->second > 59)
		timeStamp->leap = true;
	else
		timeStamp->leap = false;
	
	// 分秒
	timeStamp->ms = dateStamp->ms;
	timeStamp->us = dateStamp->us;
	
	// 时区偏移
	timeStamp->second -= timeZoneOffsetMinutes * 60;
	
	// 品质
	timeStamp->quality = dateStamp->quality;
	
	return 0;
}

#ifndef HAL_RTC_H
#define HAL_RTC_H

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

int rtc_init(void);

/* interrupt flags */
#define RTC_IRQF	0x80				/* Any of the following is active */
#define RTC_PF		0x40				/* Periodic interrupt */
#define RTC_AF		0x20				/* Alarm interrupt */
#define RTC_UF		0x10				/* Update interrupt for 1Hz RTC */
#define RTC_MAX_FREQ	8192

#endif

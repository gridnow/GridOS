#include <lock.h>
#include <rtc.h>
#include <irq.h>
#include <errno.h>
#include <param.h>
#include <tick.h>
#include <bcd.h>

#include <ddk/compatible.h>
#include <ddk/irq.h>

#include <mc146818rtc.h>

static DEFINE_SPINLOCK(rtc_lock);
static unsigned long rtc_port;
static int rtc_irq;

static unsigned long rtc_status;	/* bitmapped status byte.	*/
static unsigned long rtc_freq;		/* Current periodic IRQ rate	*/
static unsigned long rtc_irq_data;	/* our output to the world	*/
static unsigned long rtc_max_user_freq = 64; /* > this, need CAP_SYS_RESOURCE */

#ifdef	CONFIG_HPET_EMULATE_RTC
#undef	RTC_IRQ
#endif

#ifdef RTC_IRQ
static int rtc_has_irq = 1;
#endif

#ifndef CONFIG_HPET_EMULATE_RTC
#define is_hpet_enabled()			0
#define hpet_set_alarm_time(hrs, min, sec)	0
#define hpet_set_periodic_freq(arg)		0
#define hpet_mask_rtc_irq_bit(arg)		0
#define hpet_set_rtc_irq_bit(arg)		0
#define hpet_rtc_timer_init()			do { } while (0)
#define hpet_rtc_dropped_irq()			0
#define hpet_register_irq_handler(h)		({ 0; })
#define hpet_unregister_irq_handler(h)		({ 0; })
#ifdef RTC_IRQ
static irqreturn_t hpet_rtc_interrupt(int irq, void *dev_id)
{
	return 0;
}
#endif
#endif

static unsigned long epoch = 1900;	/* year corresponding to 0x00	*/
static const unsigned char days_in_mo[] =
{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/*
 *	Bits in rtc_status. (6 bits of room for future expansion)
 */
#define RTC_IS_OPEN		0x01	/* means /dev/rtc is in use	*/
#define RTC_TIMER_ON		0x02	/* missed irq timer active	*/

#ifdef RTC_IRQ
static void set_rtc_irq_bit_locked(unsigned char bit);
static void mask_rtc_irq_bit_locked(unsigned char bit);
#endif

/*
 * Returns true if a clock update is in progress
 */
static inline unsigned char rtc_is_updating(void)
{
	unsigned long flags;
	unsigned char uip;

	spin_lock_irqsave(&rtc_lock, flags);
	uip = (CMOS_READ(RTC_FREQ_SELECT) & RTC_UIP);
	spin_unlock_irqrestore(&rtc_lock, flags);
	return uip;
}
#ifdef RTC_IRQ
/*
 *	A very tiny interrupt handler. It runs with IRQF_DISABLED set,
 *	but there is possibility of conflicting with the set_rtc_mmss()
 *	call (the rtc irq and the timer irq can easily run at the same
 *	time in two different CPUs). So we need to serialize
 *	accesses to the chip with the rtc_lock spinlock that each
 *	architecture should implement in the timer code.
 *	(See ./arch/XXXX/kernel/time.c for the set_rtc_mmss() function.)
 */

static irqreturn_t rtc_interrupt(int irq, void *dev_id)
{
	/*
	 *	Can be an alarm interrupt, update complete interrupt,
	 *	or a periodic interrupt. We store the status in the
	 *	low byte and the number of interrupts received since
	 *	the last read in the remainder of rtc_irq_data.
	 */
	spin_lock(&rtc_lock);
	rtc_irq_data += 0x100;
	rtc_irq_data &= ~0xff;
	if (is_hpet_enabled()) {
		/*
		 * In this case it is HPET RTC interrupt handler
		 * calling us, with the interrupt information
		 * passed as arg1, instead of irq.
		 */
		rtc_irq_data |= (unsigned long)irq & 0xF0;
	} else {
		rtc_irq_data |= (CMOS_READ(RTC_INTR_FLAGS) & 0xF0);
	}

	/* RTC 可能丢中断而导致RTC停止工作 ,TODO: Use the soft timer to fix it. */
#if 0
	if (rtc_status & RTC_TIMER_ON)
		mod_timer(&rtc_irq_timer, jiffies + HZ/rtc_freq + 2*HZ/100);
#endif
	spin_unlock(&rtc_lock);

	return IRQ_HANDLED;
}
#endif
int rtc_init(void)
{
	unsigned int year, ctrl;
	unsigned long flags;
	char *guess = NULL;
	rtc_port = RTC_PORT(0);
	rtc_irq = RTC_IRQ;

#ifdef RTC_IRQ
	irq_handler_t rtc_int_handler_ptr;
#endif

#ifdef RTC_IRQ
	if (is_hpet_enabled()) {
		int err;

		rtc_int_handler_ptr = hpet_rtc_interrupt;
		err = hpet_register_irq_handler(rtc_interrupt);
		if (err != 0) {
			return err;
		}
	} else {
		rtc_int_handler_ptr = rtc_interrupt;
	}

	if (request_irq(RTC_IRQ, rtc_int_handler_ptr, IRQF_DISABLED,
		"rtc", NULL)) {
			/* Yeah right, seeing as irq 8 doesn't even hit the bus. */
			rtc_has_irq = 0;
			return -EIO;
	}
	hpet_rtc_timer_init();

#endif


#if defined(__alpha__) || defined(__mips__)
	rtc_freq = HZ;

	/* Each operating system on an Alpha uses its own epoch.
	   Let's try to guess which one we are using now. */

	if (rtc_is_updating() != 0)
		msleep(20);

	spin_lock_irq(&rtc_lock);
	year = CMOS_READ(RTC_YEAR);
	ctrl = CMOS_READ(RTC_CONTROL);
	spin_unlock_irq(&rtc_lock);

	if (!(ctrl & RTC_DM_BINARY) || RTC_ALWAYS_BCD)
		year = bcd2bin(year);       /* This should never happen... */

	if (year < 20) {
		epoch = 2000;
		guess = "SRM (post-2000)";
	} else if (year >= 20 && year < 48) {
		epoch = 1980;
		guess = "ARC console";
	} else if (year >= 48 && year < 72) {
		epoch = 1952;
		guess = "Digital UNIX";
#if defined(__mips__)
	} else if (year >= 72 && year < 74) {
		epoch = 2000;
		guess = "Digital DECstation";
#else
	} else if (year >= 70) {
		epoch = 1900;
		guess = "Standard PC (1900)";
#endif
	}
	if (guess)
		printk(KERN_INFO "rtc: %s epoch (%lu) detected\n",
			guess, epoch);
#endif
#ifdef RTC_IRQ
	if (rtc_has_irq == 0)
		goto no_irq2;

	spin_lock_irq(&rtc_lock);
	rtc_freq = 1024;
	if (!hpet_set_periodic_freq(rtc_freq)) {
		/*
		 * Initialize periodic frequency to CMOS reset default,
		 * which is 1024Hz
		 */
		CMOS_WRITE(((CMOS_READ(RTC_FREQ_SELECT) & 0xF0) | 0x06),
			   RTC_FREQ_SELECT);
	}
	spin_unlock_irq(&rtc_lock);
no_irq2:
#endif

	/*
		开启周期性中断
	*/
	do {
		unsigned long arg  = HZ;
		int tmp = 0;
		unsigned char val;

		while (arg > (1<<tmp))
			tmp++;

		rtc_freq = arg;


		val = CMOS_READ(RTC_FREQ_SELECT) & 0xf0;
		val |= (16 - tmp);
		CMOS_WRITE(val, RTC_FREQ_SELECT);

		spin_lock_irqsave(&rtc_lock, flags);
		set_rtc_irq_bit_locked(RTC_PIE);
		spin_unlock_irqrestore(&rtc_lock, flags);
	} while (0);


	return 0;
}
void rtc_get_rtc_time(struct rtc_time *rtc_tm)
{
	unsigned long uip_watchdog = jiffies, flags;
	unsigned char ctrl;
#ifdef CONFIG_MACH_DECSTATION
	unsigned int real_year;
#endif

	/*
	 * read RTC once any update in progress is done. The update
	 * can take just over 2ms. We wait 20ms. There is no need to
	 * to poll-wait (up to 1s - eeccch) for the falling edge of RTC_UIP.
	 * If you need to know *exactly* when a second has started, enable
	 * periodic update complete interrupts, (via ioctl) and then
	 * immediately read /dev/rtc which will block until you get the IRQ.
	 * Once the read clears, read the RTC time (again via ioctl). Easy.
	 */

	while (rtc_is_updating() != 0 &&
	       time_before(jiffies, uip_watchdog + 2*HZ/100))
		cpu_relax();

	/*
	 * Only the values that we read from the RTC are set. We leave
	 * tm_wday, tm_yday and tm_isdst untouched. Note that while the
	 * RTC has RTC_DAY_OF_WEEK, we should usually ignore it, as it is
	 * only updated by the RTC when initially set to a non-zero value.
	 */
	spin_lock_irqsave(&rtc_lock, flags);
	rtc_tm->tm_sec = CMOS_READ(RTC_SECONDS);
	rtc_tm->tm_min = CMOS_READ(RTC_MINUTES);
	rtc_tm->tm_hour = CMOS_READ(RTC_HOURS);
	rtc_tm->tm_mday = CMOS_READ(RTC_DAY_OF_MONTH);
	rtc_tm->tm_mon = CMOS_READ(RTC_MONTH);
	rtc_tm->tm_year = CMOS_READ(RTC_YEAR);
	/* Only set from 2.6.16 onwards */
	rtc_tm->tm_wday = CMOS_READ(RTC_DAY_OF_WEEK);

#ifdef CONFIG_MACH_DECSTATION
	real_year = CMOS_READ(RTC_DEC_YEAR);
#endif
	ctrl = CMOS_READ(RTC_CONTROL);
	spin_unlock_irqrestore(&rtc_lock, flags);

	if (!(ctrl & RTC_DM_BINARY) || RTC_ALWAYS_BCD) {
		rtc_tm->tm_sec = bcd2bin(rtc_tm->tm_sec);
		rtc_tm->tm_min = bcd2bin(rtc_tm->tm_min);
		rtc_tm->tm_hour = bcd2bin(rtc_tm->tm_hour);
		rtc_tm->tm_mday = bcd2bin(rtc_tm->tm_mday);
		rtc_tm->tm_mon = bcd2bin(rtc_tm->tm_mon);
		rtc_tm->tm_year = bcd2bin(rtc_tm->tm_year);
		rtc_tm->tm_wday = bcd2bin(rtc_tm->tm_wday);
	}

#ifdef CONFIG_MACH_DECSTATION
	rtc_tm->tm_year += real_year - 72;
#endif

	/*
	 * Account for differences between how the RTC uses the values
	 * and how they are defined in a struct rtc_time;
	 */
	rtc_tm->tm_year += epoch - 1900;
	if (rtc_tm->tm_year <= 69)
		rtc_tm->tm_year += 100;

	rtc_tm->tm_mon--;
}

void get_rtc_alm_time(struct rtc_time *alm_tm)
{
	unsigned char ctrl;
	unsigned long flags;
	/*
	 * Only the values that we read from the RTC are set. That
	 * means only tm_hour, tm_min, and tm_sec.
	 */
	spin_lock_irqsave(&rtc_lock,flags);
	alm_tm->tm_sec = CMOS_READ(RTC_SECONDS_ALARM);
	alm_tm->tm_min = CMOS_READ(RTC_MINUTES_ALARM);
	alm_tm->tm_hour = CMOS_READ(RTC_HOURS_ALARM);
	ctrl = CMOS_READ(RTC_CONTROL);
	spin_unlock_irqrestore(&rtc_lock,flags);

	if (!(ctrl & RTC_DM_BINARY) || RTC_ALWAYS_BCD) {
		alm_tm->tm_sec = bcd2bin(alm_tm->tm_sec);
		alm_tm->tm_min = bcd2bin(alm_tm->tm_min);
		alm_tm->tm_hour = bcd2bin(alm_tm->tm_hour);
	}
}

#ifdef RTC_IRQ
/*
 * Used to disable/enable interrupts for any one of UIE, AIE, PIE.
 * Rumour has it that if you frob the interrupt enable/disable
 * bits in RTC_CONTROL, you should read RTC_INTR_FLAGS, to
 * ensure you actually start getting interrupts. Probably for
 * compatibility with older/broken chipset RTC implementations.
 * We also clear out any old irq data after an ioctl() that
 * meddles with the interrupt enable/disable bits.
 */

static void mask_rtc_irq_bit_locked(unsigned char bit)
{
	unsigned char val;

	if (hpet_mask_rtc_irq_bit(bit))
		return;
	val = CMOS_READ(RTC_CONTROL);
	val &=  ~bit;
	CMOS_WRITE(val, RTC_CONTROL);
	CMOS_READ(RTC_INTR_FLAGS);

	rtc_irq_data = 0;
}

static void set_rtc_irq_bit_locked(unsigned char bit)
{
	unsigned char val;
	if (hpet_set_rtc_irq_bit(bit))
		return;
	val = CMOS_READ(RTC_CONTROL);
	val |= bit;
	CMOS_WRITE(val, RTC_CONTROL);
	CMOS_READ(RTC_INTR_FLAGS);

	rtc_irq_data = 0;
}
#endif
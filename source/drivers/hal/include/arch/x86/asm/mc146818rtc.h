/*
 * Machine dependent access functions for RTC registers.
 */
#ifndef _ASM_X86_MC146818RTC_H
#define _ASM_X86_MC146818RTC_H

#include <asm/io.h>
#include <asm/processor.h>
#include <mc146818rtc.h>

#ifndef RTC_PORT
#define RTC_PORT(x)	(0x70 + (x))
#define RTC_ALWAYS_BCD	1	/* RTC operates in binary mode */
#endif

#define lock_cmos_prefix(reg) do {} while (0)
#define lock_cmos_suffix(reg) do {} while (0)
#define lock_cmos(reg) do { } while (0)
#define unlock_cmos() do { } while (0)
#define do_i_have_lock_cmos() 0
#define current_lock_cmos_reg() 0

/*
 * The yet supported machines all access the RTC index register via
 * an ISA port access but the way to access the date register differs ...
 */
#define CMOS_READ(addr) rtc_cmos_read(addr)
#define CMOS_WRITE(val, addr) rtc_cmos_write(val, addr)
unsigned char rtc_cmos_read(unsigned char addr);
void rtc_cmos_write(unsigned char val, unsigned char addr);

extern int mach_set_rtc_mmss(unsigned long nowtime);
extern unsigned long mach_get_cmos_time(void);

#define RTC_IRQ 8

#endif /* _ASM_X86_MC146818RTC_H */


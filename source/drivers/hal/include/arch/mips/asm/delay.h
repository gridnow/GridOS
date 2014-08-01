#ifndef _ASM_DELAY_H
#define _ASM_DELAY_H

extern void __delay(unsigned long loops);
extern void __ndelay(unsigned long ns);
extern void __udelay(unsigned long us);

#define ndelay(ns) __ndelay(ns)
#define udelay(us) __udelay(us)

/* make sure "usecs *= ..." in udelay do not overflow. */
#if HZ >= 1000
#define MAX_UDELAY_MS	1
#elif HZ <= 200
#define MAX_UDELAY_MS	5
#else
#define MAX_UDELAY_MS	(1000 / HZ)
#endif

#endif /* _ASM_DELAY_H */

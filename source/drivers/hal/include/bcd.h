#ifndef HAL_BCD_H
#define HAL_BCD_H

#define bcd2bin(x)	const_bcd2bin(x)
#define bin2bcd(x)	const_bin2bcd(x)

#define const_bcd2bin(x)	(((x) & 0x0f) + ((x) >> 4) * 10)
#define const_bin2bcd(x)	((((x) / 10) << 4) + (x) % 10)

#endif /* HAL_BCD_H */

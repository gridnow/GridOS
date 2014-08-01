#include <types.h>

#include <asm/processor.h>
#include <asm/watch.h>

__cpuinit void mips_probe_watch_registers(struct cpuinfo_mips *c)
{
	unsigned int t;

	if ((c->options & MIPS_CPU_WATCH) == 0)
		return;
	/*
	 * Check which of the I,R and W bits are supported, then
	 * disable the register.
	 */
	write_c0_watchlo0(7);
	t = read_c0_watchlo0();
	write_c0_watchlo0(0);
	c->watch_reg_masks[0] = t & 7;

	/* Write the mask bits and read them back to determine which
	 * can be used. */
	c->watch_reg_count = 1;
	c->watch_reg_use_cnt = 1;
	t = read_c0_watchhi0();
	write_c0_watchhi0(t | 0xff8);
	t = read_c0_watchhi0();
	c->watch_reg_masks[0] |= (t & 0xff8);
	if ((t & 0x80000000) == 0)
		return;

	write_c0_watchlo1(7);
	t = read_c0_watchlo1();
	write_c0_watchlo1(0);
	c->watch_reg_masks[1] = t & 7;

	c->watch_reg_count = 2;
	c->watch_reg_use_cnt = 2;
	t = read_c0_watchhi1();
	write_c0_watchhi1(t | 0xff8);
	t = read_c0_watchhi1();
	c->watch_reg_masks[1] |= (t & 0xff8);
	if ((t & 0x80000000) == 0)
		return;

	write_c0_watchlo2(7);
	t = read_c0_watchlo2();
	write_c0_watchlo2(0);
	c->watch_reg_masks[2] = t & 7;

	c->watch_reg_count = 3;
	c->watch_reg_use_cnt = 3;
	t = read_c0_watchhi2();
	write_c0_watchhi2(t | 0xff8);
	t = read_c0_watchhi2();
	c->watch_reg_masks[2] |= (t & 0xff8);
	if ((t & 0x80000000) == 0)
		return;

	write_c0_watchlo3(7);
	t = read_c0_watchlo3();
	write_c0_watchlo3(0);
	c->watch_reg_masks[3] = t & 7;

	c->watch_reg_count = 4;
	c->watch_reg_use_cnt = 4;
	t = read_c0_watchhi3();
	write_c0_watchhi3(t | 0xff8);
	t = read_c0_watchhi3();
	c->watch_reg_masks[3] |= (t & 0xff8);
	if ((t & 0x80000000) == 0)
		return;

	/* We use at most 4, but probe and report up to 8. */
	c->watch_reg_count = 5;
	t = read_c0_watchhi4();
	if ((t & 0x80000000) == 0)
		return;

	c->watch_reg_count = 6;
	t = read_c0_watchhi5();
	if ((t & 0x80000000) == 0)
		return;

	c->watch_reg_count = 7;
	t = read_c0_watchhi6();
	if ((t & 0x80000000) == 0)
		return;

	c->watch_reg_count = 8;
}

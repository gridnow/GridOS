/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   MIPS 平台异常处理
 *
 */
#include <ddk/debug.h>

#include <compiler.h>
#include <linkage.h>
#include <irqflags.h>
#include <asm/ptrace.h>
#include <asm/mipsregs.h>

#include <arch/page.h>

#define MIPS_EXCPEITON_OTHER_COUNT 32
unsigned long exception_handlers[MIPS_EXCPEITON_OTHER_COUNT];

extern asmlinkage void handle_int(void);
extern asmlinkage void handle_tlbm(void);
extern asmlinkage void handle_tlbl(void);
extern asmlinkage void handle_tlbs(void);

extern asmlinkage void handle_adel(void);
extern asmlinkage void handle_ades(void);

extern asmlinkage void handle_sys(void);

extern asmlinkage void handle_unknown(void);
static void setup_exception_other(void *func, int num)
{
	if (num > MIPS_EXCPEITON_OTHER_COUNT)
		return;
	exception_handlers[num] = (unsigned long)func;
}

static void print_registers(struct pt_regs *regs)
{
	  	printk("Panic status %p, cause %p, epc %p,bd %p, entry hi %x\n", 
	  		regs->cp0_status,
	  		regs->cp0_cause,
	  		regs->cp0_epc,
	  		read_c0_badvaddr(),
	 		read_c0_entryhi());
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
asmlinkage void __init do_daddi_ov(struct pt_regs *regs)
{
	hal_panic("TODO");
}

asmlinkage void do_reserved(struct pt_regs *regs)
{
	hal_panic("TODO");
}

asmlinkage void do_tlbm(struct pt_regs *regs)
{
	do_page_fault(regs, 1, read_c0_badvaddr());
}
asmlinkage void do_tlbl(struct pt_regs *regs)
{	

	do_page_fault(regs, 0, read_c0_badvaddr());
}
asmlinkage void do_tlbs(struct pt_regs *regs)
{
// 	printk("do_tlbs status %p, cause %p, epc %p,bd %p.\n", 
// 		regs->cp0_status,
// 		regs->cp0_cause,
// 		regs->cp0_epc,
// 		read_c0_badvaddr());
	do_page_fault(regs, 1, read_c0_badvaddr());
}

asmlinkage void do_dsp(struct pt_regs *regs)
{
	hal_panic("TODO");
}

asmlinkage void do_mt(struct pt_regs *regs)
{
	hal_panic("TODO");
}

asmlinkage void do_mcheck(struct pt_regs *regs)
{
	hal_panic("TODO");
}

asmlinkage void do_watch(struct pt_regs *regs)
{
	hal_panic("TODO");
}

asmlinkage void do_mdmx(struct pt_regs *regs)
{
	hal_panic("TODO");
}

asmlinkage void do_fpe(struct pt_regs *regs, unsigned long fcr31)
{
	hal_panic("TODO");
}

asmlinkage void do_tr(struct pt_regs *regs)
{
	hal_panic("TODO");
}

asmlinkage void do_ov(struct pt_regs *regs)
{
	hal_panic("TODO");
}

asmlinkage void do_cpu(struct pt_regs *regs)
{
	hal_panic("TODO");
}

asmlinkage void do_ri(struct pt_regs *regs)
{
	hal_panic("TODO");
}

asmlinkage void do_bp(struct pt_regs *regs)
{
	hal_panic("TODO");
}

asmlinkage void do_be(struct pt_regs *regs)
{
	hal_panic("TODO");
}

asmlinkage void do_ade(struct pt_regs *regs)
{
	print_registers(regs);
	hal_panic("TODO");
}

void nmi_exception_handler(struct pt_regs *regs)
{
	hal_panic("TODO");
}

asmlinkage void do_unknown(struct pt_regs *regs)
{
	print_registers(regs);
	hal_panic("TODO");
}

void mips_setup_traps()
{
	int i;
	for (i = 0; i < 32; i++)
		setup_exception_other(handle_unknown, i);

	setup_exception_other(handle_int, 0);
	setup_exception_other(handle_tlbm, 1);
	setup_exception_other(handle_tlbl, 2);
	setup_exception_other(handle_tlbs, 3);

	setup_exception_other(handle_adel, 4);
	setup_exception_other(handle_ades, 5);

	setup_exception_other(handle_sys, 8);
}

void printk_mips(unsigned long t9)
{
	printk("MIPS t9 is %x\n", t9);
}
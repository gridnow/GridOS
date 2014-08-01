/*
	Traps for mips
	wuxin
*/

#include <compiler.h>
#include <linkage.h>

#include <section.h>
#include <process.h>
#include <thread.h>

#include <arch/walk.h>

#include <asm/ptrace.h>
#include <asm/mipsregs.h>

#define WRITE_LO \
	if (ctx->hirarch_id[1] & 1) \
	{ \
		entry = km_pte_read(ctx) & (~ PAGE_FLAG_SOFT_MASK); \
		entry = ((entry >> 6) & (~PAGE_FLAG_LO_HW_MASK)/*PFN 47~12*/) | (entry & PAGE_FLAG_LO_HW_MASK);	\
		write_c0_entrylo1(entry | G_FLAG);	\
		/*printk("ctx->hirarch_id[1] = %d, Pte is %x ", ctx->hirarch_id[1], entry | G_FLAG);*/  \
	\
		entry = _pte_read_pre(ctx) & (~ PAGE_FLAG_SOFT_MASK);	\
		entry = ((entry >> 6) & (~PAGE_FLAG_LO_HW_MASK)/*PFN 47~12*/) | (entry & PAGE_FLAG_LO_HW_MASK);	\
		write_c0_entrylo0(entry | G_FLAG);	\
		/*printk("PTe is %x ", entry | G_FLAG); */\
	}	\
	else	\
	{	\
		entry = km_pte_read(ctx) & (~ PAGE_FLAG_SOFT_MASK);	\
		entry = ((entry >> 6) & (~PAGE_FLAG_LO_HW_MASK)/*PFN 47~12*/) | (entry & PAGE_FLAG_LO_HW_MASK);	\
		write_c0_entrylo0(entry | G_FLAG);	\
		/* printk("ctx->hirarch_id[1] = %d, pTe is %x ", ctx->hirarch_id[1], entry | G_FLAG);  */ \
	\
		entry = _pte_read_next(ctx) & (~ PAGE_FLAG_SOFT_MASK);	\
		entry = ((entry >> 6) & (~PAGE_FLAG_LO_HW_MASK)/*PFN 47~12*/) | (entry & PAGE_FLAG_LO_HW_MASK);	\
		write_c0_entrylo1(entry | G_FLAG);	\
		/* printk("pTE is %x \n", entry | G_FLAG);*/   \
	}

static void trace_page_fault(struct pt_regs *regs, unsigned long write, unsigned long address)
{
	char * type;

	if (write == 0)
		type = "读";
	else if (write == 1)
		type = "写";
	else if (write == 2)
		type = "空";
	else
		type = "unknown";

	printk("页表'%s'异常,", type);
	printk("regs = %x status %p, cause %p, epc %p,bd %p,$25 %x\n",
		regs,
		regs->cp0_status,
		regs->cp0_cause,
		regs->cp0_epc,
		address,
		regs->regs[25]);
}

/**
	@brief Get the VPN2 of the Linear Address
*/
static inline unsigned long get_vpn2(unsigned long va)
{
	/* 把后面的13位删除掉即可(4KB模式)，在PAGEMASK中还有一道硬件比较 */
	return va & (PAGE_MASK << 1);
}

/**
	@brief Read previous
*/
static inline unsigned long _pte_read_pre(struct km_walk_ctx * ctx)
{
	volatile unsigned long * entry  = &(ctx->table_base[1][ctx->hirarch_id[1] - 1]);
// 	printk("kmm_pte_read: ctx->table_base[0] = %p, id %d, entry %p.\n",
// 	  		ctx->table_base[0],
// 	  		ctx->temp_id[0] - 1,
// 	  		entry);

	return *entry;
}

/**
	@brief Read next
*/
static inline unsigned long _pte_read_next(struct km_walk_ctx * ctx)
{
	volatile unsigned long * entry  = &(ctx->table_base[1][ctx->hirarch_id[1] + 1]);
// 	printk("kmm_pte_read: ctx->table_base[0] = %p, id %d, entry %p.\n",
// 	  		ctx->table_base[0],
// 	  		ctx->temp_id[0] + 1,
// 	  		entry);

	return *entry;
}

/**
	@brief Refill the TLB entry

	@note
		Have to in interrupt disabled
*/
static void refill_tbl(struct km_walk_ctx * ctx)
{
	unsigned char asid;
	unsigned long entry;
	unsigned long G_FLAG;

	/* Entry HI */
	asid = ctx->mem->asid & CPU_PAGE_FALG_ASID_MASK;
	entry = get_vpn2(ctx->current_virtual_address);
	entry |= asid;
	write_c0_entryhi(entry);

	/* Add the G_FLAG if ASID == 0, because the entry is from kernel and shared by all process */
	G_FLAG = (ctx->mem == &kp_get_system()->mem_ctx)? 1 : 0;

	/* Entry Low0 and Low1 */
	WRITE_LO;
	
	/* Write by type */
	mtc0_tlbw_hazard();
	tlb_write_random();
	tlbw_use_hazard();
}

static void refill_tbl_to(struct km_walk_ctx * ctx, unsigned int asid, int write, int pos)
{	
	unsigned long entry, oldl1, oldl2;
	unsigned long G_FLAG;
	int idx;
	int oldpid;

	/* Just test ASID consistency: Current ASID must equal to Given ASID, kernel process do not obay this rule. */
	oldpid = read_c0_entryhi();

	/* Entry HI */	
	asid = asid & CPU_PAGE_FALG_ASID_MASK;
	entry = get_vpn2(ctx->current_virtual_address);
	entry |= asid;
	write_c0_entryhi(entry);
	mtc0_tlbw_hazard();
	tlb_probe();
	tlb_probe_hazard();
	idx = read_c0_index();

	oldl1 = read_c0_entrylo0();
	oldl2 = read_c0_entrylo1();
	/* Add the G_FLAG if ASID == 0, because the entry is from kernel and shared by all process */
	G_FLAG = (ctx->mem == &kp_get_system()->mem_ctx)? 1 : 0;

	/* Entry Low0 and Low1 */
	WRITE_LO;

	/* Write by type, the write is random if the TLB entry is flushed for R/W flags changing */
	mtc0_tlbw_hazard();
	if (unlikely(idx < 0))
		tlb_write_random();
	else
	{
		if (write == 2)
		{
			printk("Write is forced index for %x, pos %d, idx %d,asid %d, %x %x.\n", ctx->current_virtual_address, pos, idx, asid, oldl1, oldl2);
		}
		
		tlb_write_indexed();
	}
	tlbw_use_hazard();

	/* Sanity: Just test ASID consistency: Current ASID must equal to Given ASID, kernel process do not obey this rule. */
	if ((oldpid & 0xff) != (asid & 0xff) && asid != 0/*kernel asid*/)
 		printk("Why old = %x, asid = %x. ", oldpid, asid);
}

static bool recover(unsigned long address, int write, int present)
{
	unsigned long core_error_code = 0;

	if (write)
		core_error_code |= PAGE_FAULT_W;
	if (present)
		core_error_code |= PAGE_FAULT_P;

	/* Call the exception handler of section */
	return ks_exception(kt_current(), address, core_error_code);
}

/**
	@brief 判断一个地址是否在用户空间
*/
bool kmm_arch_address_in_user(unsigned long address)
{
	if (address >= ARCH_USER_SPACE_START + ARCH_USER_SPACE_SIZE)
		return false;
	return true;
}

/**
	@brief the page fault happened

	@param[in] regs thread ctx
	@param[in] write 0 is read , 1 is write, 2 is the tlb refill
	@param[in] address the error address where the fault happened

	@note
		Interrupt must be disabled!
*/
asmlinkage void do_page_fault(struct pt_regs *regs, unsigned long write, unsigned long address)
{
	struct km * mem;
	unsigned long pte;
	struct km_walk_ctx ctx;
	struct ko_process *real;
	bool in_user = kmm_arch_address_in_user(address);

	//trace_page_fault(regs, write, address);

	/*
		The fault is triggered when accessing kernel addressing space
		when the thread is in user mode?
	*/
	if (!(regs->cp0_status & ST0_CU0))
	{
		if (in_user == false)
			goto kill;
	} 	

	//TODO: check interrupt status
	
	/* We are using the kernel space? The mem ctx should come from kernel process */
	if (in_user == false)
		real = kp_get_system();
	else
		real = KT_GET_KP(kt_current());
	mem = kp_get_mem(real);
	KM_WALK_INIT(mem, &ctx);
	if (unlikely(km_walk_to(&ctx, address) == false))
	{
		printk("MIPS 页表异常时无法定位项目.\n");
		goto kill2;
	}

	/* Read out the PTE */
	pte = km_pte_read(&ctx);

	/* Just refill ? */
	if (write == 2)
	{
		/* If is valid entry */
		if (pte & PAGE_FLAG_VALID)
		{
			/* 
				TODO: use the random version,have to close interrupt and scheduler, but the asm opened interrupt.
				否则认为是REFILL，但是可能被其他线程给填充了，导致TLB中有相同的转换信息。
			*/
			refill_tbl_to(&ctx, mem->asid/*If in user space, the asid is what we want, if the process in kernel space, refill will add G, asid not used */ , write, 0);
			mem->hw_refill++;
			goto end;
		}
	}
	else
	{
		/* Sanity check */
		/*
			The exception handler may create a new page and write it just in the handler
			before it's updated to TLB. This is forbidden, because the TLB will be refill
			again after the recover
		*/
		if (write == 1)
		{
			if (pte & PAGE_FLAG_WRITE && pte & PAGE_FLAG_VALID)
			{
				hal_panic("TLB错误: PTE 项是正确的并应该触发REFILL, 而不是WRITE.\n");
			}
		}
		else
		{
			if (pte & (PAGE_FLAG_VALID))
			{
				trace_page_fault(regs, write, address);
				printk("ASID %d.\n", mem->asid);
				hal_panic("TLB错误: PTE 项是正确的并应该触发REFILL, 而不是READ.\n");
			}
		}
	}
	kp_put_mem(mem);

	/* The PTE is not Refill nor is valid, we need to recover the page to memory */
	if (unlikely(recover(address, write, pte & PAGE_FLAG_VALID) == false))
		goto kill;
	
	/*
			REFILL exception
			Check the translation hierarchy and write to TLB.
			The refill may be triggered by read or write, but we don't care, just fill the TLB.
			Access violation will be checked if happened in second turn, but in normal case this is the end of the exception except COW.
	*/
	
	mem = kp_get_mem(real);
	refill_tbl_to(&ctx, mem->asid, write, 2);
		
end:
	kp_put_mem(mem);
	return;
kill2:
	kp_put_mem(mem);
kill:
	trace_page_fault(regs, write, address);
	kt_delete_current();
}


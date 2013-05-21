#ifndef __ASM_OFFSETS_H__
#define __ASM_OFFSETS_H__

#define S_R0 0 /* offsetof(struct pt_regs, ARM_r0)	@ */
#define S_R1 4 /* offsetof(struct pt_regs, ARM_r1)	@ */
#define S_R2 8 /* offsetof(struct pt_regs, ARM_r2)	@ */
#define S_R3 12 /* offsetof(struct pt_regs, ARM_r3)	@ */
#define S_R4 16 /* offsetof(struct pt_regs, ARM_r4)	@ */
#define S_R5 20 /* offsetof(struct pt_regs, ARM_r5)	@ */
#define S_R6 24 /* offsetof(struct pt_regs, ARM_r6)	@ */
#define S_R7 28 /* offsetof(struct pt_regs, ARM_r7)	@ */
#define S_R8 32 /* offsetof(struct pt_regs, ARM_r8)	@ */
#define S_R9 36 /* offsetof(struct pt_regs, ARM_r9)	@ */
#define S_R10 40 /* offsetof(struct pt_regs, ARM_r10)	@ */
#define S_FP 44 /* offsetof(struct pt_regs, ARM_fp)	@ */
#define S_IP 48 /* offsetof(struct pt_regs, ARM_ip)	@ */
#define S_SP 52 /* offsetof(struct pt_regs, ARM_sp)	@ */
#define S_LR 56 /* offsetof(struct pt_regs, ARM_lr)	@ */
#define S_PC 60 /* offsetof(struct pt_regs, ARM_pc)	@ */
#define S_PSR 64 /* offsetof(struct pt_regs, ARM_cpsr)	@ */
#define S_OLD_R0 68 /* offsetof(struct pt_regs, ARM_ORIG_r0)	@ */
#define S_FRAME_SIZE 72 /* sizeof(struct pt_regs)	@ */

#define VM_EXEC			0x4

#define SIZEOF_MACHINE_DESC 76 /* sizeof(struct machine_desc)	@ */
#define MACHINFO_TYPE 0 /* offsetof(struct machine_desc, nr)	@ */
#define MACHINFO_NAME 4 /* offsetof(struct machine_desc, name)	@ */

#define PROC_INFO_SZ 52 /* sizeof(struct proc_info_list)	@ */
#define PROCINFO_INITFUNC 16 /* offsetof(struct proc_info_list, __cpu_flush)	@ */
#define PROCINFO_MM_MMUFLAGS 8 /* offsetof(struct proc_info_list, __cpu_mm_mmu_flags)	@ */
#define PROCINFO_IO_MMUFLAGS 12 /* offsetof(struct proc_info_list, __cpu_io_mmu_flags)	@ */

/* proc-macros.S 要用这些来进行c结构体的转换 */
#define VMA_VM_MM 0			//Address of vm_mm same to vma
#define MM_CONTEXT_ID 0		//context_id is the 1st element of context
#define VMA_VM_FLAGS 4		//vm_flags is the 2nd element of VMA

/* cache-v6.S 中用这些c代码中的dma方向标志进行cache操作 */
#define DMA_BIDIRECTIONAL 0 /* DMA_BIDIRECTIONAL	@ */
#define DMA_TO_DEVICE 1 /* DMA_TO_DEVICE	@ */
#define DMA_FROM_DEVICE 2 /* DMA_FROM_DEVICE	@ */
#endif


unsigned long kernelsp[NR_CPUS];

void kma_setup()
{
	extern void mips_setup_traps();//traps.c
	
	mips_setup_traps();
}


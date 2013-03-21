#include "cpu.h"

void __init km_setup_percpu(int cpu, unsigned long base)
{
	hal_arch_setup_percpu(cpu, base);
}


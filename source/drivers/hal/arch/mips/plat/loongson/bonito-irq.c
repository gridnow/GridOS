#include <types.h>
#include <loongson.h>
#include <private/irq.h>
#include <private/interrupt.h>
#include <asm/io.h>

#define __maybe_unused

static inline void bonito_irq_enable(struct irq_data *d)
{
	LOONGSON_INTENSET = (1 << (d->irq - LOONGSON_IRQ_BASE));
	mmiowb();
}

static inline void bonito_irq_disable(struct irq_data *d)
{
	LOONGSON_INTENCLR = (1 << (d->irq - LOONGSON_IRQ_BASE));
	mmiowb();
}


static struct irq_chip bonito_irq_type = {
	.name	= "bonito_irq",
	.irq_ack	= bonito_irq_disable,
	.irq_mask	= bonito_irq_disable,
	.irq_mask_ack = bonito_irq_disable,
	.irq_unmask	= bonito_irq_enable,
};

static struct irqaction  dma_timeout_irqaction = {
	.handler	= no_action,
	.name		= "dma_timeout",
};

void bonito_irq_init(void)
{
	u32 i;

	for (i = LOONGSON_IRQ_BASE; i < LOONGSON_IRQ_BASE + 32; i++)
		irq_set_chip_and_handler(i, &bonito_irq_type, handle_level_irq);

#ifdef CONFIG_CPU_LOONGSON2E
	setup_irq(LOONGSON_IRQ_BASE + 10, &dma_timeout_irqaction);
#endif
}
#include <ddk/input.h>
#include <types.h>
#include "session.h"

static size_t text_read(struct ifi_package * input)
{
	return ifi_read_input((void *)input, IFI_DEV_STD_IN);
}

static size_t text_write(unsigned char * buffer, size_t size)
{
	return printk("%s", buffer);
}

static size_t dummy_read(struct ifi_package * input)
{

}

static size_t dummy_write(unsigned char * buffer, size_t size)
{

}

static struct sau_stdio_interface builtin_stdio[] =
{
	{
		.read = text_read,
		.write = text_write,
	}
	,
	{
		.read = dummy_read,
		.write = dummy_write,
	}
};

struct sau_stdio_interface * sau_get_stdio_handler()
{
	return &builtin_stdio[0];
}

int sau_read(struct ifi_package * input)
{
	return sau_get_stdio_handler()->read(input);	
}

int sau_write(char * buffer, size_t size)
{
	return sau_get_stdio_handler()->write(buffer, size);	
}

#ifndef __ASM_GENERIC_PARAM_H
#define __ASM_GENERIC_PARAM_H

# define HZ		CONFIG_HZ	/* Internal kernel timer frequency */
# define USER_HZ	100		/* some user interfaces are */
# define CLOCKS_PER_SEC	(USER_HZ)       /* in "ticks" like times() */


#ifndef HZ
#define HZ 100
#endif

#endif
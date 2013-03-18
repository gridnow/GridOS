#ifndef KP_PROCESS_H
#define KP_PROCESS_H

#include "object.h"

/* Process privilege level */
#define KP_CPL0						0
#define KP_USER						3

struct ko_process
{
	struct cl_object base;
	int cpl;
};

#endif

#ifndef __CRT_H__
#define __CRT_H__

/************************************************************************/
/* Memory part                                                          */
/************************************************************************/
void *crt_free(void *p);
void *crt_alloc(int size);
void *crt_zalloc(int size);

#endif
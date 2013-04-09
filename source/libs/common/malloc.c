/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   ͨ�� ������С�ڴ���� ģ��
*/

#include <types.h>
#include <stddef.h>
#include <list.h>
#include <string.h>

#include "memalloc.h"
#define FREE_NODE 0x00000001
#define P_MAGIC 0x12332101
#define N_MAGIC 0x12332102

typedef struct node{
	unsigned long p_magic;
	/* �ڽ����� */
	struct node* pre;
	struct node* next;
	/* Hash table */
	struct node* hash_pre;
	struct node* hash_next;
	/* �������� */
	size_t attribute;
	size_t size; //������������������С
	unsigned long n_magic;
}node_t;

#define IS_FREE_NODE(nod) (nod->attribute&FREE_NODE )
#define MAKE_FREE(nod) (nod->attribute |= FREE_NODE)
#define MAKE_OCCUPIED(nod) {nod->attribute &= ~FREE_NODE ;nod->p_magic = P_MAGIC;nod->n_magic = N_MAGIC;}
#define MAX_HASH_ENTRY 10

struct linear_memory_block
{
	struct linear_memory_block *p_pre, *p_next;	
	unsigned int used, inited;
	unsigned int size;

	node_t* free_table[MAX_HASH_ENTRY] ;
	node_t* fisrt_node;	//used for debugging.
};

#define HASH_APPEND( i, l ) { \
	i->hash_next = l;	i->hash_pre = NULL;	if(l) l->hash_pre = i; l=i;	}
#define HASH_DELETE( i, l ) { \
	if( i->hash_pre ){ i->hash_pre->hash_next = i->hash_next; }else{ l = i->hash_next; }	\
	if( i->hash_next ){ i->hash_next->hash_pre = i->hash_pre; } \
	}

/**
	@brief �Ӵ�С�õ�ɢ�б������� 
*/
static int calc_hash_index(size_t k)
{
	int i;
	k = k>>5;
	i = 0;
	while( k>1 ){
		k = k>>1;
		i ++;
	}
	if( i>=MAX_HASH_ENTRY )
		i = MAX_HASH_ENTRY - 1;
	return i;
}

/**
	@brief �������亯��
	
	ʱ�临�Ӷȷ���������W(n)  һ�����O(1)
*/
static void * alloc(struct linear_memory_block *block, size_t siz)
{
	size_t m, k, i, j;
	node_t* nod;
	
	m = siz + sizeof(node_t);
	i = calc_hash_index(m);
	
	/*
		�ڿ��п�ɢ�б����������ʵĿ�
	*/
	for (j = i; j < MAX_HASH_ENTRY; j++ )
	{
		nod = block->free_table[j];
		while (nod && nod->size < m)
			nod = nod->hash_next;

		/* �ҵ����ÿ� */
		if (nod)
		{
			size_t rest = nod->size + sizeof(node_t) - m;							//rest��С������������������С
			/* �ӿ���ɢ�б���ɾ�� */
			HASH_DELETE(nod, block->free_table[j]);
			MAKE_OCCUPIED(nod);														//ռ��

			/* ��������¿ռ䣬����ӵ�����ɢ�б��С�rest == 0 Ϊ����״̬ */
			if (rest >= sizeof(node_t))
			{ 
				node_t* nod2 = (node_t*)((unsigned long)nod + m);
				nod2->size = rest - sizeof(node_t);
				nod->size = siz;

				/* �����ڽ����� */
				nod2->next = nod->next;
				if (nod2->next)
					nod2->next->pre = nod2;
				nod->next = nod2;
				nod2->pre = nod;

				/* ����ɢ�б� */
				k = calc_hash_index(rest);
				HASH_APPEND(nod2, block->free_table[k]);
				MAKE_FREE(nod2);
			}
			return (void*)((unsigned long)nod + sizeof(node_t));
		}
		/* û�п��ÿ� */
	}
	
	return NULL;
}

/**
	@brief �ϲ�a��b��cΪa��b�в��ǵ�ǰ�ͷŵ�һ����
*/
static node_t * merge(struct linear_memory_block *block, node_t* a, node_t* b, node_t* c)
{
	int k;
	/* ����ɢ�б� */
	k = calc_hash_index(c->size+sizeof(node_t));
	HASH_DELETE(c, block->free_table[k]);
	/* ����ǰһ���С */
	a->size += b->size + sizeof(node_t);
	/* �����ڽ����� */
	a->next = b->next;
	if(a->next)
		a->next->pre = a;
	return a;
}

/**
	@brief �����ͷź���
	ʱ�临�Ӷȷ���������W(1)  һ��O(1)
*/
static int dealloc(struct linear_memory_block *block, void* p)
{
	int k;
	int this_size = 0;
	node_t* nod;

	nod = (node_t*)((unsigned long)p - sizeof(node_t));

	/* really free */
	if (IS_FREE_NODE(nod))
		goto err1;
	
	/* ����� */
	if (nod->p_magic != P_MAGIC)
		goto err1;
	if (nod ->n_magic != N_MAGIC)
		goto err1;
	
	/* �������ϲ�ǰ�� */
	this_size = nod->size;
	MAKE_FREE(nod);
	if (nod->pre && IS_FREE_NODE(nod->pre))
		nod = merge(block,nod->pre, nod, nod->pre);
	if (nod->next && IS_FREE_NODE(nod->next))
		nod = merge(block,nod, nod->next, nod->next);
	k = calc_hash_index(nod->size + sizeof(node_t));
	HASH_APPEND(nod, block->free_table[k]);
	
	return this_size;

err1:
	return 0;
}

/**
	@brief ��ȡ�����ڴ�ռ�
*/
static void init_block(struct linear_memory_block *block)
{
	/* make the first node */
	node_t* nod = (node_t *)(block + 1);
	nod->pre = nod->next = NULL;
	nod->size = block->size - sizeof(node_t)/*the first node used the size*/;
	/* attach it to free table */
	HASH_APPEND(nod, block->free_table[MAX_HASH_ENTRY-1]);
	MAKE_FREE(nod);
	block->fisrt_node = nod;
}

static void *_malloc(struct linear_memory_block *block, int size)
{
	void *pAddress;
	
	if (!size) return NULL;
	
	/*
		size ����� unsigned long�����,�����ϵͳϵͳЧ��
	*/
	if (size & (sizeof(unsigned long) - 1))
	{
		size |= (sizeof(unsigned long) - 1);
		size++;
	}
	pAddress = alloc(block, size);
	
	/* Try another block */
	if(pAddress==NULL)
	{
		//TODO
		return NULL;
	}
	else
		block->used += size;

	return pAddress;
}

static void *_free(struct linear_memory_block *block, void *aptr)
{
	void *r = NULL;
	
	if(aptr ==NULL) goto ___free_end;

	/*
		Make sure the address in the block
	*/	
	while (block != NULL)
	{
		if ((unsigned long)block < (unsigned long)aptr &&
			(unsigned long)aptr < (unsigned long)(block + 1/*Data after the block manager*/) + block->size)
		{			
			break;
		}
		block = block->p_next;
	}
	if(block==NULL)
	{
		goto ___free_end;
	}
	else
	{
		block->used -= dealloc(block, aptr);
		r = aptr;
	}

___free_end:
	return r;
}

#if 0
/**
	@brief realloc

	if new_size = 0, then free old_address
	if old_address = NULL, then pure malloc
*/
void *_realloc(void* old_address, size_t new_size)
{
	unsigned char * new_address;
	int old_size;

	/* But is free operation */
	if (new_size == 0)
	{
		free(old_address);
		return NULL;
	}

	/* And is malloc? */
	if(old_address==NULL)
	{
		//TODO
		return NULL;
	}

	/* 
		ok,recapture memory, but we malloc copy free.
		TODO to optimize.
	*/
	new_address = NULL;
//	new_address=__malloc_r(new_size);
// 	memcpy(new_address,old_address,new_size);
// 	free(old_address);

	return new_address; 
}
#endif
/************************************************************************/
/* INTERFACE                                                            */
/************************************************************************/

void memalloc_debug_print(struct linear_memory_block *block)
{
	int i;
	size_t used_size, free_size;
	node_t * nod;
	//printf("�����ڽ�����:\n");
	nod = block->fisrt_node;				
	used_size=free_size=0;
	while( nod ){
		if( IS_FREE_NODE(nod) ){
			//printf("[����:%d]", nod->size+sizeof(node_t) );
			free_size += nod->size+sizeof(node_t);
		}else{
			//printf("{ռ��:%d}", nod->size+sizeof(node_t) );
			used_size += nod->size+sizeof(node_t);
		}
		nod = nod->next;
	}
	//printf("\nһ��ռ��%d�ֽڣ�ʣ��%d�ֽڣ��ڴ�����%d�ֽ�\n", used_size, free_size, used_size+free_size);
	//printf("��������ɢ�б�:\n");
	for(i=0;i<MAX_HASH_ENTRY;i++){
		//printf("%d�ֽ� ", 1<<(i+5) );
		nod = block->free_table[i];
		while( nod ){
			//	printf("�ߴ�[%d]", nod->size+sizeof(node_t) );
			nod = nod->hash_next;
		}
		//printf("\n");
	}
}

/**
	@brief Init a block of memory for malloc
*/
bool memalloc_init_allocation(void * big_block, int size)
{
	struct linear_memory_block *block = big_block;
	memset(block, 0, sizeof(*block));
	
	/* The size is big enough ? */
	if (size - sizeof(*block) - sizeof(node_t)/*The very fist node*/ <= 0)
		return false;
	block->size = size - sizeof(*block);
	init_block(block);
	block->inited = 1;
	return true;
}

void * memalloc(void * big_block, int size)
{
	return _malloc(big_block, size);
}

void * memfree(void * big_block, void * p)
{
	return _free(big_block, p);
}
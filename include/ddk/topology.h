/**
*  @defgroup Topology
*  @ingroup DDK
*
*  ��������ϵ���ܣ���NUMA�������˽ṹ�ӿ�
*  @{
*/

#ifndef _DDK_TOPOLOGY_H_
#define _DDK_TOPOLOGY_H_

static inline int hal_mp_bus_to_node(int busnum)
{
	//TODO
	return 0;
}

/*
	��ͳ����ֱ���õĽӿڣ�����Ҫ����ת��
*/
#define get_mp_bus_to_node hal_mp_bus_to_node
#endif

/** @} */
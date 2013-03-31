/**
*  @defgroup Topology
*  @ingroup DDK
*
*  定义了体系构架（如NUMA）的拓扑结构接口
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
	传统驱动直接用的接口，我们要对其转换
*/
#define get_mp_bus_to_node hal_mp_bus_to_node
#endif

/** @} */
/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *
 *   Grid
 */
#ifndef DDK_GRID_H
#define DDK_GRID_H

/**
	@brief �û�ʹ�øýӿڽ������ݵ��շ�
*/
struct grid_netproto
{
	int version;
	const char *proto_name;

	/* Protocol ops */
	int (*connect)();	
};

#define GRID_GET_NETPROTO "grid_acquire_netproto"

#endif


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
	@brief 用户使用该接口进行数据的收发
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


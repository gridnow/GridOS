/**
*  @defgroup net_mii
*  @ingroup DDK
*
*  定义了网络媒体介质信息
*  @{
*/

#ifndef _DDK_NET_MMI_H_
#define _DDK_NET_MMI_H_

struct mii_if_info {
	int phy_id;
	int advertising;
	int phy_id_mask;
	int reg_num_mask;

	unsigned int full_duplex : 1;	/* is full duplex? */
	unsigned int force_media : 1;	/* is autoneg. disabled? */
	unsigned int supports_gmii : 1; /* are GMII registers supported? */

	struct net_device *dev;
	int (*mdio_read) (struct net_device *dev, int phy_id, int location);
	void (*mdio_write) (struct net_device *dev, int phy_id, int location, int val);
};
#endif
/** @} */

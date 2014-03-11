

void dump_raw_package(void *buf, int size)
{
	unsigned char *p = buf;
	int off = 0;

	while (size > 0)
	{		
		if ((off % 16) == 0)
			printf("\n%08x: ", off);

		printf("%02x ", p[off]);
		off++;
		size--;
	}
}

static void init_if(struct netif *nif)
{
	ip_addr_t test_ipaddr, test_netmask, test_gw;

	/* TODO: 从系统获取IP + MAC */
	IP4_ADDR(&test_gw, 192,168,1,1);
	IP4_ADDR(&test_ipaddr, 192,168,1,12);
	IP4_ADDR(&test_netmask, 255,255,255,0);
	nif->hwaddr[0] = 0x11;
	nif->hwaddr[1] = 0x22;
	nif->hwaddr[2] = 0x33;
	nif->hwaddr[3] = 0x44;
	nif->hwaddr[4] = 0x55;
	nif->hwaddr[5] = 0x66;

	netif_set_address(nif, &test_ipaddr, &test_netmask, &test_gw);	
	myip_instance_init(nif);
}

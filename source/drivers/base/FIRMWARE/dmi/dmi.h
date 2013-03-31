#ifndef __DMI_H__
#define __DMI_H__

#define DMI_MATCH(a, b)	{ a, b }

/* dmi */
enum dmi_field {
	DMI_NONE,
	DMI_BIOS_VENDOR,
	DMI_BIOS_VERSION,
	DMI_BIOS_DATE,
	DMI_SYS_VENDOR,
	DMI_PRODUCT_NAME,
	DMI_PRODUCT_VERSION,
	DMI_PRODUCT_SERIAL,
	DMI_PRODUCT_UUID,
	DMI_BOARD_VENDOR,
	DMI_BOARD_NAME,
	DMI_BOARD_VERSION,
	DMI_BOARD_SERIAL,
	DMI_BOARD_ASSET_TAG,
	DMI_CHASSIS_VENDOR,
	DMI_CHASSIS_TYPE,
	DMI_CHASSIS_VERSION,
	DMI_CHASSIS_SERIAL,
	DMI_CHASSIS_ASSET_TAG,
	DMI_STRING_MAX,
};

struct dmi_strmatch {
	unsigned char slot;
	char substr[79];
};

struct dmi_system_id {
	int (*callback)(const struct dmi_system_id *);
	const char *ident;
	struct dmi_strmatch matches[4];
	void *driver_data;
};

struct dmi_header {
	u8 type;
	u8 length;
	u16 handle;
};

#ifdef CONFIG_DMI
#error "DMI System not supported"
#else
static inline bool dmi_get_date(int field, int *yearp, int *monthp, int *dayp)
{
	if (yearp)
		*yearp = 0;
	if (monthp)
		*monthp = 0;
	if (dayp)
		*dayp = 0;
	return false;
}
static inline int dmi_check_system(const struct dmi_system_id *list) { return 0; }
static inline int dmi_walk(void (*decode)(const struct dmi_header *, void *),
	void *private_data) { return -1; }

#endif /* CONFIG_DMI */


#endif

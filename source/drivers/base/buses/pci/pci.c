#include <types.h>
#include <ddk/log.h>
#include <ddk/pci/pci_regs.h>
#include <ddk/delay.h>

#include "pci.h"
#include <asm/pci.h>

#define printk_ratelimit() 0 

u8 pci_dfl_cache_line_size  = 32 >> 2;
u8 pci_cache_line_size;

unsigned int pci_pm_d3_delay;

static void pci_dev_d3_sleep(struct pci_dev *dev)
{
	unsigned int delay = dev->d3_delay;

	if (delay < pci_pm_d3_delay)
		delay = pci_pm_d3_delay;

	msleep(delay);
}

#define PCI_FIND_CAP_TTL	48

static int __pci_find_next_cap_ttl(struct pci_bus *bus, unsigned int devfn,
								   u8 pos, int cap, int *ttl)
{
	u8 id;

	while ((*ttl)--) {
		pci_bus_read_config_byte(bus, devfn, pos, &pos);
		if (pos < 0x40)
			break;
		pos &= ~3;
		pci_bus_read_config_byte(bus, devfn, pos + PCI_CAP_LIST_ID,
			&id);
		if (id == 0xff)
			break;
		if (id == cap)
			return pos;
		pos += PCI_CAP_LIST_NEXT;
	}
	return 0;
}

static int __pci_find_next_cap(struct pci_bus *bus, unsigned int devfn,
							   u8 pos, int cap)
{
	int ttl = PCI_FIND_CAP_TTL;

	return __pci_find_next_cap_ttl(bus, devfn, pos, cap, &ttl);
}

static int __pci_bus_find_cap_start(struct pci_bus *bus,
									unsigned int devfn, u8 hdr_type)
{
	u16 status;

	pci_bus_read_config_word(bus, devfn, PCI_STATUS, &status);
	if (!(status & PCI_STATUS_CAP_LIST))
		return 0;

	switch (hdr_type) {
	case PCI_HEADER_TYPE_NORMAL:
	case PCI_HEADER_TYPE_BRIDGE:
		return PCI_CAPABILITY_LIST;
	case PCI_HEADER_TYPE_CARDBUS:
		return PCI_CB_CAPABILITY_LIST;
	default:
		return 0;
	}

	return 0;
}

int pci_find_capability(struct pci_dev *dev, int cap)
{
	int pos;

	pos = __pci_bus_find_cap_start(dev->bus, dev->devfn, dev->hdr_type);
	if (pos)
		pos = __pci_find_next_cap(dev->bus, dev->devfn, pos, cap);

	return pos;
}


/**
 * pci_find_ext_capability - Find an extended capability
 * @dev: PCI device to query
 * @cap: capability code
 *
 * Returns the address of the requested extended capability structure
 * within the device's PCI configuration space or 0 if the device does
 * not support it.  Possible values for @cap:
 *
 *  %PCI_EXT_CAP_ID_ERR		Advanced Error Reporting
 *  %PCI_EXT_CAP_ID_VC		Virtual Channel
 *  %PCI_EXT_CAP_ID_DSN		Device Serial Number
 *  %PCI_EXT_CAP_ID_PWR		Power Budgeting
 */
int pci_find_ext_capability(struct pci_dev *dev, int cap)
{
	u32 header;
	int ttl;
	int pos = PCI_CFG_SPACE_SIZE;

	/* minimum 8 bytes per capability */
	ttl = (PCI_CFG_SPACE_EXP_SIZE - PCI_CFG_SPACE_SIZE) / 8;

	if (dev->cfg_size <= PCI_CFG_SPACE_SIZE)
		return 0;

	if (pci_read_config_dword(dev, pos, &header) != PCIBIOS_SUCCESSFUL)
		return 0;

	/*
	 * If we have no capabilities, this is indicated by cap ID,
	 * cap version and next pointer all being 0.
	 */
	if (header == 0)
		return 0;

	while (ttl-- > 0) {
		if (PCI_EXT_CAP_ID(header) == cap)
			return pos;

		pos = PCI_EXT_CAP_NEXT(header);
		if (pos < PCI_CFG_SPACE_SIZE)
			break;

		if (pci_read_config_dword(dev, pos, &header) != PCIBIOS_SUCCESSFUL)
			break;
	}

	return 0;
}

static void pci_restore_bars(struct pci_dev *dev)
{
	int i;

	for (i = 0; i < PCI_BRIDGE_RESOURCES; i++)
		pci_update_resource(dev, i);
}

static struct pci_platform_pm_ops *pci_platform_pm;

int pci_set_platform_pm(struct pci_platform_pm_ops *ops)
{
	if (!ops->is_manageable || !ops->set_state || !ops->choose_state
		|| !ops->sleep_wake || !ops->can_wakeup)
		return -EINVAL;
	pci_platform_pm = ops;
	return 0;
}

static inline bool platform_pci_power_manageable(struct pci_dev *dev)
{
	return pci_platform_pm ? pci_platform_pm->is_manageable(dev) : false;
}

static inline int platform_pci_set_power_state(struct pci_dev *dev,
											   pci_power_t t)
{
	return pci_platform_pm ? pci_platform_pm->set_state(dev, t) : -ENOSYS;
}

static inline pci_power_t platform_pci_choose_state(struct pci_dev *dev)
{
	return pci_platform_pm ?
		pci_platform_pm->choose_state(dev) : PCI_POWER_ERROR;
}

static inline bool platform_pci_can_wakeup(struct pci_dev *dev)
{
	return pci_platform_pm ? pci_platform_pm->can_wakeup(dev) : false;
}

static inline int platform_pci_sleep_wake(struct pci_dev *dev, bool enable)
{
	return pci_platform_pm ?
		pci_platform_pm->sleep_wake(dev, enable) : -ENODEV;
}

static inline int platform_pci_run_wake(struct pci_dev *dev, bool enable)
{
	return pci_platform_pm ?
		pci_platform_pm->run_wake(dev, enable) : -ENODEV;
}

/**
 * pci_raw_set_power_state - Use PCI PM registers to set the power state of
 *                           given PCI device
 * @dev: PCI device to handle.
 * @state: PCI power state (D0, D1, D2, D3hot) to put the device into.
 *
 * RETURN VALUE:
 * -EINVAL if the requested state is invalid.
 * -EIO if device does not support PCI PM or its PM capabilities register has a
 * wrong version, or device doesn't support the requested state.
 * 0 if device already is in the requested state.
 * 0 if device's power state has been successfully changed.
 */
static int pci_raw_set_power_state(struct pci_dev *dev, pci_power_t state)
{
	u16 pmcsr;
	bool need_restore = false;

	/* Check if we're already there */
	if (dev->current_state == state)
		return 0;

	if (!dev->pm_cap)
		return -EIO;

	if (state < PCI_D0 || state > PCI_D3hot)
		return -EINVAL;

	/* Validate current state:
	 * Can enter D0 from any state, but if we can only go deeper 
	 * to sleep if we're already in a low power state
	 */
	if (state != PCI_D0 && dev->current_state <= PCI_D3cold
	    && dev->current_state > state) {
		dev_err(&dev->dev, "invalid power transition "
			"(from state %d to %d)\n", dev->current_state, state);
		return -EINVAL;
	}

	/* check if this device supports the desired state */
	if ((state == PCI_D1 && !dev->d1_support)
	   || (state == PCI_D2 && !dev->d2_support))
		return -EIO;

	pci_read_config_word(dev, dev->pm_cap + PCI_PM_CTRL, &pmcsr);

	/* If we're (effectively) in D3, force entire word to 0.
	 * This doesn't affect PME_Status, disables PME_En, and
	 * sets PowerState to 0.
	 */
	switch (dev->current_state) {
	case PCI_D0:
	case PCI_D1:
	case PCI_D2:
		pmcsr &= ~PCI_PM_CTRL_STATE_MASK;
		pmcsr |= state;
		break;
	case PCI_D3hot:
	case PCI_D3cold:
	case PCI_UNKNOWN: /* Boot-up */
		if ((pmcsr & PCI_PM_CTRL_STATE_MASK) == PCI_D3hot
		 && !(pmcsr & PCI_PM_CTRL_NO_SOFT_RESET))
			need_restore = true;
		/* Fall-through: force to D0 */
	default:
		pmcsr = 0;
		break;
	}

	/* enter specified state */
	pci_write_config_word(dev, dev->pm_cap + PCI_PM_CTRL, pmcsr);

	/* Mandatory power management transition delays */
	/* see PCI PM 1.1 5.6.1 table 18 */
	if (state == PCI_D3hot || dev->current_state == PCI_D3hot)
		pci_dev_d3_sleep(dev);
	else if (state == PCI_D2 || dev->current_state == PCI_D2)
		udelay(PCI_PM_D2_DELAY);

	pci_read_config_word(dev, dev->pm_cap + PCI_PM_CTRL, &pmcsr);
	dev->current_state = (pmcsr & PCI_PM_CTRL_STATE_MASK);
	if (dev->current_state != state && printk_ratelimit())
		dev_info(&dev->dev, "Refused to change power state, "
			"currently in D%d\n", dev->current_state);

	/*
	 * According to section 5.4.1 of the "PCI BUS POWER MANAGEMENT
	 * INTERFACE SPECIFICATION, REV. 1.2", a device transitioning
	 * from D3hot to D0 _may_ perform an internal reset, thereby
	 * going to "D0 Uninitialized" rather than "D0 Initialized".
	 * For example, at least some versions of the 3c905B and the
	 * 3c556B exhibit this behaviour.
	 *
	 * At least some laptop BIOSen (e.g. the Thinkpad T21) leave
	 * devices in a D3hot state at boot.  Consequently, we need to
	 * restore at least the BARs so that the device will be
	 * accessible to its driver.
	 */
	if (need_restore)
		pci_restore_bars(dev);

	if (dev->bus->self)
		pcie_aspm_pm_state_change(dev->bus->self);

	return 0;
}

/**
 * pci_update_current_state - Read PCI power state of given device from its
 *                            PCI PM registers and cache it
 * @dev: PCI device to handle.
 * @state: State to cache in case the device doesn't have the PM capability
 */
void pci_update_current_state(struct pci_dev *dev, pci_power_t state)
{
	if (dev->pm_cap) {
		u16 pmcsr;

		/*
		 * Configuration space is not accessible for device in
		 * D3cold, so just keep or set D3cold for safety
		 */
		if (dev->current_state == PCI_D3cold)
			return;
		if (state == PCI_D3cold) {
			dev->current_state = PCI_D3cold;
			return;
		}
		pci_read_config_word(dev, dev->pm_cap + PCI_PM_CTRL, &pmcsr);
		dev->current_state = (pmcsr & PCI_PM_CTRL_STATE_MASK);
	} else {
		dev->current_state = state;
	}
}

/**
 * pci_power_up - Put the given device into D0 forcibly
 * @dev: PCI device to power up
 */
void pci_power_up(struct pci_dev *dev)
{
	if (platform_pci_power_manageable(dev))
		platform_pci_set_power_state(dev, PCI_D0);

	pci_raw_set_power_state(dev, PCI_D0);
	pci_update_current_state(dev, PCI_D0);
}

/**
 * pci_platform_power_transition - Use platform to change device power state
 * @dev: PCI device to handle.
 * @state: State to put the device into.
 */
static int pci_platform_power_transition(struct pci_dev *dev, pci_power_t state)
{
	int error;

	if (platform_pci_power_manageable(dev)) {
		error = platform_pci_set_power_state(dev, state);
		if (!error)
			pci_update_current_state(dev, state);
		/* Fall back to PCI_D0 if native PM is not supported */
		if (!dev->pm_cap)
			dev->current_state = PCI_D0;
	} else {
		error = -ENODEV;
		/* Fall back to PCI_D0 if native PM is not supported */
		if (!dev->pm_cap)
			dev->current_state = PCI_D0;
	}

	return error;
}

/**
 * __pci_start_power_transition - Start power transition of a PCI device
 * @dev: PCI device to handle.
 * @state: State to put the device into.
 */
static void __pci_start_power_transition(struct pci_dev *dev, pci_power_t state)
{
	if (state == PCI_D0) {
		pci_platform_power_transition(dev, PCI_D0);
		/*
		 * Mandatory power management transition delays, see
		 * PCI Express Base Specification Revision 2.0 Section
		 * 6.6.1: Conventional Reset.  Do not delay for
		 * devices powered on/off by corresponding bridge,
		 * because have already delayed for the bridge.
		 */
		if (dev->runtime_d3cold) {
			msleep(dev->d3cold_delay);
			/*
			 * When powering on a bridge from D3cold, the
			 * whole hierarchy may be powered on into
			 * D0uninitialized state, resume them to give
			 * them a chance to suspend again
			 */
			pci_wakeup_bus(dev->subordinate);
		}
	}
}

/**
 * __pci_dev_set_current_state - Set current state of a PCI device
 * @dev: Device to handle
 * @data: pointer to state to be set
 */
static int __pci_dev_set_current_state(struct pci_dev *dev, void *data)
{
	pci_power_t state = *(pci_power_t *)data;

	dev->current_state = state;
	return 0;
}

/**
 * __pci_bus_set_current_state - Walk given bus and set current state of devices
 * @bus: Top bus of the subtree to walk.
 * @state: state to be set
 */
static void __pci_bus_set_current_state(struct pci_bus *bus, pci_power_t state)
{
	if (bus)
		pci_walk_bus(bus, __pci_dev_set_current_state, &state);
}

/**
 * __pci_complete_power_transition - Complete power transition of a PCI device
 * @dev: PCI device to handle.
 * @state: State to put the device into.
 *
 * This function should not be called directly by device drivers.
 */
int __pci_complete_power_transition(struct pci_dev *dev, pci_power_t state)
{
	int ret;

	if (state <= PCI_D0)
		return -EINVAL;
	ret = pci_platform_power_transition(dev, state);
	/* Power off the bridge may power off the whole hierarchy */
	if (!ret && state == PCI_D3cold)
		__pci_bus_set_current_state(dev->subordinate, PCI_D3cold);
	return ret;
}
EXPORT_SYMBOL_GPL(__pci_complete_power_transition);

/**
 * pci_set_power_state - Set the power state of a PCI device
 * @dev: PCI device to handle.
 * @state: PCI power state (D0, D1, D2, D3hot) to put the device into.
 *
 * Transition a device to a new power state, using the platform firmware and/or
 * the device's PCI PM registers.
 *
 * RETURN VALUE:
 * -EINVAL if the requested state is invalid.
 * -EIO if device does not support PCI PM or its PM capabilities register has a
 * wrong version, or device doesn't support the requested state.
 * 0 if device already is in the requested state.
 * 0 if device's power state has been successfully changed.
 */
int pci_set_power_state(struct pci_dev *dev, pci_power_t state)
{
	int error;

	/* bound the state we're entering */
	if (state > PCI_D3cold)
		state = PCI_D3cold;
	else if (state < PCI_D0)
		state = PCI_D0;
	else if ((state == PCI_D1 || state == PCI_D2) && pci_no_d1d2(dev))
		/*
		 * If the device or the parent bridge do not support PCI PM,
		 * ignore the request if we're doing anything other than putting
		 * it into D0 (which would only happen on boot).
		 */
		return 0;

	/* Check if we're already there */
	if (dev->current_state == state)
		return 0;

	__pci_start_power_transition(dev, state);

	/* This device is quirked not to be put into D3, so
	   don't put it in D3 */
	if (state >= PCI_D3hot && (dev->dev_flags & PCI_DEV_FLAGS_NO_D3))
		return 0;

	/*
	 * To put device in D3cold, we put device into D3hot in native
	 * way, then put device into D3cold with platform ops
	 */
	error = pci_raw_set_power_state(dev, state > PCI_D3hot ?
					PCI_D3hot : state);

	if (!__pci_complete_power_transition(dev, state))
		error = 0;
	/*
	 * When aspm_policy is "powersave" this call ensures
	 * that ASPM is configured.
	 */
	if (!error && dev->bus->self)
		pcie_aspm_powersave_config_link(dev->bus->self);

	return error;
}

static int do_pci_enable_device(struct pci_dev *dev, int bars)
{
	int err;

	err = pci_set_power_state(dev, PCI_D0);
	if (err < 0 && err != -EIO)
		return err;
	err = pcibios_enable_device(dev, bars);
	if (err < 0)
		return err;
	pci_fixup_device(pci_fixup_enable, dev);

	return 0;
}

static int __pci_enable_device_flags(struct pci_dev *dev,
				     resource_size_t flags)
{
	int err;
	int i, bars = 0;

	/*
	 * Power state could be unknown at this point, either due to a fresh
	 * boot or a device removal call.  So get the current power state
	 * so that things like MSI message writing will behave as expected
	 * (e.g. if the device really is in D0 at enable time).
	 */
	if (dev->pm_cap) {
		u16 pmcsr;
		pci_read_config_word(dev, dev->pm_cap + PCI_PM_CTRL, &pmcsr);
		dev->current_state = (pmcsr & PCI_PM_CTRL_STATE_MASK);
	}

	if (atomic_add_return(1, &dev->enable_cnt) > 1)
		return 0;		/* already enabled */

	/* only skip sriov related */
	for (i = 0; i <= PCI_ROM_RESOURCE; i++)
		if (dev->resource[i].flags & flags)
			bars |= (1 << i);
	for (i = PCI_BRIDGE_RESOURCES; i < DEVICE_COUNT_RESOURCE; i++)
		if (dev->resource[i].flags & flags)
			bars |= (1 << i);

	err = do_pci_enable_device(dev, bars);
	if (err < 0)
		atomic_dec(&dev->enable_cnt);
	return err;
}

/**
 * pci_enable_device - Initialize device before it's used by a driver.
 * @dev: PCI device to be initialized
 *
 *  Initialize device before it's used by a driver. Ask low-level code
 *  to enable I/O and memory. Wake up the device if it was suspended.
 *  Beware, this function can fail.
 *
 */
DLLEXPORT int pci_enable_device(struct pci_dev *dev)
{
	return __pci_enable_device_flags(dev, IORESOURCE_MEM | IORESOURCE_IO);
}

DLLEXPORT int pcim_enable_device(struct pci_dev *pdev)
{
	return pci_enable_device(pdev);
}

static void __pci_set_master(struct pci_dev *dev, bool enable)
{
	u16 old_cmd, cmd;

	pci_read_config_word(dev, PCI_COMMAND, &old_cmd);
	if (enable)
		cmd = old_cmd | PCI_COMMAND_MASTER;
	else
		cmd = old_cmd & ~PCI_COMMAND_MASTER;
	if (cmd != old_cmd) {
		dev_dbg(&dev->dev, "%s bus mastering\n",
			enable ? "enabling" : "disabling");
		pci_write_config_word(dev, PCI_COMMAND, cmd);
	}
	dev->is_busmaster = enable;
}

/**
 * pci_set_master - enables bus-mastering for device dev
 * @dev: the PCI device to enable
 *
 * Enables bus-mastering on the device and calls pcibios_set_master()
 * to do the needed arch specific settings.
 */
DLLEXPORT void pci_set_master(struct pci_dev *dev)
{
	__pci_set_master(dev, true);
	pcibios_set_master(dev);
}

/*  pci_intx - enables/disables PCI INTx for device dev */
DLLEXPORT void pci_intx(struct pci_dev *pdev, int enable)
{
	u16 pci_command, new;

	pci_read_config_word(pdev, PCI_COMMAND, &pci_command);

	if (enable) {
		new = pci_command & ~PCI_COMMAND_INTX_DISABLE;
	} else {
		new = pci_command | PCI_COMMAND_INTX_DISABLE;
	}

	if (new != pci_command) {
		//struct pci_devres *dr;

		pci_write_config_word(pdev, PCI_COMMAND, new);

		//dr = find_pci_dr(pdev);
		//if (dr && !dr->restore_intx) {
		//	dr->restore_intx = 1;
		//	dr->orig_intx = !enable;
		//}
	}
}

static void do_pci_disable_device(struct pci_dev *dev)
{
	u16 pci_command;

	pci_read_config_word(dev, PCI_COMMAND, &pci_command);
	if (pci_command & PCI_COMMAND_MASTER) {
		pci_command &= ~PCI_COMMAND_MASTER;
		pci_write_config_word(dev, PCI_COMMAND, pci_command);
	}

	pcibios_disable_device(dev); 
}

/**
 * pci_disable_device - Disable PCI device after use
 * @dev: PCI device to be disabled
 *
 * Signal to the system that the PCI device is not in use by the system
 * anymore.  This only involves disabling PCI bus-mastering, if active.
 *
 * Note we don't actually disable the device until all callers of
 * pci_enable_device() have called pci_disable_device().
 */
DLLEXPORT void pci_disable_device(struct pci_dev *dev)
{
	if (atomic_sub_return(1, &dev->enable_cnt) != 0)
		return;

	do_pci_disable_device(dev);

	dev->is_busmaster = 0; 
}

/**
 * pci_wakeup - Wake up a PCI device
 * @dev: Device to handle.
 * @ign: ignored parameter
 */
static int pci_wakeup(struct pci_dev *pci_dev, void *ign)
{
// 	pci_wakeup_event(pci_dev);
// 	pm_request_resume(&pci_dev->dev);
	return 0;
}

/**
 * pci_wakeup_bus - Walk given bus and wake up devices on it
 * @bus: Top bus of the subtree to walk.
 */
void pci_wakeup_bus(struct pci_bus *bus)
{
	if (bus)
		pci_walk_bus(bus, pci_wakeup, NULL);
}

/**
 * pci_resource_bar - get position of the BAR associated with a resource
 * @dev: the PCI device
 * @resno: the resource number
 * @type: the BAR type to be filled in
 *
 * Returns BAR position in config space, or 0 if the BAR is invalid.
 */
int pci_resource_bar(struct pci_dev *dev, int resno, enum pci_bar_type *type)
{
	int reg;

	if (resno < PCI_ROM_RESOURCE) {
		*type = pci_bar_unknown;
		return PCI_BASE_ADDRESS_0 + 4 * resno;
	} else if (resno == PCI_ROM_RESOURCE) {
		*type = pci_bar_mem32;
		return dev->rom_base_reg;
	} else if (resno < PCI_BRIDGE_RESOURCES) {
		/* device specific resource */
		reg = pci_iov_resource_bar(dev, resno, type);
		if (reg)
			return reg;
	}

	dev_err(&dev->dev, "BAR %d: invalid resource\n", resno);
	return 0;
}
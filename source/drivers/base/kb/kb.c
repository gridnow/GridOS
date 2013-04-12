#include <types.h>
#include <asm/io.h>

#include <hal_irq.h>
#include <ddk/ifi.h>

#define KBD_INIT_TIMEOUT	3000
/*
*	Keyboard Controller Registers on normal PCs.
*/
#define KBD_STATUS_REG		0x64	/* Status register (R) */
#define KBD_CNTL_REG		0x64	/* Controller command register (W) */
# define KBD_DATA_REG		0x60	/* Keyboard data register (R/W) */

/* How to access the keyboard macros on this platform.  */
#define kbd_read_input() inb(KBD_DATA_REG)
#define kbd_read_status() inb(KBD_STATUS_REG)
#define kbd_write_output(val) outb(val, KBD_DATA_REG)
#define kbd_write_command(val) outb(val, KBD_CNTL_REG)


/*
 *	Keyboard Controller Commands
 */

#define KBD_CCMD_READ_MODE	0x20	/* Read mode bits */
#define KBD_CCMD_WRITE_MODE	0x60	/* Write mode bits */
#define KBD_CCMD_GET_VERSION	0xA1	/* Get controller version */
#define KBD_CCMD_MOUSE_DISABLE	0xA7	/* Disable mouse interface */
#define KBD_CCMD_MOUSE_ENABLE	0xA8	/* Enable mouse interface */
#define KBD_CCMD_TEST_MOUSE	0xA9	/* Mouse interface test */
#define KBD_CCMD_SELF_TEST	0xAA	/* Controller self test */
#define KBD_CCMD_KBD_TEST	0xAB	/* Keyboard interface test */
#define KBD_CCMD_KBD_DISABLE	0xAD	/* Keyboard interface disable */
#define KBD_CCMD_KBD_ENABLE	0xAE	/* Keyboard interface enable */
#define KBD_CCMD_WRITE_AUX_OBUF	0xD3    /* Write to output buffer as if
					   initiated by the auxiliary device */
#define KBD_CCMD_WRITE_MOUSE	0xD4	/* Write the following byte to the mouse */

/*
 *	Keyboard Commands
 */
#define KBD_CMD_SET_LEDS	0xED	/* Set keyboard leds */
#define KBD_CMD_SET_RATE	0xF3	/* Set typematic rate */
#define KBD_CMD_ENABLE		0xF4	/* Enable scanning */
#define KBD_CMD_DISABLE		0xF5	/* Disable scanning */
#define KBD_CMD_RESET		0xFF	/* Reset */
#define 	LED_SCR		0x01	/* scroll lock led */
#define 	LED_CAP		0x04	/* caps lock led */
#define 	LED_NUM		0x02	/* num lock led */
/*
 *	Keyboard Replies
 */

#define KBD_REPLY_POR		0xAA	/* Power on reset */
#define KBD_REPLY_ACK		0xFA	/* Command ACK */
#define KBD_REPLY_RESEND	0xFE	/* Command NACK, send the cmd again */

/*
 *	Status Register Bits
 */

#define KBD_STAT_OBF 		0x01	/* Keyboard output buffer full */
#define KBD_STAT_IBF 		0x02	/* Keyboard input buffer full */
#define KBD_STAT_SELFTEST	0x04	/* Self test successful */
#define KBD_STAT_CMD		0x08	/* Last write was a command write (0=data) */
#define KBD_STAT_UNLOCKED	0x10	/* Zero if keyboard locked */
#define KBD_STAT_MOUSE_OBF	0x20	/* Mouse output buffer full */
#define KBD_STAT_GTO 		0x40	/* General receive/xmit timeout */
#define KBD_STAT_PERR 		0x80	/* Parity error */

#define AUX_STAT_OBF (KBD_STAT_OBF | KBD_STAT_MOUSE_OBF)

/*
 *	Controller Mode Register Bits
 */

#define KBD_MODE_KBD_INT	0x01	/* Keyboard data generate IRQ1 */
#define KBD_MODE_MOUSE_INT	0x02	/* Mouse data generate IRQ12 */
#define KBD_MODE_SYS 		0x04	/* The system flag (?) */
#define KBD_MODE_NO_KEYLOCK	0x08	/* The keylock doesn't affect the keyboard if set */
#define KBD_MODE_DISABLE_KBD	0x10	/* Disable keyboard interface */
#define KBD_MODE_DISABLE_MOUSE	0x20	/* Disable mouse interface */
#define KBD_MODE_KCC 		0x40	/* Scan code conversion to PC format */
#define KBD_MODE_RFU		0x80

/*
 *	Mouse Commands
 */

#define AUX_SET_RES		0xE8	/* Set resolution */
#define AUX_SET_SCALE11		0xE6	/* Set 1:1 scaling */
#define AUX_SET_SCALE21		0xE7	/* Set 2:1 scaling */
#define AUX_GET_SCALE		0xE9	/* Get scaling factor */
#define AUX_SET_STREAM		0xEA	/* Set stream mode */
#define AUX_SET_SAMPLE		0xF3	/* Set sample rate */
#define AUX_ENABLE_DEV		0xF4	/* Enable aux device */
#define AUX_DISABLE_DEV		0xF5	/* Disable aux device */
#define AUX_RESET		0xFF	/* Reset aux device */
#define AUX_ACK			0xFA	/* Command byte ACK. */

#define AUX_BUF_SIZE		2048	/* This might be better divisible by
					   three to make overruns stay in sync
					   but then the read function would need
					   a lock etc - ack */
#define AUX_INTS_OFF (KBD_MODE_KCC | KBD_MODE_DISABLE_MOUSE | KBD_MODE_SYS | KBD_MODE_KBD_INT)
#define AUX_INTS_ON  (KBD_MODE_KCC | KBD_MODE_SYS | KBD_MODE_MOUSE_INT | KBD_MODE_KBD_INT)

/* IRQ number */
#define ARCH_KBD_IRQ	1
#define ARCH_MOUSE_IRQ	12

static void kbd_write_command_w(int data)
{
	int status;
	do 
	{
		status = kbd_read_status();
	} while (status & KBD_STAT_IBF);
	kbd_write_command((unsigned char)data);

}

static void kbd_write_output_w(int data)
{
	int status;

	do {
		status = kbd_read_status();
	} while (status & KBD_STAT_IBF);
	kbd_write_output((unsigned char)data);
}

static void kbd_write_cmd(int cmd)
{
	kbd_write_command_w(KBD_CCMD_WRITE_MODE);
	kbd_write_output_w(cmd);
}
#define KBD_NO_DATA		(-1)				/* No data */
#define KBD_BAD_DATA	(-2)				/* Parity or other error */
static int kbd_read_data(void)
{
	int retval = KBD_NO_DATA;
	unsigned char status;

	status = kbd_read_status();
	if (status & KBD_STAT_OBF)
	{
		unsigned char data = kbd_read_input();

		retval = data;
		if (status & (KBD_STAT_GTO | KBD_STAT_PERR))
			retval = KBD_BAD_DATA;
	}
	return retval;
}

static int kbd_wait_for_input(void)
{
	long timeout = KBD_INIT_TIMEOUT;

	do {
		int retval = kbd_read_data();
		if (retval >= 0)
			return retval;
// 		mdelay(1);
	} while (--timeout);
	return KBD_NO_DATA;
}

unsigned char handle_kbd_event(int irq, void * dev)
{
	unsigned char status = kbd_read_status();
	unsigned int work = 100000;
	unsigned char scancode;
	unsigned char ret=0;

	/* Loop the status to read the data */
	while (status & KBD_STAT_OBF) 
	{
		if (status & KBD_STAT_MOUSE_OBF)
		{
			/* Mouse */
		}
		else 
		{
			scancode = kbd_read_input();
			ifi_input_stream(dev, &scancode, 1);

			/* Next status */
			status = kbd_read_status();
		}		
	}

	return ret;
}

static void init_keyboard()
{
	int times;

	/*
	* Test the keyboard interface.
	* This seems to be the only way to get it going.
	* If the test is successful a x55 is placed in the input buffer.
	*/
	//kbd_write_command_w(KBD_CCMD_SELF_TEST);

	/*
	* Perform a keyboard interface test.  This causes the controller
	* to test the keyboard clock and data lines.  The results of the
	* test are placed in the input buffer.
	*/
	kbd_write_command_w(KBD_CCMD_KBD_TEST);

	/*
	* Enable the keyboard by allowing the keyboard clock to run.
	*/
	kbd_write_command_w(KBD_CCMD_KBD_ENABLE);

	/*
	* Reset keyboard. If the read times out
	* then the assumption is that no keyboard is
	* plugged into the machine.
	* This defaults the keyboard to scan-code set 2.
	*
	* Set up to try again if the keyboard asks for RESEND.
	*/
	times = 3;
	do {
		int status ;
	
		kbd_write_output_w(KBD_CMD_RESET);

		status = kbd_wait_for_input();
		if (status == KBD_REPLY_ACK){
			break;
		}

// 		if (status != KBD_REPLY_RESEND)
// 			return 2;

		times --;
	} while (times>0);

	kbd_write_command_w(KBD_CCMD_WRITE_MODE);
	kbd_write_output_w(KBD_MODE_KBD_INT
		| KBD_MODE_SYS
		| KBD_MODE_DISABLE_MOUSE
		| KBD_MODE_KCC);
}

void start_kbd()
{	
	/* Create an IFI device */
	struct ifi_device * dev = ifi_device_create("KBD");

	/* Request irq */
	hal_irq_request(ARCH_KBD_IRQ, handle_kbd_event, 0, "hal_ps2kbd", dev);

	init_keyboard();
}
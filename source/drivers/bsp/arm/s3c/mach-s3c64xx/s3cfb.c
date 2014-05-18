/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   Ben Dooks <ben@simtec.co.uk>
 *   S3C64XX frame buffer 
 *
 */
#include <stddef.h>

#include <ddk/compiler.h>
#include <ddk/types.h>
#include <mach/arch.h>
#include <mach/map.h>

#include <mach-types.h>
#include <plat/samsung_fimd.h>
#include "common.h"
#include <screen.h>
#include <asm/io.h>
#include "plat/map-base.h"
#include "mach/map_s3c.h"
#include "mach/regs-gpio.h"
#include "mach/regs-modem.h"
#include "mach/regs-clock.h"
//#include "mach/resgs-lcd.h"

#define  GPICON S3C64XX_GPI_BASE
#define  GPIPUD (S3C64XX_GPI_BASE + 0x08)

#define  GPJCON S3C64XX_GPJ_BASE
#define  GPJPUD (S3C64XX_GPJ_BASE + 0x08)

#define  S3C_VIDCON0 S3C_VA_LCD
#define  S3C_VIDCON1 (S3C_VA_LCD + 0x4)
#define  S3C_VIDCON2 (S3C_VA_LCD + 0x8)
#define  S3C_VIDTCON0 (S3C_VA_LCD + 0x10)
#define  S3C_VIDTCON1 (S3C_VA_LCD + 0x14)
#define  S3C_VIDTCON2 (S3C_VA_LCD + 0x18)
#define	 S3C_WINCON0  (S3C_VA_LCD + 0x20)
#define  S3C_VIDOSD0A  (S3C_VA_LCD + 0x40)
#define  S3C_VIDOSD0B  (S3C_VA_LCD + 0x44)
#define  S3C_VIDOSD0C  (S3C_VA_LCD + 0x48)
#define  S3C_VIDW00ADD0B0 (S3C_VA_LCD + 0xa0)
#define  S3C_VIDW00ADD1B0 (S3C_VA_LCD + 0xd0)
#define  S3C_VIDW00ADD2 (S3C_VA_LCD + 0x100)
#define  S3C_DITHMODE (S3C_VA_LCD + 0x170)




#define VIDCON0_INTERLACE_F (0 << 29)/*  渐行  */
#define VIDCON0_VIDOUT (0 << 26)		/*  RGB */
#define VIDCON0_L1_DATA16 (0 << 23)/*I80 (LDI1)输出16bit模式*/
#define VIDCON0_L0_DATA16 (0 << 20)/*I80 (LDI0)输出16bit模式*/

#define VIDCON0_PNRMODE (0 << 17)/*显示格式 RGB格式*/
//#define VIDCON0_CLKVALUP (1 << 16)/*选择CLKVAL_F 更新时序控制 ,帧开始时控制*/
//#define VIDCON0_CLKVAL_F (3 << 6)/*时钟分频,最大66MHz */
#define VIDCON0_VCLKFREE (1 << 5)/*VCLK 自由运行控制（只有在RGB IF 模式下有效）:自由运行模式*/
//#define VIDCON0_CLKDIR (1 << 4)/*用CLKVAL_F 寄存器划分时钟源*/
#define VIDCON0_CLKSEL_F (0 << 2)/*选择时钟源,HCLK = 133MHz*/
//#define VIDCON0_ENVID (0 << 1)/*视频输出及显示控制信号禁止*/
//#define VIDCON0_ENVID_F (0 << 0)/*视频输出及显示控制信号禁止*/

#define VIDCON1_IVCLK (1 << 7)/*时钟上升沿有效*/
#define VIDCON1_IHSYNC (1 << 6)/*HSYNC反向*/
#define VIDCON1_IVSYNC (1 << 5)/*VSYNC反向*/
#define VIDCON1_IVDEN (0 << 4)/*DE信号不反向*/

#define LCD_WIDTH 480
#define LCD_HIGH  272
static u32 lcd_buffer[LCD_HIGH][LCD_WIDTH] = {{1}};

static void lcd_disable(void)  
{  
	u32 tmp = 0;
	tmp = __raw_readl(S3C_VIDCON0);
    tmp &= (~(VIDCON0_ENVID | VIDCON0_ENVID_F));  
	__raw_writel(tmp,S3C_VIDCON0);

	tmp = __raw_readl(S3C_WINCON0);
	tmp &= ~(0x1);
	__raw_writel(tmp,S3C_WINCON0);
	return ; 
}  

static void lcd_enable(void)  
{  
	u32 tmp = 0;
	tmp = __raw_readl(S3C_VIDCON0);
    tmp |= VIDCON0_ENVID | VIDCON0_ENVID_F;  
	__raw_writel(tmp,S3C_VIDCON0);

	tmp = __raw_readl(S3C_WINCON0);
	tmp |= 0x1;
	__raw_writel(tmp,S3C_WINCON0);
	return ; 
}  

static void lcd_clr(void)
{
	int i = 0;
	int j = 0;
	
	for(i = 0; i < LCD_HIGH; i++)  
	{  
		for(j = 0; j < LCD_WIDTH; j++)	
		{
			u8 r = 0x0;
			u8 g = 0x0;
			u8 b = 0x0;
			u32 co = ((r << 16) | (g << 8) | b);
			lcd_buffer[i][j] = co;  
	   }  
	} 
	return ;
}

void s3cfb_init_lcd(void)
{

	unsigned long vidcon0;
	unsigned long vidcon1;
	unsigned long vidcon2;
	unsigned long vidtcon0;
	unsigned long vidtcon1;
	unsigned long vidtcon2;
	unsigned long wincon0;
	unsigned long val;
	unsigned long phy;
	int i;
	
	/*
		 将GPIO 设置为VD0 ~15,RGB输出 
		 每两位对应一个端口，设置10将端口用于传输video data
	*/		
	__raw_writel(0xaaaaaaaa, GPICON);
	__raw_writel(0x0,GPIPUD);//屏蔽下拉电平
	
	__raw_writel(0xaaaaaaaa, GPJCON);// 管脚设置为VD16 ~23,HSYNC VSYNC VDEN VCLK
	__raw_writel(0x0, GPJPUD);//屏蔽下拉电平
	

	/* remove the lcd bypass */
	/* lcd重启后默认bypass模式，不经过控制器;再次设置为normal模式*/
	val = __raw_readl(S3C64XX_MODEM_MIFPCON);
	val &= ~MIFPCON_LCD_BYPASS;
	__raw_writel(val, S3C64XX_MODEM_MIFPCON);
	
	/* set the LCD type RGB */
	val = __raw_readl(S3C64XX_SPCON);
	val &= ~S3C64XX_SPCON_LCD_SEL_MASK;
	val |= S3C64XX_SPCON_LCD_SEL_RGB;
	__raw_writel(val, S3C64XX_SPCON);
	
	/* disable */
	lcd_disable();
	
	/* 
		configure Video output format 
		根据6410手册及linux源码设置，部分设置目前找不到原因
	*/
	vidcon0 = __raw_readl(S3C_VIDCON0);
	/* 初始化video output format 时钟源HCLK 频率= 时钟源 /（CLKVAL+1） CLKVAL>=1 */
	vidcon0 =  VIDCON0_INTERLACE_F | VIDCON0_VIDOUT | VIDCON0_L1_DATA16 | VIDCON0_L0_DATA16 | VIDCON0_PNRMODE | VIDCON0_CLKVALUP | VIDCON0_CLKVAL_F(4) | VIDCON0_VCLKFREE | VIDCON0_CLKDIR | VIDCON0_CLKSEL_F;
	__raw_writel(vidcon0, S3C_VIDCON0);

  /*
  	bit[7]ivclk=0下降沿触发，VCLK下降沿是获取数据
  	bit[6]ihsync = 1行同步信号要反向，LCD需要低脉冲,而6410提供高脉冲  	
  */
	vidcon1 = __raw_readl(S3C_VIDCON1);
	vidcon1 = VIDCON1_IHSYNC | VIDCON1_IVSYNC | VIDCON1_IVDEN;
	__raw_writel(vidcon1, S3C_VIDCON1);


	vidcon2 = 0;
	__raw_writel(vidcon2, S3C_VIDCON2);

	/* 
	  设置时序,查6410手册及LCD手册的时序图
	  VBPD:在垂直同步信号后，有效行之前(一帧数据的首航)，所要延迟的时间，以行同步信号为单位，4.3寸lcd一般是2
	  VFPD:在有效行之后（一帧数据的末行），垂直同步信号之前，所要延迟的时间，以行同步信号为单位，4.3寸lcd一般是2
	  VSPW:垂直同步信号宽度，单位为行，4.3手册10
	*/
	vidtcon0 = VIDTCON0_VBPD(2-1) | VIDTCON0_VFPD(2-1) | VIDTCON0_VSPW(10-1);
	__raw_writel(vidtcon0, S3C_VIDTCON0);

	/* 
	  设置时序,查6410手册及LCD手册的时序图
	  HBPD:在水平同步信号后，行有效数据前延迟的时间 以clk周期数计算，4.3lcd手册上2
	  HFPD：在有行有效数据后，水平同步信号之前延迟的时间，以clk周期数计算  4.3lcd手册上2
	  HSPW：水平同步信号时间，以clk周期计算 4.3lcd手册上41
	*/
	vidtcon1 = VIDTCON1_VFPDE(0) | VIDTCON1_HBPD(2-1) | VIDTCON1_HFPD(2-1) | VIDTCON1_HSPW(41-1);
	__raw_writel(vidtcon1, S3C_VIDTCON1);

	/* 
		size of display
		根据手册：HOZVAL = (Horizontal display size) -1； 
							LINEVAL = (Vertical display size) –1 
	*/
	vidtcon2 = VIDTCON2_LINEVAL(LCD_HIGH - 1) | VIDTCON2_HOZVAL(LCD_WIDTH - 1);
	__raw_writel(vidtcon2, S3C_VIDTCON2);

	/*  24bpp */
	wincon0 = 11 << 2;
	__raw_writel(wincon0, S3C_WINCON0);
	
	/* 屏幕左上角坐标(0,0) */
	__raw_writel((0<<11)|(0<<0), S3C_VIDOSD0A);
	
	/* 屏幕右下角坐标 */
	__raw_writel(((LCD_WIDTH-1)<<11) | ((LCD_HIGH-1)<<0), S3C_VIDOSD0B);
	
	/* window size  high*width */
	__raw_writel((LCD_WIDTH* LCD_HIGH), S3C_VIDOSD0C); /* 0 - 23位有效*/
	
	/* buffer的物理首地址 */
	phy = HAL_GET_BASIC_PHYADDRESS(lcd_buffer);
	__raw_writel((u32)phy, S3C_VIDW00ADD0B0);
	
	/* buffer的物理末地址 */
	__raw_writel(((u32)phy + LCD_WIDTH* LCD_HIGH* 4 ) & 0xffffff, S3C_VIDW00ADD1B0);
	
	/* 
		本行首与上一行尾之间的offset | 行宽 
		指buffer中的地址相差，以字节计
	*/
	__raw_writel((0 << 13)|(LCD_WIDTH* 4 ), S3C_VIDW00ADD2);
	
	/* 禁止抖动 */
	__raw_writel((2 << 5) |( 2 << 1 )| (1 << 3) | (0<< 0 ), S3C_DITHMODE);
	
	hal_fb_register_simple(((u32)lcd_buffer), LCD_WIDTH, LCD_HIGH, 32, 0);//24bpp,but set 32 for video_draw_pixel()

	lcd_enable();
	
	for(i = 0; i < 50000; i ++);	/* nop */

	/* enable clock to LCD */
	val = __raw_readl(S3C_HCLK_GATE);
	val |= S3C_CLKCON_HCLK_LCD;
	__raw_writel(val, S3C_HCLK_GATE);

	
	lcd_clr();
    return;
}



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




#define VIDCON0_INTERLACE_F (0 << 29)/*  ����  */
#define VIDCON0_VIDOUT (0 << 26)		/*  RGB */
#define VIDCON0_L1_DATA16 (0 << 23)/*I80 (LDI1)���16bitģʽ*/
#define VIDCON0_L0_DATA16 (0 << 20)/*I80 (LDI0)���16bitģʽ*/

#define VIDCON0_PNRMODE (0 << 17)/*��ʾ��ʽ RGB��ʽ*/
//#define VIDCON0_CLKVALUP (1 << 16)/*ѡ��CLKVAL_F ����ʱ����� ,֡��ʼʱ����*/
//#define VIDCON0_CLKVAL_F (3 << 6)/*ʱ�ӷ�Ƶ,���66MHz */
#define VIDCON0_VCLKFREE (1 << 5)/*VCLK �������п��ƣ�ֻ����RGB IF ģʽ����Ч��:��������ģʽ*/
//#define VIDCON0_CLKDIR (1 << 4)/*��CLKVAL_F �Ĵ�������ʱ��Դ*/
#define VIDCON0_CLKSEL_F (0 << 2)/*ѡ��ʱ��Դ,HCLK = 133MHz*/
//#define VIDCON0_ENVID (0 << 1)/*��Ƶ�������ʾ�����źŽ�ֹ*/
//#define VIDCON0_ENVID_F (0 << 0)/*��Ƶ�������ʾ�����źŽ�ֹ*/

#define VIDCON1_IVCLK (1 << 7)/*ʱ����������Ч*/
#define VIDCON1_IHSYNC (1 << 6)/*HSYNC����*/
#define VIDCON1_IVSYNC (1 << 5)/*VSYNC����*/
#define VIDCON1_IVDEN (0 << 4)/*DE�źŲ�����*/

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
		 ��GPIO ����ΪVD0 ~15,RGB��� 
		 ÿ��λ��Ӧһ���˿ڣ�����10���˿����ڴ���video data
	*/		
	__raw_writel(0xaaaaaaaa, GPICON);
	__raw_writel(0x0,GPIPUD);//����������ƽ
	
	__raw_writel(0xaaaaaaaa, GPJCON);// �ܽ�����ΪVD16 ~23,HSYNC VSYNC VDEN VCLK
	__raw_writel(0x0, GPJPUD);//����������ƽ
	

	/* remove the lcd bypass */
	/* lcd������Ĭ��bypassģʽ��������������;�ٴ�����Ϊnormalģʽ*/
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
		����6410�ֲἰlinuxԴ�����ã���������Ŀǰ�Ҳ���ԭ��
	*/
	vidcon0 = __raw_readl(S3C_VIDCON0);
	/* ��ʼ��video output format ʱ��ԴHCLK Ƶ��= ʱ��Դ /��CLKVAL+1�� CLKVAL>=1 */
	vidcon0 =  VIDCON0_INTERLACE_F | VIDCON0_VIDOUT | VIDCON0_L1_DATA16 | VIDCON0_L0_DATA16 | VIDCON0_PNRMODE | VIDCON0_CLKVALUP | VIDCON0_CLKVAL_F(4) | VIDCON0_VCLKFREE | VIDCON0_CLKDIR | VIDCON0_CLKSEL_F;
	__raw_writel(vidcon0, S3C_VIDCON0);

  /*
  	bit[7]ivclk=0�½��ش�����VCLK�½����ǻ�ȡ����
  	bit[6]ihsync = 1��ͬ���ź�Ҫ����LCD��Ҫ������,��6410�ṩ������  	
  */
	vidcon1 = __raw_readl(S3C_VIDCON1);
	vidcon1 = VIDCON1_IHSYNC | VIDCON1_IVSYNC | VIDCON1_IVDEN;
	__raw_writel(vidcon1, S3C_VIDCON1);


	vidcon2 = 0;
	__raw_writel(vidcon2, S3C_VIDCON2);

	/* 
	  ����ʱ��,��6410�ֲἰLCD�ֲ��ʱ��ͼ
	  VBPD:�ڴ�ֱͬ���źź���Ч��֮ǰ(һ֡���ݵ��׺�)����Ҫ�ӳٵ�ʱ�䣬����ͬ���ź�Ϊ��λ��4.3��lcdһ����2
	  VFPD:����Ч��֮��һ֡���ݵ�ĩ�У�����ֱͬ���ź�֮ǰ����Ҫ�ӳٵ�ʱ�䣬����ͬ���ź�Ϊ��λ��4.3��lcdһ����2
	  VSPW:��ֱͬ���źſ�ȣ���λΪ�У�4.3�ֲ�10
	*/
	vidtcon0 = VIDTCON0_VBPD(2-1) | VIDTCON0_VFPD(2-1) | VIDTCON0_VSPW(10-1);
	__raw_writel(vidtcon0, S3C_VIDTCON0);

	/* 
	  ����ʱ��,��6410�ֲἰLCD�ֲ��ʱ��ͼ
	  HBPD:��ˮƽͬ���źź�����Ч����ǰ�ӳٵ�ʱ�� ��clk���������㣬4.3lcd�ֲ���2
	  HFPD����������Ч���ݺ�ˮƽͬ���ź�֮ǰ�ӳٵ�ʱ�䣬��clk����������  4.3lcd�ֲ���2
	  HSPW��ˮƽͬ���ź�ʱ�䣬��clk���ڼ��� 4.3lcd�ֲ���41
	*/
	vidtcon1 = VIDTCON1_VFPDE(0) | VIDTCON1_HBPD(2-1) | VIDTCON1_HFPD(2-1) | VIDTCON1_HSPW(41-1);
	__raw_writel(vidtcon1, S3C_VIDTCON1);

	/* 
		size of display
		�����ֲ᣺HOZVAL = (Horizontal display size) -1�� 
							LINEVAL = (Vertical display size) �C1 
	*/
	vidtcon2 = VIDTCON2_LINEVAL(LCD_HIGH - 1) | VIDTCON2_HOZVAL(LCD_WIDTH - 1);
	__raw_writel(vidtcon2, S3C_VIDTCON2);

	/*  24bpp */
	wincon0 = 11 << 2;
	__raw_writel(wincon0, S3C_WINCON0);
	
	/* ��Ļ���Ͻ�����(0,0) */
	__raw_writel((0<<11)|(0<<0), S3C_VIDOSD0A);
	
	/* ��Ļ���½����� */
	__raw_writel(((LCD_WIDTH-1)<<11) | ((LCD_HIGH-1)<<0), S3C_VIDOSD0B);
	
	/* window size  high*width */
	__raw_writel((LCD_WIDTH* LCD_HIGH), S3C_VIDOSD0C); /* 0 - 23λ��Ч*/
	
	/* buffer�������׵�ַ */
	phy = HAL_GET_BASIC_PHYADDRESS(lcd_buffer);
	__raw_writel((u32)phy, S3C_VIDW00ADD0B0);
	
	/* buffer������ĩ��ַ */
	__raw_writel(((u32)phy + LCD_WIDTH* LCD_HIGH* 4 ) & 0xffffff, S3C_VIDW00ADD1B0);
	
	/* 
		����������һ��β֮���offset | �п� 
		ָbuffer�еĵ�ַ�����ֽڼ�
	*/
	__raw_writel((0 << 13)|(LCD_WIDTH* 4 ), S3C_VIDW00ADD2);
	
	/* ��ֹ���� */
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



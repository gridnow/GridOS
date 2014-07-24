/**
	InfoNES ģ������װ��
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ystd.h>
#include <pthread.h>
#include <unistd.h>

#include "InfoNES.h"
#include "InfoNES_System.h"
#include "InfoNES_pAPU.h"

#define START_PAUSE			0
#define SELECT_PLAYER		1
#define SHOT_SUBTRACT		2
#define SHOT_ADD			3
#define	UP					4
#define	DOWN				5
#define	LEFT				6
#define	RIGHT				7

/* Palette data */
WORD NesPalette[ 64 ] =
{
	0x39ce, 0x1071, 0x0015, 0x2013, 0x440e, 0x5402, 0x5000, 0x3c20,
	0x20a0, 0x0100, 0x0140, 0x00e2, 0x0ceb, 0x0000, 0x0000, 0x0000,
	0x5ef7, 0x01dd, 0x10fd, 0x401e, 0x5c17, 0x700b, 0x6ca0, 0x6521,
	0x45c0, 0x0240, 0x02a0, 0x0247, 0x0211, 0x0000, 0x0000, 0x0000,
	0x7fff, 0x1eff, 0x2e5f, 0x223f, 0x79ff, 0x7dd6, 0x7dcc, 0x7e67,
	0x7ae7, 0x4342, 0x2769, 0x2ff3, 0x03bb, 0x0000, 0x0000, 0x0000,
	0x7fff, 0x579f, 0x635f, 0x6b3f, 0x7f1f, 0x7f1b, 0x7ef6, 0x7f75,
	0x7f94, 0x73f4, 0x57d7, 0x5bf9, 0x4ffe, 0x0000, 0x0000, 0x0000
};

char key_arry[8] = {0};


#define RGB(r,g,b) (((r)<<16)|((g)<<8)|((b)))
#define EXPANDRGB16_TO_32(wcolor) (RGB( ((wcolor&0x7C00)>>10)<<3 , ((wcolor&0x03E0)>>5)<<3 ,(wcolor&0x001F)<<3 ))	

/**
 *	Transfer the contents of work frame on the screen 
 *
 *	@Param:NULL
 */
void InfoNES_LoadFrame()
{
	unsigned char pixel[4];
	int w, h;
	int x, y;
	int u, v;

#define STRETCH 1
#define DRAW_WIDTH (NES_DISP_WIDTH * STRETCH)
#define DRAW_HEIGHT (NES_DISP_HEIGHT * STRETCH)
	static unsigned int bitmap_32[DRAW_HEIGHT][DRAW_WIDTH];

	/* The screen size to get the center position*/
	sys_get_screen_resolution(&w, &h, NULL);
	x = (w / 2) - (DRAW_WIDTH / 2);
	y = (h / 2) - (DRAW_HEIGHT / 2);

	/* Exchange 16-bit to 24-bit  */
	for (register int y = 0, v = 0; y < NES_DISP_HEIGHT; y++, v += STRETCH)
	{
		int j;
		for (j = 0; j < STRETCH; j++)
		{
			for ( register int x = 0, u = 0; x < NES_DISP_WIDTH; x++, u += STRETCH)
			{  
				WORD wColor = WorkFrame[ NES_DISP_WIDTH * y + x ];
				int i;
				for (i = 0; i < STRETCH; i++)
					bitmap_32[v + j][u + i] = EXPANDRGB16_TO_32(wColor);
			}
		}
	}	
	sys_draw_screen(x, y, DRAW_WIDTH, DRAW_HEIGHT, 32, bitmap_32); 
}

/**
 *	��ȡ���̵İ���
 *
 */
void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem )
{
 	static DWORD dwSysOld;
 	DWORD dwTemp;
 
 	// Joypad 1
 	*pdwPad1 =   ( key_arry[START_PAUSE] ) |
 			 ( ( key_arry[SELECT_PLAYER] ) << 1 ) |
 			 ( ( key_arry[SHOT_SUBTRACT] ) << 2 ) |
			 ( ( key_arry[SHOT_ADD] ) << 3 ) |
 			 ( ( key_arry[UP] ) << 4 ) |
 			 ( ( key_arry[DOWN] ) << 5 ) |
 			 ( ( key_arry[LEFT] ) << 6 ) |
 			 ( ( key_arry[RIGHT] ) << 7 );
 
 	*pdwPad1 = *pdwPad1 | ( *pdwPad1 << 8 );
 	
 	*pdwPad2 = 0;
 
 	/* Input for InfoNES */
 	dwTemp = ( false );
 
 	/* Only the button pushed newly should be inputted */
 	*pdwSystem = ~dwSysOld & dwTemp;
 
 	/* keep this input */
 	dwSysOld = dwTemp;
 	// Joypad 2
}

/* Release a memory for ROM */
void InfoNES_ReleaseRom()
{
	
}

int InfoNES_ReadRom( const char *pszFileName )
{
	/*
	*  Read ROM image file
	*
	*  Parameters
	*    const char *pszFileName          (Read)
	*
	*  Return values
	*     0 : Normally
	*    -1 : Error
	*/

	FILE *fp;

	/* Open ROM file */
	fp = fopen( pszFileName, "r");
	if ( fp == NULL )
		return -1;

	/* Read ROM Header */
	fread( &NesHeader, sizeof NesHeader, 1, fp );
	if ( memcmp( NesHeader.byID, "NES\x1a", 4 ) != 0 )
	{
		printf("%s����һ��NES����.\n", pszFileName);
		/* not .nes file */
		fclose( fp );
		return -1;
	}

	/* Clear SRAM */
	memset( SRAM, 0, SRAM_SIZE );

	/* If trainer presents Read Triner at 0x7000-0x71ff */
	if ( NesHeader.byInfo1 & 4 )
	{
		fread( &SRAM[ 0x1000 ], 512, 1, fp );
	}

	/* Allocate Memory for ROM Image */
	ROM = (BYTE *)malloc( NesHeader.byRomSize * 0x4000 );

	/* Read ROM Image */
	fread( ROM, 0x4000, NesHeader.byRomSize, fp );

	if ( NesHeader.byVRomSize > 0 )
	{
		/* Allocate Memory for VROM Image */
		VROM = (BYTE *)malloc( NesHeader.byVRomSize * 0x2000 );

		/* Read VROM Image */
		fread( VROM, 0x2000, NesHeader.byVRomSize, fp );
	}

	/* File close */
	fclose( fp );

	/* Successful */
	return 0;
}

/**
 *	Memroy Copy for InfoNES
 *
 *	@Param:Ŀ���ַ
 *	@Param:Դ��ַ
 *	@Param:������
 */
void *InfoNES_MemoryCopy(void *dest, const void *src, int count)
{
	memcpy(dest, src, count);
	return dest;
}

/**
 *	Memory Set for InfoNES 
 *
 *	@Param:Ŀ���ַ
 *	@Param:ֵ
 *	@Param:������
 */
void *InfoNES_MemorySet(void *dest, int c, int count)
{
	memset(dest, c, count);
	return dest;
}

/**
 *	��ӡ������Ϣ
 *
 *	@Param:������Ϣ
 *
 */
void InfoNES_DebugPrint( char *pszMsg )
{
	/*dprintf ( "\n%s" , pszMsg );*/
}

/**
 *	��ʼ������
 *
 *	@Param:NULL
 */
void InfoNES_SoundInit( void ) 
{
	return ;
}

/**
 *	�������豸
 *
 *	@Param:δʹ��
 *	@Param:δʹ��
 */
int InfoNES_SoundOpen( int samples_per_sync, int sample_rate ) 
{
	return(true);
}

/**
 *	�ر������豸
 *
 *	@Param:NULL
 */
void InfoNES_SoundClose( void ) 
{
	return;
}

void InfoNES_SoundOutput( int samples, BYTE *wave1, BYTE *wave2, BYTE *wave3, BYTE *wave4, BYTE *wave5 )
{

}

/**
 *	�ȴ�ģ�����Ļ�Ӧ
 *
 *	@Param:NULL
 */
void InfoNES_Wait()
{
}

/**
 *	��ʾ�Ի���
 *
 *	@Param:��ʾ���ַ���
 */
void InfoNES_MessageBox(char *pszMsg, ... )
{

}


/**
 *	InfoNess_Menu
 * 
 *	@Param:NULL
 */
int InfoNES_Menu()
{
	// Nothing to do here
	return 0;
}

/************************************************************************/
/* MAIN loop                                                            */
/************************************************************************/
static void * play_thread(void * para)
{
	InfoNES_Main();
}

static int translate()
{
	char ch = getch();

	if (ch == 0)
		memset(key_arry, 0, sizeof(key_arry));
	else if (ch == '\033') 
	{
		ch = getch();
		if (ch == '[') 
		{
			ch = getch();
			switch(ch) 
			{
			case 'A':
				key_arry[UP] = 1;
				break;
			case 'B':
				key_arry[DOWN] = 1;
				break;
			case 'D':
				key_arry[LEFT] = 1;
				break;
			case 'C':
				key_arry[RIGHT] = 1;
				break;		
			}
		}
		else
		{
			/* ESC ��, end program */
			return 1;
		}
	}
	else 
	{
		switch (ch) 
		{
		case 'w':
			key_arry[SHOT_ADD] = 1;
			break;
		case 's':
			key_arry[SHOT_SUBTRACT] = 1;
			break;
		case 'd':
			key_arry[START_PAUSE] = 1;
			break;
		case 'a':
			key_arry[SELECT_PLAYER] = 1;
			break;
		}
	}

	return 0;
}



int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("��������ʱ�����һ����Ϸ�ļ���\n");
		return 0;
	}
	if (InfoNES_Load(argv[1]))
	{
		printf("�ļ�װ�벻�ԣ��������ļ�����һ����Ϸ�ļ�.\n");
		return 0;
	}

	pthread_create(NULL, NULL, play_thread, (void*)0x1234);
	
	while (1) 
	{
		/* GET key, or we exit */
		if (translate())
			break;
	}
}
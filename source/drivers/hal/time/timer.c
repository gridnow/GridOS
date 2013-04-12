/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Sihai.Yao
*   HAL的软件定时器
*/

/* !!! Must it it */
unsigned long long jiffies_64;

/**
	@brief Sleep milliseconds 

	休眠milliseconds并且不退出休眠直到时间流逝完
*/
void hal_msleep(unsigned int msecs)
{
	//TODO: msleep
}

/**
	@brief Sleep in us unit

	由于US的单位比较小，线程不一定进入休眠状态，可能进入轮询状态
*/
void hal_usleep(unsigned int us)
{
	//TODO:
}

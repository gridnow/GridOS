/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   变量到（10进制）字符串的转换
*/

int h2d(char *p, int revert, unsigned long hex)
{
	unsigned long r;
	int i = 0;   
	int r_t, r_f;

	if (hex < 10)
	{
		hex = hex + '0';
		p[i] = (char)hex;
		i++;
		goto end;
	}
	
	do
	{
		r = hex % 10;
		hex = hex / 10;
		r = r + '0';
		p[i] = (char)r;
		i++;

		if (hex < 10)
		{
			hex = hex + '0';
			p[i] = (char)hex;
			i++;
			break;
		}
	}while(1);
	
	/* 把字符串掉头，奇数个数与偶数个数的调法不一样*/
	if (!revert) goto end;
	r_f = i - 1;
	r_t = 0;

	if ((i % 2) == 0)
	{
		do
		{
			r = p[r_t];
			p[r_t] = p[r_f];
			p[r_f] = r;
			r_t++; r_f--;
		}while (r_f > r_t);
	}
	else
	{
		/* have a middle char */
		do
		{
			r = p[r_t];
			p[r_t] = p[r_f];
			p[r_f] = r;
			r_t++; r_f--;
		}while (r_f != r_t);
	}
	
end:
	return i;
}

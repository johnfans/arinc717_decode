// findx.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

char	write_flag = 0;

void parseUnsignedShort(unsigned short num, char* buffer) {
	for (int i = 0; i < 16; i++) {
		buffer[i] = num & 1 ? 1 : 0; // 取出最低位的值，转成字符存入数组
		num >>= 1; // 将 num 右移一位，去掉最低位
	}
}

int btd(char *a, int length)
{
	int res=0;
	for (int i = 0; i <length ; i++)
	{
		res += a[i]*pow(2,i);
		//printf("%d ", a[i]);
	}
	return res;
}

int r_altitude(char a[4][3073],char i)
{	
	int	res=0;
	//for (size_t i = 0; i < 4; i++)
	//{
		res = btd(&a[i][1176], 11);
		if (a[i][1187] == 0)
		{
			return res*2;
		}
		else
		{
			return	(res - pow(2, 11))*2;
		}
		//printf("%d\n", res);
	//}
}

int fuelflow1(char a[4][3073],char i)
{
	int	res = 0;
	//for (size_t i = 0; i < 4; i++)
	//{
		res = btd(&a[i][1440], 11) * 4;
		
		return res;
		
	//}
}

int fuelflow2(char a[4][3073],char i)
{
	int	res = 0;
	//for (size_t i = 0; i < 4; i++)
	//{
		res = btd(&a[i][2976], 11) * 4;

		return res;

	//}
}

int airspeed(char a[4][3073],char i)
{
	int	res = 0;
	
	//for (size_t i = 0; i < 4; i++)
	//{
		res = btd(&a[i][432], 11)/2;

		return res;

	//}
	
}

int temperature(char a[4][3073])
{
	double	res = 0.0;
	for (size_t i = 1; i < 4; i+=2)
	{
		res = btd(&a[i][2376], 12)*0.06253;
		if (a[i][2387]==0)
		{
			printf("%.2f  ", res);
		}
		else
		{
			printf("null");
		}
		//printf("%d\n", res);
	}
}

void time(char a[4][3073])
{
	int	res = 0;
	for (size_t i = 0; i < 4; i++)
	{
		res = btd(&a[i][24], 6);

		printf("%d  ", res);

	}
}


//专用打印器
void printer(char a[4][3073])
{
	int	i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 3072; j++)
		{
			printf("%d", a[i][j]);
		}
		printf("\n\n");
	}
	printf("\n\n\n");
}

int main()
{
    unsigned short buff = 0;
    unsigned short buff2 = 0;
    unsigned short tail = 0;
    unsigned short wind = 0;
    unsigned short target1 = 0x0247;
    unsigned short target2 = 0x05b8;
    unsigned short target3 = 0x0a47;
    unsigned short target4 = 0x0db8;
    unsigned char  flag = 0;
	char frametest[12] = { 1,0,0,1,0,0,1,1,0,1,1,1 };
    int bits = 16;
    int bitlast = 0;
    long int temp_cur = 0;
	char frame[4][3073], sbit;
	unsigned short temp;
	int	f1, f2, as, ra;
	

    
    FILE* file;
    fopen_s(&file, "30651214.DLU", "rb"); // 打开二进制文件

    if (file == NULL) {
        printf("无法打开文件。\n");
        return 1;
    }

	FILE* file2;
	fopen_s(&file2, "30651214.csv", "w"); // 打开二进制文件

	if (file2 == NULL) {
		printf("无法打开文件2。\n");
		return 1;
	}

    fseek(file, 0, SEEK_END);
    long long int size = ftell(file)/2;
    fseek(file, 0, SEEK_SET);

    fread(&buff, 2, 1, file);
    wind = buff;
    long long int ie = 0;

    for (long long int i = 0; i < size; i++)
    {
        fread(&buff, 2, 1, file);
        for (int j = 15; j >=0; j--)
        {
			//printf("%d", write_flag);
            
            wind = ((wind >> 1) & 0x7fff) | ((buff << j) & 0x8000);//移入1bit
			if (write_flag)
			{

				if (write_flag == 1)
				{
					temp = (buff << j) & 0x8000;
					if (temp != 0)
					{
						sbit = 1;
						
					}
					else
					{
						sbit = 0;
					}
					frame[flag - 1][bits] = sbit;
					//printf("%c", sbit);
					bits++;
				}
				if (bits>=3072)
				{
					write_flag = 0;
					bits = 16;
					//frame[flag - 1][bits] = '\0';
					//if (flag == 4)
					//{	
						/*
						这里进行解帧
						
						*/
						//ie++;
						ra = r_altitude(frame,flag-1);
						f1 = fuelflow1(frame,flag-1);
						f2 = fuelflow2(frame,flag-1);
						//temperature(frame);
						//printf("%d\n",btd(frametest, 12));
						//time(frame);
						as = airspeed(frame,flag-1);
						fprintf(file2,"%d,%d,%d,%d\n",ra,f1,f2,as);
						//printer(frame);//帧打印器，完成后请删除
					//}
				}
			}
			
            if ((wind & 0x0fff) == (target1 & 0x0fff))
            {
                //printf("%lld  %d\n", i, 1);
				
                temp_cur = ftell(file);
                fseek(file, 380, SEEK_CUR);
                fread(&tail, 2, 1, file);
                fread(&buff2, 2, 1, file);
                tail = ((tail >> (16-j)) & 0x7fff)  | (buff2 << j);
                if ((tail & 0x0fff) == (target2 & 0x0fff))
                {
                    //printf("%lld\n", i);
                    
					write_flag = 1;
					flag = 1;
					parseUnsignedShort(wind, &frame[flag - 1][0]);
					//printf("%d\n", flag);
                }
                fseek(file, temp_cur, SEEK_SET);
                
                

            }
            //printf("%d  %d\n", wind & 0x0fff, target & 0x0fff);

            if ((wind & 0x0fff) == (target2 & 0x0fff))
            {
				
				temp_cur = ftell(file);
				fseek(file, 380, SEEK_CUR);
				fread(&tail, 2, 1, file);
				fread(&buff2, 2, 1, file);
				tail = ((tail >> (16 - j)) & 0x7fff) | (buff2 << j);
				if ((tail & 0x0fff) == (target3 & 0x0fff))
				{
					//printf("%lld\n", i);
					write_flag = 1;
					flag = 2;
					parseUnsignedShort(wind, &frame[flag - 1][0]);
					//printf("%d\n", flag);
				}
				fseek(file, temp_cur, SEEK_SET);
            }

            if ((wind & 0x0fff) == (target3 & 0x0fff))
            {
				
				temp_cur = ftell(file);
				fseek(file, 380, SEEK_CUR);
				fread(&tail, 2, 1, file);
				fread(&buff2, 2, 1, file);
				tail = ((tail >> (16 - j)) & 0x7fff) | (buff2 << j);
				if ((tail & 0x0fff) == (target4 & 0x0fff))
				{
					//printf("%lld\n", i);
					
					write_flag = 1;
					flag = 3;
					parseUnsignedShort(wind, &frame[flag - 1][0]);
					//printf("%d\n", flag);
				}
				fseek(file, temp_cur, SEEK_SET);
            }

            if ((wind & 0x0fff) == (target4 & 0x0fff))
            {
				
				temp_cur = ftell(file);
				fseek(file, 380, SEEK_CUR);
				fread(&tail, 2, 1, file);
				fread(&buff2, 2, 1, file);
				tail = ((tail >> (16 - j)) & 0x7fff) | (buff2 << j);
				if ((tail & 0x0fff) == (target1 & 0x0fff))
				{
					//printf("%lld\n", i);
					
					write_flag = 1;
					flag = 4;
					parseUnsignedShort(wind, &frame[flag - 1][0]);
					//printf("%d\n", flag);
				}
				fseek(file, temp_cur, SEEK_SET);
            }

            //if ((flag == 1) && ()

            
            

        }
    }
    fclose(file);
	fclose(file);
    return 0;

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

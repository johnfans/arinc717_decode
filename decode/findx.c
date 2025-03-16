// findx.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>





int main()
{
	char	write_flag = 0;
	int FLEN;

    unsigned short buff = 0;
    unsigned short buff2 = 0;
    unsigned short tail = 0;
    unsigned short wind = 0;
    unsigned short target1 = 0x0247;
    unsigned short target2 = 0x05b8;
    unsigned short target3 = 0x0a47;
    unsigned short target4 = 0x0db8;
    unsigned char  flag = 0;
	int bits = 0;
	int words = 1;
	char subs[4] = { 0 };
    int bitlast = 0;
    long int temp_cur = 0;
	unsigned short frame[4][512];
	
	

    
    FILE* file;
    fopen_s(&file, "30651214.DLU", "rb"); // 打开二进制文件

    if (file == NULL) {
        printf("无法打开文件。\n");
        return 1;
    }

	FILE* file2;
	fopen_s(&file2, "30651214.csv", "w"); // 打开文件

	if (file2 == NULL) {
		printf("无法打开文件2。\n");
		return 1;
	}

	char count_flag = 0;
	int count = 0;
	while (1)
	{
		fread(&buff, 2, 1, file);
		for (int k = 15; k >= 0; k--)
		{
			wind = ((wind >> 1) & 0x7fff) | ((buff << k) & 0x8000);//移入1bit
			if (count_flag == 1)
			{
				count++;
				if ((wind & 0x0fff) == (target2 & 0x0fff))
				{
					if (count%768==0 && count<=6144)
					{
						FLEN = count/12;
						count_flag = 2;
						break;
						
					}
				}
				
			}

			if ((wind & 0x0fff) == (target1 & 0x0fff))
			{
				count_flag = 1;
				count = 0;
			}

			
		}
		if (count_flag==2)
		{
			break;
		}
	}

    fseek(file, 0, SEEK_END);
    long long int size = ftell(file)/2;
    fseek(file, 0, SEEK_SET);
	wind = 0;

	

    for (long long int i = 0; i < size; i++)
    {
        fread(&buff, 2, 1, file);
        for (int j = 15; j >=0; j--)
        {
			//printf("%d", write_flag);
            
            wind = ((wind >> 1) & 0x7fff) | ((buff << j) & 0x8000);//移入1bit
			if (write_flag)
			{
				bits++;
				if (bits==12)
				{
					frame[flag-1][words] = (wind & 0x0fff);
					bits = 0;
					words++;
				}

				if (words==FLEN)
				{
					words = 1;
					write_flag = 0;
					if (subs[0]+subs[1]+subs[2]+subs[3]==4)
					{
						/*
							完成一主帧的同步，可在此处进行解析或保存数据
						*/
						fprintf(file2, "%d,%d,%d%d%d%d\n", (frame[0][2] >> 6) & 0x003f, frame[0][2] & 0x003f, frame[0][164]&0x000f, (frame[1][164]>>8)&0x000f, (frame[1][164] >> 4) & 0x000f, frame[1][164] & 0x000f);
						fprintf(file2, "%d,%d\n", (frame[1][2] >> 6) & 0x003f, frame[1][2] & 0x003f);
						fprintf(file2, "%d,%d,%d%d\n", (frame[2][2] >> 6) & 0x003f, frame[2][2] & 0x003f, (frame[2][4] >> 8) & 0x000f, (frame[2][4] >> 4) & 0x000f);
						fprintf(file2, "%d,%d\n", (frame[3][2] >> 6) & 0x003f, frame[3][2] & 0x003f);

						//reset subframe flag
						memset(subs, 0, 4);

					}
				}
				
			}
			
            if ((wind & 0x0fff) == (target1 & 0x0fff))
            {
                //printf("%lld  %d\n", i, 1);
				
                temp_cur = ftell(file);
                fseek(file, FLEN * 12 / 8 - 4, SEEK_CUR);
                fread(&tail, 2, 1, file);
                fread(&buff2, 2, 1, file);
                tail = ((tail >> (16-j)) & 0x7fff)  | (buff2 << j);
                if ((tail & 0x0fff) == (target2 & 0x0fff))
                {
                    //printf("%lld\n", i);
					memset(subs, 0, 4);
					write_flag = 1;
					flag = 1;
					subs[0] = 1;
					frame[0][0] = target1;
					
					
					//printf("%d\n", flag);
                }
                fseek(file, temp_cur, SEEK_SET);
                
                

            }
            //printf("%d  %d\n", wind & 0x0fff, target & 0x0fff);

            if ((wind & 0x0fff) == (target2 & 0x0fff))
            {
				
				temp_cur = ftell(file);
				fseek(file, FLEN * 12 / 8 - 4, SEEK_CUR);
				fread(&tail, 2, 1, file);
				fread(&buff2, 2, 1, file);
				tail = ((tail >> (16 - j)) & 0x7fff) | (buff2 << j);
				if ((tail & 0x0fff) == (target3 & 0x0fff))
				{
					//printf("%lld\n", i);
					write_flag = 1;
					flag = 2;
					subs[1] = 1;
					frame[1][0] = target2;
					
					//printf("%d\n", flag);
				}
				fseek(file, temp_cur, SEEK_SET);
            }

            if ((wind & 0x0fff) == (target3 & 0x0fff))
            {
				
				temp_cur = ftell(file);
				fseek(file, FLEN * 12 / 8 - 4, SEEK_CUR);
				fread(&tail, 2, 1, file);
				fread(&buff2, 2, 1, file);
				tail = ((tail >> (16 - j)) & 0x7fff) | (buff2 << j);
				if ((tail & 0x0fff) == (target4 & 0x0fff))
				{
					//printf("%lld\n", i);
					
					write_flag = 1;
					flag = 3;
					subs[2] = 1;;
					frame[2][0] = target3;
					
					//printf("%d\n", flag);
				}
				fseek(file, temp_cur, SEEK_SET);
            }

            if ((wind & 0x0fff) == (target4 & 0x0fff))
            {
				
				temp_cur = ftell(file);
				fseek(file, FLEN * 12 / 8 - 4, SEEK_CUR);
				fread(&tail, 2, 1, file);
				fread(&buff2, 2, 1, file);
				tail = ((tail >> (16 - j)) & 0x7fff) | (buff2 << j);
				if ((tail & 0x0fff) == (target1 & 0x0fff))
				{
					//printf("%lld\n", i);
					
					write_flag = 1;
					flag = 4;
					subs[3] = 1;;
					frame[3][0] = target1;
					
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

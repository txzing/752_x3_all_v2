#include "../bsp.h"

#if defined (SW_VER_BY_COMPILE_TIME)

//时间结构体
typedef struct
{
    u8 nHour;
    u8 nMinute;
    u8 nSecond;

    //公历日月年周
    u8  nMonth;
    u8  nDay;
    u8  nWeek;
    u16 nYear;

} RTC_TIME_DEF;

static char *str_n_cpy(char *dest, const char *src, int n);
static RTC_TIME_DEF GetSoftWareBuildTargetTime(void);
static u8 Bcd2ToByte(u8 Value);
static u8 ByteToBcd2(u8 Value);


u32 __SW_VER__;

//字符串截取
char *str_n_cpy(char *dest, const char *src, int n)
{
    int size = sizeof(char)*(n + 1);
    char *tmp = (char*)malloc(size);  // 开辟大小为n+1的临时内存tmp

    if (tmp)
    {
        memset(tmp, '\0', size);  // 将内存初始化为0
        memcpy(tmp, src, size - 1);  // 将src的前n个字节拷贝到tmp
        memcpy(dest, tmp, size);  // 将临时空间tmp的内容拷贝到dest

        free(tmp);  // 释放内存
        return dest;
    }
    else
    {
        return NULL;
    }
}

//得到软件编译时间
RTC_TIME_DEF GetSoftWareBuildTargetTime(void)
{
    char arrDate[20]; //Jul 03 2018
    char arrTime[20]; //06:17:05
    char pDest[20];
    RTC_TIME_DEF stTime;

    sprintf(arrDate,"%s",__DATE__);//
    sprintf(arrTime,"%s",__TIME__);// UCT for Xilinx compiler, +8 == CST (E8区)

    sprintf(pDest, "%s", str_n_cpy(pDest, arrDate, 3));

    if (strcmp(pDest, "Jan") == 0) stTime.nMonth = 1;
    else if (strcmp(pDest, "Feb") == 0) stTime.nMonth = 2;
    else if (strcmp(pDest, "Mar") == 0) stTime.nMonth = 3;
    else if (strcmp(pDest, "Apr") == 0) stTime.nMonth = 4;
    else if (strcmp(pDest, "May") == 0) stTime.nMonth = 5;
    else if (strcmp(pDest, "Jun") == 0) stTime.nMonth = 6;
    else if (strcmp(pDest, "Jul") == 0) stTime.nMonth = 7;
    else if (strcmp(pDest, "Aug") == 0) stTime.nMonth = 8;
    else if (strcmp(pDest, "Sep") == 0) stTime.nMonth = 9;
    else if (strcmp(pDest, "Oct") == 0) stTime.nMonth = 10;
    else if (strcmp(pDest, "Nov") == 0) stTime.nMonth = 11;
    else if (strcmp(pDest, "Dec") == 0) stTime.nMonth = 12;
    else stTime.nMonth = 1;

     sprintf(pDest, "%s", str_n_cpy(pDest, arrDate+4, 2));
    //int atoi(const char *nptr);
    stTime.nDay = atoi(pDest);

    sprintf(pDest, "%s", str_n_cpy(pDest, arrDate + 4 + 3, 4));
    //int atoi(const char *nptr);
    stTime.nYear = atoi(pDest);

    //time
    sprintf(pDest, "%s", str_n_cpy(pDest, arrTime, 2));
    stTime.nHour = atoi(pDest) + 8; // +8 for E8 CST
    sprintf(pDest, "%s", str_n_cpy(pDest, arrTime+3, 2));
    stTime.nMinute = atoi(pDest);
    sprintf(pDest, "%s", str_n_cpy(pDest, arrTime + 3 + 3, 2));
    stTime.nSecond = atoi(pDest);

    return stTime;
}

//BCD转换为二进制
static u8 Bcd2ToByte(u8 Value)
{
    u32 tmp=0U;
    tmp=((u8)(Value&(u8)0xF0)>>(u8)0x4)*10U;
    return(tmp+(Value&(u8)0x0F));
}

//二进制转换为BCD
static u8 ByteToBcd2(u8 Value)
{
    u32 bcdhigh=0U;

    while(Value>=10U)
    {
        bcdhigh++;
        Value-=10U;
    }

    return((u8)(bcdhigh<<4U)|Value);
}


u32 GetSoftWareVersion(void)
{
    u32 Version = 0;
    u8 b1;
    u8 b2;
    u8 b3;
    u8 b4;
    RTC_TIME_DEF stTime;

	RTC_TIME_DEF software_time;
	software_time = GetSoftWareBuildTargetTime();
//	printf("Year = %d\n",software_time.nYear);
//	printf("Day = %d\n",software_time.nDay);
//	printf("Month = %d\n",software_time.nMonth);
//	printf("Hour = %d\n",software_time.nHour);
//	printf("Minute = %d\n",software_time.nMinute);
//	printf("Second = %d\n",software_time.nSecond);
//
//
//	printf("hex_Year = %x\n",ByteToBcd2(software_time.nYear - 2000));
//	printf("hex_Day = %x\n",ByteToBcd2(software_time.nDay));
//	printf("hex_Month = %x\n",ByteToBcd2(software_time.nMonth));
//	printf("hex_Hour = %x\n",ByteToBcd2(software_time.nHour));
//	printf("hex_Minute = %x\n",ByteToBcd2(software_time.nMinute));
//	printf("hex_Second = %x\n",ByteToBcd2(software_time.nSecond));

	b1 = ByteToBcd2(software_time.nYear - 2000);
	b2 = ByteToBcd2(software_time.nMonth);
	b3 = ByteToBcd2(software_time.nDay);
	b4 = ByteToBcd2(software_time.nHour);

	Version = b1 << 24 | b2 << 16 | b3 << 8 | b4;

    return Version;
}
#endif // SW_VER_BY_COMPILE_TIME


/*
usage: version as compiled time, especially useful during debug, but no need after release
call follows before main_loop
```
    __SW_VER__ = GetSoftWareVersion();
    bsp_printf("software ver = 0x%08x\n\r", __SW_VER__);
    bsp_printf("***************************\n\r");
```
*/

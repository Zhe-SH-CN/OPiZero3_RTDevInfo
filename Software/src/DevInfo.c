#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/statfs.h>
#include <string.h>
#include "DevInfo.h"

#define TEMP_FILE_PATH  "/sys/class/thermal/thermal_zone0/temp"
#define MAX_SIZE        32

float cal_cpuoccupy(CPU_OCCUPY *o,CPU_OCCUPY *n)
{
    unsigned long long od, nd;
    unsigned long long busy;
    float cpu_use = 0;
    od = o->user + o->nice + o->system + o->idle + o->iowait + o->irq + o->softirq + o->steal;  //第一次总时间
    nd = n->user + n->nice + n->system + n->idle + n->iowait + n->irq + n->softirq + n->steal;  //第二次总时间

    busy = (unsigned long long)(n->user - o->user) +
           (unsigned long long)(n->nice - o->nice) +
           (unsigned long long)(n->system - o->system) +
           (unsigned long long)(n->iowait - o->iowait) +
           (unsigned long long)(n->irq - o->irq) +
           (unsigned long long)(n->softirq - o->softirq) +
           (unsigned long long)(n->steal - o->steal);
    if((nd-od) != 0)
        cpu_use = (busy * 100.0) / (nd-od); //忙碌时间乘100后除以总时间差
    else 
        cpu_use = 0;
    return cpu_use;
}

void get_cpuoccupy (CPU_OCCUPY *cpust)  //对无类型get函数含有一个形参结构体类弄的指针o
{
    FILE *fd;
    char buff[256] = "";
    CPU_OCCUPY *cpu_occupy = NULL;
    cpu_occupy = cpust;

    fd = fopen("/proc/stat","r");
    if (fd != NULL)
    {
        fgets(buff,sizeof(buff),fd);
        memset(cpu_occupy, 0, sizeof(*cpu_occupy));
        sscanf(buff,"%19s %llu %llu %llu %llu %llu %llu %llu %llu",
                cpu_occupy->name,
                &cpu_occupy->user,
                &cpu_occupy->nice,
                &cpu_occupy->system,
            &cpu_occupy->idle,
            &cpu_occupy->iowait,
            &cpu_occupy->irq,
            &cpu_occupy->softirq,
            &cpu_occupy->steal);
        fclose(fd);
    }
}

float GetCpuUsage()
{
    CPU_OCCUPY cpu_stat1;
    CPU_OCCUPY cpu_stat2;
    float cpu;
    //第一次获取cpu使用情况
    get_cpuoccupy((CPU_OCCUPY *)&cpu_stat1);
    sleep(1);
    //第二次获取cpu使用情况
    get_cpuoccupy((CPU_OCCUPY *)&cpu_stat2);
    //计算cpu使用率
    cpu = cal_cpuoccupy((CPU_OCCUPY *)&cpu_stat1,(CPU_OCCUPY *)&cpu_stat2);
    return cpu;
}

unsigned long GetMemTotal()
{
    /*Byte - Read from /proc/meminfo for accurate values*/
    FILE *fp;
    char buff[256] = "";
    unsigned long totalMem = 0;

    fp = fopen("/proc/meminfo", "r");
    if (fp != NULL)
    {
        while (fgets(buff, sizeof(buff), fp))
        {
            if (sscanf(buff, "MemTotal: %lu kB", &totalMem) == 1)
            {
                fclose(fp);
                return totalMem * 1024;  // Convert KB to Bytes
            }
        }
        fclose(fp);
    }
    return 0;
}

unsigned long GetMemFree()
{
    /*Byte - Read from /proc/meminfo for accurate values*/
    FILE *fp;
    char buff[256] = "";
    unsigned long memAvailable = 0;

    fp = fopen("/proc/meminfo", "r");
    if (fp != NULL)
    {
        while (fgets(buff, sizeof(buff), fp))
        {
            // Try MemAvailable first (more accurate - includes reclaimable memory)
            if (sscanf(buff, "MemAvailable: %lu kB", &memAvailable) == 1)
            {
                fclose(fp);
                return memAvailable * 1024;  // Convert KB to Bytes
            }
        }
        fclose(fp);
    }
    return 0;
}

float GetCpuTemp()
{   
    FILE* fp;
    char buf[MAX_SIZE] = "";
    double tempVal = 0.0;

    fp = fopen(TEMP_FILE_PATH, "r");
    if (fp == NULL)
    {
        printf("Fail to Read Temp...\n");
    }
    else
    {
        fread(buf, 1, MAX_SIZE, fp);
        tempVal = atof(buf) / 1000;
        fclose(fp);
    }
    
    return tempVal;
}

long long GetAvailDisk()
{
    struct statfs diskInfo;
    if(statfs("/opt", &diskInfo)==-1)
    {
        return 0;
    }
    unsigned long long availableDisk = diskInfo.f_bavail * diskInfo.f_bsize;
    return availableDisk >> 20;
}

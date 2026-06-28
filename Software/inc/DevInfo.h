#ifndef CPU_INFO_H
#define CPU_INFO_H

typedef struct
{
    char name[20];  
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
}CPU_OCCUPY;

float GetCpuUsage();
unsigned long GetMemFree();
unsigned long GetMemTotal();
float GetCpuTemp();
long long GetAvailDisk();
float cal_cpuoccupy(CPU_OCCUPY *o,CPU_OCCUPY *n);
void get_cpuoccupy (CPU_OCCUPY *cpust);

#endif //CPU_INFO_H

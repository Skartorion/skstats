#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>


using namespace std;

struct CpuStats 
{
    long long user, nice, system, idle, iowait, irq, softirq, steal;

    long long total() const 
    {
        return user + nice + system + idle + iowait + irq + softirq + steal;
    }

    long long idleTime() const 
    {
        return idle + iowait;
    }
};

CpuStats getCpuStats() 
{
    ifstream file("/proc/stat");
    string line;
    CpuStats stats = {0};

    if (getline(file, line)) 
    {
        istringstream ss(line);
        string cpu;
        ss >> cpu;
        ss >> stats.user >> stats.nice >> stats.system >> stats.idle
           >> stats.iowait >> stats.irq >> stats.softirq >> stats.steal;
    }

    return stats;
}

float getCpuUsage() 
{
    CpuStats stat1 = getCpuStats();
    this_thread::sleep_for(chrono::milliseconds(500));
    CpuStats stat2 = getCpuStats();

    long long totalDiff = stat2.total() - stat1.total();
    long long idleDiff = stat2.idleTime() - stat1.idleTime();

    return 100.0f * (1.0f - (float)idleDiff / totalDiff);
}

void getMemoryUsage(long long &totalMem, long long &freeMem) 
{
    ifstream file("/proc/meminfo");
    string label;
    long long value;
    string unit;

    while (file >> label >> value >> unit) 
    {
        if (label == "MemTotal:") totalMem = value;
        if (label == "MemAvailable:") freeMem = value;
    }
}

int main() 
{
    while (true)
    {
        float cpu = getCpuUsage();

        long long totalMem = 0, freeMem = 0;
        getMemoryUsage(totalMem, freeMem);
        long long usedMem = totalMem - freeMem;

        

        cout << "CPU Usage: " << cpu << "%" << endl;
        cout << "Memory Usage: " << usedMem / 1024 << " MB / " << totalMem / 1024 << " MB" << endl;
        cout << "-----------------------------" << endl;
        
        cout << "\033[2J\033[1;1H";
        
        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}

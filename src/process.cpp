#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Instantiate a Process object with PID.
// pid_ will be used to gather other information related to the Process.
Process::Process(int pid) {
    pid_ = pid;
    user_= LinuxParser::User(pid);
    command_ = LinuxParser::Command(pid);
    cpuUtilization_ = ProcessGetUtil();
    ram_ = ProcessGetRam();
    uptime_ = ProcessGetUpTime();
}

float Process::ProcessGetUtil() {
    /*
    To calculate CPU usage for a specific process you'll need the following:

    /proc/uptime
        #1 uptime of the system (seconds)
    /proc/[PID]/stat
        #14 utime - CPU time spent in user code, measured in clock ticks
        #15 stime - CPU time spent in kernel code, measured in clock ticks
        #16 cutime - Waited-for children's CPU time spent in user code (in clock ticks)
        #17 cstime - Waited-for children's CPU time spent in kernel code (in clock ticks)
        #22 starttime - Time when the process started, measured in clock ticks
    Hertz (number of clock ticks per second) of your system.
        In most cases, getconf CLK_TCK can be used to return the number of clock ticks.
        The sysconf(_SC_CLK_TCK) C function call may also be used to return the hertz value.

    */
    long uptime = LinuxParser::UpTime();
    int hertz = sysconf(_SC_CLK_TCK);
    long total_time;
    long n1;
    string c2, c3;
    long n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, utime, stime, cutime, cstime, n18, n19;
    long n20, n21, starttime;
    string line;
    string userName;
    std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid_) + "/stat");
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> n1 >> c2 >> c3 >> n4 >> n5 >> n6 >> n7 >> n8 >> n9 >>
        n10 >> n11 >> n12 >> n13 >> utime >> stime >> cutime >> cstime >>
        n18 >> n19 >> n20 >> n21 >> starttime;;
    }
    //std::cout << hertz << " ";
    total_time = utime + stime;
    long seconds = uptime - (starttime / hertz);
    float cpu_usage = (((float) total_time / (float) hertz) / (float) seconds);
    return cpu_usage;
}

std::string Process::ProcessGetRam() {
    return "1M";
}

int Process::ProcessGetUpTime() {
    return 10000;
}

// Done: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    return cpuUtilization_; 
}

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return ram_; }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return uptime_; }

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& other) const { 
    Process a = other;
    Process b = *this;
    return a.CpuUtilization() < b.CpuUtilization(); 
    
}
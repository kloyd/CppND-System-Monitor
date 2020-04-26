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
    cpuUtilization_ = LinuxParser::CpuUtilizationPerProcess(pid);
    ram_ = LinuxParser::Ram(pid);
    uptime_ = LinuxParser::UpTime(pid);
}

// Done: Return this process's ID
int Process::Pid() { return pid_; }

// Done: Return this process's CPU utilization
float Process::CpuUtilization() { 
    return cpuUtilization_; 
}

// Done: Return the command that generated this process
string Process::Command() { return command_; }

// Done: Return this process's memory utilization
string Process::Ram() { return ram_; }

// Done: Return the user (name) that generated this process
string Process::User() { return user_; }

// Done: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// Done: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& other) const { 
    Process a = other;
    Process b = *this;
    long aMemory = std::stol(a.Ram());
    long bMemory = std::stol(b.Ram());
    //return aMemory < bMemory; 
    return a.CpuUtilization() < b.CpuUtilization();
    
}
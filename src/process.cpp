#include <unistd.h>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::vector;

// Set the pid for the process
// pid will be used to gather other information related to the Process.
void Process::setPid(int pid) {
  pid_ = pid;
  user_ = LinuxParser::User(pid);
  command_ = LinuxParser::Command(pid);
  cpuUtilization_ = LinuxParser::CpuUtilization(pid);
  ram_ = LinuxParser::Ram(pid);
  uptime_ = LinuxParser::UpTime(pid);
}

// Done: Return this process's ID
int Process::Pid() const { return pid_; }

// Done: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpuUtilization_; }

// Done: Return the command that generated this process
string Process::Command() const { return command_; }

// Done: Return this process's memory utilization
string Process::Ram() const { return ram_; }

// Done: Return the user (name) that generated this process
string Process::User() const { return user_; }

// Done: Return the age of this process (in seconds)
long int Process::UpTime() const { return uptime_; }

/* Overload the "less than" comparison operator for Process objects
 *  Use CPU Utilization.
 *  Future: allow choosing CPU / Memory for sort.
 */
bool Process::operator<(Process const& other) const {
  /* in case we want to sort by memory use.
  long aMemory = std::stol(this->Ram());
  long bMemory = std::stol(other.Ram());
  return aMemory < bMemory;
  */
  // sort by CPU Utilization.
  return this->CpuUtilization() < other.CpuUtilization();
}
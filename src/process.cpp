#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Instantiate a Process object with PID.
// pid_ will be used to gather other information related to the Process.
Process::Process(int pid) {
    pid_ = pid;
    user_= ProcessGetUser();
    command_ = ProcessGetCommand();
    cpuUtilization_ = ProcessGetUtil();
    ram_ = ProcessGetRam();
    uptime_ = ProcessGetUpTime();
}

std::string Process::ProcessGetUser() {
    // use /proc/<pid>/status and /etc/passwd
    // to obtain the user name.
  string key, userId, dontCare, readUserName, readUserId;
  string line;
  string userName;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid_) + "/status");
  if (stream.is_open()) {

    while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        linestream >> key >> readUserId;
        if (key == "Uid:") {
            userId = readUserId;
            break;
        }
    }
    std::ifstream unamestream("/etc/passwd");
    if (unamestream.is_open()) {
        while(std::getline(unamestream, line)) {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream linestreamuser(line);
            linestreamuser >> readUserName >> dontCare>> readUserId;
            if (readUserId == userId) {
                // winner winner chicken dinner
                userName = readUserName;
                break;
            }

        }
    }
  }
  return userName;
}

std::string Process::ProcessGetCommand() {
    return "bash";
}

float Process::ProcessGetUtil() {
    return 0.0;
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
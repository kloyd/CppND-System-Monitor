#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Done: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  float memFree, memTotal;
  string value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          memTotal = std::stof(value);
        }
        if (key == "MemFree:") {
          memFree = std::stof(value);
        }
      }
    }
  }
  // Return as a percentage 
  // Based on HTOP method. Total used memory = MemTotal - MemFree
  // Percent = Total used / Total. 
  return (memTotal - memFree) / memTotal;
}

// Done: Read and return the system uptime
long LinuxParser::UpTime() {   
  string line;
  string uptime_s, idletime;
  long uptime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> uptime_s >> idletime;
      uptime = std::stol(uptime_s);
    }
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  string line;
  string key;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice; 
  long totalJiff;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      // Read every line from /proc/stat, match on key = "cpu"
      while (linestream >> key >> user >> nice >> system >> idle >> 
             iowait >> irq >> softirq >> steal >> guest >> guest_nice) {
        if (key == "cpu") {
          totalJiff = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
          break;
        }
      }
    }
  }

  return totalJiff; 
  
}

float LinuxParser::CpuUtilizationPerProcess(int pid) {
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

    /* I think cpu utilization should be based on ActiveJiffies per PID
       and it should be computed off of active = utime + stime */
    
    long uptime = LinuxParser::UpTime();
    int hertz = sysconf(_SC_CLK_TCK);
    long total_time;
    string skip;
    long utime, stime, cutime, cstime, n18, n19;
    long n20, n21, starttime;
    string line;
    string userName;
    std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + "/stat");
    while (std::getline(stream, line)) {

      std::istringstream linestream(line);
      linestream.ignore(256, ')');
      // Starts at 'S' which is column 3.
      //             3       4      5        6       7       8       9
      linestream >> skip >> skip >> skip >> skip >> skip >> skip >> skip >>
      //  10     11      12      13      14
        skip >> skip >> skip >> skip >> utime >> stime >> cutime >> cstime >>
        n18 >> n19 >> n20 >> n21 >> starttime;;
    }
    total_time = utime + stime;
    long seconds = uptime - (starttime / hertz);
    float cpu_usage = (((float) total_time / (float) hertz) / (float) seconds);
    return cpu_usage;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  string line;
  string key;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice; 
  long activeJiffies;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      // Read every line from /proc/stat, match on key = "cpu"
      while (linestream >> key >> user >> nice >> system >> idle >> 
             iowait >> irq >> softirq >> steal >> guest >> guest_nice) {
        if (key == "cpu") {
          activeJiffies = user + nice + system + irq + softirq + steal;
          break;
        }
      }
    }
  }

  return activeJiffies;
 }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {  
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      // Read every line from /proc/stat, match on key = "processes"
      while (linestream >> key >> value) {
        if (key == "processes") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return std::stoi(value);
        }
      }
    }
  }
  return 0;
}

// Done: Read and return the number of running processes
int LinuxParser::RunningProcesses() {   
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      // Read every line from /proc/stat, match on key = "procs_running"
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return std::stoi(value);
        }
      }
    }
  }
  return 0; 
}

// Done: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
    string cmdline;
    string line;
    // Linux stores the command used to launch the function in the /proc/[pid]/cmdline file.
    std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + "/cmdline");
    if (stream.is_open()) {
        while (std::getline(stream, line)) {
            std::istringstream linestream(line);
            linestream >> cmdline;
        }
    }
    return cmdline;

}

// Done: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string key, ramUsed;
  long vmsize = 0;
  string line;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + "/status");
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        linestream >> key >> ramUsed;
        if (key == "VmSize:") {
          vmsize = std::stol(ramUsed);
          break;
        }
    }
  }
  // convert to MB
  if (vmsize == 0) {
    return "0";
  } else {
    vmsize = vmsize / 1024;
    ramUsed = std::to_string(vmsize);
    return ramUsed; 
  }
}

// Done: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string key, userId, readUserId;
  string line;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + "/status");
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        linestream >> key >> readUserId;
        if (key == "Uid:") {
            userId = readUserId;
            break;
        }
    }
  }
  return userId;
 }

// Done: Read and return the user associated with a process
string LinuxParser::User(int pid) { 
    // use /proc/<pid>/status and /etc/passwd
    // to obtain the user name.
  string key, userId, dontCare, readUserName, readUserId;
  string line;
  string userName;
  userId = Uid(pid);
  std::ifstream unamestream("/etc/passwd");
  if (unamestream.is_open()) {
    while(std::getline(unamestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestreamuser(line);
      linestreamuser >> readUserName >> dontCare>> readUserId;
      if (readUserId == userId) {
        // found matching user id in /etc/passwd, save it.
        userName = readUserName;
        break;
      }

    }
  }
  return userName;
 }

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  /*
  Currently getting this from starttime... maybe this should be utime + stime ??
  */
    int hertz = sysconf(_SC_CLK_TCK);
    // long n1;
    string c2, c3, c4, c5, c6, skip;
    //long n4, n5, n6, n7, n8, n9, n10, n11, n12, n13;
    string utime, stime, cutime, cstime, n18, n19;
    long starttime;
    starttime = 0;
    string line;
    string userName;
    std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + "/stat");
    while (std::getline(stream, line)) {

      std::istringstream linestream(line);
      linestream.ignore(256, ')');
      // should skip n1, c2 (c2 = "(some thing)")
      linestream >> c3 >> c4 >> c5 >> c6 >> skip >> skip >> skip >>
        skip >> skip >> skip >> skip >> utime >> stime >> cutime >> cstime >>
        skip >> skip >> skip >> skip >> starttime;;
    }

    return starttime / hertz; 
  
}
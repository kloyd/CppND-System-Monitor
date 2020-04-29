#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

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

// Done: Read and return the number of jiffies for the system
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
      while (linestream >> key >> user >> nice >> system >> idle >> iowait >>
             irq >> softirq >> steal >> guest >> guest_nice) {
        if (key == "cpu") {
          totalJiff = user + nice + system + idle + iowait + irq + softirq +
                      steal + guest + guest_nice;
          break;
        }
      }
    }
  }

  return totalJiff;
}

float LinuxParser::CpuUtilization(int pid) {
  /*
   To calculate CPU usage for a specific process you'll need the following:

   /proc/uptime
       #1 uptime of the system (seconds)
   /proc/[PID]/stat
       #14 utime - CPU time spent in user code, measured in clock ticks
       #15 stime - CPU time spent in kernel code, measured in clock ticks
       #16 cutime - Waited-for children's CPU time spent in user code (in clock
   ticks) #17 cstime - Waited-for children's CPU time spent in kernel code (in
   clock ticks) #22 starttime - Time when the process started, measured in clock
   ticks Hertz (number of clock ticks per second) of your system. In most cases,
   getconf CLK_TCK can be used to return the number of clock ticks. The
   sysconf(_SC_CLK_TCK) C function call may also be used to return the hertz
   value.

   */

  long uptime = LinuxParser::UpTime();
  int hertz = sysconf(_SC_CLK_TCK);
  long total_time;
  string skip;
  long utime{0}, stime{0}, cutime{0}, cstime{0}, starttime{0};
  string line;
  string userName;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       "/stat");
  while (std::getline(stream, line)) {
    std::istringstream linestream(line);
    // Skip past executable name - it may contain a space, but always ends with
    // ')'
    linestream.ignore(256, ')');
    // skip next 11 columns.
    for (int i = 0; i < 11; i++) {
      linestream >> skip;
    }
    // starting at column 14.
    linestream >> utime >> stime >> cutime >> cstime;
    // skip to column 22.
    for (int i = 0; i < 4; i++) {
      linestream >> skip;
    }
    linestream >> starttime;
    ;
  }
  total_time = utime + stime + cutime + cstime;
  long seconds = uptime - (starttime / hertz);
  // don't do division by zero... assume 100% cpu.
  if (seconds == 0) {
    return 1.0;
  } else {
    float cpu_usage = (((float)total_time / (float)hertz) / (float)seconds);
    return cpu_usage;
  }
}

/*
    Read and return the number of active jiffies for a PID
      use /proc/<pid>/stat
      select utime and stime columns.
*/
long LinuxParser::ActiveJiffies(int pid) {
  string skip;
  long utime{0}, stime{0}, cutime{0}, cstime{0};
  string line;
  string userName;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       "/stat");
  while (std::getline(stream, line)) {
    std::istringstream linestream(line);
    // skip over the first 2 columns - the executable may have a space
    // which will throw out the istringstream parser.
    linestream.ignore(256, ')');
    // skip over columns 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, and 13
    // http://man7.org/linux/man-pages/man5/proc.5.html
    for (int i = 0; i < 11; i++) {
      linestream >> skip;
    }
    linestream >> utime >> stime >> cutime >> cstime;
  }
  return utime + stime + cutime + cstime;
}

// Read and return the number of active jiffies for the system
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
      while (linestream >> key >> user >> nice >> system >> idle >> iowait >>
             irq >> softirq >> steal >> guest >> guest_nice) {
        if (key == "cpu") {
          activeJiffies = user + nice + system + irq + softirq + steal;
          break;
        }
      }
    }
  }

  return activeJiffies;
}

// Done: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string line;
  string key;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  long idleJiffies;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      // Read every line from /proc/stat, match on key = "cpu"
      while (linestream >> key >> user >> nice >> system >> idle >> iowait >>
             irq >> softirq >> steal >> guest >> guest_nice) {
        if (key == "cpu") {
          idleJiffies = idle;
          break;
        }
      }
    }
  }
  return idleJiffies;
}

// Return CPU Utilization as active jiffies / total jiffies.
float LinuxParser::CpuUtilization() {
  float active, total;
  active = LinuxParser::ActiveJiffies();
  total = LinuxParser::Jiffies();
  return active / total;
}

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
  // default to "None" - if we don't find a cmdline entry, we won't
  // show the process. (May be special root only process)
  string cmdline{"None"};
  string line;
  // Linux stores the command used to launch the function in the
  // /proc/[pid]/cmdline file.
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       "/cmdline");
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
  string key, vmsize_s;
  long vmsize{0};
  string line;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       "/status");
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> vmsize;
      if (key == "VmSize:") {
        // vmsize = std::stol(ramUsed);
        break;
      }
    }
  }
  // convert to MB
  if (vmsize == 0) {
    return "0";
  } else {
    vmsize = vmsize / 1024;
    vmsize_s = std::to_string(vmsize);
    return vmsize_s;
  }
}

// Done: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string key, userId, readUserId;
  string line;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       "/status");
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
  std::ifstream unamestream(LinuxParser::kPasswordPath);
  if (unamestream.is_open()) {
    while (std::getline(unamestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestreamuser(line);
      linestreamuser >> readUserName >> dontCare >> readUserId;
      if (readUserId == userId) {
        // found matching user id in /etc/passwd, save it.
        userName = readUserName;
        break;
      }
    }
  }
  return userName;
}

// Done: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  // Use ActiveJiffies for process, divide by clock ticks.
  int hertz = sysconf(_SC_CLK_TCK);
  long jiffies = ActiveJiffies(pid);
  return jiffies / hertz;
}
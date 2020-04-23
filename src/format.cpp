#include <string>

#include "format.h"

using std::string;

// Done: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    long minutes = seconds / 60;
    long elapsed_seconds = seconds % 60;
    long hours = minutes / 60;
    minutes = minutes % 60;

    std::string str_hours = std::to_string(hours);
    if (str_hours.length() < 2) {
        str_hours = "0" + str_hours;
    }
    std::string str_minutes = std::to_string(minutes);
    if (str_minutes.length() < 2) {
        str_minutes = "0" + str_minutes;
    }
    std::string str_seconds = std::to_string(elapsed_seconds);
    if (str_seconds.length() < 2) {
        str_seconds = "0" + str_seconds;
    }

    return str_hours + ":" + str_minutes + ":" + str_seconds;
    
}
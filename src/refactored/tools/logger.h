#include <string>
#include <stdexcept>

typedef enum { LOG_ERROR, LOG_INFO, LOG_DEBUG } log_type;

using std::string;
 

string log_type_name(log_type type) {
    switch (type)
    {
        case LOG_ERROR:
            return "Error";
        case LOG_INFO:
            return "Info";
        case LOG_DEBUG:
            return "Debug";
        default: // will probably show only int value?
            throw std::runtime_error("Unhandled log type");
    }
    return "\0";
}

void log(string message) {
    // Enum is c and c++
    // how to concatinate in C++
    string type_name = log_type_name(LOG_DEBUG);
    type_name = "["+ type_name +"]";
    std::cout << type_name << " " << message << std::endl;
}

void log(log_type type, string message) {
    // Enum is c and c++
    // how to concatinate in C++
    string type_name = log_type_name(type);
    type_name = "["+ type_name +"]";
    std::cout << type_name << " " << message << std::endl;
}
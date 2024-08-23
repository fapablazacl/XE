
#ifndef XE_LOGGER_H
#define XE_LOGGER_H

#include <string>

class ILogger {
public:
    virtual ~ILogger() {}
    virtual void log(const std::string &info) = 0;
};


class Logger : public ILogger {
public:
    explicit Logger(const std::string &parentName);

    virtual ~Logger();

    void log(const std::string &info) override;

private:
    std::string mParentName;
};

#endif //XE_LOGGER_H

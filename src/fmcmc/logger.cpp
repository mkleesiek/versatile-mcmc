/*
 * logger.cxx
 *
 *  Created on: 25.07.2016
 *      Author: marco@kleesiek.com
 */

#include "fmcmc/logger.h"
#include "fmcmc/exception.h"

#include <iomanip>
#include <map>
#include <utility>

// COLOR DEFINITIONS
#define COLOR_NORMAL "0"
#define COLOR_BRIGHT "1"
#define COLOR_FOREGROUND_RED "31"
#define COLOR_FOREGROUND_GREEN "32"
#define COLOR_FOREGROUND_YELLOW "33"
#define COLOR_FOREGROUND_CYAN "36"
#define COLOR_FOREGROUND_WHITE "37"
#define COLOR_PREFIX "\033["
#define COLOR_SUFFIX "m"
#define COLOR_SEPARATOR ";"

constexpr const char* skEndColor =   COLOR_PREFIX COLOR_NORMAL COLOR_SUFFIX;
constexpr const char* skFatalColor = COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_RED    COLOR_SUFFIX;
constexpr const char* skErrorColor = COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_RED    COLOR_SUFFIX;
constexpr const char* skWarnColor =  COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_YELLOW COLOR_SUFFIX;
constexpr const char* skInfoColor =  COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_GREEN  COLOR_SUFFIX;
constexpr const char* skDebugColor = COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_CYAN   COLOR_SUFFIX;
constexpr const char* skOtherColor = COLOR_PREFIX COLOR_BRIGHT COLOR_SEPARATOR COLOR_FOREGROUND_WHITE  COLOR_SUFFIX;


using namespace std;

namespace fmcmc {

namespace {

const char* level2Color(Logger::ELevel level)
{
    switch(level) {
        case Logger::ELevel::Fatal:
            return skFatalColor;
        case Logger::ELevel::Error:
            return skErrorColor;
        case Logger::ELevel::Warn:
            return skWarnColor;
        case Logger::ELevel::Info:
            return skInfoColor;
        case Logger::ELevel::Debug:
            return skDebugColor;
        case Logger::ELevel::Trace:
            return skDebugColor;
        default:
            return skOtherColor;
    }
}

const char* level2Str(Logger::ELevel level)
{
    switch(level) {
        case Logger::ELevel::Trace :
            return "TRACE";
        case Logger::ELevel::Debug :
            return "DEBUG";
        case Logger::ELevel::Info  :
            return "INFO";
        case Logger::ELevel::Warn  :
            return "WARN";
        case Logger::ELevel::Error :
            return "ERROR";
        case Logger::ELevel::Fatal :
            return "FATAL";
        default     :
            return "XXX";
    }
}

}

Logger::Logger(const std::string& name) :
    fName( name ),
    fActiveStream( nullptr ),
    fMinLevel( ELevel::Debug ),
    fColouredOutput( true )
{ }

Logger::~Logger()
{ }

bool Logger::IsLevelEnabled(ELevel level) const
{
    return fMinLevel <= level;
}

Logger::ELevel Logger::GetLevel() const
{
    return fMinLevel;
}

void Logger::SetLevel(ELevel level)
{
    fMinLevel = level;
}

void Logger::SetColoured(bool coloured)
{
    fColouredOutput = coloured;
}

void Logger::StartMessage(ELevel level, const Location& loc)
{
    const char* levelStr = level2Str(level);

    fActiveStream = (level >= ELevel::Error) ? &cerr : &cout;

    if (fColouredOutput)
    {
        const char* color = level2Color(level);
        *fActiveStream << color << __DATE__ " " __TIME__ " [" << setw(5) << levelStr << "] " << setw(16) << fName << ": ";
    }
    else
    {
        *fActiveStream << __DATE__ " " __TIME__ " [" << setw(5) << levelStr << "] " << setw(16) << fName << ": ";
    }
}

ostream& Logger::Log()
{
    if (!fActiveStream)
        throw Exception() << "No active logger availabe.";

    return *fActiveStream;
}

void Logger::EndMessage()
{
    if (fColouredOutput)
        Log() << skEndColor;

    Log() << endl;
}

}

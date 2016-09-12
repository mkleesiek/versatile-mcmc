/**
 * @file
 *
 * @date 25.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/logger.hpp>
#include <vmcmc/exception.hpp>

#include <iomanip>
#include <map>
#include <utility>
#include <chrono>
#include <unordered_map>

using namespace std;

namespace std {
/**
 * Hash specialization for the vmcmc::Logger::ELevel enumeration.
 * @cond PRIVATE
 */
template<>
struct hash<vmcmc::Logger::ELevel>
{
    size_t operator() (const vmcmc::Logger::ELevel& level) const {
        return static_cast<int>(level);
    }
};
/** @endcond */
}

namespace vmcmc {

namespace {

const string kResetColor =  "\033[0m";  // reset to standard console color

const unordered_map<Logger::ELevel, string> loggerColors = {
    { Logger::ELevel::Fatal,     "\033[1;31m" },    // red
    { Logger::ELevel::Error,     "\033[1;31m" },    // red
    { Logger::ELevel::Warn,      "\033[1;33m" },    // yellow
    { Logger::ELevel::Info,      "\033[1;32m" },    // green
    { Logger::ELevel::Debug,     "\033[1;36m" },    // cyan
    { Logger::ELevel::Trace,     "\033[1;37m" },    // white
    { Logger::ELevel::Undefined, kResetColor  }     // white
};

const unordered_map<Logger::ELevel, string> loggerStrings = {
    { Logger::ELevel::Fatal,     "FATAL" },
    { Logger::ELevel::Error,     "ERROR" },
    { Logger::ELevel::Warn,      "WARN" },
    { Logger::ELevel::Info,      "INFO" },
    { Logger::ELevel::Debug,     "DEBUG" },
    { Logger::ELevel::Trace,     "TRACE" },
    { Logger::ELevel::Undefined, "" }
};

void printTime(ostream& strm)
{
    using namespace std::chrono;

    const auto now = system_clock::now();
    const time_t cTime = system_clock::to_time_t(now);

    auto duration = now.time_since_epoch();
    duration -= duration_cast<seconds>(duration);

    /*
     * Unfortunately, g++ < 5.0 does not implement std::put_time, so I have to
     * resort to strftime at this point:
     */
    char dateTimeStr[24];
    strftime(dateTimeStr, sizeof(dateTimeStr), "%F %T", localtime(&cTime));
    strm << dateTimeStr;
    strm << "." << setfill('0') << setw(3) << duration_cast<milliseconds>(duration).count();
    strm << setfill(' ');
}

}

Logger::Location::Location(const char* const fileName, const char* const functionName, int lineNumber) :
    fLineNumber(lineNumber),
    fFileName(fileName),
    fFunctionName(functionName)
{
    const size_t slashPos = fFileName.find_last_of('/');
    if (slashPos != string::npos)
        fFileName = fFileName.substr( slashPos+1 );
}

Logger::Logger(const std::string& name) :
    fName( name ),
    fActiveStream( nullptr ),
    fMinLevel( ELevel::Trace ),
    fColouredOutput( true )
{
#ifdef NDEBUG
    fMinLevel = ELevel::Info;
#endif
}

Logger::~Logger()
{ }

void Logger::StartMessage(ELevel level, const Location& loc)
{
    const string& levelStr = loggerStrings.find(level)->second;

    fActiveStream = (level >= ELevel::Error) ? &cerr : &cout;

    printTime( *fActiveStream );

    *fActiveStream << " [";

    if (fColouredOutput) {
        const string& startColor = loggerColors.find(level)->second;
        *fActiveStream << startColor << setw(5) << levelStr << kResetColor;
    }
    else {
        *fActiveStream << setw(5) << levelStr;
    }

    *fActiveStream << "] " << setw(17) << loc.fFileName
        << "(" << setw(3) << loc.fLineNumber << ") ";
}

void Logger::EndMessage()
{
    *fActiveStream << endl;

    fActiveStream->flush();
}

}

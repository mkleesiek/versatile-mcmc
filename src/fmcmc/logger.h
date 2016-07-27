/*
 * logger.h
 *
 *  Created on: 25.07.2016
 *      Author: marco@kleesiek.com
 */

#ifndef FMCMC_LOGGER_H_
#define FMCMC_LOGGER_H_


// UTILITY MACROS

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define __FILE_LINE__      __FILE__ " (" TOSTRING(__LINE__) ")"
#define __FILENAME_LINE__  (strrchr(__FILE__, '/') ? strrchr(__FILE_LINE__, '/') + 1 : __FILE_LINE__)

#if defined(_MSC_VER)
#if _MSC_VER >= 1300
      #define __FUNC__ __FUNCSIG__
#endif
#else
#if defined(__GNUC__)
      #define __FUNC__ __PRETTY_FUNCTION__
#endif
#endif
#if !defined(__FUNC__)
#define __FUNC__ ""
#endif

#define va_num_args(...) va_num_args_impl(__VA_ARGS__, 5,4,3,2,1)
#define va_num_args_impl(_1,_2,_3,_4,_5,N,...) N

#define macro_dispatcher(func, ...) macro_dispatcher_(func, va_num_args(__VA_ARGS__))
#define macro_dispatcher_(func, nargs) macro_dispatcher__(func, nargs)
#define macro_dispatcher__(func, nargs) func ## nargs

// INCLUDES

#include <string>
#include <iostream>
#include <sstream>

namespace fmcmc {

/**
 * Very primitive MACRO-centered logging facility.
 * No external configuration mechanisms (yet).
 */
class Logger
{
public:
    enum class ELevel {
        Trace, Debug, Info, Warn, Error, Fatal, Undefined
    };

public:
    /**
     * A simple struct used by the Logger macros to pass information about the filename and line number.
     * Not to be used directly by the user.
     */
    struct Location {
        Location(const char* const fileName = "", const char* const functionName = "", int lineNumber = -1) :
            fLineNumber(lineNumber), fFileName(fileName), fFunctionName(functionName)
            { }
        int fLineNumber;
        const char* fFileName;
        const char* fFunctionName;
    };

public:
    /**
     * Standard constructor assigning a name to the logger instance.
     * @param name The logger name.
     */
    Logger(const std::string& name = "");

    virtual ~Logger();

    /**
     * Check whether a certain log-level is enabled.
     * @param level The log level as string representation.
     * @return
     */
    bool IsLevelEnabled(ELevel level) const;

    /**
     * Get a loggers minimum logging level
     * @return level enum item identifying the log level
     */
    ELevel GetLevel() const;

    /**
     * Set a loggers minimum logging level
     * @param level enum item identifying the log level
     */
    void SetLevel(ELevel level);

    /**
     * Set whether the output should be coloured or not.
     * @param coloured
     */
    void SetColoured(bool coloured);

    /**
     * Log a message with the specified level.
     * Use the macro LOG(logger, level, message).
     * @param level The log level.
     * @param message The message.
     * @param loc Source code location (set automatically by the corresponding macro).
     */
    void StartMessage(ELevel level, const Location& loc = Location());

    std::ostream& Log();

    void EndMessage();

private:
    std::string fName;
    std::ostream* fActiveStream;
    ELevel fMinLevel;
    bool fColouredOutput;
};

} /* namespace fmcmc */

// PRIVATE MACROS

#define __LOG_LOCATION         fmcmc::Logger::Location(__FILE__, __FUNC__, __LINE__)

#define __LOG_DEFINE_2(I,K)    static fmcmc::Logger I(K);
#define __LOG_DEFINE_1(K)      static fmcmc::Logger sLocalLoggerInstance(K);

#define __LOG_4(I,L,M,O) \
{ \
    if (I.IsLevelEnabled(fmcmc::Logger::ELevel::L)) { \
        static bool _sLoggerMarker = false; \
        if (!O || !_sLoggerMarker) { \
            _sLoggerMarker = true; \
            std::ostringstream stream; stream << M; \
            I.StartMessage(fmcmc::Logger::ELevel::L, __LOG_LOCATION); \
            I.Log() << M; \
            I.EndMessage(); \
        } \
    } \
}

#define __LOG_3(I,L,M)     __LOG_4(I,L,M,false)
#define __LOG_2(L,M)       __LOG_4(sLocalLoggerInstance,L,M,false)
#define __LOG_1(M)         __LOG_4(sLocalLoggerInstance,Debug,M,false)

#define __LOG_ONCE_3(I,L,M)     __LOG_4(I,L,M,true)
#define __LOG_ONCE_2(L,M)       __LOG_4(sLocalLoggerInstance,L,M,true)
#define __LOG_ONCE_1(M)         __LOG_4(sLocalLoggerInstance,Debug,M,true)

#define __LOG_ASSERT_3(I,C,M)       if (!(C)) { __LOG_3(I,Error,M) }
#define __LOG_ASSERT_2(C,M)         if (!(C)) { __LOG_3(sLocalLoggerInstance,Error,M) }


// PUBLIC MACROS

#define LOG_DEFINE(...)   macro_dispatcher(__LOG_DEFINE_, __VA_ARGS__)(__VA_ARGS__)

#define LOG(...)          macro_dispatcher(__LOG_, __VA_ARGS__)(__VA_ARGS__)

#define LOG_ONCE(...)     macro_dispatcher(__LOG_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define LOG_ASSERT(...)   macro_dispatcher(__LOG_ASSERT_, __VA_ARGS__)(__VA_ARGS__)


#endif /* FMCMC_LOGGER_H_ */

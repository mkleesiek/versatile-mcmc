/**
 * @file
 *
 * @date 25.07.2016
 * @author marco@kleesiek.com
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

namespace vmcmc {

/**
 * Very primitive MACRO-centered logging facility. It should primarily be used
 * through MACROs:
 *
 * @code{.cpp}
 * // define a static logger instance:
 * LOG_DEFINE("some.logger.name");
 *
 * // log an expression with given log-level
 * LOG(Info, "Some example log: " << someVariable << " more text ...");
 *
 * // log an expression only once during execution
 * LOG_ONCE(Info, "This is displayed only once.");
 *
 * // make an assertion and abort if false:
 * LOG_ASSERT( someCondition, "That assertion just failed." );
 * @endcode
 *
 * Available log levels are: Trace, Debug, Info, Warn, Error, Fatal
 *
 * By default, if the library is compiled in release-mode, Trace and Debug
 * outputs are silenced.
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
        Location(const char* const fileName = "", const char* const functionName = "", int lineNumber = -1);
        int fLineNumber;
        std::string fFileName;
        std::string fFunctionName;
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
    bool IsLevelEnabled(ELevel level) const { return fMinLevel <= level; }

    /**
     * Get a loggers minimum logging level
     * @return level enum item identifying the log level
     */
    ELevel GetLevel() const { return fMinLevel; }

    /**
     * Set a loggers minimum logging level
     * @param level enum item identifying the log level
     */
    void SetLevel(ELevel level) { fMinLevel = level; }

    /**
     * Set whether the output should be coloured or not.
     * @param coloured
     */
    void SetColoured(bool coloured) { fColouredOutput = coloured; }

    /**
     * Log a message with the specified level.
     * Use the macro LOG(logger, level, message).
     * @param level The log level.
     * @param message The message.
     * @param loc Source code location (set automatically by the corresponding macro).
     */
    void StartMessage(ELevel level, const Location& loc = Location());

    std::ostream& Log() { return *fActiveStream; }

    void EndMessage();

private:
    std::string fName;
    std::ostream* fActiveStream;
    ELevel fMinLevel;
    bool fColouredOutput;
};

} /* namespace vmcmc */

// PRIVATE MACROS

#define __LOG_LOCATION         vmcmc::Logger::Location(__FILE__, __FUNC__, __LINE__)

#define __LOG_DEFINE_2(I,K)    static vmcmc::Logger I(K);
#define __LOG_DEFINE_1(K)      static vmcmc::Logger sLocalLoggerInstance(K);

#define __LOG_3(I,L,M) \
{ \
    if (I.IsLevelEnabled(vmcmc::Logger::ELevel::L)) { \
        I.StartMessage(vmcmc::Logger::ELevel::L, __LOG_LOCATION); \
        I.Log() << M; \
        I.EndMessage(); \
    } \
}

#define __LOG_ONCE_3(I,L,M) \
{ \
    if (I.IsLevelEnabled(vmcmc::Logger::ELevel::L)) { \
        static bool _sLogMarker = false; \
        if (!_sLogMarker) { \
            _sLogMarker = true; \
            I.StartMessage(vmcmc::Logger::ELevel::L, __LOG_LOCATION); \
            I.Log() << M; \
            I.EndMessage(); \
        } \
    } \
}

#define __LOG_ASSERT_3(I,C,M) \
{ \
    if (I.IsLevelEnabled(vmcmc::Logger::ELevel::Error) && !(C)) { \
        I.StartMessage(vmcmc::Logger::ELevel::Error, __LOG_LOCATION); \
        I.Log() << "Assertion '(" << TOSTRING(C) << ")' failed. " << M; \
        I.EndMessage(); \
        abort(); \
    } \
}

#define __LOG_2(L,M)          __LOG_3(sLocalLoggerInstance,L,M)
#define __LOG_1(M)            __LOG_3(sLocalLoggerInstance,Debug,M)

#define __LOG_ONCE_2(L,M)     __LOG_ONCE_3(sLocalLoggerInstance,L,M)
#define __LOG_ONCE_1(M)       __LOG_ONCE_3(sLocalLoggerInstance,Debug,M)

#define __LOG_ASSERT_1(C)     __LOG_ASSERT_3(sLocalLoggerInstance,C,"")
#define __LOG_ASSERT_2(C,M)   __LOG_ASSERT_3(sLocalLoggerInstance,C,M)


// PUBLIC MACROS

#define LOG_DEFINE(...)   macro_dispatcher(__LOG_DEFINE_, __VA_ARGS__)(__VA_ARGS__)

#define LOG(...)          macro_dispatcher(__LOG_, __VA_ARGS__)(__VA_ARGS__)

#define LOG_ONCE(...)     macro_dispatcher(__LOG_ONCE_, __VA_ARGS__)(__VA_ARGS__)
#define LOG_ASSERT(...)   macro_dispatcher(__LOG_ASSERT_, __VA_ARGS__)(__VA_ARGS__)


#endif /* FMCMC_LOGGER_H_ */
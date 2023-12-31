#ifndef _QLLOGGER_H_
#define _QLLOGGER_H_

#include <stdarg.h>
#include "../config.h"

#ifdef HAVE_LOG4CXX
    #include <log4cxx/logger.h>
#endif

#define MAX_ROW_LENGTH  512


/**
 * This is facade class for logging facility
 */
class QLLogger {

    public:
        /**
         * Level enum represents supported logging levels
         */
        enum Level {
            TRACE,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL
        };

        /**
         * ctors
         */
        QLLogger();
        QLLogger(Level);

        /** 
         * This method initializes logging facility.
         * @param Level logging level
         * @param logFile a destination file name
         */
        void initialize(Level, const char * logFile);

        /**
         * This method logs a message with given level
         */
        void log(Level level, const char * message);

        void logT(const char * format, ...);
        void logD(const char * format, ...);
        void logI(const char * format, ...);
        void logW(const char * format, ...);
        void logE(const char * format, ...);
        void logF(const char * format, ...);


    private:

#ifdef HAVE_LOG4CXX
        log4cxx::LoggerPtr logger; //(log4cxx::Logger::getLogger("qlbar"));
        log4cxx::LevelPtr getLog4cxxLevel(Level level);
#else
        Level logLevel;
#endif


};

#endif


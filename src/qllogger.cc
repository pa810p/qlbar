#include <string.h>
#include <stdio.h>

#include "qllogger.h"

#ifdef HAVE_LOG4CXX
    #include <log4cxx/basicconfigurator.h>
    #include <log4cxx/fileappender.h>
    #include <log4cxx/consoleappender.h>
    #include <log4cxx/writerappender.h>
    #include <log4cxx/simplelayout.h>
    #include <log4cxx/patternlayout.h>

    using namespace log4cxx;

#endif

/**
 * One static fantastic global logger in the application
 **/
QLLogger qllogger;


QLLogger::QLLogger() {
#ifdef HAVE_LOG4CXX
    this->logger = Logger::getLogger("qlbar");
#endif
}

QLLogger::QLLogger(Level level) {
}

void QLLogger::initialize(Level level, const char * logFile) {
#ifdef HAVE_LOG4CXX

    // creating appender
    WriterAppenderPtr appender;

    if (NULL != logFile) {
        appender = new FileAppender();
        ((FileAppenderPtr)appender)->setAppend(true);
        ((FileAppenderPtr)appender)->setFile(logFile);
    }
    else {
        appender = new ConsoleAppender();
    }

    appender->setLayout(new PatternLayout("%d{yyyy-MM-dd HH:mm:ss,SSS} [%p] %m"));

    helpers::Pool p;
    appender->activateOptions(p);
    
    //logger->addAppender(fileAppender);

    BasicConfigurator::configure(appender);
    logger->setLevel(getLog4cxxLevel(level));
#else

    this->logLevel = level;
#endif
}


#ifdef HAVE_LOG4CXX
LevelPtr QLLogger::getLog4cxxLevel(Level level) {
    switch (level) {
        case TRACE: 
            return log4cxx::Level::getTrace();
        case DEBUG:
            return log4cxx::Level::getDebug();
        case INFO:
            return log4cxx::Level::getInfo();
        case WARN:
            return log4cxx::Level::getWarn();
        case ERROR:
            return log4cxx::Level::getError();
        case FATAL:
            return log4cxx::Level::getFatal();
        default:
            return log4cxx::Level::getTrace();
    };

}
#endif

#define QLLOG(level, slevel, message) \
    if (level >= logLevel) { \
        fprintf(stderr, slevel); \
        fprintf(stderr, message); \
    }


/**
 * This method logs a message with given level
 */
void QLLogger::log(Level level, const char * message) {

    switch (level) {
        case TRACE: 
#ifdef HAVE_LOG4CXX
            LOG4CXX_TRACE(logger, message);
#else
            QLLOG(level, "TRACE: ", message);
#endif
            break;
        case DEBUG:
#ifdef HAVE_LOG4CXX
            LOG4CXX_DEBUG(logger, message);
#else
            QLLOG(level, "DEBUG: ", message);
#endif
            break;
        case INFO:
#ifdef HAVE_LOG4CXX
            LOG4CXX_INFO(logger, message);
#else
            QLLOG(level, "INFO: ", message);
#endif
            break;
        case WARN:
#ifdef HAVE_LOG4CXX
            LOG4CXX_WARN(logger, message);
#else
            QLLOG(level, "WARN: ", message);
#endif
            break;
        case ERROR:
#ifdef HAVE_LOG4CXX
            LOG4CXX_ERROR(logger, message);
#else
            QLLOG(level, "ERROR: ", message);
#endif
            break;
        case FATAL:
#ifdef HAVE_LOG4CXX
            LOG4CXX_FATAL(logger, message);
#else
            QLLOG(level, "FATAL: ", message);
#endif
            break;
        default:
            fprintf(stderr, "Unknown logging level.");
            break;
    };

}


#define FORMAT_MESSAGE \
    char message [MAX_ROW_LENGTH]; \
    va_list list;\
    va_start(list, format); \
    vsnprintf(message, MAX_ROW_LENGTH-3, format, list); \
    va_end(list);\
    strcat(message, "\n");
 


/**
 * This method logs a TRACE massage
 */
void QLLogger::logT(const char * format, ...) {
    FORMAT_MESSAGE
 
    log(TRACE, message);

}

/**
 * This method log a DEBUG message
 */
void QLLogger::logD(const char * format, ...) {
    FORMAT_MESSAGE

    log(DEBUG, message);
}

/**
 * This method logs an INFO message
 */
void QLLogger::logI(const char * format, ...) {
    FORMAT_MESSAGE

    log(INFO, message);
}

/**
 * This method logs a WARNING message
 */
void QLLogger::logW(const char * format, ...) {
    FORMAT_MESSAGE

    log(WARN, message);
}

/**
 * This method logs an ERROR message
 */
void QLLogger::logE(const char * format, ...) {
    FORMAT_MESSAGE

    log(ERROR, message);
}

/**
 * This method logs a FATAL message
 */
void QLLogger::logF(const char * format, ...) {
    FORMAT_MESSAGE

    log(FATAL, message);
}

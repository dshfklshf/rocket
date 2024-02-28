#include "rocket/common/log.h"
#include <sys/time.h>
#include <sstream>
#include <stdio.h>
#include "rocket/common/util.h"
#include "rocket/common/config.h"
#include "rocket/common/mutex.h"

namespace rocket {

    static Logger* g_logger = NULL;

    Logger* Logger::GetGlobalLogger() {
        
        return g_logger;
    }

   void Logger::InitGlobalLogger() {
        LogLevel global_log_level = StringToLogLevel(Config::GetGlobalConfig()->m_log_level);
        printf("Init log level [%s]\n", LogLevelToString(global_log_level).c_str());
        g_logger = new Logger(global_log_level);
    }

    std::string LogLevelToString(LogLevel level) {
        switch (level) {
            case Debug: return "DEBUG";
            case Info:  return "INFO";
            case Error: return "ERROR";
            default:    return "UNKNOWN";
        }
    }

    LogLevel StringToLogLevel(const std::string& log_level) {
        if (log_level == "DEBUG") return Debug;
        else if (log_level == "INFO") return Info;
        else if (log_level == "ERROR") return Error;
        else return Unknown;
    }


    std::string LogEvent::toString() {
        // 获取时间
        struct timeval now_time;

        gettimeofday(&now_time, nullptr);

        struct tm now_time_t;
        localtime_r(&(now_time.tv_sec), &now_time_t);

        char buf[128];
        strftime(&buf[0], 128, "%y-%m-%d %H:%M:%S", &now_time_t);
        std::string time_str(buf);  // 1.创建一个time_str的string对象，2.使用buf中的内容初始化time_str对象
        int ms = now_time.tv_usec / 1000;
        time_str = time_str + "." + std::to_string(ms);

        // 获取线程id，进程id
        m_pid = getPid();
        m_thread_id = getThreadId();

        // 将日志的级别、时间、源文件名以及行号按照一定的格式写入到 ss 中
        std::stringstream ss;
        ss << "[" << LogLevelToString(m_level) << "]\t"
            << "[" << time_str << "]\t"
            << "[" << m_pid << ":" << m_thread_id << "]\t";
            
        
        return ss.str();
    }

    void Logger::pushLog(const std::string& msg) {
        ScopeMutex<Mutex> lock(m_mutex);
        m_buffer.push(msg);
        lock.unlock();
    }

    void Logger::log() {
        ScopeMutex<Mutex> lock(m_mutex);    
        while (!m_buffer.empty()) {
            std::string msg = m_buffer.front();
            m_buffer.pop();

            //printf(msg.c_str());
            printf("%s",msg.c_str());
        }
        lock.unlock();
    }
}
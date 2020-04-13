#ifndef PROFILER_H
#define PROFILER_H

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>
#include <mutex>

// define PROFILING 0 to turn it off

#define PROFILING 1
#if PROFILING
#define PROFILE_SCOPE(name) ProfileTimer timer##__LINE__(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__PRETTY_FUNCTION__) // in Windows change __PRETTY_FUNCTION__ -> __FUNCSIG__

// this implementation is unsafe to work with threads.

struct ProfileResult
{
    std::string name;
    long long start, end;
    uint32_t threadID;
};


class ProfileWriter {
	friend class ProfileTimer;
private:
    std::string m_currentSession;
    std::ofstream m_outputStream;
    int m_profileCount;
    std::mutex m_mutex;
    bool m_inSession;

// private constructors: 
private:
    ProfileWriter()
        : m_profileCount(0), m_inSession(false) { }

// private functions: 
private:
    template <typename T>
	void beginSessionImpl(T&& name, const char* filepath = "results.json") {        
        if (m_inSession) {
            std::cout << "WARNING: Profiler session is already started\n";
            return;
        }        
		m_outputStream.open(filepath);
        m_inSession = true;
		if (!m_outputStream.is_open()) {
			std::cout << "WARNING: couldn't open or create the file for profiler\n";
            m_inSession = false;
            return;
        }        
        writeHeader();
        m_currentSession = std::forward<T>(name);
	}

	void endSessionImpl() {
        if (m_inSession) {
		    writeFooter();
            m_outputStream.close();        
            m_currentSession = "";
            m_profileCount = 0;
            m_inSession = false;
        }
        else
            std::cout << "WARNING: Profiler session is already finished\n";
	}

	void writeHeader() {
        m_outputStream << "{\n\"otherData\": {},\n\"traceEvents\": \n[\n";
        m_outputStream.flush();
    }

    void writeFooter() {
        m_outputStream << "]}";
        m_outputStream.flush();
    }

	void writeProfile(ProfileResult&& result) {
        if (!m_inSession)
            return;
        std::lock_guard<std::mutex> locker(m_mutex);

        if (m_profileCount++ > 0)
            m_outputStream << ",";

        std::string name = std::move(result.name);
        std::replace(name.begin(), name.end(), '"', '\'');

        m_outputStream << "\t{\n";
        m_outputStream << "\t\"cat\": \"function\",\n";
        m_outputStream << "\t\"dur\": " << (result.end - result.start) << ",\n";
        m_outputStream << "\t\"name\": \"" << name << "\",\n";
        m_outputStream << "\t\"ph\": \"X\",\n";
        m_outputStream << "\t\"pid\": 0,\n";
        m_outputStream << "\t\"tid\": " << result.threadID << ",\n";
        m_outputStream << "\t\"ts\": " << result.start << '\n';
        m_outputStream << "\t}\n";        

        m_outputStream.flush();
    }

	static ProfileWriter& get() {
        static ProfileWriter instance;
        return instance;
    }

// public constructors and assignment operators are deleted
public:
	ProfileWriter(const ProfileWriter& other) = delete;
	ProfileWriter(ProfileWriter&& other) = delete;
	ProfileWriter& operator=(const ProfileWriter& other) = delete;
	ProfileWriter&& operator=(ProfileWriter&& other) = delete;

	~ProfileWriter() {
		if (m_outputStream.is_open())
			m_outputStream.close();
	}

public:

    template <typename T>
    static void beginSession(T&& name, const char* filepath = "results.json") {
        static_assert(std::is_constructible_v<std::string, T>);
        get().beginSessionImpl(std::forward<T>(name), filepath);
    }

    static void endSession() {
        get().endSessionImpl();
    }
};



class ProfileTimer {
private:
	std::string m_name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;
	bool m_stopped;

public:
    template <typename T>
    explicit ProfileTimer(T&& name)
        : m_name(std::forward<T>(name)), m_stopped(false) {
        static_assert(std::is_constructible_v<std::string, T>);
        m_startTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~ProfileTimer() {
        if (!m_stopped)
            stop();
    }

    void stop() {
        auto endTimepoint = std::chrono::high_resolution_clock::now();

        long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
        long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

        uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
        ProfileWriter::get().writeProfile({ m_name, start, end, threadID });

        m_stopped = true;
    }
};

#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()

class ProfileWriter {
public:
    template <typename T>
    static void beginSession(T&&, const char* = "filename") { }
    static void endSession() { }
};
#endif
#endif // PROFILER_H
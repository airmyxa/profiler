#ifndef PROFILER_H
#define PROFILER_H

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>

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


class ProfileWriter
{
	friend class ProfileTimer;
private:
    std::string m_currentSession;
    std::ofstream m_outputStream;
    int m_profileCount;

// private constructors: 
private:
    ProfileWriter() : m_profileCount(0) { }

// private functions: 
private:
	void beginSessionImpl(const std::string& name, const std::string& filepath = "results.json") {
		m_outputStream.open(filepath);
		if (!m_outputStream.is_open())
			std::cerr << "couldn't open or create the file for profiler\n";
        writeHeader();
        m_currentSession = name;
	}

	void endSessionImpl() {
		writeFooter();
        m_outputStream.close();        
        m_currentSession = "";
        m_profileCount = 0;
	}

	void writeHeader()
    {
        m_outputStream << "{\"otherData\": {},\"traceEvents\":[";
        m_outputStream.flush();
    }

    void writeFooter()
    {
        m_outputStream << "]}";
        m_outputStream.flush();
    }

	void writeProfile(const ProfileResult& result)
    {
        if (m_profileCount++ > 0)
            m_outputStream << ",";

        std::string name = result.name;
        std::replace(name.begin(), name.end(), '"', '\'');

        m_outputStream << "{";
        m_outputStream << "\"cat\":\"function\",";
        m_outputStream << "\"dur\":" << (result.end - result.start) << ',';
        m_outputStream << "\"name\":\"" << name << "\",";
        m_outputStream << "\"ph\":\"X\",";
        m_outputStream << "\"pid\":0,";
        m_outputStream << "\"tid\":" << result.threadID << ",";
        m_outputStream << "\"ts\":" << result.start;
        m_outputStream << "}";        

        m_outputStream.flush();
    }

	static ProfileWriter& get()
    {
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

    static void beginSession(const std::string& name, const std::string& filepath = "results.json")
    {
        get().beginSessionImpl(name, filepath);
    }

    static void endSession()
    {
        get().endSessionImpl();
    }
};



class ProfileTimer
{
private:
	const char* m_name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;
	bool m_stopped;

public:
    explicit ProfileTimer(const char* name)
        : m_name(name), m_stopped(false)
    {
        m_startTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~ProfileTimer()
    {
        if (!m_stopped)
            Stop();
    }

    void Stop()
    {
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
    static void beginSession(const std::string&, const std::string& = "filename") { }
    static void endSession() { }
};
#endif
#endif // PROFILER_H
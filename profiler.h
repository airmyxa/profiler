#ifndef PROFILER_H
#define PROFILER_H

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>

// this implementation is unsafe to work with threads.

struct ProfileResult
{
    std::string Name;
    long long Start, End;
    uint32_t ThreadID;
};


class ProfileWriter
{
	friend class ProfileTimer;
private:
    std::string m_CurrentSession;
    std::ofstream m_OutputStream;
    int m_ProfileCount;

// private constructors: 
private:
    ProfileWriter() : m_ProfileCount(0) { }

// private functions: 
private:
	void BeginSessionImpl(const std::string& name, const std::string& filepath = "results.json") {
		m_OutputStream.open(filepath);
		if (!m_OutputStream.is_open())
			std::cerr << "couldn't open or create the file for profiler\n";
        WriteHeader();
        m_CurrentSession = name;
	}

	void EndSessionImpl() {
		WriteFooter();
        m_OutputStream.close();        
        m_CurrentSession = "";
        m_ProfileCount = 0;
	}

	void WriteHeader()
    {
        m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
        m_OutputStream.flush();
    }

    void WriteFooter()
    {
        m_OutputStream << "]}";
        m_OutputStream.flush();
    }

	void WriteProfile(const ProfileResult& result)
    {
        if (m_ProfileCount++ > 0)
            m_OutputStream << ",";

        std::string name = result.Name;
        std::replace(name.begin(), name.end(), '"', '\'');

        m_OutputStream << "{";
        m_OutputStream << "\"cat\":\"function\",";
        m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
        m_OutputStream << "\"name\":\"" << name << "\",";
        m_OutputStream << "\"ph\":\"X\",";
        m_OutputStream << "\"pid\":0,";
        m_OutputStream << "\"tid\":" << result.ThreadID << ",";
        m_OutputStream << "\"ts\":" << result.Start;
        m_OutputStream << "}";        

        m_OutputStream.flush();
    }

	static ProfileWriter& Get()
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
		if (m_OutputStream.is_open())
			m_OutputStream.close();
	}

public:

    static void BeginSession(const std::string& name, const std::string& filepath = "results.json")
    {
        Get().BeginSessionImpl(name, filepath);
    }

    static void EndSession()
    {
        Get().EndSessionImpl();
    }
};



class ProfileTimer
{
private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
	bool m_Stopped;

public:
    ProfileTimer(const char* name)
        : m_Name(name), m_Stopped(false)
    {
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~ProfileTimer()
    {
        if (!m_Stopped)
            Stop();
    }

    void Stop()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();

        long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
        long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

        uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
        ProfileWriter::Get().WriteProfile({ m_Name, start, end, threadID });

        m_Stopped = true;
    }
};

#define PROFILING 1
#if PROFILING
#define PROFILE_SCOPE(name) ProfileTimer timer##__LINE__(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__PRETTY_FUNCTION__) // in Windows change __PRETTY_FUNCTION__ -> __FUNCSIG__
#else
#define PROFILE_SCOPE(name)
#endif


#endif // PROFILER_H
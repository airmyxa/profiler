#include <chrono>

class ProfilerTimer {
public:	
	ProfilerTimer(const char* name) : m_name(name), m_stoped(false) {		
		m_start = std::chrono::high_resolution_clock::now();	
	}

	~ProfilerTimer() {
		if (!m_stoped)
			stop();
	}

	void stop() {
		auto endTimePoint = std::chrono::high_resolution_clock::now();
		auto endTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();
		auto startTime = std::chrono::time_point_cast<std::chrono::microseconds>(m_start).time_since_epoch().count();
		auto duration = endTime - startTime;
		//double ms = duration * 0.001;	
		m_stoped = true;
		// here json should be written
	}

private:
	const char* m_name;
	bool m_stoped;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;	
};
#include <chrono>
#include <fstream>
#include <exception>

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
		m_stoped = true;
		// here json should be written
	}

private:
	const char* m_name;
	bool m_stoped;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;	
};


struct ProfilerData {
	const char* function_name;
	uint start_timepoint;
	uint end_timepoint;
};


class ProfilerWriter {
public:

	ProfilerWriter(const ProfilerWriter& other) = delete;
	ProfilerWriter(ProfilerWriter&& other) = delete;
	ProfilerWriter& operator=(const ProfilerWriter& other) = delete;
	ProfilerWriter&& operator=(ProfilerWriter&& other) = delete;
	
	~ProfilerWriter() {
		if (fout.is_open())		
			fout.close();
	}

	static void begin_session(const char* name) {
		get().write_header(name);

	}

	static void end_session() {
		get().write_footer();
		get().fout.close();		
	}

	static ProfilerWriter& get() {
		static ProfilerWriter instance;
		return instance;
	}

	static void write(const ProfilerData& pd) {
		get().writeImpl(pd);
	}

private:
	void writeImpl(const ProfilerData& pd) {

	}

	void write_header(const char* header) {

	}

	void write_footer() {

	}

	ProfilerWriter(const char* filename = "profile.json") { 
		fout.open(filename);
		if (!fout.is_open())
			throw std::invalid_argument("could not open or create the file");
	}

private:
	std::ofstream fout;
};
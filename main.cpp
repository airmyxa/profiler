#include <iostream>
#include <thread>
#include "profiler.h"

int func1() {
    PROFILE_FUNCTION();
    int i = 0;
    for(; i < 3; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return i;
}

int func2() {
    PROFILE_FUNCTION();
    int i = 0;
    for(; i < 2; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return i;
}

void func3() {
    PROFILE_FUNCTION();
    auto x = func1();    
    auto y = func2();
}


int main()
{
    ProfileWriter::BeginSession("start");
    func3();
    ProfileWriter::EndSession();

    return EXIT_SUCCESS;
}
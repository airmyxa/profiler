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
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto x = func1();    
    auto y = func2();
}


void func4() {
    PROFILE_FUNCTION();
    std::this_thread::sleep_for(std::chrono::seconds(5));
}


int main()
{
    ProfileWriter::beginSession("start");
    std::thread thr(func4);        
    func3();    
    thr.join();
    ProfileWriter::endSession();

    return EXIT_SUCCESS;
}
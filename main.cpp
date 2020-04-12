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


int main()
{
    ProfileWriter::BeginSession("start");
    auto res1 = func1();
    auto res2 = func2();
    ProfileWriter::EndSession();

    std::cout << res1 << res2;

    return EXIT_SUCCESS;
}
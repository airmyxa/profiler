#include <iostream>
#include "profiler.h"
#include <thread>


void func1() {
    ProfilerTimer("func1");
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

void func2() {
    ProfilerTimer("func2");
    std::this_thread::sleep_for(std::chrono::seconds(3));
}


int main()
{

    
    func1();
    func2();

    return EXIT_SUCCESS;
}
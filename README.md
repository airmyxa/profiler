# Profiler
you need only the header profiler.h. in main.cpp you can see simple example of using it.

to use this profiler in your programm place define PROFILE_FUNCTION() in each
function you want to measure. if you want to turn the profiler off change
the define in profiler.h to zero: #define PROFILING 0.

to use this profiler you should open your chrome browser and go to chrome://tracing page.
than just move your output json file in this page and you have the result.
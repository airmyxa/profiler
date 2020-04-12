# Profiler
to use this profiler you should open your chrome browser and open chrome://tracing page.
than just move your output json file in this page and you have the result.

to use this prifiler in your programm place define PROFILE_FUNCTION() in each
function you want to measure. if you want to turn the profiler off change
the define in profiler.h to zero: #define PROFILING 0.

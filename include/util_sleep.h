#ifndef __UTIL_SLEEP_H
#define __UTIL_SLEEP_H

/* OS portable sleep function:
   If _WIN32 is defined, we are on a Windows system else Unix-based system.
   Static tells the compiler that the function has internal linkage, which means it won't be visible to other translation units. 
*/
#if defined(_WIN32)
    #include <windows.h>
    static void portableSleep(unsigned int seconds) {
        Sleep(seconds * 1000); // The argument to Sleep is in milliseconds
    }
#else
    #include <unistd.h>
    static void portableSleep(unsigned int seconds) {
        sleep(seconds); // The argument to sleep is in seconds
    }
#endif

#endif // __UTIL_SLEEP_H

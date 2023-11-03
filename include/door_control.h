#ifndef DOOR_CONTROL_H
#define DOOR_CONTROL_H

// If _WIN32 is defined, we are on a Windows system else Unix-based system.
#if defined(_WIN32)
    #include <windows.h>
    void portableSleep(unsigned int seconds) {
        Sleep(seconds * 1000); // The argument to Sleep is in milliseconds
    }
#else
    #include <unistd.h>
    void portableSleep(unsigned int seconds) {
        sleep(seconds); // The argument to sleep is in seconds
    }
#endif

enum doorLock {
    DOOR_LOCKED = 0,
    DOOR_UNLOCKED = 1
};

void remoteOpenDoor(void);
void lockUnlockMechanism(int lock);

#endif

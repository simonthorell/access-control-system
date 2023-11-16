#ifndef DOOR_CONTROL_H
#define DOOR_CONTROL_H

enum doorLock {
    DOOR_LOCKED = 0,
    DOOR_UNLOCKED = 1
};

// Global int for whole system to check if door is currently locked/unlocked
extern int doorStatus;

void remoteOpenDoor(void);
void lockUnlockMechanism(int lock);

#endif

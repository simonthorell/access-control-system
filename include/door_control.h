#ifndef DOOR_CONTROL_H
#define DOOR_CONTROL_H

enum lockState {
    DOOR_LOCKED = 0,
    DOOR_UNLOCKED = 1
};

void remoteOpenDoor(void);
void lockUnlockMechanism(int lock);

#endif

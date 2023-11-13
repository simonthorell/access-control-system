# System Architecture Overview
This document outlines the core architecture and functionalities of the RFID-based access control system, structured into distinct components.  
The system will use a microcontroller (Arduino or ESP32) to read and write to RFID cards via serial port.   
The microcontroller will also contain to LED's (green and red) to emulate unlocked/locked door depending if access is granted.

## Admin Menu
The Admin Menu is the central hub for system administrators to manage access control and system settings. Implemented in `admin_menu.c`.   
Admin menu provides the following options:

- **Remote Open Door**: Allows the administrator to remotely unlock the door. This function can be found in `door_control.c`.
- **List All Cards**: Displays a list of all RFID cards that are registered in the system, handled by `card_management.c`.
- **Add/Remove Access**: Enables the administrator to grant or revoke access privileges for individual RFID cards. This functionality is also part of `card_management.c`.
- **Exit**: Safely terminates the admin session and closes the application.
- **FAKE TEST SCAN CARD**: A testing function that simulates the scanning of an RFID card. It's used for testing purposes in `card_reader.c`.

## Door Control
Handles the mechanics of locking and unlocking the door, interfacing directly with the Arduino-controlled locking mechanism.

- **Lock/Unlock Mechanism**: Engages or disengages the door lock via commands sent to the Arduino in `door_control.c`.

## Card Management
Manages the database of RFID cards that are authorized to access the facility. 

- **Card Database Operations**: Adding, removing, and querying card details within the system's persistent storage, managed by `card_management.c`.

## Data Persistence
Ensures all configuration settings and authorized card lists are maintained between system reboots.

- **Storage Operations**: Saves and retrieves data from a file or database system, ensuring the integrity and availability of card and system data in `data_storage.c`.

## Card Reader Interface
Responsible for the direct interaction with the RFID hardware, providing the means to read RFID cards and authenticate them against the system.

- **RFID Reading**: Actively listens for RFID cards presented to the reader and reads their data in `card_reader.c`.
- **Card Authentication**: Validates the scanned RFID cards against the list of authorized cards maintained by the system.
- **Test Scanning**: Facilitates testing and debugging of the RFID reading process through simulated card scans.

## Run in docker
- **Build**: docker build -t access_control_system .
- **RUN**: docker run -it  --name access-control-app access_control_system (-it allows you to interact with a Docker container via the command line)

- **ACCESS TERMINAL FROM RUNNING CONTAINER**: docker attach access-control-app

- **START**: docker start -i access-control-app (-i activates STDIN for interactive mode)
- **STOP**: docker stop access-control-app    (NOTE! run -it flag to add command line access for container app.)

- **SHARE IMAGE BY FILE**:
● docker save -o <path for generated tarball> <image name> // Saving image to share
● docker load -i <path to tarball> // Loding shared image

## Autorun docker container on remote Linux Server
- **PUSH IMAGE TO DOCKER HUB** (Use your Docker Hub username. Tag could be f.e. version_1.0)
docker tag access_control_system:latest USERNAME/access-control-app:TAG
docker push USERNAME/access-control-app:TAG

- **PULL IMAGE FROM DOCKER HUB**
docker pull USERNAME/access-control-app:TAG

- **RUN IMAGE ON SERVER** (Use your Dockerhub Username. Tag could be f.e. version_1.0. Also change the USB device to your RFID reader - below usually work in Linux)
docker run -it --device=/dev/ttyUSB0:/dev/ttyUSB0 --name access-control-app USERNAME/access-control-app:TAG
docker update --restart always access-control-app

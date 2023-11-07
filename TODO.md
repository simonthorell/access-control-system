# TODO

## Card Management
- Change REALLOC STRUCTURE! Pass variable capacity from data_storage.c - Realloc 2x capacity once capacity is < 
- Datatype for RFID cards?
- Add "cold storage" for deleted cards ?

## UI external libraries
- Ok to implement external libraries for user interface. 

## Atomic Boolean
- Change keepRunning to atomic bool in order to have threads wait for eachother. 

## Change Malloc/Realloc with typecast (best practise)
- Typecast (best practise) = (PLAYER *)malloc(sizeof(PLAYER));

## Data Encrypt/Decrypt
- Ok to use external libraries such as OpenSSL!

## Input validation
- Do not accept strings when using GetInputInt.

## Fileformat
- Change from .csv to .json? 

## conio.h
- UI interface





# TODO

## Card Management
- Realloc add new card. 
- Sort array of cards. 
    1) Sort and realloc when add/remove card. 
    2) Sort (and remove NULL lines) when saving cards to file. 
- Implement merge sort
- Change for-loop to list cards faster when sorted. 

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





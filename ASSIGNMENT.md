# TODO
## Card Management
- Add "cold storage" for deleted cards ?

## Atomic Boolean
- Change keepRunning to atomic bool in order to have threads wait for eachother. 

## Change Malloc/Realloc with typecast (best practise)
- Typecast (best practise) = (PLAYER *)malloc(sizeof(PLAYER));

## Struct name AccessCard
- Change to start with capital letter for good convention!


# Inlämning individuell
Detta är administrationsdelen (körs på PC) för ett inpasseringssystem där besökaren scannar sitt kort.  

## Meny (Consol)
1. Remote open door  
2. List all cards in system  
3. Add/remove access  
4. Exit  
9. FAKE TEST SCAN CARD  

## 1. Remote open door
När man väljer denna ska systemet öppna dörren, “visas” genom GRÖN lampa enl nedan.  
Dörren ska vara öppen (grön lampa) i 3 sekunder.  
  
● CURRENTLY LAMP IS: Off  
● CURRENTLY LAMP IS: Green  
● CURRENTLY LAMP IS: Red  

## 2. List all cards in system
Lista ALLA kortnummer och skriv om dom har access eller INTE  
(1212 har access, 1213 har nekats samt DATUM när dom lades till i systemet)  
  
Här ska kort eventuellt skapas ifall det inte redan finns:  
● All cards in system:  
● 1212 Access added to system: 2019-10-19  
● 1213 No access to system: 2019-10-19  
● Pree any key to continue  

## 3. Add/remove access
Här ska kort eventuellt skapas ifall det inte redan finns:  
● Enter cardnumber:  
● Enter 1 for access, 2 for no access.  
● This card has access:   

## 4. Exit
Avsluta program.  

## 9. FAKE TEST SCAN CARD
Det finns alltså en lampa på kortenheten. Den kommer lysa:  
RÖTT: när man scannat ett kort som ej har access  
GRÖNT: när man scannat ett kort som har access  
  
Exempel:  
● Please scan card to enter or X to go back to admin menu.  
● CURRENTLY LAMP IS: Off  
● *** USER INPUT ***  
● CURRENTLY LAMP IS: Green  
● CURRENTLY LAMP IS: Red  
  
Men vi fejkar med att skriva ut “CURRENTLY LAMP IS: Green” och Red  

## Att tänka på:
● buffer overflow (inmatning)  
● stack overflow (flöde)  
● dela upp i funktioner  
● inga globala variabler  
  
● Inga globala variabler tips: samla "state" i STRUCT – Stefan visar på fredag 20230311  
  
● inte krascha vid DÅLIG inmatning  
  
● felhantering, snäll mot användaren  
  
● systemet skall klara “OBEGRÄNSAT” antal kort (dynamisk minnesallokering – mallloc/realloc)  
  
- lämna in genom privat repository på Github.  
- Skicka invite som Collaborator till stefan.holmberg@systementor.se  
- Ange URL till din github här i Studentportalen  
  
*** FÖR VG ***  
● Kort ska kunna SPARAS i FIL! Dvs efter omstart ska alla kort läsas upp och finnas kvar  
● Bra kod, best practices etc  


# FlightsCollector
CLI App for finding flight deals.

##
To compile

```shell
g++ app.cpp ./commands/viewDeals.cpp ./commands/removeDeal.cpp ./commands/findDeals.cpp ./commands/email.cpp -o fl  -lsqlite3 -ljsoncpp
```


### Usage
```
  ______ _ _       _     _      _____      _ _           _             
 |  ____| (_)     | |   | |    / ____|    | | |         | |            
 | |__  | |_  __ _| |__ | |_  | |     ___ | | | ___  ___| |_ ___  _ __ 
 |  __| | | |/ _` | '_ \| __| | |    / _ \| | |/ _ \/ __| __/ _ \| '__|
 | |    | | | (_| | | | | |_  | |___| (_) | | |  __/ (__| || (_) | |   
 |_|    |_|_|\__, |_| |_|\__|  \_____\___/|_|_|\___|\___|\__\___/|_|   
              __/ |                                                    
             |___/   

  --------------------------------------------------------------------
  |                                                                   |
  |                    Find the best flight deals                     |
  |                                                                   |
  --------------------------------------------------------------------

  Options:
   -f -dep <departure> -dest <destination> -ld <YYYY-MM-DD> -rd <YYYY-MM-DD>    Find deals
   -v                                                                           View saved deals
   -s                                                                           Search saved deals
   -r -fn <flight number> || -all                                               Remove saved flight deals
   -h                                                                           Help page

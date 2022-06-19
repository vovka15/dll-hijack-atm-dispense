# dll-hijack-atm-dispense

## Description:
The project was carried out to study the possibility of issuing funds using a vulnerability phantom dll injection. 

## How it works:
![image](https://user-images.githubusercontent.com/25028684/174481463-fca39f04-6c1d-477d-91e2-540a5b4e65a5.png) \
Application loads malicious library (if it is vulnerable to dll phantom injection).
Loaded library starting to perform server polling (http) which intruder controls. 
If permission from the server recieved then library dispenses 100 rub note.
In real environment (real ATM) intruder will not be able to poll the server - in this situation he can tie dispenses at certain points in time - hours, days of the week

## Delivery methods:
- remote control\update systems
- service engineer (or intruder who acts as service engineer)

## Used sources:
- XFS documentation. Part 1. API - [link](https://www.cencenelec.eu/media/CEN-CENELEC/AreasOfWork/CEN%20sectors/Digital%20Society/CWA%20Download%20Area/XFS/16926-1.pdf)
- XFS documentation. Part 5. CDM - [link](https://www.cencenelec.eu/media/CEN-CENELEC/AreasOfWork/CEN%20sectors/Digital%20Society/CWA%20Download%20Area/XFS/16926-5.pdf)

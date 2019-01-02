# LANrelay
Ethernet relay control with arduino nano and module ENC28J60
ALL CREDITS TO http://academicfox.com/aurduino-ethernet-module-enc28j60-or-web-control-relay-arduino-webserver/

I only added an analog voltage reading to measure the status of a battery (tension reported in the homepage) 
and a digital input check also reported in the homepage as a boolean variable

Remember to modify IP addresses and value of resistors for voltage divider input

To save relays remember to use normally closed contact: ON status on webpage signify no current in relay's coil.

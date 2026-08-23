# General Communication

## High Level Overview
* ESP's communicate over MQTT to Backend, Vice Versa (technically, ESP32 -> MQTT Broker -> Backend)
* Backend communicates to Frontend via websockets
* Frontend communicates to Backend via API calls.

## Entity Specific Notes
### Firmware/ESP32
* ESP will Register over MQTT
* MQTT client ID's will be MAC Addresses.
* MQTT should allow the "Fleet" or specific ESP32's to receive Commands.
* ESP's should send status updates, errors, metrics over MQTT.
* Status Updates should go out on a job specific Queue EX: che/*MACADDR*/job-*jobId*/status (status is optional since there isnt another purpose for a job specific queue)
* Status Update queue could be che/status/client-*clientmac*

### Backend
* Backend should record all communications to DB.
* Backend should subscribe to Status Updates, Command responses(? # 4 & 6), Errors.
* Backend should publish Commands
* Commands queue could be che/commands/all, che/commands/job-*jobid*, and che/commands/client-*clientmac*

### Frontend
* Listen to live updates via Backend relay of MQTT
* All else is API to backend

## Low Level Details
### Command List (specifically what the backend can tell the ESP32 to do, this is not the config command):
* Change "Job"
* Trigger firmware or config update
* Register ESP32
* Reboot
* Pause
* Resume
* Identify (Signal which ESP32 you're looking at in the frontend)
* Cancel Identify
* Go Idle (? isnt this basically pause?)

### Status LED (Color blind accessible)
* Execution Error - RED
* No Internet - Flashing RED
* Unplanned USB Disconnect - Double Flashing red, pause, repeat
* Firmware update error (not sure if this would be a thing since it might be bricked) - Triple flashing red, pause, repeat
* Waiting For Job - Flashing Amber
* Waiting for config - Double Pulse Amber (2 rapid amber blinks, pause, repeat)
* Executing - Green/Bright White
* Identify - Rapid Flashing Blue
* DO NOT TOUCH (Firmware OTA Update) - Alternate Bright Purple & White (Magenta?)

### Status Update Format (should include the metrics. This is the same thing as step execution)
* Should include time spent waiting for step to complete in MS
* JobID if applicable
* Step Indicator/ Details
* Step required Human Interaction based on step config
* ClientID

### Firmware "boot" steps
1) Connect to WiFi
2) Check firmware updates
3) Wait for job
4) When job assigned, check for config via API (? # 3 or have config sent over MQTT with the job change command)
5) Check for command messages
   * Report Command ACK (? # 4)
6) Attempt to Execute Step N
7) check for completion of Step N or timeout
8) If completed, report completion of step N, if timeout, report that and hold.
9) Report next step started, or report completion.

### ESP32 Config details:
* string/unsigned int- Step Type - Required -- can be an ID to be lightweight and simpler (& faster?) to compare (Hold Key, Release Key, Wait For Reboot, etc)
* char/unsigned int? - key - (optional for release key, but not for Hold Key) - This would be the key to press is identified.
* unsigned int - timeout - optional, but a default should be set in code(? # 5). in MS.
* unsigned int - Step Number
* unsigned int - Version Number
* boolean - required human interaction

### Error detection:
* When sending a key command, wait for the usb to be connected. Use the timeout for this. If the usb isnt connected within the timeout, the key command isnt sent, and this should trigger an error.


### Error Format (? # 2)
* Step Number - unsigned int
* Job Number - unsigned int
* Version Number - unsigned int
* ClientID - string


## Questions:

1) ~~Should overall job status be a queue? Leaning towards no. Who needs this info and why?~~ No
2) Are there types of errors? My assumption is most errors will be sourced from 
3) Should config and/or firmware be sent over MQTT? Worry about size.
4) Do i need to ACK a command?
5) Set default config step timeout default in firmware/Backend? Should no timeout be allowed?
6) What does the backend do with ACK of commands? Record it? Is there a flag on the command entity that is checked when its ACKed?
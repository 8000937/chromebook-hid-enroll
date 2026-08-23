# General Communication

## High Level Overview
* ESP's communicate over MQTT to Backend, Vice Versa (technically, ESP32 -> MQTT Broker -> Backend)
* Backend communicates to Frontend via websockets
* Frontend communicates to Backend via API calls.
* Would be ideal to use something like mDNS or other technology that doesn't require us to hard code an IP, or domain to point toward the API, and the MQTT broker.

## Entity Specific Notes
### Firmware/ESP32
* ESP will Register over MQTT
* MQTT client ID's will be MAC Addresses.
* MQTT should allow the "Fleet" or specific ESP32's to receive Commands.
* ESP's should send status updates, errors, metrics over MQTT.
* Status Updates should go out on a job specific Queue EX: che/*MACADDR*/job-*jobId*/status (status is optional since there isnt another purpose for a job specific queue)
* Status Update queue could be che/status/client-*clientmac*
* ESP will likely need to unregister and/or have a heartbeat setup. Need to revisit this since MQTT might handle both.

### Backend
* Backend should record all communications to DB.
* Backend should subscribe to Status Updates, Command ACK(? # 4 & ? #  6), Errors.
* Backend should publish Commands
* Commands queue could be che/commands/all, che/commands/job-*jobid*, and che/commands/client-*clientmac*
* Store configs with version numbers. Configs should map to a job.

### Frontend
* Listen to live updates via Backend relay of MQTT
* All else is API to backend

## Low Level Details
### Command List (specifically what the backend can tell the ESP32 to do, this is not the config command):
* Set Job (This should force a config update request)
* Trigger firmware
* Trigger config update (optionally request specific version; default to latest)
* Register ESP32
* Reboot
* Pause
* Resume
* Identify (Signal which ESP32 you're looking at in the frontend)
* Cancel Identify

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
* Paused - Flashing Green/Bright White

### Status IDs
#### Hardware
* Unplanned USB Disconnect - ID: -20

#### Execution
* Execution Error - ID: -10
* Waiting For Job - ID# 10
* Waiting for config - ID# 11
* Executing - ID# 12
* Step Completed (Not visible on LED or UI) - ID# 13
* Paused - ID# 14
* Resumed - ID# I5

#### Firmware
* Firmware update error (not sure if this would be a thing since it might be bricked) - ID# -30
* Firmware OTA Update - ID# 30

#### Information / Utilities
* Identify - ID# 0

#### Status ID Design
* Negative Statuses are Errors
* positive are informational or success
* +/- 10 through +/- 19 are execution related (timeout issue, step completed, waiting for job or config)
* +/- 20 through +/- 29 are hardware related
* +/- 30 through +/- 39 are firmware related
* 0-9 are informational/utilities

### Status Update Format (should include the metrics. This is the same thing as step execution)
* Should include time spent waiting for status change in MS
* Current JobID if applicable
* Current StepID
* Config Version
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
* char/unsigned int? - key - optional depending on the step type. This would be the key to press is identified.
* unsigned int - timeout - ~~optional, but a default should be set in code(? # 5).~~ Required. in MS.
* unsigned int - Step Number
* unsigned int - Version Number
* boolean - required human interaction

### Error detection:
* When sending a key command, wait for the usb to be connected. Use the timeout for this. If the usb isnt connected within the timeout, the key command isnt sent, and this should trigger an error.


### Status Update Format (? # 2)
* Step Number - unsigned int
* Job Number - unsigned int
* Version Number - unsigned int
* ClientID - string
* is error - boolean
* status - signed int

## Questions:

1) ~~Should overall job status be a queue? Leaning towards no. Who needs this info and why?~~ No
2) ~~Are there types of errors? My assumption is most errors will be sourced from timeouts.~~ I think this heading was a bit misleading. This should be a status update message with an error boolean, and status id.
3) Should config and/or firmware be sent over MQTT? Worry about size.
4) Do i need to ACK a command?
5) ~~Set default config step timeout default in firmware/Backend? Should no timeout be allowed?~~ Yes, we should require a timeout. This helps with error detection. 
6) What does the backend do with ACK of commands? Record it? Is there a flag on the command entity that is checked when its ACKed?
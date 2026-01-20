# Remote Master
This project is designed to read and program cheap 315/433MHz RF remotes.
Requires ESP32 with custom firmware to handle RF receive/transmit functionality.

All parsing is processed on PC.

## Requirements
* .NET 8.0
* [IRIS](https://github.com/H1M4W4R1/IRIS)
* [IRIS.Serial](https://github.com/H1M4W4R1/IRIS.Serial)
* ESP32 (WeMos D1 R32) with custom firmware (included in ESP32 Code folder)
    * GPIO22: RF Receiver
    * GPIO26: RF Transmitter


## Implementing custom protocol
You can either hook into `AttemptToDetectRemoteProtocol` when your remote is 
constantly sending data or read remote data using oscilloscope hooked to
RF received data pin.

Then you can use this data to create custom class that implements `IRadioProtocol`
interface and configure all abstract members according to your protocol. You can see
included protocols for examples.

All protocols will be recognized automatically when instance of device is created.

## Note for working with raw data transmission
Transmission sometimes may not work properly due to high interference. It is recommended to 
send a large amount of packets with cca. `10ms` delay between packets.

## How to use?
### Connecting
If you've built an ESP32 dongle and programmed it with custom firmware you can proceed to connect.
Start up the software and enter ESP32 serial port address (e.g. COM7).

If everything's correct you will be greeted with a few options.

### Listing protocols
You can list protocols by typing "list" command. This is useful when memory fails. 
You can close the list by pressing any key on your keyboard.

### Reading remotes
Option 'read' will log all known protocol detections with their values. It's pretty useful
for decoding remotes with unknown codes.

You can quit to mode selection by pressing any key on keyboard.

### Writing remotes
Option 'program' will ask for protocol and value to write to your remote. If both are provided
and correct it will start sending packets each 10ms.

You can quit to mode selection by pressing any key on keyboard.

### Quitting the app
Simply type 'quit' in mode selection.
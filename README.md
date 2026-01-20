# Remote Master
This project is designed to read and program cheap 315/433MHz RF remotes.
Requires ESP32 with custom firmware to handle RF receive/transmit functionality.

All parsing is processed on PC.

## Requirements
* .NET 8.0
* [IRIS](https://github.com/H1M4W4R1/IRIS)
* [IRIS.Serial](https://github.com/H1M4W4R1/IRIS.Serial)
* ESP32 with custom firmware (included in ESP32 Code folder)
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

## W.I.P.
Programming remotes is not yet available (in both firmware and PC code).
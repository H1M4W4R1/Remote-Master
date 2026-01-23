using System.Globalization;
using System.IO.Ports;
using System.Reflection;
using System.Runtime.InteropServices;
using IRIS.Operations;
using IRIS.Serial.Addressing;
using IRIS.Utility;
using RadioRemote.Devices;
using RadioRemote.Protocols;
using RadioRemote.Protocols.Data;

namespace RadioRemote;

class Program
{
    private const string DEFAULT_PORT = "COM7";
    private static string _serialPort = DEFAULT_PORT;
    private static RadioSerialDevice _device;

    static void Main(string[] args)
    {
        RunApp();
        while (true)
        {
        }
    }

    static async ValueTask ConnectToDevice()
    {
        connect_start:

        Console.Clear();
        Console.SetCursorPosition(0, 0);

        Console.ForegroundColor = ConsoleColor.White;
        Console.WriteLine($"Please provide serial port [{DEFAULT_PORT}]");
        Console.Write("> ");
        _serialPort = Console.ReadLine() ?? DEFAULT_PORT;
        if (string.IsNullOrEmpty(_serialPort)) _serialPort = DEFAULT_PORT;

        // Check if port exists
        if (!SerialPort.GetPortNames().Contains(_serialPort))
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine($"Port {_serialPort} does not exist. Is your device connected?");
            await Task.Delay(1000);
            goto connect_start;
        }
        
        // Handle connection
        _device = new RadioSerialDevice(new SerialPortDeviceAddress(_serialPort));

        if (DeviceOperation.IsFailure(await _device.Connect(new RequestTimeout(1000))))
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Failed to connect to device");
            await Task.Delay(1000);
            goto connect_start;
        }


        Console.ForegroundColor = ConsoleColor.Green;
        Console.WriteLine("Connected to device");
        await Task.Delay(500);
    }

    public static async ValueTask ChangeMode()
    {
        mode_start:

        Console.Clear();
        Console.SetCursorPosition(0, 0);


        Console.ForegroundColor = ConsoleColor.White;
        Console.WriteLine("Please provide mode (read, list or program)");
        Console.Write("> ");
        string? option = Console.ReadLine();

        switch (option?.ToLower() ?? string.Empty)
        {
            case "read":
            case "log":
            case "scan": await RunRead(); break;
            case "write":
            case "program": await RunProgram(); break;
            case "programread": case "program_read": 
                _device.OnRadioSignalReceived += OnRadioSignal;
                await RunProgram(); 
                _device.OnRadioSignalReceived -= OnRadioSignal;
                break;
            case "quit":
            case "exit":
            case "qqq":
            case "q": Environment.Exit(0); break;
            case "list":
            {
                Console.Clear();
                Console.SetCursorPosition(0, 0);

                Console.ForegroundColor = ConsoleColor.White;
                Console.WriteLine("Available protocols:");
                
                Console.ForegroundColor = ConsoleColor.DarkGray;
                Console.WriteLine();
                Console.WriteLine("Press any key to quit");
                
                for (int n = 0; n < _device.KnownRadioProtocols.Count; n++)
                {
                    Console.ForegroundColor = ConsoleColor.White;
                    Console.Write("* ");
                    Console.ForegroundColor = ConsoleColor.DarkCyan;
                    Console.WriteLine($"{_device.KnownRadioProtocols[n].Name}");
                }

                Console.ReadKey();

                break;
            }
            default:
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("Invalid option selected. Use: read, list or program");
                goto mode_start;
        }

        await Task.Delay(1000);
        goto mode_start;
    }

    static async ValueTask RunApp()
    {
        Console.WriteLine("Welcome to Remote Master by H1M4W4R1");
        Console.WriteLine();
        await Task.Delay(500);

        await ConnectToDevice();
        await ChangeMode();
    }

    static ValueTask RunRead()
    {
        _device.OnRadioSignalReceived += OnRadioSignal;
        Console.Clear();
        Console.SetCursorPosition(0, 0);

        Console.ForegroundColor = ConsoleColor.White;
        Console.WriteLine("Waiting for RF signals...");
        
        Console.ForegroundColor = ConsoleColor.DarkGray;
        Console.WriteLine();
        Console.WriteLine("Press any key to quit");
        
        Console.ReadKey();
        _device.OnRadioSignalReceived -= OnRadioSignal;
        return ValueTask.CompletedTask;
    }

    private static void OnRadioSignal(IRadioProtocol protocol, IProtocolData data)
    {
        Console.ForegroundColor = ConsoleColor.White;
        Console.Write("Received RF signal for ");
        Console.ForegroundColor = ConsoleColor.DarkCyan;
        Console.Write(protocol.Name);
        Console.ForegroundColor = ConsoleColor.White;
        Console.Write(" protocol with value: ");
        Console.ForegroundColor = ConsoleColor.DarkYellow;
        Console.Write($"{data.Value}");
        Console.ForegroundColor = ConsoleColor.White;
        Console.Write(" | ");
        Console.ForegroundColor = ConsoleColor.DarkYellow;
        Console.Write($"{data.Value:B}");
        Console.ForegroundColor = ConsoleColor.White;
        Console.Write(" | ");
        Console.ForegroundColor = ConsoleColor.DarkYellow;
        Console.Write($"{data.Value:X}");
        Console.ForegroundColor = ConsoleColor.White;
        Console.Write(" | [");
        Console.ForegroundColor = ConsoleColor.DarkMagenta;
        Console.Write($"{data.Bits}");
        Console.ForegroundColor = ConsoleColor.White;
        Console.Write("b]");
        Console.WriteLine();
    }

    static async ValueTask RunProgram()
    {
        ask_for_protocol:
        Console.Clear();
        Console.SetCursorPosition(0, 0);

        Console.ForegroundColor = ConsoleColor.White;
        Console.WriteLine("Please provide protocol name");
        Console.Write("> ");
        Console.ForegroundColor = ConsoleColor.DarkCyan;
        string? protocolName = Console.ReadLine();
        Console.ForegroundColor = ConsoleColor.White;
        
        // Find protocol
        IRadioProtocol? protocol = _device.KnownRadioProtocols.FirstOrDefault(protocol
            => protocol.Name.ToLower().Equals(protocolName?.ToLower()));
        if (protocol == null)
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.Write("Protocol '");
            Console.ForegroundColor = ConsoleColor.DarkCyan;
            Console.Write($"{protocolName}");
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine($"' is not supported");
            await Task.Delay(1000);
            goto ask_for_protocol;
        }
        
        ask_for_value:
        Console.Clear();
        Console.SetCursorPosition(0, 0);
        
        Console.ForegroundColor = ConsoleColor.White;
        Console.WriteLine("Please provide value to write");
        Console.Write("> ");
        Console.ForegroundColor = ConsoleColor.DarkYellow;
        string? protocolValueString = Console.ReadLine();
        Console.ForegroundColor = ConsoleColor.White;
        if (!ulong.TryParse(protocolValueString, out ulong transmitValue))
        {
            if (!ulong.TryParse(protocolValueString, NumberStyles.HexNumber, null, out transmitValue))
            {
                if (!ulong.TryParse(protocolValueString, NumberStyles.BinaryNumber, null, out transmitValue))
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.Write($"Value '");
                    Console.ForegroundColor = ConsoleColor.DarkYellow;
                    Console.Write($"{protocolValueString}");
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("' cannot be parsed.");
                    await Task.Delay(1000);
                    goto ask_for_value;
                }
            }
        }

        Type protocolType = protocol.GetType();
        MethodInfo? method = _device.GetType().GetMethod(nameof(RadioSerialDevice.Transmit));
        MethodInfo? builtMethod = method?.MakeGenericMethod(protocolType);
        
        Console.Clear();
        Console.SetCursorPosition(0, 0);

        Console.ForegroundColor = ConsoleColor.White;
        Console.Write($"Transmitting packet [");
        Console.ForegroundColor = ConsoleColor.DarkYellow;
        Console.Write($"{transmitValue}");
        Console.ForegroundColor = ConsoleColor.White;
        Console.Write($"] using protocol ");
        Console.ForegroundColor = ConsoleColor.DarkCyan;
        Console.WriteLine($"{protocol.Name}");
        Console.ForegroundColor = ConsoleColor.DarkGray;
        Console.WriteLine();
        Console.WriteLine("Press any key to quit");
        
        while (true)
        {
            builtMethod?.Invoke(_device, [transmitValue]);
            await Task.Delay(10);

            // Wait for key without blocking
            if (Console.KeyAvailable)
            {
                break;
            }
        }
    }
}
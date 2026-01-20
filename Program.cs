using IRIS.Operations;
using IRIS.Serial.Addressing;
using IRIS.Utility;
using RadioRemote.Devices;
using RadioRemote.Protocols;

namespace RadioRemote;

class Program
{
    private const string DEFAULT_PORT = "COM7";
    private static string _serialPort = DEFAULT_PORT;
    
    
    static void Main(string[] args)
    {
        Console.WriteLine("Please provide serial port: ");
        _serialPort = Console.ReadLine() ?? DEFAULT_PORT;
        if (string.IsNullOrEmpty(_serialPort)) _serialPort = DEFAULT_PORT;


        Run();

        Console.WriteLine("Press any key to exit");
        Console.ReadKey();
    }

    static async void Run()
    {
        // Create new device
        RadioSerialDevice device = new RadioSerialDevice(new SerialPortDeviceAddress(_serialPort));
        device.OnRadioSignalReceived += OnRadioSignal;
        
        if (DeviceOperation.IsFailure(await device.Connect(new RequestTimeout(1000))))
        {
            Console.WriteLine("Failed to connect to device");
            return;
        }

        Console.WriteLine("Connected to device");
        
        while (true)
        {
            await device.Transmit<EV1527Protocol>(7300528);
            await Task.Delay(10);
        }
    }

    private static void OnRadioSignal(IRadioProtocol protocol, ulong value)
    {
        Console.WriteLine($"Radio signal received for {protocol.Name} with value: {value}");
    }
}
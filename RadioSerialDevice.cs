using System.Diagnostics;
using System.Reflection;
using System.Text;
using IRIS.Serial.Addressing;
using IRIS.Serial.Communication.Settings;
using IRIS.Serial.Devices;
using RadioRemote.Protocols;

namespace RadioRemote
{
    public sealed class RadioSerialDevice : SerialDeviceBase
    {
        private const int LIST_MAX_SIZE = 256;
        
        private string _cachedString = "";
        private readonly List<ushort> _receivedValues = [];
        
        /// <summary>
        ///     List of all radio protocols registered in entire app (including custom libraries)
        /// </summary>
        public List<IRadioProtocol> KnownRadioProtocols { get; private init; }

        /// <summary>
        ///     Event raised whenever radio code was parsed successfully
        /// </summary>
        public event Delegates.OnRadioSignalReceivedHandler? OnRadioSignalReceived; 
        
        public RadioSerialDevice(SerialPortDeviceAddress deviceAddress) : base(deviceAddress,
            new SerialInterfaceSettings(2_000_000))
        {
            UseCustomDataEvent(OnDataReceived);

            // Get all protocols and init table
            KnownRadioProtocols = GetAllRadioProtocolTypes();
        }

        private async void OnDataReceived(int count)
        {
            try // Prevent fuck-up crashes
            {
                // Receive data
                byte[] data = await ReadBytes(count);
                _cachedString += Encoding.ASCII.GetString(data);

                while (_cachedString.Contains(':'))
                {
                    int fIndex = _cachedString.IndexOf(':');
                    string hexValue = _cachedString.Substring(0, fIndex);
                    _cachedString = _cachedString.Remove(0, fIndex + 1);

                    if (ushort.TryParse(hexValue, System.Globalization.NumberStyles.HexNumber, null,
                            out ushort value))
                    {
                        _receivedValues.Add(value);
                    }
                    else
                    {
                        Debug.WriteLine($"Invalid hex value: {hexValue}");
                    }
                }

                // Limit received values lenght
                while (_receivedValues.Count >= LIST_MAX_SIZE)
                {
                    _receivedValues.RemoveAt(0);

                    // Run protocol detection
                    AttemptToDetectRemoteProtocol();
                }
            }
            catch (Exception ex)
            {
                Debug.Write(ex);
            }
        }

        private void AttemptToDetectRemoteProtocol()
        {
            for (int index = 0; index < KnownRadioProtocols.Count; index++)
            {
                // Check if protocol is valid for current data
                IRadioProtocol protocol = KnownRadioProtocols[index];
                if (!protocol.TryParse(_receivedValues, out long remoteCode)) continue;
                
                // Handle parsing radio data
                OnRadioSignalReceived?.Invoke(protocol, remoteCode);                  
                return;
            }
        }

        private static List<IRadioProtocol> GetAllRadioProtocolTypes()
        {
            List<IRadioProtocol> result = [];
            Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();

            for (int i = 0; i < assemblies.Length; i++)
            {
                Assembly assembly = assemblies[i];
                Type?[] types;

                try
                {
                    types = assembly.GetTypes();
                }
                catch (ReflectionTypeLoadException ex)
                {
                    // Handle partially loadable assemblies
                    types = ex.Types;
                }

                for (int j = 0; j < types.Length; j++)
                {
                    Type? type = types[j];

                    if (type is not {IsClass: true, IsAbstract: false} ||
                        !typeof(IRadioProtocol).IsAssignableFrom(type))
                        continue;

                    try
                    {
                        object? obj = Activator.CreateInstance(type);
                        if (obj is not IRadioProtocol protocol) continue;
                        result.Add(protocol);
                    }
                    catch
                    {
                        // Prevent unknown constructors
                    }
                }
            }

            return result;
        }
    }
}
using RadioRemote.Protocols.Data;

namespace RadioRemote.Protocols
{
    public interface IValueRadioProtocol<TProtocolData> : IRadioProtocol<TProtocolData>, IValueRadioProtocol
        where TProtocolData : IProtocolData
    {
        List<ushort> IValueRadioProtocol.BuildPacket(ulong value) =>
            BuildPacket((TProtocolData) CreateValueProtocolData(value, -1));
    }

    public interface IValueRadioProtocol : IRadioProtocol
    {
        public IProtocolData CreateValueProtocolData(ulong value, short bits);
        public List<ushort> BuildPacket(ulong value);
    }

    public interface IRadioProtocol<TProtocolData> : IRadioProtocol
        where TProtocolData : IProtocolData
    {
        public TProtocolData TryParse(List<ushort> timings);
        public List<ushort> BuildPacket(in TProtocolData data);

        // Override to ensure proper interface implementation
        IProtocolData IRadioProtocol.TryParseRaw(List<ushort> timings) => TryParse(timings);
    }

    public interface IRadioProtocol
    {
        public string Name { get; }
        
        public IProtocolData TryParseRaw(List<ushort> timings);
    }
}
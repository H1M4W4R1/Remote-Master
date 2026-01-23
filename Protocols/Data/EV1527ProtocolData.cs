namespace RadioRemote.Protocols.Data
{
    public struct EV1527ProtocolData(bool isValid, ulong value, byte bits) : IProtocolData<EV1527ProtocolData>
    {
        public static EV1527ProtocolData Invalid { get; } = new(false, 0, 0);
        
        public bool IsValid { get; set; } = isValid;

        public ulong Value { get; set; } = value;

        public byte Bits { get; set; } = bits;
        
    }
}
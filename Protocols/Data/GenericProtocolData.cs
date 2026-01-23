namespace RadioRemote.Protocols.Data
{
    public struct GenericProtocolData(bool isValid, ulong value, byte bits) : IProtocolData<GenericProtocolData>
    {
        public static GenericProtocolData Invalid { get; } = new(false, 0, 0);

        public bool IsValid { get; set; } = isValid;

        public ulong Value { get; set; } = value;

        public byte Bits { get; set; } = bits;

    }
}
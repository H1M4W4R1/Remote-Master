namespace RadioRemote.Protocols.Data
{
    public interface IProtocolData<out TSelf> : IProtocolData
        where TSelf : IProtocolData<TSelf>
    {
        public static abstract TSelf Invalid { get; }
    }

    public interface IProtocolData
    {
        public bool IsValid { get; set; }

        public byte Bits { get; }
        public ulong Value { get; }
    }
}
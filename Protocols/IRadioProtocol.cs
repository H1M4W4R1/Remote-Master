namespace RadioRemote.Protocols
{
    public interface IRadioProtocol
    {
        public string Name { get; }
        
        public ushort Bits { get; }
        
        public bool TryParse(List<ushort> values, out ulong value);
        public List<ushort> BuildPacket(in ulong value);

    }

}
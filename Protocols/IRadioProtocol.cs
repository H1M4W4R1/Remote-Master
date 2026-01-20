namespace RadioRemote.Protocols
{
    public interface IRadioProtocol
    {
        public string Name { get; }
        
        public ushort Bits { get; }
        
        public bool TryParse(List<ushort> values, out long value);
        
    }

}
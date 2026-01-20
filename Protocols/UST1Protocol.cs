namespace RadioRemote.Protocols
{
    public sealed class UST1Protocol : IRadioProtocol
    {
        // -------- protocol timing (µs) --------
        public const ushort SYNC_MIN = 6500;
        public const ushort SYNC_MAX = 7500;

        public const ushort ONE_H_MIN = 1300;
        public const ushort ONE_H_MAX = 1900;
        public const ushort ONE_L_MIN = 600;
        public const ushort ONE_L_MAX = 1200;

        public const ushort ZERO_H_MIN = 400;
        public const ushort ZERO_H_MAX = 1000;
        public const ushort ZERO_L_MIN = 2550;
        public const ushort ZERO_L_MAX = 3150;

        public const ushort BITS = 18;
        public ushort Bits => BITS;
        public string Name => "Unknown Standard Type 1";

        private static bool VerifySync(ushort h, ushort l)
        {
            return (h is >= SYNC_MIN and <= SYNC_MAX && l is >= SYNC_MIN and <= SYNC_MAX);
        }

        public bool TryParse(List<ushort> values, out long value)
        {
            const int PACKET_LENGTH = 2 + BITS * 2; // (Sync + bits of data) x2
            
            value = -1;
            if (values.Count < PACKET_LENGTH) return false;  
            if (!VerifySync(values[0], values[1])) return false;

            value = 0;
            
            for (int n = 2; n < PACKET_LENGTH; n+= 2)
            {
                uint h = values[n];
                uint l = values[n + 1];

                // Shift value left
                value = value << 1;
                if (h > l) value |= 1; // Set bit if high is longer than low 
            }
            
            return true; 
        }
    }
}
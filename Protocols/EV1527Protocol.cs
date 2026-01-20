namespace RadioRemote.Protocols
{
    public sealed class EV1527Protocol : IRadioProtocol
    {
        // -------- protocol timing (µs) --------
        public const float SYNC_H_TO_L_RATIO = 11_600f / 375;
        public const float SYNC_H_TO_L_MIN = SYNC_H_TO_L_RATIO / 1.2f;
        public const float SYNC_H_TO_L_MAX = SYNC_H_TO_L_RATIO * 1.2f;

        public const float GENERIC_RATIO = 1150f / 375;
        public const float GENERIC_RATIO_MIN = GENERIC_RATIO / 1.2f;
        public const float GENERIC_RATIO_MAX = GENERIC_RATIO * 1.2f;

        public const ushort BITS = 24;
        
        public ushort Bits => BITS;
        public string Name => "EV1527";

        private static bool VerifyOne(ushort h, ushort l)
        {
            float ratio = (float) h / l;
            return ratio is > GENERIC_RATIO_MIN and < GENERIC_RATIO_MAX;
        }
        
        private static bool VerifyZero(ushort h, ushort l)
        {
            float ratio = (float) l / h;
            return ratio is > GENERIC_RATIO_MIN and < GENERIC_RATIO_MAX;
        }
        
        private static bool VerifySync(ushort h, ushort l)
        {
            float ratio = (float) h / l;
            return ratio is > SYNC_H_TO_L_MIN and < SYNC_H_TO_L_MAX ;
        }



        public bool TryParse(List<ushort> values, out long value)
        {
            const int PACKET_LENGTH = 2 + BITS * 2; // (Sync + bits of data) x2

            value = -1;
            if (values.Count < PACKET_LENGTH) return false;
            if (!VerifySync(values[0], values[1])) return false;

            value = 0;

            for (int n = 2; n < PACKET_LENGTH; n += 2)
            {
                ushort h = values[n];
                ushort l = values[n + 1];

                // Shift value left
                value = value << 1;
                if (VerifyOne(h, l)) value |= 1; // Parse ratio
            }

            return true;
        }
    }
}
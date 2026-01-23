using RadioRemote.Protocols.Data;

namespace RadioRemote.Protocols
{
    public sealed class ProprietaryBetaProtocol : IValueRadioProtocol<GenericProtocolData>
    {
        // -------- protocol timing (µs) --------
        public const ushort SYNC_DEFAULT = 7000;
        public const ushort SYNC_MIN = 6500;
        public const ushort SYNC_MAX = 7500;

        public const ushort ONE_H_DEFAULT = 1600;
        public const ushort ONE_H_MIN = 1300;
        public const ushort ONE_H_MAX = 1900;
        public const ushort ONE_L_DEFAULT = 900;
        public const ushort ONE_L_MIN = 600;
        public const ushort ONE_L_MAX = 1200;

        public const ushort ZERO_H_DEFAULT = 700;
        public const ushort ZERO_H_MIN = 400;
        public const ushort ZERO_H_MAX = 1000;
        public const ushort ZERO_L_DEFAULT = 2850;
        public const ushort ZERO_L_MIN = 2550;
        public const ushort ZERO_L_MAX = 3150;

        public string Name => "P-BETA";
        
        private static bool VerifyOne(ushort h, ushort l)
        {
            return h is >= ONE_H_MIN and <= ONE_H_MAX && l is >= ONE_L_MIN and <= ONE_L_MAX;
        }

        private static bool VerifyZero(ushort h, ushort l)
        {
            return h is >= ZERO_H_MIN and <= ZERO_H_MAX && l is >= ZERO_L_MIN and <= ZERO_L_MAX;
        }

        private static bool VerifySync(ushort sync, ushort data)
        {
            return sync is >= SYNC_MIN and <= SYNC_MAX;
        }



        public GenericProtocolData TryParse(List<ushort> timings)
        {
            ulong value = 0;
            if (!VerifySync(timings[0], timings[1])) return GenericProtocolData.Invalid;

            byte nBitsRegistered = 0;
            for (int n = 1; n < timings.Count; n += 2)
            {
                ushort h = timings[n];
                ushort l = timings[n + 1];

                if (VerifyOne(h, l))
                {
                    value = value << 1;
                    value |= 1; // Parse ratio
                    nBitsRegistered++;
                }
                else if (VerifyZero(h, l))
                {
                    value = value << 1;
                    nBitsRegistered++;
                }
                else if (nBitsRegistered >= 8 && nBitsRegistered % 2 == 0)
                {
                    break;
                }
                else
                {
                    return GenericProtocolData.Invalid;
                }
            }

            return new GenericProtocolData(true, value, nBitsRegistered);
        }

        public List<ushort> BuildPacket(in GenericProtocolData data)
        {
            // Start building list using sync
            List<ushort> list = [SYNC_DEFAULT, SYNC_DEFAULT];

            // Run value using binary inversion
            for (int n = data.Bits - 1; n >= 0; n--)
            {
                // Check if bit is high
                if ((data.Value & (1UL << n)) > 0)
                {
                    list.Add(ONE_H_DEFAULT);
                    list.Add(ONE_L_DEFAULT);
                }
                else
                {
                    list.Add(ZERO_H_DEFAULT);
                    list.Add(ZERO_L_DEFAULT);
                }
            }

            return list;
        }

        public IProtocolData CreateValueProtocolData(ulong value, short bits)
        {
            if (bits <= 0) bits = 18;
            if (bits > 0xFF) bits = 0xFF;
            
            return new GenericProtocolData(true, value, (byte)bits);
        }
    }
}
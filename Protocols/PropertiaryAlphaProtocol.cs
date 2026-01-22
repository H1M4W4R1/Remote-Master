using JetBrains.Annotations;

namespace RadioRemote.Protocols
{
    [UsedImplicitly] public sealed class PropertiaryAlphaProtocol : IRadioProtocol
    {
        private ushort _desiredSyncLength = 4400;

        // -------- protocol timing (µs) --------
        public const float SYNC_H_TO_DL_RATIO = 4400f / 350;
        public const float SYNC_H_TO_DL_MIN = SYNC_H_TO_DL_RATIO / 1.2f;
        public const float SYNC_H_TO_DL_MAX = SYNC_H_TO_DL_RATIO * 1.2f;

        public const float SYNC_H_TO_DH_RATIO = 4400f / 920f;
        public const float SYNC_H_TO_DH_MIN = SYNC_H_TO_DH_RATIO / 1.2f;
        public const float SYNC_H_TO_DH_MAX = SYNC_H_TO_DH_RATIO * 1.2f;
        
        public const float GENERIC_RATIO = 920f / 350;
        public const float GENERIC_RATIO_MIN = GENERIC_RATIO / 1.2f;
        public const float GENERIC_RATIO_MAX = GENERIC_RATIO * 1.2f;

        public const ushort BITS = 16;

        public ushort Bits => BITS;
        public string Name => "P-ALPHA";

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

        private static bool VerifySync(ushort sync, ushort data)
        {
            float ratio = (float) sync / data;
            
            return ratio is > SYNC_H_TO_DH_MIN and < SYNC_H_TO_DH_MAX or > SYNC_H_TO_DL_MIN and < SYNC_H_TO_DL_MAX;
        }

        public bool TryParse(List<ushort> values, out ulong value)
        {
            const int PACKET_LENGTH = 1 + BITS * 2; // (Sync + bits of data) x2

            value = 0;
            if (values.Count < PACKET_LENGTH) return false;
            if (!VerifySync(values[0], values[1])) return false;

            for (int n = 1; n < PACKET_LENGTH; n += 2)
            {
                ushort h = values[n];
                ushort l = values[n + 1];

                // Shift value left
                value = value << 1;
                if (VerifyOne(h, l)) value |= 1; // Parse ratio
                else if (!VerifyZero(h, l)) return false;
            }

            return true;
        }



        public void SetSyncLenght(ushort desiredSyncLength = 11_600)
        {
            _desiredSyncLength = desiredSyncLength;
        }

        public List<ushort> BuildPacket(in ulong value)
        {
            ushort shortPulse = (ushort)(_desiredSyncLength / SYNC_H_TO_DL_RATIO);
            ushort longPulse = (ushort) (shortPulse * GENERIC_RATIO);
            
            // Start building list using sync
            List<ushort> list = [_desiredSyncLength, shortPulse];
            
            // Run value using binary inversion
            for (int n = BITS - 1; n >= 0; n--)
            {
                // Check if bit is high
                if ((value & (1UL << n)) > 0)
                {
                    list.Add(longPulse);
                    list.Add(shortPulse);
                }
                else
                {
                    list.Add(shortPulse);
                    list.Add(longPulse);
                }
            }

            return list;
        }
    }
}
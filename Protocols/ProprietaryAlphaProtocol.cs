using JetBrains.Annotations;
using RadioRemote.Protocols.Data;

namespace RadioRemote.Protocols
{
    [UsedImplicitly] public sealed class ProprietaryAlphaProtocol : IValueRadioProtocol<GenericProtocolData>
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

        public void SetSyncLenght(ushort desiredSyncLength = 11_600)
        {
            _desiredSyncLength = desiredSyncLength;
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
            ushort shortPulse = (ushort) (_desiredSyncLength / SYNC_H_TO_DL_RATIO);
            ushort longPulse = (ushort) (shortPulse * GENERIC_RATIO);

            // Start building list using sync
            List<ushort> list = [_desiredSyncLength, shortPulse];

            // Run value using binary inversion
            for (int n = data.Bits - 1; n >= 0; n--)
            {
                // Check if bit is high
                if ((data.Value & (1UL << n)) > 0)
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

        public IProtocolData CreateValueProtocolData(ulong value, short bits)
        {
            if (bits <= 0) bits = 16;
            if (bits > 0xFF) bits = 0xFF;
            
            return new GenericProtocolData(true, value, (byte) bits);
        }
    }
}
using RadioRemote.Protocols;
using RadioRemote.Protocols.Data;

namespace RadioRemote
{
    public sealed class Delegates
    {
        /// <summary>
        ///     Handler used for parsing radio signal events
        /// </summary>
        public delegate void OnRadioSignalReceivedHandler(IRadioProtocol protocol, IProtocolData data);
    }
}
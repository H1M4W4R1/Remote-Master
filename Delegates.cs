using RadioRemote.Protocols;

namespace RadioRemote
{
    public sealed class Delegates
    {
        /// <summary>
        ///     Handler used for parsing radio signal events
        /// </summary>
        public delegate void OnRadioSignalReceivedHandler(IRadioProtocol protocol, long value);
    }
}
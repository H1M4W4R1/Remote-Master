#include "protocols/impl/protocol_ev1527.h"

EV1527Protocol::EV1527Protocol() {
    // Constructor - no initialization needed with RC-Switch
}

const char* EV1527Protocol::getName() const {
    return "EV1527";
}

int EV1527Protocol::getProtocolId() const {
    return 1;  // RC-Switch protocol ID for EV1527
}

bool EV1527Protocol::supportsRx() const {
    return true;  // RC-Switch supports receiving EV1527
}

bool EV1527Protocol::supportsTx() const {
    return true;  // RC-Switch supports transmitting EV1527
}

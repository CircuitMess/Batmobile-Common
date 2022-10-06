#include "CommunicationCommon.h"

void CommunicationCommon::setDcListener() {

}

bool CommunicationCommon::isConnected() {
    return isWiFiConnected() && isClientConnected();
}

void CommunicationCommon::setClient(AsyncClient *client) {
    delete stream;
    stream = client;
}

ControlPacket CommunicationCommon::sendPacket(ControlPacket &packet) {
    return ControlPacket();
}

bool CommunicationCommon::isClientConnected() {
    return stream->connected();
}

void CommunicationCommon::loop(uint micros) {

}

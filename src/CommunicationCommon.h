#ifndef BATMOBILE_COMMON_COMMUNICATIONCOMMON_H
#define BATMOBILE_COMMON_COMMUNICATIONCOMMON_H

#include <Loop/LoopListener.h>
#include <AsyncTCP.h>
#include "ComType.h"
#include "DisconnectListener.h"
#include <Buffer/RingBuffer.h>

struct ControlPacket {
    ComType type;
    uint8_t data;
};

class CommunicationCommon : private LoopListener{
public:
    CommunicationCommon();
    bool isConnected();
    void setDcListener(DisconnectListener* listener);

protected:
    virtual void setClient(AsyncClient *client);
    void sendPacket(const ControlPacket& packet);
    virtual bool isWiFiConnected() = 0;
    virtual void processPacket(ControlPacket& packet) = 0;

private:
    void loop(uint micros) override;
    bool isClientConnected();
    DisconnectListener* dcListener  = nullptr;
    AsyncClient* client = nullptr;
    RingBuffer data;
};

#endif //BATMOBILE_COMMON_COMMUNICATIONCOMMON_H

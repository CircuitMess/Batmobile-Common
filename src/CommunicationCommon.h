#ifndef BATMOBILE_COMMON_COMMUNICATIONCOMMON_H
#define BATMOBILE_COMMON_COMMUNICATIONCOMMON_H

#include <Loop/LoopListener.h>
#include <AsyncTCP.h>

struct ControlPacket {
    //TODO: change ControlPacket
    char dummyData[10];
};

class CommunicationCommon : private LoopListener{
public:
    void setDcListener();
    bool isConnected();
    void loop(uint micros) override;

protected:
    virtual void setClient(AsyncClient *client);
    ControlPacket sendPacket(ControlPacket& packet);
    virtual bool isWiFiConnected() = 0;
    virtual void processPacket(ControlPacket& packet) = 0;

private:
    AsyncClient* stream;
    bool isClientConnected();

};

#endif //BATMOBILE_COMMON_COMMUNICATIONCOMMON_H

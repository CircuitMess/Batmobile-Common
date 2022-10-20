#include "CommunicationCommon.h"

CommunicationCommon::CommunicationCommon() : data(1024){
}

void CommunicationCommon::setDcListener(DisconnectListener* listener){
	dcListener = listener;
}

bool CommunicationCommon::isConnected(){
	Serial.println("CC.isConnected()");
	return isWiFiConnected() && isClientConnected();
}

void CommunicationCommon::setClient(AsyncClient* aClient){
	delete client;
	client = aClient;

	client->onConnect([this](void*, AsyncClient* client){
		Serial.printf("Connected\n");
	}, nullptr);

	client->onDisconnect([this](void*, AsyncClient* server){
		//TODO - reconnect to wifi in case of total disconnect
		Serial.println("client disconnected from port");
	});

	client->onData([this](void* arg, AsyncClient* server, void* data, size_t len){
		Serial.printf("\nData received from %s \n", client->remoteIP().toString().c_str());
		Serial.write((uint8_t*) data, len);
		this->data.write(static_cast<uint8_t*>(data), len);
	}, client);

	client->onError([this](void*, AsyncClient* c, int8_t error){
		Serial.print("error occurred: ");
		Serial.println(c->errorToString(error));
	}, nullptr);

	client->onTimeout([this](void*, AsyncClient*, uint32_t time){
		Serial.printf("timeout error %d passed\n", time);
	}, nullptr);
}

void CommunicationCommon::sendPacket(const ControlPacket& packet){
	if(!client->canSend()) return; //Needs to be added, otherwise it crashes
	client->add(reinterpret_cast<const char*>(&packet), sizeof(ControlPacket));
	client->send();
}

bool CommunicationCommon::isClientConnected(){
	return client->connected();
}

void CommunicationCommon::loop(uint micros){
	ControlPacket packet;
	while(data.readAvailable() >= sizeof(ControlPacket)){
		data.read(reinterpret_cast<uint8_t*>(&packet), sizeof(ControlPacket));
		processPacket(packet);
	}
}

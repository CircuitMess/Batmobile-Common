#ifndef BATMOBILE_COMMON_NETWORKCONFIG_H
#define BATMOBILE_COMMON_NETWORKCONFIG_H

#include <WiFi.h>

const extern IPAddress batmobileIP;
const extern IPAddress controllerIP;
const extern IPAddress gateway;
const extern IPAddress subnet;
const extern uint16_t controlPort;
const extern uint16_t feedPort;

#define FEED_ENV_LEN 8
const extern uint8_t FrameHeader[FEED_ENV_LEN];
const extern uint8_t FrameTrailer[FEED_ENV_LEN];


#endif //BATMOBILE_COMMON_NETWORKCONFIG_H

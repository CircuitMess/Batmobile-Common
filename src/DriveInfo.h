#ifndef BATMOBILE_COMMON_DRIVEINFO_H
#define BATMOBILE_COMMON_DRIVEINFO_H

#include <Arduino.h>
#include "DriveMode.h"
#include <glm.h>
#include <Buffer/RingBuffer.h>
#include <vector>
#include <memory>

using namespace glm;

enum class LineStatus : uint8_t {
	OnLine, OffLine
};

struct Ball {
	vec<2, int16_t> pos;
	uint16_t r;
	uint8_t hue;
};

struct Marker {
	vec<2, int16_t> projected[4];
	uint16_t id;
};

struct CamFrame {
	size_t size;
	void* data;
};

struct MotorInfo {
	int8_t frontLeft;
	int8_t frontRight;
	int8_t backLeft;
	int8_t backRight;
};

enum class MarkerAction : uint8_t {
	None,
	Forward,           //Forward
	Backward,          //Backward
	Do360,             //360
	Burnout,           //Burnout
	HeadlightOn,       //Headlight  ledice ON
	HeadlightOff,      //Headlight ledice OFF
	Honk,              //Potrubi
	Batsplosion,       //zvuk + svijetlo
	RGBOff,            //RGB donje ledice OFF
	RGBBreathe,        //RGB donje ledice breathing
	RGBSolid,          //RGB donje ledice solid
	Bats,              //Zvuk šišmiša + na ekranu šišmiši
};


struct BallDriveInfo;
struct LineDriveInfo;
struct MarkerDriveInfo;

struct DriveInfo {
	DriveMode mode;
	MotorInfo motors{};
	CamFrame frame{};

	static constexpr size_t baseSize = sizeof(DriveMode) + sizeof(MotorInfo) + sizeof(CamFrame::size);

	/**
	 * Returns size of struct in binary form, including all elements and sub-elements.
	 * @return Struct size.
	 */
	virtual size_t size() const;

	/**
	 * Serializes data to destination buffer. Buffer must be at least DriveInfo::size() bytes long.
	 * @param dest Destination buffer
	 */
	void toData(void* dest) const;

	virtual ~DriveInfo();

	/**
	 * Deserializes data from a RingBuffer to a DriveInfo struct.
	 * Buffer is expected to have the binary data on the first byte, and contains 'size' bytes of binary DriveInfo data.
	 * @param buf RingBuffer containing the binary data, starting from the first byte.
	 * @param size Size of DriveInfo binary data (bytes from header to trailer in a UDP packet)
	 * @return unique_ptr to a DriveInfo struct
	 */
	static std::unique_ptr<DriveInfo> deserialize(RingBuffer& buf, size_t size);

	/**
	 * Converts the DriveInfo struct to its BallDriveInfo derivative.
	 * @return BallDriveInfo pointer if mode == BallDrive, otherwise nullptr
	 */
	BallDriveInfo* toBall() const;

		/**
	 * Converts the DriveInfo struct to its LineDriveInfo derivative.
	 * @return LineDriveInfo pointer if mode == LineDrive, otherwise nullptr
	 */
	LineDriveInfo* toLine() const;

		/**
	 * Converts the DriveInfo struct to its MarkerDriveInfo derivative.
	 * @return MarkerDriveInfo pointer if mode == MarkerDrive, otherwise nullptr
	 */
	MarkerDriveInfo* toMarker() const;
};

struct BallDriveInfo : DriveInfo {
	std::vector<Ball> balls;
};

struct LineDriveInfo : DriveInfo {
	LineStatus lineStatus;
};

struct MarkerDriveInfo : DriveInfo {
	MarkerAction action;
	std::vector<Marker> markers;
};

#endif //BATMOBILE_COMMON_DRIVEINFO_H

#include "DriveInfo.h"
#include <memory>
#include <esp_heap_caps.h>
static const char* tag = "DataModel";

size_t DriveInfo::size() const{
	size_t size = baseSize + frame.size;
	switch(mode){
		case DriveMode::Ball:
			size += 1; //length of balls vector
			size += toBall()->balls.size() * sizeof(Ball); //balls vector
			break;

		case DriveMode::Line:
			size += 1; //line status
			break;

		case DriveMode::Marker:
			size += 1; //length of markers vector
			size += toMarker()->markers.size() * sizeof(Marker); //markers vector
			size += sizeof(MarkerAction); //current action
			break;

		default:
			break;
	}

	return size;
}

DriveInfo::~DriveInfo(){
	if(frame.data){
		free(frame.data);
		frame.data = nullptr;
	}
}

void DriveInfo::toData(void* dest) const{
	auto data = (uint8_t*) dest;
	memcpy(data, &mode, sizeof(DriveMode));
	data += sizeof(DriveMode);

	memcpy(data, &motors, sizeof(MotorInfo));
	data += sizeof(MotorInfo);

	memcpy(data, &frame.size, sizeof(CamFrame::size));
	data += sizeof(CamFrame::size);

	memcpy(data, frame.data, frame.size);
	data += frame.size;

	if(mode == DriveMode::Manual || mode == DriveMode::Idle) return;

	switch(mode){
		case DriveMode::Ball:
			*data = (uint8_t) toBall()->balls.size();
			data++;
			for(auto& ball: toBall()->balls){
				memcpy(data, (uint8_t*) &ball, sizeof(Ball));
				data += sizeof(Ball);
			}
			break;

		case DriveMode::Line:
			*data = (uint8_t) toLine()->lineStatus;
			break;

		case DriveMode::Marker:
			*data = (uint8_t) toMarker()->markers.size();
			data++;
			for(auto& marker: toMarker()->markers){
				memcpy(data, (uint8_t*) &marker, sizeof(Marker));
				data += sizeof(Marker);
			}
			*data = (uint8_t) toMarker()->action;
			break;

		default:
			break;
	}
}

std::unique_ptr<DriveInfo> DriveInfo::deserialize(RingBuffer& buf, size_t size){
	if(size < baseSize){
		ESP_LOGE(tag, "Couldn't create DriveInfo from data, required size: %d, received: %d\n", baseSize, size);
		return nullptr;
	}
	DriveMode mode;

	buf.read((uint8_t*) (&mode), sizeof(DriveMode));

	std::unique_ptr<DriveInfo> info;
	switch(mode){
		case DriveMode::Ball:
			info = std::make_unique<BallDriveInfo>();
			break;
		case DriveMode::Line:
			info = std::make_unique<LineDriveInfo>();
			break;
		case DriveMode::Marker:
			info = std::make_unique<MarkerDriveInfo>();
			break;
		default:
			info = std::make_unique<DriveInfo>();
			break;
	}

	info->mode = mode;
	buf.read((uint8_t*) &info->motors, sizeof(MotorInfo));
	buf.read((uint8_t*) &info->frame.size, sizeof(CamFrame::size));


	if(buf.readAvailable() < info->frame.size){
		ESP_LOGE(tag, "Deserialize data too short, lacks JPG frame");
		return nullptr;
	}

	info->frame.data = malloc(info->frame.size);

	if(info->frame.data == nullptr){
		ESP_LOGE(tag, "Couldn't allocate buffer for jpg frame data, free heap: %zu, largest alloc: %zu, needed: %zu", ESP.getFreeHeap(), heap_caps_get_largest_free_block(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL), info->frame.size);
		return nullptr;
	}
	buf.read((uint8_t*) info->frame.data, info->frame.size);

	if(mode == DriveMode::Manual || mode == DriveMode::Idle) return info;

	uint8_t numElements;
	if(!buf.read(&numElements, 1)){
		ESP_LOGE(tag, "Deserialize data is of type %d, but lacks type-specific data after JPG");
		return nullptr;
	}

	auto ballInfo = info->toBall();
	auto markerInfo = info->toMarker();

	switch(mode){
		case DriveMode::Ball:
			ballInfo->balls.reserve(numElements);

			if(buf.readAvailable() < numElements * sizeof(Ball)){
				ESP_LOGE(tag, "Deserialize data is of type %d, but lacks type-specific data after JPG");
				return nullptr;
			}

			for(uint8_t i = 0; i < numElements; ++i){
				Ball ball{};
				buf.read((uint8_t*) (&ball), sizeof(Ball));
				ballInfo->balls.push_back(ball);
			}

			break;


		case DriveMode::Line:
			info->toLine()->lineStatus = (LineStatus) numElements;
			break;


		case DriveMode::Marker:
			info->toMarker()->markers.reserve(numElements);
			if(buf.readAvailable() < numElements * sizeof(Marker) + sizeof(MarkerAction)){
				ESP_LOGE(tag, "Deserialize data is of type %d, but lacks type-specific data after JPG");
				return nullptr;
			}

			for(uint8_t i = 0; i < numElements; ++i){
				Marker marker{};
				buf.read((uint8_t*) (&marker), sizeof(Marker));
				markerInfo->markers.push_back(marker);
			}

			buf.read((uint8_t*) &markerInfo->action, sizeof(MarkerAction));

			break;

		default:
			break;
	}

	return info;
}

LineDriveInfo* DriveInfo::toLine() const{
	if(mode != DriveMode::Line) return nullptr;
	return (LineDriveInfo*) this;
}

BallDriveInfo* DriveInfo::toBall() const{
	if(mode != DriveMode::Ball) return nullptr;
	return (BallDriveInfo*) this;
}

MarkerDriveInfo* DriveInfo::toMarker() const{
	if(mode != DriveMode::Marker) return nullptr;
	return (MarkerDriveInfo*) this;
}

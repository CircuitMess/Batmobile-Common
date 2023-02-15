#ifndef BATMOBILE_LIBRARY_COMTYPE_H
#define BATMOBILE_LIBRARY_COMTYPE_H

enum class ComType : uint8_t {
	BallHue,
	Battery,
	Boost,
	DriveDir,
	DriveMode,
	DriveSpeed,
	Honk,
	Volume,
	SignalStrength,
	SettingsSound,
	Disconnect,
	Shutdown,
	ShutdownAck,
	Headlights,
	Taillights,
	Underlights,
	SoundEffect,
	Dance,
	OverrideSound,
	MotorsTimeout,
	MotorsTimeoutClear
};

#endif //BATMOBILE_LIBRARY_COMTYPE_H

// ofxACNSender.h
// Based on:
//
// ofxE131Client.h
//
// Modified by Eduard Frigola on 24/05/21
//
// ORIGINAL: https://github.com/sosolimited/Cinder-DMX/blob/master/src/sACN/E131Client.h
//
//  Created by Alex Olivier on 04/05/16.
//
//  Some code from: https://github.com/tobiasebsen/ofxArtNode/blob/master/src/ofxArtNode.h

#pragma once

#include "ofMain.h"
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>

class ofxACNSender {
public:
	static constexpr unsigned LOCAL_PORT_START = 7777;
	static constexpr unsigned DESTINATION_PORT = 5568;

	~ofxACNSender();

	// NM: implement poco multicast later
	void setup(std::string addr);//, bool mCast = false);
	void update();

	// Return next Universe/Channel
	std::pair<int, int> setChannel(int universe, int channel, u_char value);
	std::pair<int, int> setChannels(int universe, int startChannel, u_char* values, size_t size);
	std::pair<int, int> setChannels(int startUniverse, int startChannel, ofPixels dataIn);
	std::pair<int, int> setPixel(int startUniverse, int startChannel, ofColor col);

	void setPriority(int priority);


	// ACT - Gamma Adjustment.
	inline void RecalculateGamma(void);


	void SetGammaValues(float _fGamma, int redMod = 255, int greenMod = 255, int blueMod = 255);
private:

	inline ofColor setGamma(ofColor in) const;

	void connectUDP(unsigned localPort = 0);
	void sendDMX();
	void setLengthFlags();
	void setPacketUniverse(int universe);
	void createNewUniverse(int universe);

	// Data structure to store payload and sequence num for each universe
	struct UniverseData {
		char universeSequenceNum;
		std::array<char, 512> payload;
	};

	// BIG ENDIAN
	std::array<char, 638> sac_packet = { {
			// ROOT LAYER (RLP)
			0x00, 0x10,                   // Define RLP Preamble Size
			0x00, 0x00,                   // RLP Post amble size
			0x41, 0x53, 0x43, 0x2d, 0x45, // E131 Packet identifier
			0x31, 0x2e, 0x31, 0x37, 0x00, 0x00, 0x00, 0x00,
			0x00, // FILL OUT, low 12 bits = PDU length
			// High 4 bits = 0x7
			0x00, 0x00, 0x00, 0x04, // Identifies RLP Data as 1.31 Protocol PDU
			0x00, 0x00, 0x00, 0x00, // CID, Sender's unique CID
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

			// FRAMING LAYER
			0x00, 0x00,             // Low 12 bits = PDU length, High 4 bits=07
			0x00, 0x00, 0x00, 0x02, // Identifies 1.31 data as DMP protocol PDU

			0x00, 0x00, 0x00, 0x00, // User assigned name of source
			0x00, 0x00, 0x00, 0x00, // UTF-8 string, null-terminated
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

			0x64,       // DATA PRIORITY, 0-200, default of 100 (0x64)
			0x00, 0x00, // RESERVED, transmitters send 0, receivers ignore
			0x00,       // Sequence number to detect duplicate or out of order packets
			0x00,       // Options flag, bit 7 = preview data, bit 6 = stream terminated
			0x00, 0x00, // UNIVERSE number

			// DMP Layer
			0x00, 0x00, // Protocol flags + length, Low 12 bits = PDU length
			// High 4 bits = 0x7
			0x02,                    // Identifies DMP set property message PDU
			static_cast<char>(0xA1), // Identifies format of address and data
			0x00, 0x00, // Indicates DMX_start.  Code is at DMP address 0.
			0x00, 0x01, // Indicates each property is 1 octet / byte
			0x02, 0x01, // Indicates 1+the number of slots in packet
			0x00,       // DMX start code (0 is standard)
			char(512)   // DMX payload (all 512 channels)
		} };

	/*
		The range for multicast addresses is from
		224.0.0.0 to 239.255.255.255
	*/
	// NM: implement Poco multicast later
	//bool bMcast = false;
	bool loggedException = false;

	int packet_length = 638; // Length when all 512 DMX channels are sent
	int priority = 100;

	std::string ipAddress;

	static unsigned portOffset;
	Poco::Net::DatagramSocket localSocket;
	Poco::Net::SocketAddress destinationAddress;
	bool localSocketBound{ false };

	std::map<int, UniverseData> universePackets;

	// ACT - Gamma Adjustment.

	float m_fGamma;
	int  m_iRedMod;
	int  m_iGreenMod;
	int  m_iBlueMod;
	uint8_t gamma[256][3];
};

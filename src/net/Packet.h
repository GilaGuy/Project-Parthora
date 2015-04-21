#ifndef PACKET_H
#define PACKET_H

#include <SFML/Network.hpp>
#include "Protocol.h"

#define PACKET_SIZE 1024
#define PACKET_SEP 0x7F

struct Packet
{
	template < class T >
	void add(T t);

	std::string encode() const;
	bool decode(std::string raw);

	MessageType mType;
	std::vector<std::string> mParams;
};

#endif // PACKET_H

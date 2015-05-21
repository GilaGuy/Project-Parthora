/**
 * Packet.
 *
 * @date       April 20, 2015
 *
 * @revisions  May 21, 2015
 *             Improved packet decoding and encoding.
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      This Packet class provides a convenient way for the server and client to communicate with each other.
 *             Included are adding parameters, encoding everything into a string and decoding it back into a packet.
 *
 *             A thanks goes to an answer on Stack Overflow for the convenient string splitter:
 *             http://stackoverflow.com/questions/236129/split-a-string-in-c
 */

#include "Packet.h"

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

bool Packet::decode(const char* raw, size_t numOfBytes)
{
	mData = split(std::string(raw, numOfBytes), DATA_SEPARATOR);
	mType = static_cast<PacketType>(get<int>(0));
	mData.erase(mData.begin());

	return true;
}

size_t Packet::encode(std::string& encoded) const
{
	encoded.clear();

	encoded += static_cast<int>(mType)+'0';

	for (const std::string d : mData)
	{
		encoded += DATA_SEPARATOR;
		encoded += d;
	}

	// If we want to make all the packets have a fixed length
	/*
	if (encoded.length() < MAX_SIZE)
	encoded += std::string(MAX_SIZE - encoded.length(), '\0');
	*/

	return encoded.length();
}

std::string Packet::toString() const
{
	std::string str;

	str += static_cast<int>(mType)+'0';

	for (const std::string d : mData)
	{
		str += DATA_SEPARATOR;
		str += d;
	}

	return str;
}

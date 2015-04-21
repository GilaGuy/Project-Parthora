/**
 * Packet.
 *
 * @date       April 20, 2015
 *
 * @revisions
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
#include <sstream>
#include <string>
#include <vector>

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

template < class T >
void Packet::add(T t)
{
	std::stringstream converter;
	std::string data;

	converter << t;
	converter >> data;

	mParams.push_back(data);
}

std::string Packet::encode() const
{
	std::string data;

	data += (int)mType;
	data += PACKET_SEP;

	for (std::string p : mParams)
	{
		data += p;
		data += PACKET_SEP;
	}

	data.pop_back();

	return data;
}

bool Packet::decode(std::string raw)
{
	auto dataArray = split(raw, PACKET_SEP);

	if (dataArray.empty()) return false;

	mType = static_cast<MessageType>(std::stoi(dataArray.at(0)));
	mParams = std::vector<std::string>(dataArray.begin() + 1, dataArray.end());

	return true;
}

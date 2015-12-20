#ifndef PACKET_H
#define PACKET_H

#include <sstream>
#include <string>
#include <vector>
#include "Shared.h"

struct Packet
{
	static const size_t MAX_SIZE = 1024;
	static const char DATA_SEPARATOR = 0x1F;

	template < class T >
	static std::string ToString(T t)
	{
		std::stringstream converter;
		std::string converted;

		converter << t;
		converter >> converted;

		return converted;
	}

	template < class T >
	T get(size_t pos) const
	{
		std::stringstream converter;
		T converted;

		converter << data.at(pos);
		converter >> converted;

		return converted;
	}

	inline std::string get(size_t pos) const
	{
		return data.at(pos);
	}

	inline size_t getDataSize() const
	{
		return data.size();
	}

	inline size_t last() const
	{
		return getDataSize() - 1;
	}

	template < class T >
	void add(T t)
	{
		data.push_back(ToString(t));
	}

	inline void add(std::string str)
	{
		data.push_back(str);
	}

	inline void rem(size_t idx)
	{
		if (idx == getDataSize() - 1)
			data.pop_back();
		else
			data.erase(data.begin() + idx);
	}

	template < class T >
	inline void replace(size_t idx, T newValue)
	{
		data.at(idx) = ToString(newValue);
	}

	Packet& combine(const Packet& other);

	bool decode(const char* raw, size_t numOfBytes);
	size_t encode(std::string& encoded) const;
	std::string toString() const;

	PacketType type;
	std::vector<std::string> data;
};

#endif // PACKET_H

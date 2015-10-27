#ifndef PACKET_H
#define PACKET_H

#include <sstream>
#include <string>
#include <vector>
#include "Protocol.h"

struct Packet
{
	static const size_t MAX_SIZE = 1024;
	static const char DATA_SEPARATOR = 0x1F;

	template < class T >
	static std::string ToString(T t)
	{
		std::stringstream converter;
		std::string data;

		converter << t;
		converter >> data;

		return data;
	}

	template < class T >
	T get(size_t pos) const
	{
		std::stringstream converter;
		T data;

		converter << mData.at(pos);
		converter >> data;

		return data;
	}

	inline std::string get(size_t pos) const
	{
		return mData.at(pos);
	}

	inline size_t getDataSize() const
	{
		return mData.size();
	}

	template < class T >
	void add(T t)
	{
		std::stringstream converter;
		std::string data;

		converter << t;
		converter >> data;

		mData.push_back(data);
	}

	inline void add(std::string str)
	{
		mData.push_back(str);
	}

	inline std::string rem(size_t idx)
	{
		mData.erase(mData.begin() + idx);
	}

	inline void remLast()
	{
		mData.pop_back();
	}

	inline void replace(size_t idx, std::string newstr)
	{
		mData.at(idx) = newstr;
	}

	bool decode(const char* raw, size_t numOfBytes);
	size_t encode(std::string& encoded) const;
	std::string toString() const;

	PacketType mType;
	std::vector<std::string> mData;
};

#endif // PACKET_H

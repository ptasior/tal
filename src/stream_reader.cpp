#include "stream_reader.h"
#include "log.h"
#include <iostream>
#include <istream>


StreamReader::Membuf::Membuf()
{}

StreamReader::Membuf::Membuf(char* begin, char* end)
{
	this->setg(begin, begin, end);
}

std::streambuf::pos_type StreamReader::Membuf::seekpos(pos_type sp, std::ios_base::openmode which)
{
  return seekoff(sp - pos_type(off_type(0)),
				std::ios_base::beg,
				which
			);
}

std::streambuf::pos_type StreamReader::Membuf::seekoff(off_type off,
			 std::ios_base::seekdir dir,
			 std::ios_base::openmode which)
{
	if (dir == std::ios_base::cur)
		gbump(off);
	else if (dir == std::ios_base::end)
		setg(eback(), egptr() + off, egptr());
	else if (dir == std::ios_base::beg)
		setg(eback(), eback() + off, egptr());
	return gptr() - eback();
}

// -----------------------------------------------------------------------------

StreamReader::StreamReader():
	mStream(&mBuf)
{}

StreamReader::~StreamReader()
{
	closeDataSource();
}

void StreamReader::closeDataSource()
{
	if(mChildren.size()) // TODO Copy data when closing
		Log(Log::DIE) << "StreamReader: Closing still used data source. Implement copying data";

	for(auto c : mChildren)
		c->closeDataSource();
	
	mType = Type::Empty;
}

void StreamReader::openFile(const std::string& name)
{
	if(mType != Type::Empty)
		Log(Log::DIE) << "StreamReader: Stream already opened";

	mType = Type::File;

	mFile.open(name, std::ios::binary|std::ios::in);
}

void StreamReader::openString(const char* data, uint32_t begin, uint32_t end)
{
	if(mType != Type::Empty)
		Log(Log::DIE) << "StreamReader: Stream already opened";

	mType = Type::Memory;
	mBegin = data + begin;
	mSize = end - begin;

	mBuf = Membuf(const_cast<char*>(data + begin), const_cast<char*>(data + end));
}

std::istream* StreamReader::get() const
{
	switch(mType)
	{
		case Type::File:
			return &mFile;
		case Type::Memory:
		case Type::Chunk:
			return &mStream;
		case Type::Empty:
			Log(Log::DIE) << "StreamReader: Attempting to read from a not open stream";
	}
	return nullptr;
}

std::shared_ptr<StreamReader> StreamReader::getChunk(uint32_t position, uint32_t size) const
{
	std::shared_ptr<StreamReader> ret = std::make_shared<StreamReader>();

	// if(mType == Type::Memory)
	// {
	// 	Log() << "StreamReader: getChunk share data";
	// 	ret->openString(mData.data(), position, size);
	// 	ret->mType = Type::Memory;
	// 	ret->mChildren.push_back(ret.get());
	// 	return ret;
	// }

	// Copy data from stream
	Log() << "StreamReader: getChunk - THIS IS EXPENSIVE";

	ret->mData.resize(size);
	get()->seekg(position);
	get()->read(ret->mData.data(), size);

	ret->openString(ret->mData.data(), 0, size);

	ret->mType = Type::Chunk;

	return ret;
}

const char* StreamReader::data() const
{
	if(mType == Type::Empty || mType == Type::File)
		Log(Log::DIE) << "StreamReader: Empty and File types do not support data()";

	if(mType == Type::Memory)
		return mBegin;
	else
		return mData.data();
}

uint32_t StreamReader::size() const
{
	if(mType == Type::Empty || mType == Type::File)
		Log(Log::DIE) << "StreamReader: Empty and File types do not support size()";

	if(mType == Type::Memory)
		return mSize;
	else
		return mData.size();
}

std::string StreamReader::readToString() const
{
	return std::string(std::istreambuf_iterator<char>(mStream), {});
}


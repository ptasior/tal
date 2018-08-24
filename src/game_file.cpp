#include "game_file.h"
#include "stream_reader.h"
#include "log.h"

GameFile::GameFile(std::shared_ptr<StreamReader> reader)
{
	mStreamReader = reader;
	std::istream *stream = reader->get();

	if(!stream->good())
		Log(Log::DIE) << "GameFile: Cannot open file";

	uint8_t type;
	uint8_t version;
	stream->read(reinterpret_cast<char*>(&type), 1);
	stream->read(reinterpret_cast<char*>(&version), 1);

	// Log() << "Reading file, type: " << type << " ver: " << version;

	if(type != 0x12 || version < 1)
		Log(Log::DIE) << "GameFile: Format not supported type: " << type << " version: " << version;

	std::string file;
	char buf[512];
	uint32_t start, stop;
	uint16_t size;

	uint32_t headerStop = 0, lastEnd = 0; // For validating the format

	while(stream->good())
	{
		stream->read(reinterpret_cast<char*>(&size), 2);
		if(!size)
		{
			// Log() << "size == 0";
			break; // End of file descriptor
		}
		// Log() << "size: " << size;

		stream->read(buf, size);
		stream->read(reinterpret_cast<char*>(&start), 4);
		stream->read(reinterpret_cast<char*>(&stop), 4);
		file = std::string(buf, size);

		// Log() << file << " start: " << start << " stop: " << stop;
		m_positions[file] = {start, stop};

		if(!headerStop)
			headerStop = start;
		else if(start != lastEnd)
			Log(Log::DIE) << "GameFile: Embedded file does not start immediately after previous one: "
				<< file << " lastEnd: " << lastEnd << " start: " << start;

		lastEnd = start+stop;
	}

	if(stream->tellg() != -1 && // More than 1 file
			(uint32_t)stream->tellg() != headerStop)
		Log(Log::DIE) << "GameFile: First file index not immediately after header "
				<< " headerStop: "<< headerStop << " file pos: " << (uint32_t)stream->tellg();

	stream->seekg(0, std::ios_base::end); // Move to the end of file
	if((uint32_t)stream->tellg() != start + stop)
			Log(Log::DIE) << "GameFile: File size different than declared "
				<< " fileSize: "<< (uint32_t)stream->tellg() << " last file end: " << start + stop;

	Log() << "GameFile: file read";
}

GameFile::~GameFile()
{
}

std::vector<char> GameFile::readData(std::string file) const
{
	std::vector<char> buf;
	if(!m_positions.count(file))
	{
		Log() << "GameFile: File does not exist: " << file;
		return buf;
	}

	Pos p = m_positions.at(file);

	buf.resize(p.size);
	mStreamReader->get()->seekg(p.position);
	mStreamReader->get()->read(buf.data(), p.size);

	return buf;
}

std::string GameFile::readString(std::string file) const
{
	std::string buf;
	if(!m_positions.count(file))
	{
		Log() << "GameFile: File does not exist: " << file;
		return buf;
	}

	Pos p = m_positions.at(file);

	buf.resize(p.size);
	mStreamReader->get()->seekg(p.position);
	mStreamReader->get()->read(&buf[0], p.size);

	return buf;
}

std::shared_ptr<StreamReader> GameFile::readStream(std::string file) const
{
	if(!m_positions.count(file))
	{
		Log() << "GameFile: File does not exist: " << file;
		return std::shared_ptr<StreamReader>();
	}

	Pos p = m_positions.at(file);

	return mStreamReader->getChunk(p.position, p.size);
}

bool GameFile::hasFile(const std::string& name) const
{
	return m_positions.count(name);
}


#include "data_reader.h"
#include "log.h"

DataReader *global_dataReader = nullptr;

DataReader::DataReader(std::string name)
{
	m_file.open(name, std::ios::binary);

	if(!m_file.good())
		Log(Log::DIE) << "DataReader: Cannot open file: " << name;

	uint8_t type;
	uint8_t version;
	m_file.read(reinterpret_cast<char*>(&type), 1);
	m_file.read(reinterpret_cast<char*>(&version), 1);

	// Log() << "Reading file, type: " << type << " ver: " << version;

	if(type != 0x12 || version < 1)
		Log(Log::DIE) << "DataReader: Format not supported type: " << type << " version: " << version;

	std::string file;
	char buf[512];
	uint32_t start, stop;
	uint16_t size;

	uint32_t headerStop = 0, lastEnd = 0; // For validating the format

	while(m_file.good())
	{
		m_file.read(reinterpret_cast<char*>(&size), 2);
		if(!size)
		{
			// Log() << "size == 0";
			break; // End of file descriptor
		}
		// Log() << "size: " << size;

		m_file.read(buf, size);
		m_file.read(reinterpret_cast<char*>(&start), 4);
		m_file.read(reinterpret_cast<char*>(&stop), 4);
		file = std::string(buf, size);

		// Log() << file << " start: " << start << " stop: " << stop;
		m_positions[file] = {start, stop};

		if(!headerStop)
			headerStop = start;
		else if(start != lastEnd)
			Log(Log::DIE) << "DataReader: Embedded file does not start immediately after previous one: "
				<< file << " lastEnd: " << lastEnd << " start: " << start;

		lastEnd = start+stop;
	}

	if((uint32_t)m_file.tellg() != headerStop)
		Log(Log::DIE) << "DataReader: First file index not immediately after header "
				<< " headerStop: "<< headerStop << " file pos: " << m_file.tellg();

	m_file.seekg(0, std::ios_base::end); // Move to the end of file
	if((uint32_t)m_file.tellg() != start + stop)
			Log(Log::DIE) << "DataReader: File size different than declared "
				<< " fileSize: "<< m_file.tellg() << " last file end: " << start + stop;

	Log() << "DataReader: file read: " << name;
}

DataReader::~DataReader()
{
	m_file.close();
}

std::vector<char> DataReader::read(std::string file) const
{
	std::vector<char> buf;
	if(!m_positions.count(file))
	{
		Log() << "DataReader: File does not exist: " << file;
		return buf;
	}

	Pos p = m_positions.at(file);

	buf.resize(p.size);
	m_file.seekg(p.position);
	m_file.read(buf.data(), p.size);

	return buf;
}

std::string DataReader::readString(std::string file) const
{
	std::string buf;
	if(!m_positions.count(file))
	{
		Log() << "DataReader: File does not exist: " << file;
		return buf;
	}

	Pos p = m_positions.at(file);

	buf.resize(p.size);
	m_file.seekg(p.position);
	m_file.read(&buf[0], p.size);

	return buf;
}


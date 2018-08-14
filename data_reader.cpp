#include "data_reader.h"
#include "log.h"

DataReader::DataReader(std::string name)
{
	m_file.open(name, std::ios::binary);

	if(!m_file.good())
		Log(Log::DIE) << "Cannot open file: " << name;

	uint8_t type;
	uint8_t version;
	m_file.read(reinterpret_cast<char*>(&type), 1);
	m_file.read(reinterpret_cast<char*>(&version), 1);

	// Log() << "Reading file, type: " << type << " ver: " << version;

	if(type != 0x12 || version < 1)
		Log(Log::DIE) << "Format not supported";

	std::string file;
	char buf[512];
	uint32_t start, stop;
	uint16_t size;

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

		Log() << file << " start: " << start << " stop: " << stop;
		m_positions[file] = {start, stop};
	}
}

DataReader::~DataReader()
{
	m_file.close();
}

std::string DataReader::read(std::string file) const
{
	std::string buf;
	if(!m_positions.count(file))
	{
		Log() << "File does not exist: " << file;
		return buf;
	}

	Pos p = m_positions.at(file);

	// Log() << "reading: " << file << " size: " << p.size;

	buf.reserve(p.size);
	m_file.seekg(p.position);
	m_file.read(&buf[0], p.size);

	return buf;
}


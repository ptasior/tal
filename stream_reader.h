#pragma once

#include <fstream>
#include <sstream>
#include <streambuf>
#include <iostream>
#include <memory>
#include <vector>


class StreamReader
{
public:
	StreamReader();

	void openFile(const std::string& name);
	void openString(const char* data, uint32_t begin, uint32_t end);

	std::istream* get() const;
	std::shared_ptr<StreamReader> getChunk(uint32_t position, uint32_t size) const;

	char* data() const;
	uint32_t size() const;

private:
	struct Membuf : std::streambuf
	{
		Membuf();
		Membuf(char* begin, char* end);

		pos_type seekpos(pos_type sp, std::ios_base::openmode which) override;

		pos_type seekoff(off_type off,
				 std::ios_base::seekdir dir,
				 std::ios_base::openmode which = std::ios_base::in) override;
	};

	enum class Type {Empty, File, Memory, Chunk};

	Type mType = Type::Empty;

	mutable std::ifstream mFile;

	Membuf mBuf;
	mutable std::istream mStream;

	std::vector<char> mData; // Owned data if created as a chunk
};


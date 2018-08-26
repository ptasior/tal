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
	~StreamReader();

	void openFile(const std::string& name);
	void openString(const char* data, uint32_t begin, uint32_t end);

	std::istream* get() const;
	std::shared_ptr<StreamReader> getChunk(uint32_t position, uint32_t size);

	const char* data() const;
	uint32_t size() const;
	std::string name() const;

	std::string readToString() const;

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

	void closeDataSource();

	Type mType = Type::Empty;
	std::string mName;

	// File
	mutable std::ifstream mFile;

	// Memory
	Membuf mBuf;
	mutable std::istream mStream;
	const char *mBegin = nullptr;
	uint32_t mSize = 0;
	std::vector<StreamReader*> mChildren;

	// Chunk
	std::vector<char> mData; // Owned data if created as a chunk
};


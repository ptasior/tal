#pragma once
#include <string>

class Log
{
public:
	enum Action {LOG, ERR, DIE};
	Log(Action a=LOG);
	~Log();

	Log& operator << (const std::string & str);
	Log& operator << (const char* str);
	Log& operator << (double val);
	Log& operator << (float val);
	Log& operator << (unsigned long val);
	Log& operator << (long val);
	Log& operator << (unsigned int val);
	Log& operator << (int val);
	Log& operator << (char val);

	void flush();

	static const char *endl;

private:
	void write(const char* str);

	std::string mMessage;
	Action mAction;
};


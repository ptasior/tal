#include "shared_data.h"
#include "lua.h"
#include "log.h"
#include <assert.h>

std::queue<std::string> SharedData::mChanges;
bool SharedData::mOnline = false;
SharedData SharedData::mRoot;

SharedData::SharedData()
{

}

void SharedData::applyChange(std::string line)
{
	assert(mOnline);

	Log() << "SharedData: Applying line: " << line;
	Lua::getInstance()->sharedDataUpdated(line);

	size_t pos = 0;
	std::string token;

	SharedData* p = &root();
	while ((pos = line.find('\1')) != std::string::npos) {
		token = line.substr(0, pos);
		line.erase(0, pos + 1);

		p = &(p->operator[](token.c_str()));
	}
	
	p->mValue = line;
}

std::queue<std::string>& SharedData::getChanges()
{
	return mChanges;
}

void SharedData::setOnline(bool v)
{
	Log() << "SharedData: Online: " << v;
	mOnline = v;
}

SharedData& SharedData::root()
{
	return mRoot;
}

void SharedData::print()
{
	print_i(0);
}

void SharedData::print_i(int idn)
{
	if(idn)
	{
		auto l = std::string(idn, ' ') + "- " + mKey;
		if(!mValue.empty())
			l += " = " + mValue;

		Log() << l;
	}
	else
		Log() << "-";

	for(auto b : mBranches)
		b.second.print_i(idn+1);
}

SharedData* SharedData::i_at(const std::string& key)
{
	if(!mBranches.count(key))
	{
		mBranches[key].mKey = key;
		mBranches[key].mParent = this;
	}
	return &mBranches[key];
}

void SharedData::i_set(const std::string& v)
{
	if(!mOnline)
		mValue = v; // If offline, wait till comes back from server to keep the order
	else
	{
		std::string line = mKey + "\1" + v;
		auto p = mParent;
		while(p->mParent)
		{
			line = p->mKey + "\1" + line;
			p = p->mParent;
		}

		mChanges.push(line);
	}
}

SharedData* SharedData::at(std::string key)
{
	return i_at(key.c_str());
}

std::string SharedData::get()
{
	return mValue;
}

void SharedData::set(std::string s)
{
	i_set(s);
}

SharedData& SharedData::operator[](const char* key)
{
	return *i_at(key);
}

void SharedData::operator=(const std::string& v)
{
	i_set(v);
}

void SharedData::operator=(int v)
{
	i_set(std::to_string(v));
}

SharedData::operator std::string()
{
	return mValue;
}

SharedData::operator int()
{
	return std::stoi(mValue);
}


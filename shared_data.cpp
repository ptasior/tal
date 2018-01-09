#include "shared_data.h"
#include "window.h"
#include "config.h"
#include "log.h"
#include <assert.h>

std::vector<std::string> SharedData::mChanges;
bool SharedData::mOffline = false;
SharedData SharedData::mRoot;

SharedData::SharedData()
{

}

SharedData& SharedData::operator[](const char* key)
{
	if(!mBranches.count(key))
	{
		mBranches[key].mKey = key;
		mBranches[key].mParent = this;
	}
	return mBranches[key];
}

void SharedData::operator=(const std::string& v)
{
	if(mOffline)
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

		mChanges.push_back(line);
	}
}

void SharedData::operator=(int v)
{
	this->operator=(std::to_string(v));
}

SharedData::operator std::string()
{
	return mValue;
}

SharedData::operator int()
{
	return std::stoi(mValue);
}

void SharedData::applyChange(std::string line)
{
	assert(!mOffline);

	Log() << "SharedData: Applying line: " << line;

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

std::vector<std::string>& SharedData::getChanges()
{
	return mChanges;
}

void SharedData::init()
{
	mOffline = global_window->getConfig()->get("offline") == "true";
}

SharedData& SharedData::root()
{
	return mRoot;
}

void SharedData::print(int idn)
{
	if(idn)
	{
		auto l = std::string(idn, ' ') + mKey;
		if(!mValue.empty())
			l += " = " + mValue;

		Log() << l;
	}

	for(auto b : mBranches)
		b.second.print(idn+1);
}


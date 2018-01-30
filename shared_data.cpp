#include "shared_data.h"
#include "lua.h"
#include "log.h"
#include <assert.h>

void SharedData::applyChange(std::string line)
{
	assert(mOnline);
	std::string origLine = line;

	Log() << "SharedData: Applying line: " << origLine;

	size_t pos = 0;
	std::string token;

	DataNode* p = &root();
	while ((pos = line.find('\1')) != std::string::npos) {
		token = line.substr(0, pos);
		line.erase(0, pos + 1);

		p = p->at(token.c_str());
	}

	p->mValue = line;

	Lua::getInstance()->sharedDataUpdated(origLine);
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

DataNode& SharedData::root()
{
	return mRoot;
}

void SharedData::print()
{
	mRoot.print(0);
}

void SharedData::addChange(const std::string& line)
{
	mChanges.push(line);
}

//-----------------------------------------------------------------------------

void DataNode::print(int idn)
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

	for(auto &b : mBranches)
		b.second.print(idn+1);
}

DataNode* DataNode::at(std::string key)
{
	if(!mBranches.count(key))
	{
		mBranches[key].mKey = key;
		mBranches[key].mParent = this;
	}
	return &mBranches[key];
}

void DataNode::set(int i)
{
	set(std::to_string(i));
}

void DataNode::set(std::string v)
{
	if(!global_sharedData->mOnline)
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

		global_sharedData->addChange(line);
	}
}

std::string DataNode::get()
{
	return mValue;
}

DataNode& DataNode::operator[](const char* key)
{
	return *at(key);
}

void DataNode::operator=(const std::string& v)
{
	set(v);
}

void DataNode::operator=(int v)
{
	set(std::to_string(v));
}

DataNode::operator std::string()
{
	return mValue;
}

DataNode::operator int()
{
	return std::stoi(mValue);
}

std::vector<std::string> DataNode::branches()
{
	std::vector<std::string> ret;
	for(auto i: mBranches)
		ret.push_back(i.first);
	return ret;
}


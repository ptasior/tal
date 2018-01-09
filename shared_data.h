#pragma once
#include <map>
#include <queue>
#include <string>

/* @example
 *
 * 	SharedData::setOnline(true);
 * 	SharedData::root()["players"]["count"] = 5;
 * 	SharedData::root()["players"]["visible"] = "yes";
 *
 * 	SharedData::sendChanges();
 * 	Log() << "first print";
 * 	SharedData::root().print();
 *
 * 	for(auto c : SharedData::getChanges())
 * 		SharedData::applyChange(c);
 *
 * 	Log() << "second print";
 * 	SharedData::root().print();
 */

class SharedData
{
public:
	SharedData();
	SharedData& operator[](const char* key);
	void operator=(const std::string& v);
	void operator=(int v);
	operator std::string();
	operator int();
	void print(int idn = 0);

	static void applyChange(std::string line);
	static void setOnline(bool v);
	static SharedData& root();
	static std::queue<std::string>& getChanges();

private:
	std::map<std::string, SharedData> mBranches;
	SharedData *mParent = nullptr;
	std::string mKey;
	std::string mValue;

	static std::queue<std::string> mChanges;
	static bool mOnline;
	static SharedData mRoot;
};


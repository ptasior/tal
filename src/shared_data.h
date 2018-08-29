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

class Net;

class DataNode
{
public:
	DataNode& operator[](const char* key);
	void operator=(const std::string& v);
	void operator=(int v);
	operator std::string();
	operator int();

	void print(int idn = 0);
	DataNode* at(std::string key);
	std::string get();
	std::vector<std::string> branches();
	void set(std::string s);
	// Set the value immediately, dont wait for srv confirmation
	void set_i(std::string s);
	void set(int s);


private:
	std::map<std::string, DataNode> mBranches;
	DataNode *mParent = nullptr;
	std::string mKey;
	std::string mValue;

	friend class SharedData;
};

class SharedData
{
public:
	void print();
	void setOnline(bool v);
	DataNode& root();

private:
	// Availalble from Net
	void applyChange(std::string line);
	std::queue<std::string>& getChanges();

	// Available from DataNode
	void addChange(const std::string& line);


	std::queue<std::string> mChanges;
	bool mOnline;
	DataNode mRoot;

	friend class Net;
	friend class DataNode;
};


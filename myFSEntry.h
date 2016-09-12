#ifndef MYFSENTRY_H_
#define MYFSENTRY_H_

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class myFSEntry{

protected:

	string name;
	string path;
	
public:

	myFSEntry(const string& file_name);
	virtual ~myFSEntry();
	
	const string& EntryName() const { return name; }
	const string& EntryPath() const { return path; }
	void changePath(const string& new_path){ (path) = new_path;}
	void changeName(const string& new_name){ (name) = new_name;}
};

#endif
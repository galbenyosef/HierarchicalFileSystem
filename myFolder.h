#ifndef MYFOLDER_H_
#define MYFOLDER_H_

#include <typeinfo>
#include "myFSEntry.h"

class myFolder : public myFSEntry{

	vector<myFSEntry*> entries; //polymorphic vector that holds myFolders/myFiles

public:
	
	vector<myFSEntry*>& get_entries(); //in order to iterate through entries

	void mkdir(const std::string&);
	myFolder* chdir(const string&);
	void rmdir();
	void ls();

	myFolder(const string&);
	~myFolder();

};

#endif
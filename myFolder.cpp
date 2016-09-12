#include "myFolder.h"

myFolder::myFolder(const string& folder_name):myFSEntry(folder_name) {}
myFolder::~myFolder(){ 

		 rmdir();

}

vector<myFSEntry*>& myFolder::get_entries()
{return entries;}


void myFolder::mkdir(const std::string& newdir) {

	for(vector<myFSEntry*>::const_iterator it = entries.begin(); it != entries.end(); ++it){
		myFSEntry* temp = *it;
		if (temp != NULL && temp->EntryName() == newdir){
			if (typeid(*temp)==typeid(myFolder)){
				cout << "Dir already exists" << endl;
				return;
				//throw exception
			}
		}
	}
	entries.push_back(new myFolder(newdir));
	return;
}
myFolder* myFolder::chdir(const string& newpath){

		if (newpath.length() == 0)
			return this;
	
		for(vector<myFSEntry*>::iterator it = entries.begin(); it != entries.end(); ++it){
			string bams= (*it)->EntryName();
			if (bams == newpath){
				if (typeid(**it) == typeid(myFolder)){
					return dynamic_cast<myFolder*>(*it);
				}
			}
		}
		//throw folder not found
		return 0;
}
void myFolder::rmdir(){

	vector<myFSEntry*>::iterator it = (entries.begin());
	while (it != entries.end()){
			delete (*it);
		it++;
	}

	entries.clear();
}
void myFolder::ls(){

	for(vector<myFSEntry*>::iterator it = entries.begin(); it != entries.end(); it++){
		myFSEntry* temp = *it;
		if (temp != 0){
			cout << temp->EntryName() << endl;
		}
	}
}
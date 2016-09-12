#ifndef MY_TERMINAL_H_
#define MY_TERMINAL_H_

#include "myFSEntry.h"
#include "myFolder.h"
#include "myFile.h"
#include <map>

enum {Command,Arg1,Arg2,Arg3};

const unsigned DEFAULT_MAX_COMMAND_BUFFER = 80;

class myTerminal{

	typedef void (myTerminal::*constFunction)(const vector<string>&)const;
	typedef void (myTerminal::*Function)(const vector<string>&);
	typedef map< string, constFunction> ReadCommandMap;
	typedef map< string, Function> WriteCommandMap;

public:

	myTerminal(const string&);
	~myTerminal(){delete root;};

	void start();

	class myFile_Not_Exists : public exception  {

	public:

		myFile_Not_Exists(const char* i) :Filename(i) {}

		virtual const char* what() const throw(){

			return "FILE NOT EXIST\n";
		}

	private:

		const char* Filename;

	};

	myFile* getFileEntry(const string&)const;

	void READ(const vector<string>&)const;
	void WRITE(const vector<string>&);
	void TOUCH(const vector<string>&);
	void COPY(const vector<string>&);
	void REMOVE(const vector<string>&);
	void MOVE(const vector<string>&);
	void CAT(const vector<string>&)const;
	void WC(const vector<string>&)const;
	void LS(const vector<string>&)const;
	void LN(const vector<string>&);
	void LPROOT(const vector<string>&)const;
	void CHDIR(const vector<string>&);
	void ChangeDir(const string&);
	void recursive_lproot(const vector<myFSEntry*>&)const;
	void MAKEDIR(const vector<string>&);
	void RMDIR(const vector<string>&);
	void PWD(const vector<string>&)const;
	string CDdotdot(const string&)const;


private:

	const myFSEntry* root;
	myFSEntry* current_folder;
	myFSEntry* tmp_dir;

	string currentPath;

	ReadCommandMap read_commands;
	WriteCommandMap write_commands;
};

#endif
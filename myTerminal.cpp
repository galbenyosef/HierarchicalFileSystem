#include "myTerminal.h"
#include "myFolder.h"

myTerminal::myTerminal(const string& drive_name) : root(new myFolder("")){

	dynamic_cast<myFolder*>(const_cast<myFSEntry*>(root))->mkdir(drive_name);
	
	current_folder = dynamic_cast<myFolder*>(const_cast<myFSEntry*>(root))->chdir(drive_name);

	currentPath = current_folder->EntryPath() + current_folder->EntryName() +'/';

	//commands mapper (string,command)

	read_commands["read"]=&myTerminal::READ;
	read_commands["lproot"]=&myTerminal::LPROOT;
	read_commands["cat"]=&myTerminal::CAT;
	read_commands["wc"]=&myTerminal::WC;
	read_commands["pwd"]=&myTerminal::PWD;	
	read_commands["ls"]=&myTerminal::LS;
	write_commands["write"]=&myTerminal::WRITE;
	write_commands["touch"]=&myTerminal::TOUCH;
	write_commands["copy"]=&myTerminal::COPY;
	write_commands["move"]=&myTerminal::MOVE;
	write_commands["remove"]=&myTerminal::REMOVE;
	write_commands["ln"]=&myTerminal::LN;
	write_commands["mkdir"]=&myTerminal::MAKEDIR;
	write_commands["chdir"]=&myTerminal::CHDIR;
	write_commands["rmdir"]=&myTerminal::RMDIR;


}

void myTerminal::start(){

	while (true){
		//reset tmp_dir to current folder
		tmp_dir=current_folder;
		//set current path
		currentPath=current_folder->EntryPath() + current_folder->EntryName() + '/';
		//pwd indicator
		cout << currentPath << ">";
		//user input workspace
		char* userInput = new char[DEFAULT_MAX_COMMAND_BUFFER];
		//vector holds the parsed input which enummed by: Command,Arg1,Arg2,Arg3
		vector<string> Arguments;
		//gets input,parse
		cin.getline(userInput,DEFAULT_MAX_COMMAND_BUFFER);
		do{
			string arg;
			while(*userInput != ' ' && *userInput)
				arg+=*(userInput++);
			Arguments.push_back(arg);
		} while (0 != *userInput++);
		
		//finds last delimiter (except last character, thats why length-1) and cuts what's after
		unsigned int index,delim_pos;
		string parent_directory_path;

		if (Arguments.size() > 1){
			index=0;
			delim_pos=0;
			while (index != Arguments[Arg1].length()-1){
				if (Arguments[Arg1].at(index++) == '/')
					delim_pos=index;
			}
			parent_directory_path = Arguments[Arg1].substr(0,delim_pos);
		}
		//change directory if needed
		tmp_dir = const_cast<myFSEntry*>(root);
		ChangeDir(parent_directory_path);

		//non-const mapped commands executor
		if (write_commands[Arguments[Command]]){
			Function FSWriter = write_commands[Arguments[Command]];
			(this->*FSWriter)(Arguments);
		}
		//const mapped commands executor
		if (read_commands[Arguments[Command]]){
			constFunction FSReader = read_commands[Arguments[Command]];
			(this->*FSReader)(Arguments);
		}

		if (Arguments[Command] == "exit")
			break;
	}
}	

myFile* myTerminal::getFileEntry(const string& entry_path)const 
{
	//file name parsing
	string parent_directory = CDdotdot(entry_path);
	string filename = entry_path.substr(parent_directory.length());
	//iterate over "entries" by "content iterator" called it
	vector<myFSEntry*>* content = &dynamic_cast<myFolder*>(tmp_dir)->get_entries();
	vector<myFSEntry*>::iterator it = (content->begin());
	while (it != content->end()){

		if (typeid(**it) == typeid(myFile) && (*it)->EntryName() == filename)
			//file has been found, return it
			return dynamic_cast<myFile*>(*it);
		it++;
	}
	//file hasn't been found
	throw myFile_Not_Exists(filename.c_str());
}

void myTerminal::READ(const vector<string>& Args)const{

	//parent directory of Arg1's length tells function either file is real or not (virtual)
	// and uses for 
	//file name parsing
	string parent_directory = CDdotdot(Args[Arg1]);
	string filename = Args[Arg1].substr(parent_directory.length());
	//index to read
	const char* index = Args[Arg2].c_str();
	//if file is real
	if (parent_directory.length() == 0 ){
		ifstream localfile(filename.c_str());
		if (localfile){
			localfile.seekg(*index);
			cout << localfile.peek() << endl;
			localfile.close();
		}
	}
	else{//file is virtual
		myFile* target_file;
		try{
			target_file = (getFileEntry(Args[Arg1]));
		}
		catch (myFile_Not_Exists ex){
			cout << ex.what();
			return;
		}
		target_file->open(ios::in);
		try{
			cout << (*target_file)[*index-'0'] << endl;}
		catch(...){
		}
		target_file->close();
	}
	return;

}
void myTerminal::WRITE(const vector<string>& Args){

	//parent directory of Arg1's length tells function either file is real or not (virtual)
	// and uses for 
	//file name parsing
	string parent_directory = CDdotdot(Args[Arg1]);
	string filename = Args[Arg1].substr(parent_directory.length());
	//index to read
	const char* index = Args[Arg2].c_str();
	//character to write
	const char* character = Args[Arg3].c_str();
	//if file is real
	if (parent_directory.length() == 0 ){
		if (ifstream(filename.c_str())){
			ofstream localfile(filename.c_str());
			localfile.seekp(*index-'0');
			localfile.put(*character);
			localfile.close();
		}
		else
			throw myFile_Not_Exists(filename.c_str());
	}
	else{//file is virtual
		myFile* target_file;
		try{
			target_file = (getFileEntry(Args[Arg1]));
		}
		catch (myFile_Not_Exists ex){
			cout << ex.what();
			return;
		}
		target_file->open(ios::out | ios::in);
		const char* character = Args[Arg3].c_str();
		try{
		(*target_file)[*index-'0']=*character;
		}
		catch(...){
		}
		target_file->close();
	}
	return;

}
void myTerminal::TOUCH(const vector<string>& Args){

	//parent directory of Arg1's length tells function either file is real or not (virtual)
	// and uses for 
	//file name parsing
	string parent_directory = CDdotdot(Args[Arg1]);
	string filename = Args[Arg1].substr(parent_directory.length());

	//file is real
	if (parent_directory.length() == 0 ){
		ofstream localfile(filename.c_str());
		localfile.flush();
		localfile.close();
	}
	//file is virtual
	else{
		myFile* target_file;
		try{//and exists
			target_file = (getFileEntry(Args[Arg1]));
		}
		catch (myFile_Not_Exists ex){
			//file not exists
			string fullpath;
			fullpath = tmp_dir->EntryPath() + tmp_dir->EntryName() + '/';
			//create new one
			myFile* newfile=new myFile(filename);
			newfile->changePath(fullpath);
			vector<myFSEntry*>* content = &dynamic_cast<myFolder*>(tmp_dir)->get_entries();
			(content)->push_back(newfile);
			newfile->open(ios::out);
			newfile->close();
			return;
		}
		target_file->open(ios::out);
		target_file->touch();
		target_file->close();
	}


}
void myTerminal::COPY(const vector<string>& Args){

	string parent_directory = CDdotdot(Args[Arg1]);
	string copied_parent_dir = CDdotdot(Args[Arg2]);
	string filename = Args[Arg1].substr(parent_directory.length());
	string copied_filename = Args[Arg2].substr(copied_parent_dir.length());

	//src real file
	ifstream rlocalfile;
	//target real file
	ofstream wlocalfile;

	//src virtual file
	myFile* src_file;
	//target virtual file
	myFile* copied_file;

	//new virtual file
	myFile* newVal=0;

	//source file is virtual
	if (parent_directory.length() != 0){
		try{//find src file
			src_file = (getFileEntry(Args[Arg1]));
		}
		catch (myFile_Not_Exists ex){
			cout << ex.what();
			return;
		}
	}

	//goto target file's parent dir
	//copied file has virtual location
	//therefore, set tmp_dir as root (default) and enter relevant dir
	if (copied_parent_dir.length() != 0){

		tmp_dir = const_cast<myFSEntry*>(root);
		ChangeDir(copied_parent_dir);

		try{//check if copied file already exists or not
			copied_file = (getFileEntry(Args[Arg2]));
		}
		catch (myFile_Not_Exists ex){
			copied_file=0;
		}
	}
	// copy virtual to virtual //
	if (parent_directory.length() != 0 && copied_parent_dir.length() != 0 && copied_file){
		//src file is virtual
		//trgt file is virtual and exists
		src_file->open(ios::in);
		char p;
		copied_file->open(ios::out | ios::trunc);
		for (unsigned i = 0; i < (src_file)->size(); i++){

			p =(*src_file)[i];
			(*copied_file)[i]=p;
		}
		copied_file->close();
		src_file->close();
	}
	else if (parent_directory.length() != 0 && copied_parent_dir.length() != 0 && !copied_file){
		vector<myFSEntry*>* content;
		//src file is virtual
		//trgt file is virtual and not exists
		string fullpath = tmp_dir->EntryPath() + tmp_dir->EntryName() + '/';
		newVal = new myFile(copied_filename);
		(newVal)->setPhysicalName(copied_filename);
		char p;
		string physical_file_name = (newVal)->getPhysicalName();
		newVal->open(ios::out | ios::trunc);
		src_file->open(ios::in);
		for (unsigned i = 0; i < (src_file)->size(); i++){
			p =(*src_file)[i];
			(*newVal)[i]=p;
		}
		newVal->close();
		src_file->close();
		newVal->changePath(fullpath);
		content = &dynamic_cast<myFolder*>(tmp_dir)->get_entries();
		content->push_back(newVal);
	}

	// copy real to virutal //

	else if (parent_directory.length() == 0 && copied_parent_dir.length() != 0 && copied_file){
		//src file is real
		//trgt file is virtual and exists
		rlocalfile.open(filename.c_str(),ios::in);
		wlocalfile.open(copied_file->getPhysicalName().c_str(),ios::trunc);
		string str;
		while(getline(rlocalfile,str))
			wlocalfile<<str;
		rlocalfile.close();
		wlocalfile.close();
	}

	else if (parent_directory.length() == 0 && copied_parent_dir.length() != 0 && !copied_file){
		vector<myFSEntry*>* content;
		//src file is real
		//trgt file is virtual and not exists
		string fullpath = tmp_dir->EntryPath() + tmp_dir->EntryName() + '/';
		newVal = new myFile(copied_filename);
		dynamic_cast<myFile*>(newVal)->setPhysicalName(copied_filename);
		string physical_file_name = dynamic_cast<myFile*>(newVal)->getPhysicalName();
		if (parent_directory.length() == 0){
			rlocalfile.open(filename.c_str(),ios::in);
			wlocalfile.open(physical_file_name.c_str());
			string str;
			while(getline(rlocalfile,str))
				wlocalfile<<str;
			rlocalfile.close();
			wlocalfile.close();
			newVal->changePath(fullpath);
			content = &dynamic_cast<myFolder*>(tmp_dir)->get_entries();
			content->push_back(newVal);
		}
	}

	// copy virtual to real //

	else if (parent_directory.length() != 0 && copied_parent_dir.length() == 0){
		//src file is virtual
		//trgt file is real and exists
		src_file->open(ios::in);
		wlocalfile.open(copied_filename.c_str(),ios::trunc);
		for (unsigned i = 0; i < (src_file)->size(); i++)
				wlocalfile << (*src_file)[i];
		wlocalfile.close();
		src_file->close();
	}

	//copy real to real //

	else{
		//src file is real
		//trgt file is real
		rlocalfile.open(filename.c_str());
		wlocalfile.open(copied_filename.c_str());
		string str;
		while(getline(rlocalfile,str))
			wlocalfile<<str;
		rlocalfile.close();
		wlocalfile.close();
	}
}
void myTerminal::REMOVE(const vector<string>& Args){

	string parent_directory = CDdotdot(Args[Arg1]);
	string filename = Args[Arg1].substr(parent_directory.length());

	if (parent_directory.length() == 0){
		if(remove(filename.c_str()) != 0 ){
			perror( "Error deleting file" );
		}
		else{
			puts( "File successfully deleted" );
		}
		return;
	}

	vector<myFSEntry*>* content = &dynamic_cast<myFolder*>(tmp_dir)->get_entries();
	vector<myFSEntry*>::iterator it = (content->begin());

	while (it != content->end()){

		if (typeid(**it) == typeid(myFile) && (*it)->EntryName() == filename){
			delete (*it);
			content->erase(it);
			return;
		}
		it++;
	}
	return;

}
void myTerminal::MOVE(const vector<string>& Args){

	string parent_directory_path = CDdotdot(Args[Arg1]);
	COPY(Args);
	tmp_dir = const_cast<myFSEntry*>(root);
	ChangeDir(parent_directory_path);
	REMOVE(Args);

}
void myTerminal::CAT(const vector<string>& Args)const{

	myFile* target_file = (getFileEntry(Args[Arg1]));
	target_file->open(ios::in|ios::out);
	target_file->cat();
	target_file->close();

}
void myTerminal::WC(const vector<string>& Args)const{

	myFile* target_file = (getFileEntry(Args[Arg1]));
	target_file->open(ios::in|ios::out);
	target_file->wc();
	target_file->close();

}

void myTerminal::RMDIR(const vector<string>& Args){

	string parent_directory = CDdotdot(Args[Arg1]);

	string folder_to_delete = Args[Arg1].substr(parent_directory.length());
	if (folder_to_delete.find('/') != string::npos)
		folder_to_delete.erase(folder_to_delete.find('/'));
	vector<myFSEntry*>* content = &dynamic_cast<myFolder*>(tmp_dir)->get_entries();
	vector<myFSEntry*>::iterator it = (content->begin());
	while (it != content->end()){
		string folder_name = (*it)->EntryName();
		if (folder_name == folder_to_delete){
			if (typeid(**it) == typeid(myFolder)){
				delete (*it);
				content->erase(it);
				break;
			}
			it++;
		}
	}
	return;
}
void myTerminal::LS(const vector<string>& Args)const{

	string parent_directory = CDdotdot(Args[Arg1]);
	
	string fullpath;

	string foldername = Args[Arg1].substr(parent_directory.length());

	fullpath = tmp_dir->EntryPath() + tmp_dir->EntryName() + '/';

	vector<myFSEntry*>* content = &dynamic_cast<myFolder*>(tmp_dir)->get_entries();

	vector<myFSEntry*>::iterator it = (content->begin());
	while (it != content->end())
	{
		if (typeid(**it) == typeid(myFolder) && (*it)->EntryName() == foldername){
			dynamic_cast<myFolder*>(*it)->ls();
			return;
		}
		it++;
	}
}
void myTerminal::LN(const vector<string>& Args){

	string parent_directory = CDdotdot(Args[Arg1]);
	string linked_directory_path = CDdotdot(Args[Arg2]);
	string filename = Args[Arg1].substr(parent_directory.length());
	string linked_filename = Args[Arg2].substr(linked_directory_path.length());
	//intended linked_file to be added
	myFSEntry* newVal=0;
	//content of source parent directory
	vector<myFSEntry*>* content = &dynamic_cast<myFolder*>(tmp_dir)->get_entries();
	//iterator starts from begin
	vector<myFSEntry*>::iterator it = (content->begin());
	//till end
	while (it != content->end())
	{
		//if is a file
		if (typeid(**it) == typeid(myFile) && (*it)->EntryName() == filename){
			//polymorphic usage
			myFile* morph = dynamic_cast<myFile*>(*it);
			morph->open(ios::in);
			//create new alias (using RC copy ctor(refCount++))
			newVal = morph->ln();
			morph->close();
			break;
		}
		it++;
	}

	tmp_dir = const_cast<myFSEntry*>(root);
	ChangeDir(linked_directory_path);

	//if alias is exist
	content = &dynamic_cast<myFolder*>(tmp_dir)->get_entries();
	it = (content->begin());
	while (it != content->end())
	{
	  if (typeid(**it) == typeid(myFile) && (*it)->EntryName() == linked_filename){
	    //remove it physically
			delete (*it);
	    //also from structure
		    content->erase(it);
		    break;
	  }
	  it++;
	}
	string fullpath = tmp_dir->EntryPath() + tmp_dir->EntryName() + '/';
	newVal->changePath(fullpath);
	//newVal instance created by copy ctor of myFile causing name duplication therefore
	newVal->changeName(linked_filename);
	content->push_back(newVal);
	return;
}
void myTerminal::CHDIR(const vector<string>& Args){

	string parent_directory = CDdotdot(Args[Arg1]);
	unsigned it=0;
	char delim='/';

	//parse enter directory
	string enter_dir=Args[Arg1].substr(parent_directory.length());

	string target_folder;

	while (it < enter_dir.length() && enter_dir.at(it) != delim){
		target_folder+=(enter_dir.at(it++));
	}

	tmp_dir = dynamic_cast<myFolder*>(tmp_dir)->chdir(target_folder);
	if (!tmp_dir)
		return;
	//update current_folder to tmp_dir, as main objective
	current_folder=tmp_dir;
	//update current path as well
	currentPath=current_folder->EntryPath() + current_folder->EntryName() + '/';

}
void myTerminal::ChangeDir(const string& path){ 

	//recursively enters directories from tmp_dir (default: root)

	//end of recursion*/
	if ( path.length() == 0) {
			return;
	}

	//parse enter directory and next directory
	unsigned it=0;
	char delim='/';
	string enter_dir;
	string next_dir;

	while (it < path.length() && path.at(it) != delim){
		enter_dir+=(path.at(it++));
	}

	//in case next dir has no delimiter end
	try{
		next_dir=path.substr(enter_dir.length()+1);
	}
	catch (...){ 
			next_dir=path.substr(enter_dir.length());
	}

	//set tmp_dir as enter directory
	tmp_dir = dynamic_cast<myFolder*>(tmp_dir)->chdir(enter_dir);
	//set path as enter directory path and name
	currentPath=tmp_dir->EntryPath() + tmp_dir->EntryName() + '/';
	//recursion call to next dir
	return ChangeDir(next_dir);

}
string myTerminal::CDdotdot(const string& path)const{ 

	//get full path of folder/file, return up_folder

	string up_folder;
	int delimCount = 0;
	int c=0;
	for (unsigned i = 0 ; i < path.length()-1; i++){
		if (path.at(i) == '/')
			delimCount++;
	}
	while (delimCount > 0){ 
		up_folder+=path.at(c);
		if (path.at(c++) == '/')
			delimCount--;
	}
	if (up_folder.size() < 1)
		return "";
	return up_folder;

}
void myTerminal::MAKEDIR(const vector<string>& Args){

	//parse target name
	string new_dir_name=Args[Arg1].substr(CDdotdot(Args[Arg1]).length());
	if (new_dir_name.find('/') != string::npos)
		new_dir_name.erase(new_dir_name.find('/'));
	if (new_dir_name.size() != 0){
		//mkdir direct from tmp folder
		dynamic_cast<myFolder*>(tmp_dir)->mkdir(new_dir_name);
		string path = tmp_dir->EntryPath() + tmp_dir->EntryName() + '/';
		//update new folder path
		dynamic_cast<myFolder*>(tmp_dir)->chdir(new_dir_name)->changePath(path);
	}

}
void myTerminal::PWD(const vector<string>& Args)const{

	cout << currentPath << endl;

}
void myTerminal::LPROOT(const vector<string>& Args)const{

	recursive_lproot(dynamic_cast<myFolder*>(const_cast<myFSEntry*>(root))->get_entries());

}
void myTerminal::recursive_lproot(const vector<myFSEntry*>& root_entries)const{

	vector<myFSEntry*>::const_iterator it = (root_entries.begin());
	while (it != root_entries.end()){
		if (typeid(**it) == typeid(myFile)){
			cout << (*it)->EntryName() + ' ' << dynamic_cast<myFile*>(*it)->getrefCount() << endl;
		}
		it++;
	}
	it = (root_entries.begin());
	while (it != root_entries.end()){
		if (typeid(**it) == typeid(myFolder)){
			cout  << (*it)->EntryPath() << (*it)->EntryName() << "/" << endl;
			recursive_lproot(dynamic_cast<myFolder*>(*it)->get_entries());
		}

		it++;
	}
	return;
}
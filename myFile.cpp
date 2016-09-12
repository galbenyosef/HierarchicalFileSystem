#include "myFile.h"

myFile::myFileValue::myFileValue():refCount(1){

}
myFile::myFileValue::~myFileValue(){

    delete data;

}
myFile::myFile(const string& initValue):myFSEntry(initValue),value(new myFileValue){
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer,80,"_%d-%m-%y_%H-%M-%S",timeinfo);
	
	string orig_name(initValue);
	string dateformat(buffer);
	string extension(".myvFile");
	string physical_name = orig_name+=dateformat+=extension;

	value->data=new fstream(physical_name.c_str());
	value->file_name=physical_name;
	value->data->close();
}

int myFile::getrefCount() const
{return value->refCount;}
string& myFile::getPhysicalName() const
{ return value->file_name;}
void myFile::setPhysicalName(const string& new_name) 
{ value->file_name=new_name;}

myFile::myFile(const myFile& rhs) :myFSEntry(rhs.name), value(rhs.value){

	++value->refCount;

}
myFile& myFile::operator=(const myFile& rhs){

	if (value == rhs.value)
		return *this;
	if (--value->refCount == 0)
		delete value;
	value = rhs.value;
	++value->refCount;
	return *this;

}
myFile::~myFile(){

    if(--value->refCount == 0){
		rm();
        delete value;
	}
}

char myFile::operator[] (fstream::pos_type i) const{

		value->data->seekg(0,ios::end);
		int end = (int)value->data->tellg();
		if (i < 0 || i > end)
			throw Bad_Index(i);
		value->data->seekg(i,ios::beg);
		return char(value->data->peek());
}
void myFile::open(ios_base::openmode mode)const{

		value->data->open(this->value->file_name.c_str(), mode);

}
void myFile::close()const{

	value->data->close();

}
void myFile::touch(){

	flush(*value->data);

}
void myFile::rm(){

	if( remove((this)->getPhysicalName().c_str()) != 0 )
		perror( "Error deleting file" );
	else
		puts( "File successfully deleted" );

}
myFile* myFile::copy(const string& dst)const{

		myFile* copied_object = new myFile(dst);
		ofstream out(copied_object->getPhysicalName().c_str());
		string str;
		while(getline(*(value->data),str)){
			out<<str;
		}
		out.close();
		return copied_object;
}
myFile* myFile::ln()const{

		myFile* alias = new myFile(*this);
		return alias;

}
void myFile::cat() const{

    string line;

	while(getline( *(value->data),line))
		cout << line << endl;

}
void myFile::wc() const {

    string line;
    int c,l,w;
    l=c=w=0;

    value->data->clear();
    value->data->seekg(0, ios::beg);
    istream_iterator<string> in(*(value->data)), end;

    w = distance(in, end);

    value->data->clear();
    value->data->seekg(0, ios::beg);

    while( getline ( *(value->data), line ) ){
        c += line.length();
        l++;
    }

    cout << endl << "char: "<< c;
    cout << ", word: "<< w;
    cout << ", line: "<< l << endl;

	return;
}
unsigned myFile::size(){

	unsigned retval;

	ifstream file(this->getPhysicalName().c_str());
	file.seekg(0,ios::end);
	retval = (unsigned)file.tellg();

	return retval;
}

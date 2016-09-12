#ifndef MYFILE_H_
#define MYFILE_H_

#include<fstream>
#include<cstdio>//introducing remove()
#include<ctime>//introducing physical file names
#include<iterator>

#include "myFSEntry.h"

using namespace std;

class myFile : public myFSEntry{

public:

	myFile(const string& initValue); // default c'tor
	myFile(const myFile&); // copy c'tor introducing reference counter
	myFile& operator= (const myFile&);
	~myFile();

	//fstream casing
	class CharProxy{

	public:

		CharProxy(fstream &file, fstream::pos_type pos) : file_(file), pos_(pos) {}
		CharProxy& operator=(char c){
			file_.seekg(0,ios::end);
			int end = (int)file_.tellg();
			if (pos_ < 0 || pos_ > end)
				throw Bad_Index(pos_);
			file_.seekp(pos_);
			file_.put(c);
			return *this;
		}
		operator char(){
			file_.seekg(pos_);
			return char(file_.get());
		}

	private:

		fstream& file_;
		fstream::pos_type pos_;

	};
	//out of range index within file(read/write) exception
	class Bad_Index : public exception {

	public:

		Bad_Index(const fstream::pos_type& i):itsIndex(i){}

		virtual const char* what() const throw(){

			return "Error";
		}

	private:

		const fstream::pos_type itsIndex;

	};
	
	//read
	char operator[] (fstream::pos_type i) const;
	//write
	CharProxy operator[](fstream::pos_type i)  {
		if (value->refCount > 1) {
		// the value is shared by other instances
		//--value->refCount;
		//value = new myFileValue();
		}

		return CharProxy(*(value->data),i);
	}

	//open by physical file name
	void open(ios_base::openmode)const;
	void close()const;

	//characters counter
	unsigned size();

	void touch();
	myFile* copy(const string&)const;
	void rm();
    void cat() const;
	void wc() const;
	myFile* ln()const;

	int getrefCount() const;
	string& getPhysicalName() const;
	void setPhysicalName(const string&);

private:

	struct myFileValue{
		int refCount;
		string file_name;
		fstream* data;
		myFileValue();
		~myFileValue();
	};

	myFileValue* value;
};

#endif
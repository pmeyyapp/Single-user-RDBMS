#include "pf.h"
#include <iostream>
#include<fstream>



PF_Manager::PF_Manager()
{
}


PF_Manager::~PF_Manager()
{
}

    
RC PF_Manager::CreateFile(const char *fileName)
{
	 fstream myfile;
	 ifstream::pos_type size;
	  myfile.open (fileName, ios::in | ios::binary );
	  if(myfile.is_open())
	  {
		  return 2;

	  }
	  else
	  {
		  myfile.open (fileName, ios::out | ios::binary );
		  myfile.close();
		  return 0;
	  }

}


RC PF_Manager::DestroyFile(const char *fileName)
{
	remove(fileName);
    return -1;
}


RC PF_Manager::OpenFile(const char *fileName, PF_FileHandle &fileHandle)
{

	fileHandle.fh1.open(fileName,ios::in | ios::out | ios::binary);
    return -1;
}


RC PF_Manager::CloseFile(PF_FileHandle &fileHandle)
{
	fileHandle.fh1.close();
    return -1;
}


PF_FileHandle::PF_FileHandle()
{

}
 

PF_FileHandle::~PF_FileHandle()
{
}


RC PF_FileHandle::ReadPage(PageNum pageNum, void *data)
{
	int num=0;
	num=pageNum*PF_PAGE_SIZE;
	fh1.seekp(num);
	char s[PF_PAGE_SIZE];
	fh1.read((char*)data,PF_PAGE_SIZE);
	return -1;
}


RC PF_FileHandle::WritePage(PageNum pageNum,const void *data)
{
		int num=pageNum*PF_PAGE_SIZE;
		fh1.seekp(num);
		fh1.write((char *)data,PF_PAGE_SIZE);
		return -1;
}


RC PF_FileHandle::AppendPage(const void *data)
{
    fh1.seekp(0,ios::end);
    fh1.write((char *)data,PF_PAGE_SIZE);
	return -1;
}


unsigned PF_FileHandle::GetNumberOfPages()
{

	fh1.seekp(0,ios::end);
	int fsize= fh1.tellp();
	unsigned pages=fsize/PF_PAGE_SIZE;
	return pages;

}



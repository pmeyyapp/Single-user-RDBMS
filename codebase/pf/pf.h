#ifndef _pf_h_
#define _pf_h_

using namespace std;

typedef int RC;
typedef unsigned PageNum;

#include <fstream>

#define PF_PAGE_SIZE 4096

class PF_FileHandle;

class PF_Manager
{
public:
    PF_Manager();                                                       // Constructor
    ~PF_Manager   ();                                                   // Destructor
    
    RC CreateFile    (const char *fileName);                            // Create a new file
    RC DestroyFile   (const char *fileName);                            // Destroy a file
    RC OpenFile      (const char *fileName, PF_FileHandle &fileHandle); // Open a file
    RC CloseFile     (PF_FileHandle &fileHandle);                       // Close a file
};


class PF_FileHandle
{
public:
	fstream fh1;
    PF_FileHandle();                                                    // Default constructor
    ~PF_FileHandle();                                                   // Destructor


    RC ReadPage(PageNum pageNum, void *data);                           // Get a specific page
    RC WritePage(PageNum pageNum, const void *data);                    // Write a specific page
    RC AppendPage(const void *data);                                    // Append a specific page
    unsigned GetNumberOfPages();                                        // Get the number of pages in the file

 };
 
 #endif


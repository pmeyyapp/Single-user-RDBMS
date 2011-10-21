
#ifndef _ix_h_
#define _ix_h_

#include <vector>

#include "../pf/pf.h"
#include "../rm/rm.h"

using namespace std;

class IX_IndexHandle;

class IX_Manager {
 public:
  IX_Manager   ();                             // Constructor
  ~IX_Manager  ();                             // Destructor

  int open;

  RC CreateIndex(const char *tableName,      // create new index
		 const char *attributeName);
  RC DestroyIndex(const char *tableName,      // destroy an index
		  const char *attributeName);
  RC OpenIndex(const char *tableName,      // open an index
	       const char *attributeName,
	       IX_IndexHandle &indexHandle);
  RC CloseIndex(IX_IndexHandle &indexHandle); // close index

  RM rm1;
  void setRM(RM &rm);
};


class IX_IndexHandle {
 public:
	fstream fp;
	char tname[100];
	char att_name[100];
	int att_type,att_length;
	int leafnodesize;
	int nonleafnodesize;
	int maxentries_l,maxentries_nl;
	int done;

  IX_IndexHandle  ();                           // Constructor
  ~IX_IndexHandle ();                           // Destructor

  // The following two functions are using the following
  // format for the passed key value.
  //
  //    For int and real: use 4 bytes
  //    For char and varchar: use 4 bytes for the length followed by
  //                          the characters
  RC InsertEntry(void *key, const RID &rid);  // Insert new index entry
  RC DeleteEntry(void *key, const RID &rid);  // Delete index entry
  RC insert(int pgnode,void *key, const RID &rid,char newchild[],int &newnodeptr);
  RC deletekey(int pgnode,void *key, const RID &rid,int &flag);

};


class IX_IndexScan {
 public:
  IX_IndexScan();  // Constructor
  ~IX_IndexScan(); // Destructor

  char key[100],val[100];
  RID ridscan;

  int scan,ival;
  float fval;

  char RECORDS[PF_PAGE_SIZE],att_name1[100];
  int sibling,no_entry,curr,size;
  int att_type1,att_len;
  CompOp op;

  // for the format of "value", please see IX_IndexHandle::InsertEntry()
  RC OpenScan(const IX_IndexHandle &indexHandle, // Initialize index scan
	      CompOp      compOp,
	      void        *value);           

  RC GetNextEntry(RID &rid);  // Get next matching entry
  RC CloseScan();             // Terminate index scan
  RC scanfirstpage(int pgnode,int &lpage,const IX_IndexHandle &indexHandle);
  void Iterate(int &flag);
};

// print out the error message for a given return code
void IX_PrintError (RC rc);

int search(char PGBUFF[],int no_entries,void *newkey,int att_length,int att_type);
int nonleafsearch(char PGBUFF[],int no_entries,void *newkey,int att_length,int att_type);
int delsearch(char PGBUFF[],int no_entries,void *newkey,int att_length,int att_type);


#endif

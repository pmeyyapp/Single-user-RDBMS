#ifndef _rm_h_
#define _rm_h_

#include <vector>
#include "../pf/pf.h"

using namespace std;

// Record ID
typedef struct
{
  unsigned pageNum;
  unsigned slotNum;
}RID;


typedef enum { TypeInt = 0, TypeReal, TypeChar, TypeVarChar } AttrType;

typedef unsigned AttrLength;

typedef enum { EQ_OP = 0,  // =
	       LT_OP,      // <
	       GT_OP,      // >
	       LE_OP,      // <=
	       GE_OP,      // >=
	       NE_OP,      // !=
	       NO_OP       // no condition
} CompOp;


struct Attribute {
    string   name;     // attribute name
    AttrType type;     // attribute type
    AttrLength length; // attribute length
    friend ostream &operator<<(ostream &s, const Attribute &attr)
    {
    	s<<"Attribute Name"<<attr.name<<endl;
    	s<<"Attribute Type"<<attr.type<<endl;
    	s<<"Attribute Length"<<attr.length<<endl;
    }
};

// an iteratr to go through the records in a scan operator
// The way to use it is something like:
//
//  RM_ScanIterator rmScanIterator;
//  rm.open(..., rmScanIterator);
//  while (rmScanIterator(rid, data) != RM_EOF) {
//    process the data;
//  }
//  rmScanIterator.close();
//

# define RM_EOF (-1)  // end of a scan operator


class RM_ScanIterator
{
public:
RM_ScanIterator() ;
~RM_ScanIterator() ;

  // "data" is using the same format as RM.insertTuple()

  int pslots[PF_PAGE_SIZE];
  int p[100];
  int no_pages;
  int no_rec_in_page[100];
  vector<char*>attnames;
  vector<RID> rid;
  int end1;
  char tname[100];
  RID nrid;



  RC getNextTuple(RID &rid, void *data);
  RC close();

private:
};

// record manager
class RM
{
public:
  RM(); // create a record manager on a specified disk

  ~RM();

  int numofattr;

  RC createTable(const char *tableName, 
		 const vector<char *> &attributeNames,
		 const vector<AttrType> &attributeTypes,
		 const vector<AttrLength> &attributeLengths);



  RC find_length(const char *tableName,int b,RID &rid);
  RC deleteTable(const char *tableName);


  // format of passed data:
  //  concatenation of values of the attributes
  //    For int and real: use 4 bytes
  //    For char and varchar: use 4 bytes for the length followed by
  //                          the characters
  // The same format is used for the returned data of readTuple()
  RC insertTuple(const char *tableName, const void *data, RID &rid);

  RC readTuple(const char *tableName, const RID &rid, void *data);

  RC readAttribute(const char *tableName, const RID &rid, const char *attributeName, void *data);

  RC readAttribute(const char *tableName, const RID &rid, const char *attributeName,int &b, void *data);

  RC deleteTuple(const char *tableName, const RID &rid);

  RC deleteTuples(const char *tableName);

  // assuming the rid does not change
  RC updateTuple(const char *tableName, const void *data, const RID &rid);

  RC reorganizePage(const char *tableName, const unsigned pageNumber);

  // Newly Added function for Project 3
  RC getAttributes(const char *tableName,
        vector<Attribute> &attrs);

  // it returns an iterator to allow the caller to go through the
  // results one by one. 
  RC scan(const char *tableName, 
	  const vector<char *> &attributeNames, // a list of projected attributes
	  RM_ScanIterator &rm_ScanIterator);


// extra credit
public:
  RC dropAttribute(const char *tableName, const char *attributeName);

  RC addAttribute(const char *tableName, 
		  const char *attributeName,
		  const AttrType attributeType,
		  const AttrLength attributeLength);

  RC reorganizeTable(const char *tableName);

  // it returns an iterator to allow the caller to go through the
  // results one by one with a predicate (e.g., "salary > 50,000")
  RC scan(const char *tableName,
	  const char *conditionAttribute,
	  const CompOp compOp, // comparision type such as "<" and "="
	  const void *value, // used in the comparison
	  const vector<char *> &attributeNames, // a list of projected attributes
	  RM_ScanIterator &rm_ScanIterator);


private:

};


#endif

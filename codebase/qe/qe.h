#ifndef _qe_h_
#define _qe_h_

#include <vector>
#include <iostream>
#include <cstring>

#include "../pf/pf.h"
#include "../rm/rm.h"
#include "../ix/ix.h"

using namespace std;

typedef enum{ MIN = 0, MAX, SUM, AVG, COUNT } AggregateOp;


// The following functions are using the following 
// format for the passed data.
//    For int and real: use 4 bytes
//    For char and varchar: use 4 bytes for the length followed by
//                          the characters

struct Value {
    AttrType type;          // type of value               
    void     *data;         // value                       
    friend ostream &operator<<(ostream &s, const Value &v)
    {
    	char buff[100],temp[100];
    	s<<"Value type :"<<v.type<<endl;
    	if(v.type==TypeInt)
    	{
    	memset(buff,0,100);
    	memcpy(buff,v.data,sizeof(buff));
    	s<<"Data :"<<*(int*)&buff<<endl;
    	}

    	else if(v.type==TypeReal)
    	{
    	memset(buff,0,100);
    	memcpy(buff,v.data,sizeof(buff));
    	s<<"Data :"<<*(float*)&buff<<endl;
    	}


    	else
    	{
    	memset(buff,0,100);
    	memcpy(buff,v.data,sizeof(buff));
    	memset(temp,0,100);
    	memcpy(temp,buff,4);
    	s<<"Length  : "<<*(int*)&temp<<endl;
    	s<<"Data :"<<buff<<endl;
    	}


    }
};


struct Condition {
    string lhsAttr;         // left-hand side attribute                     
    CompOp  op;             // comparison operator                          
    bool    bRhsIsAttr;     // TRUE if right-hand side is an attribute and not a value
    string rhsAttr;         // right-hand side attribute if bRhsIsAttr = TRUE
    Value   rhsValue;       // right-hand side value if bRhsIsAttr = FALSE
    friend ostream &operator<<(ostream &s, const Condition &c)
    {
    	s<<"Left-hand side attribute  :"<<c.lhsAttr<<endl;
    	s<<"comparison operator :"<<c.op<<endl;
    	s<<"Value or not :"<<c.bRhsIsAttr<<endl;
    	s<<"Right-hand side attribute :"<<c.rhsAttr<<endl;
    	s<<c.rhsValue<<endl;
    }
};


class Iterator {
    // All the relational operators and access methods are iterators.
    public:
        virtual RC getNextTuple(void *data) = 0;
        virtual void getAttributes(vector<Attribute> &attrs) const = 0;
        virtual ~Iterator() {};
};


class TableScan : public Iterator
{
    // A wrapper inheriting Iterator over RM_ScanIterator
    public:
        RM &rm;
        RM_ScanIterator *iter;
        string tablename;
        vector<Attribute> attrs;
        vector<char *> attrNames;
        
        TableScan(RM &rm, const char *tablename, const char *alias = NULL):rm(rm)
        {
            // Get Attributes from RM
            rm.getAttributes(tablename, attrs);
            
            // Get Attribute Names from RM
            unsigned i;
            for(i = 0; i < attrs.size(); ++i)
            {
                // convert to char *
                attrNames.push_back(const_cast<char *>(attrs[i].name.c_str()));

            }
            // Call rm scan to get iterator
            iter = new RM_ScanIterator();
            rm.scan(tablename, attrNames, *iter);
            
            // Store tablename
            this->tablename = string(tablename);
            if(alias) this->tablename = string(alias);
        };
       
        // Start a new iterator given the new compOp and value
        void setIterator()
        {
            iter->close();
            delete iter;
            iter = new RM_ScanIterator();
            rm.scan(tablename.c_str(), attrNames, *iter);
        };
       
        RC getNextTuple(void *data)
        {
            RID rid;
            return iter->getNextTuple(rid, data);
        };
        
        void getAttributes(vector<Attribute> &attrs) const
        {
            attrs.clear();
            attrs = this->attrs;
            unsigned i;
            
            // For attribute in vector<Attribute>, name it as rel.attr
            for(i = 0; i < attrs.size(); ++i)
            {
                string tmp = tablename;
                tmp += ".";
                tmp += attrs[i].name;
                attrs[i].name = tmp;
            }
        };
        
        ~TableScan() 
        {
            iter->close();
        };
};


class IndexScan : public Iterator
{
    // A wrapper inheriting Iterator over IX_IndexScan
    public:
        RM &rm;
        IX_IndexScan *iter;
        IX_IndexHandle &handle;
        string tablename;
        vector<Attribute> attrs;
        
        IndexScan(RM &rm,IX_IndexHandle &indexHandle, const char *tablename, const char *alias = NULL):rm(rm),handle(indexHandle)
        {
            // Get Attributes from RM
            rm.getAttributes(tablename, attrs);
                     
            // Store tablename
            this->tablename = string(tablename);
            if(alias) this->tablename = string(alias);
            
            // Store Index Handle
            iter = NULL;
            //this->handle = indexHandle;
        };
       
        // Start a new iterator given the new compOp and value
        void setIterator(CompOp compOp, void *value)
        {
            if(iter != NULL)
            {
                iter->CloseScan();
                delete iter;
            }
            iter = new IX_IndexScan();

            iter->OpenScan(handle, compOp, value);
        };
       
        RC getNextTuple(void *data)
        {
            RID rid;
            int rc = iter->GetNextEntry(rid);
            //cout<<"outside if"<<endl;
            if(rc == 0)
            {
                rc = rm.readTuple(tablename.c_str(), rid, data);
               // cout<<"innsidde if"<<endl;
            }
            return rc;
        };
        
        void getAttributes(vector<Attribute> &attrs) const
        {
            attrs.clear();
            attrs = this->attrs;
            unsigned i;

            // For attribute in vector<Attribute>, name it as rel.attr
            for(i = 0; i < attrs.size(); ++i)
            {
                string tmp = tablename;
                tmp += ".";
                tmp += attrs[i].name;
                attrs[i].name = tmp;
            }
        };
        
        ~IndexScan() 
        {
            iter->CloseScan();
        };
};


class Filter : public Iterator {
    // Filter operator
    public:
			Iterator *it2;
			Condition filcond;
			vector<Attribute> filter_attr;

        Filter(Iterator *input,                         // Iterator of input R
               const Condition &condition               // Selection condition 
        );
        ~Filter();
        
        RC getNextTuple(void *data);
        // For attribute in vector<Attribute>, name it as rel.attr
        void getAttributes(vector<Attribute> &attrs) const;
};


class Project : public Iterator {
    // Projection operator
    public:
	Iterator *it1;
	vector<string> att_name;
	vector<Attribute> proj_attr,curr_attr;
	int ipsize;

        Project(Iterator *input,                            // Iterator of input R
                const vector<string> &attrNames);           // vector containing attribute names
        ~Project();
        
        RC getNextTuple(void *data);
        // For attribute in vector<Attribute>, name it as rel.attr
        void getAttributes(vector<Attribute> &attrs) const;
};


class NLJoin : public Iterator {
    // Nested-Loop join operator
    public:
			Iterator *left_it;
			TableScan *right_it;
			vector<Attribute> left_attr,right_attr;
			vector<Attribute>combined;
			AttrType atype;
			int flag;
			Condition joincd;
			char lbuff[100],rbuff[100],lval[100],rval[100];
			int left_length,right_length;

        NLJoin(Iterator *leftIn,                             // Iterator of input R
               TableScan *rightIn,                           // TableScan Iterator of input S
               const Condition &condition                    // Join condition
        );
        ~NLJoin();
        
        RC getNextTuple(void *data);
        // For attribute in vector<Attribute>, name it as rel.attr
        void getAttributes(vector<Attribute> &attrs) const;
};


class INLJoin : public Iterator {
    // Index Nested-Loop join operator
    public:

				Iterator *left_it2;
				IndexScan *right_it2;
				vector<Attribute> left_attr2,right_attr2;
				vector<Attribute>indexjoin;
				AttrType atype2;
				int flag2;
				Condition indexcd;
				char lbuff2[100],rbuff2[100],lval2[100],rval2[100];
				int left_length2,right_length2;

        INLJoin(Iterator *leftIn,                               // Iterator of input R
                IndexScan *rightIn,                             // IndexScan Iterator of input S
                const Condition &condition                      // Join condition
        );
        
        ~INLJoin();

        RC getNextTuple(void *data);
        // For attribute in vector<Attribute>, name it as rel.attr
        void getAttributes(vector<Attribute> &attrs) const;
};


class HashJoin : public Iterator {
    // Hash join operator
    public:
        HashJoin(Iterator *leftIn,                                // Iterator of input R
                 Iterator *rightIn,                               // Iterator of input S
                 const Condition &condition                       // Join condition
        );
        
        ~HashJoin();

        RC getNextTuple(void *data);
        // For attribute in vector<Attribute>, name it as rel.attr
        void getAttributes(vector<Attribute> &attrs) const;
};


class Aggregate : public Iterator {
    // Aggregation operator
    public:
		Iterator *it5;
		Attribute ag;
		AggregateOp op1;
		vector<Attribute> agg_attr;

		char val[100],buff[100],mnac[100];
		int num;
		float fnum;

		int imin,imax,isum,iavg,icount;
		float fmin,fmax,fsum,favg,fcount;


        Aggregate(Iterator *input,                              // Iterator of input R
                  Attribute aggAttr,                            // The attribute over which we are computing an aggregate
                  AggregateOp op                                // Aggregate operation
        );

        // Extra Credit
        Aggregate(Iterator *input,                              // Iterator of input R
                  Attribute aggAttr,                            // The attribute over which we are computing an aggregate
                  Attribute gAttr,                              // The attribute over which we are grouping the tuples
                  AggregateOp op                                // Aggregate operation
        );     

        ~Aggregate();
        
        RC getNextTuple(void *data);
        // Please name the output attribute as aggregateOp(aggAttr)
        // E.g. Relation=rel, attribute=attr, aggregateOp=MAX
        // output attrname = "MAX(rel.attr)"
        void getAttributes(vector<Attribute> &attrs) const;
};

#endif


#include "qe.h"
#include <vector>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <cstdio>



//PROJECT
Project::Project(Iterator *input, const vector<string> &attrNames):it1(input)
{
int k,i;
att_name=attrNames;
input->getAttributes(proj_attr);
ipsize=att_name.size();
k=0;

while(k<ipsize)
{
for(i=0;i<proj_attr.size();i++)
{
	 if(proj_attr[i].name== att_name[k])
	 {
		 curr_attr.insert(curr_attr.end(),proj_attr[i]);
		 break;
	 }
}
k++;
}


}

Project::~Project()
{

}

RC Project::getNextTuple(void *data)
{
	char proj_data[100],buff[100],temp[100];
	int rc,k;
	int i,bytestomove,bytestoread,t1,track;
	memset(proj_data,0,sizeof(proj_data));
	memset(buff,0,sizeof(buff));


rc=it1->getNextTuple(buff);


i=0;
track=0;
while(i<ipsize)
{
	bytestomove=0;
	bytestoread=0;
	 for(int j=0;j<proj_attr.size();j++)
   	 {
		 if(att_name[i]==proj_attr[j].name)
		 {
			 if(proj_attr[j].type==TypeInt || proj_attr[j].type==TypeReal)
			 {
				bytestoread=4;
			 }

			 if(proj_attr[j].type==TypeChar || proj_attr[j].type==TypeVarChar)
			 {
				 memset(temp,0,sizeof(temp));
				 memcpy(temp,buff+bytestomove,4);
				 t1=*(int*)&temp;
				 bytestoread=t1+4;

			 }

			 break;
		 }
		 else
		 {
			 if(proj_attr[j].type==TypeInt || proj_attr[j].type==TypeReal)
			 {
				 bytestomove=bytestomove+4;
			 }
			 if(proj_attr[j].type==TypeChar || proj_attr[j].type==TypeVarChar)
			 {
				 memset(temp,0,sizeof(temp));
				 memcpy(temp,buff+bytestomove,4);
				 t1=*(int*)&temp;

				 bytestomove=bytestomove+4+t1;
			 }
		 }

   	 }

	 memcpy(proj_data+track,buff+bytestomove,bytestoread);
	 track=track+bytestoread;


	 i++;
}


memcpy((char*)data,proj_data,sizeof(proj_data));
return rc;
return 0;
}

void Project::getAttributes(vector<Attribute> &attrs) const
{
attrs.clear();

attrs=curr_attr;
}




//FILTER
Filter::Filter(Iterator *input,const Condition &condition):it2(input)
{
filcond=condition;
input->getAttributes(filter_attr);
}

Filter::~Filter()
{

}

RC Filter::getNextTuple(void *data)
{
	int rc,bytestomove,bytestoread,t1,num,rval;
	float num1,rval1;
	char buff[100],val[100],temp[100],left[100];
	char temp1[100],temp2[100];
	int x,y;


l1:;
memset(buff,0,sizeof(buff));
memset(left,0,sizeof(left));
memset(val,0,sizeof(val));

rc=it2->getNextTuple(buff);
if(rc==RM_EOF)
{
	return RM_EOF;
}
bytestoread=0;
bytestomove=0;

for(int j=0;j<filter_attr.size();j++)
{

	 if(filcond.lhsAttr==filter_attr[j].name)
	 {
		 if(filter_attr[j].type==TypeInt || filter_attr[j].type==TypeReal)
		 {
			bytestoread=4;
		 }

		 if(filter_attr[j].type==TypeChar || filter_attr[j].type==TypeVarChar)
		 {
			 memset(temp,0,sizeof(temp));
			 memcpy(temp,buff+bytestomove,4);
			 t1=*(int*)&temp;
			 bytestoread=t1+4;

		 }

		 break;
	 }
	 else
	 {
		 if(filter_attr[j].type==TypeInt || filter_attr[j].type==TypeReal)
		 {
			 bytestomove=bytestomove+4;
		 }
		 if(filter_attr[j].type==TypeChar || filter_attr[j].type==TypeVarChar)
		 {
			 memset(temp,0,sizeof(temp));
			 memcpy(temp,buff+bytestomove,4);
			 t1=*(int*)&temp;
			 bytestomove=bytestomove+4+t1;
		 }
	 }
}

memcpy(left,buff+bytestomove,bytestoread);
memcpy(val,filcond.rhsValue.data,sizeof(val));




if(filcond.op==GT_OP)
{
	 switch(filcond.rhsValue.type)
	 {
	 case TypeInt:
		 num=*(int*)&left;
		 memcpy(val,filcond.rhsValue.data,4);
		 rval=*(int*)&val;
		 if(num>rval)
		 {
			 memcpy(data,buff,sizeof(buff));

		 }
		 else
		 {
			 goto l1;
		 }
		 break;

	 case TypeReal:

				  num1=*(float*)&left;
				 memcpy(val,filcond.rhsValue.data,4);
				 rval1=*(float*)&val;
				 if(num1>rval1)
				 {
					 memcpy(data,buff,sizeof(buff));

				 }
				 else
				 {
					 goto l1;
				 }
				 break;

	 }
}


if(filcond.op==GE_OP)
{
	 switch(filcond.rhsValue.type)
	 {
	 case TypeInt:
		  num=*(int*)&left;

		 memcpy(val,filcond.rhsValue.data,4);
		 rval=*(int*)&val;
		 if(num>=rval)
		 {
			 memcpy(data,buff,sizeof(buff));

		 }
		 else
		 {
			 goto l1;
		 }
		 break;

	 case TypeReal:
				  num1=*(float*)&left;
				 memcpy(val,filcond.rhsValue.data,4);
				 rval1=*(float*)&val;
				 if(num1>=rval1)
				 {
					 memcpy(data,buff,sizeof(buff));

				 }
				 else
				 {
					 goto l1;
				 }
				 break;

	 }
}

if(filcond.op==LT_OP)
{
	 switch(filcond.rhsValue.type)
	 {
	 case TypeInt:
		 num=*(int*)&left;

		 memcpy(val,filcond.rhsValue.data,4);
		 rval=*(int*)&val;
		 if(num<rval)
		 {
			 memcpy(data,buff,sizeof(buff));

		 }
		 else
		 {
			 goto l1;
		 }
		 break;

	 case TypeReal:
		 	 num1=*(float*)&left;

				 memcpy(val,filcond.rhsValue.data,4);
				 rval1=*(float*)&val;
				 if(num1<rval1)
				 {
					 memcpy(data,buff,sizeof(buff));

				 }
				 else
				 {
					 goto l1;
				 }
				 break;

	 }
}

if(filcond.op==LE_OP)
{
	 switch(filcond.rhsValue.type)
	 {
	 case TypeInt:
		 num=*(int*)&left;

		 memcpy(val,filcond.rhsValue.data,4);
		 rval=*(int*)&val;
		 if(num<=rval)
		 {
			 memcpy(data,buff,sizeof(buff));

		 }
		 else
		 {
			 goto l1;
		 }
		 break;

	 case TypeReal:
				 num1=*(float*)&left;

				 memcpy(val,filcond.rhsValue.data,4);
				 rval1=*(float*)&val;
				 if(num1<=rval1)
				 {
					 memcpy(data,buff,sizeof(buff));

				 }
				 else
				 {
					 goto l1;
				 }
				 break;

	 }
}

if(filcond.op==NE_OP)
{
	 switch(filcond.rhsValue.type)
	 {
	 case TypeInt:
		  num=*(int*)&left;

		 memcpy(val,filcond.rhsValue.data,4);
		 rval=*(int*)&val;
		 if(num!=rval)
		 {
			 memcpy(data,buff,sizeof(buff));

		 }
		 else
		 {
			 goto l1;
		 }
		 break;

	 case TypeReal:
				  num1=*(float*)&left;

				 memcpy(val,filcond.rhsValue.data,4);
				 rval1=*(float*)&val;
				 if(num1!=rval1)
				 {
					 memcpy(data,buff,sizeof(buff));

				 }
				 else
				 {
					 goto l1;
				 }
				 break;

	 case TypeChar:

		 memcpy(val,filcond.rhsValue.data,sizeof(val));
  	 memset(temp1,0,sizeof(temp1));
 	 memcpy(temp1,left,4);
 	 x=*(int*)&temp1;
 	 memset(temp1,0,sizeof(temp1));
 	 memcpy(temp1,left+4,x);

 	 memset(temp2,0,sizeof(temp2));
 	 memcpy(temp2,val,4);
 	 y=*(int*)&temp2;
 	 memset(temp2,0,sizeof(temp2));
 	 memcpy(temp2,val+4,y);

 	 if(strcmp(temp1,temp2)!=0)
 	 {

 		memcpy(data,buff,sizeof(buff));
 	 }
 	 else
		 {
			 goto l1;
		 }
		 break;

		case TypeVarChar:

			memcpy(val,filcond.rhsValue.data,sizeof(val));
  	 memset(temp1,0,sizeof(temp1));
 	 memcpy(temp1,left,4);
 	 x=*(int*)&temp1;
 	 memset(temp1,0,sizeof(temp1));
 	 memcpy(temp1,left+4,x);

 	 memset(temp2,0,sizeof(temp2));
 	 memcpy(temp2,val,4);
 	 y=*(int*)&temp2;
 	 memset(temp2,0,sizeof(temp2));
 	 memcpy(temp2,val+4,y);

 	 if(strcmp(temp1,temp2)!=0)
 	 {

 		memcpy(data,buff,sizeof(buff));
 	 }
 	 else
		 {
			 goto l1;
		 }
		 break;

	 }
}

if(filcond.op==EQ_OP)
{
	 switch(filcond.rhsValue.type)
	 {
	 case TypeInt:
		 num=*(int*)&left;

		 memcpy(val,filcond.rhsValue.data,4);
		 rval=*(int*)&val;
		 if(num==rval)
		 {
			 memcpy(data,buff,sizeof(buff));

		 }
		 else
		 {
			 goto l1;
		 }
		 break;

	 case TypeReal:
				 num1=*(float*)&left;
				 memcpy(val,filcond.rhsValue.data,4);
				 rval1=*(float*)&val;
				 if(num1==rval1)
				 {
					 memcpy(data,buff,sizeof(buff));

				 }
				 else
				 {
					 goto l1;
				 }
				 break;


	 case TypeChar:
		 	 memcpy(val,filcond.rhsValue.data,sizeof(val));
		  	 memset(temp1,0,sizeof(temp1));
		 	 memcpy(temp1,left,4);
		 	 x=*(int*)&temp1;
		 	 memset(temp1,0,sizeof(temp1));
		 	 memcpy(temp1,left+4,x);

		 	 memset(temp2,0,sizeof(temp2));
		 	 memcpy(temp2,val,4);
		 	 y=*(int*)&temp2;
		 	 memset(temp2,0,sizeof(temp2));
		 	 memcpy(temp2,val+4,y);

		 	 if(strcmp(temp1,temp2)==0)
		 	 {

		 		memcpy(data,buff,sizeof(buff));
		 	 }
		 	 else
				 {
					 goto l1;
				 }
				 break;


		case TypeVarChar:

			memcpy(val,filcond.rhsValue.data,sizeof(val));
  	 memset(temp1,0,sizeof(temp1));
 	 memcpy(temp1,left,4);
 	 x=*(int*)&temp1;
 	 memset(temp1,0,sizeof(temp1));
 	 memcpy(temp1,left+4,x);

 	 memset(temp2,0,sizeof(temp2));
 	 memcpy(temp2,val,4);
 	 y=*(int*)&temp2;
 	 memset(temp2,0,sizeof(temp2));
 	 memcpy(temp2,val+4,y);

 	 if(strcmp(temp1,temp2)==0)
 	 {

 		memcpy(data,buff,sizeof(buff));
 	 }
 	 else
		 {
			 goto l1;
		 }
		 break;



	 }
}

return rc;
}

void Filter::getAttributes(vector<Attribute> &attrs) const
{
attrs.clear();
attrs=filter_attr;
}



//NESTED JOIN
NLJoin::NLJoin(Iterator *leftIn,TableScan *rightIn,const Condition &condition):left_it(leftIn),right_it(rightIn)
{

leftIn->getAttributes(left_attr);
rightIn->getAttributes(right_attr);
joincd=condition;
flag=0;

memset(lbuff,0,sizeof(lbuff));
memset(rbuff,0,sizeof(rbuff));
memset(lval,0,sizeof(lval));
memset(rval,0,sizeof(rval));
left_length=0;
right_length=0;


	for(int p=0;p<left_attr.size();p++)
	{
		combined.insert(combined.end(),left_attr[p]);
	}
	for(int q=0;q<right_attr.size();q++)
   	{
        combined.insert(combined.end(),right_attr[q]);
	}

right_it->setIterator();

}

NLJoin::~NLJoin()
{

}



RC NLJoin::getNextTuple(void *data)
{

	int rc,rc2;
	int lmove,lread,rmove,rtread,t1;
	char temp[100],final[200];



	//left
	l3:;
		if(flag==0)
		{
			flag=1;
		//right_it->setIterator();
		memset(lbuff,0,sizeof(lbuff));
		memset(lval,0,sizeof(lval));
		//cout<<"LEFT SIDE"<<endl;
		left_length=0;
		lread=0;
		lmove=0;
		rc=left_it->getNextTuple(lbuff);
		//cout<<"LEFT RC1 VALUE"<<rc<<endl;
		memset(temp,0,sizeof(temp));
		memcpy(temp,lbuff,4);
		//cout<<"--->"<<*(int*)&temp<<endl;
		if(rc==-1)
			return RM_EOF;

		//cout<<"left 1st READ"<<endl;
		for(int r=0;r<left_attr.size();r++)
		{
		 if(left_attr[r].type==TypeInt || left_attr[r].type==TypeReal)
			 {
				 left_length=left_length+4;
			 }
			 if(left_attr[r].type==TypeChar || left_attr[r].type==TypeVarChar)
			 {
				 memset(temp,0,sizeof(temp));
				 memcpy(temp,lbuff+left_length,4);
				 t1=*(int*)&temp;
				 left_length=left_length+4+t1;
			 }
		}


		for(int j=0;j<left_attr.size();j++)
      	 {
			 if(joincd.lhsAttr==left_attr[j].name)
			 {
				 atype=left_attr[j].type;
				 if(left_attr[j].type==TypeInt || left_attr[j].type==TypeReal)
				 {
					lread=4;
				 }

				 if(left_attr[j].type==TypeChar || left_attr[j].type==TypeVarChar)
				 {
					 memset(temp,0,sizeof(temp));
					 memcpy(temp,lbuff+lmove,4);
					 t1=*(int*)&temp;
					 lread=t1+4;
				 }
				 break;
			 }
			 else
			 {
				 if(left_attr[j].type==TypeInt || left_attr[j].type==TypeReal)
				 {
					 lmove=lmove+4;
				 }
				 if(left_attr[j].type==TypeChar || left_attr[j].type==TypeVarChar)
				 {
					 memset(temp,0,sizeof(temp));
					 memcpy(temp,lbuff+lmove,4);
					 t1=*(int*)&temp;
					 lmove=lmove+4+t1;
				 }
			 }
      }
	memcpy(lval,lbuff+lmove,lread);
	//cout<<"LEFT VALUE "<<*(int*)&lval<<endl;
	}


	//right

	l2:;

	//cout<<"RIGHT SIDE"<<endl;
	//right_it->setIterator();

		memset(rbuff,0,sizeof(rbuff));
		rc2=right_it->getNextTuple(rbuff);
		if(rc2==RM_EOF)
		{
			flag=0;
			right_it->setIterator();
			goto l3;
		}
		memset(temp,0,100);
		memcpy(temp,rbuff,4);
		right_length=0;
		rmove=0;
		rtread=0;
		memset(rval,0,sizeof(rval));
		//cout<<"Right val : "<<*(int*)&temp<<endl;

	for(int q=0;q<right_attr.size();q++)
	{
	 if(right_attr[q].type==TypeInt || right_attr[q].type==TypeReal)
		 {
			 right_length=right_length+4;
		 }
		 if(right_attr[q].type==TypeChar || right_attr[q].type==TypeVarChar)
		 {
			 memset(temp,0,sizeof(temp));
			 memcpy(temp,rbuff+right_length,4);
			 t1=*(int*)&temp;
			 right_length=right_length+4+t1;
		 }
	}

	//cout<<"LEFT LENGTH"<<left_length<<endl;
	//cout<<"RIGHT LENGTH"<<right_length<<endl;

	for(int k=0;k<right_attr.size();k++)
	{
		 if(joincd.rhsAttr==right_attr[k].name)
		 {
			 if(right_attr[k].type==TypeInt || right_attr[k].type==TypeReal)
			 {
				rtread=4;
			 }

			 if(right_attr[k].type==TypeChar || right_attr[k].type==TypeVarChar)
			 {
				 memset(temp,0,sizeof(temp));
				 memcpy(temp,rbuff+rmove,4);
				 t1=*(int*)&temp;
				 rtread=t1+4;
			 }
			 break;
		 }
		 else
		 {
			 if(right_attr[k].type==TypeInt || right_attr[k].type==TypeReal)
			 {
				 rmove=rmove+4;
			 }
			 if(right_attr[k].type==TypeChar || right_attr[k].type==TypeVarChar)
			 {
				 memset(temp,0,sizeof(temp));
				 memcpy(temp,rbuff+rmove,4);
				 t1=*(int*)&temp;
				 rmove=rmove+4+t1;
			 }
		 }
    }
	memcpy(rval,rbuff+rmove,rtread);
	//cout<<"RIGHT VALUE "<<*(int*)&rval<<endl;


	if(joincd.op==EQ_OP)
	{
	if(atype==TypeInt)
	{
		int l1,r1;
		l1=*(int*)&lval;
		r1=*(int*)&rval;
		if(l1==r1)
		{
			//cout<<"matched"<<endl;
			memset(final,0,sizeof(final));
			memcpy(final,lbuff,left_length);
			memcpy(final+left_length,rbuff,right_length);
			memcpy((char*)data,final,sizeof(final));
			return rc;

		}
		else
		{
			goto l2;
		}
	}

	if(atype==TypeReal)
	{
		float l2,r2;
		l2=*(int*)&lval;
		r2=*(int*)&rval;
		if(l2==r2)
		{
			//cout<<"matched"<<endl;
			memset(final,0,sizeof(final));
			memcpy(final,lbuff,left_length);
			memcpy(final+left_length,rbuff,right_length);
			memcpy((char*)data,final,sizeof(final));
			return rc;

		}
		else
		{
			goto l2;
		}
	}


	if(atype==TypeChar || atype==TypeVarChar)
	{
		if(strcmp(lval,rval)==0)
		{
			//cout<<"matched"<<endl;
			memset(final,0,sizeof(final));
			memcpy(final,lbuff,left_length);
			memcpy(final+left_length,rbuff,right_length);
			memcpy((char*)data,final,sizeof(final));
			return rc;

		}
		else
		{
			goto l2;
		}
	}
	}

	if(joincd.op==LT_OP)
	{
	if(atype==TypeInt)
	{
		int l1,r1;
		l1=*(int*)&lval;
		r1=*(int*)&rval;
		if(l1<r1)
		{
			//cout<<"matched"<<endl;
			memset(final,0,sizeof(final));
			memcpy(final,lbuff,left_length);
			memcpy(final+left_length,rbuff,right_length);
			memcpy((char*)data,final,sizeof(final));
			return rc;

		}
		else
		{
			goto l2;
		}
	}

	if(atype==TypeReal)
	{
		float l2,r2;
		l2=*(int*)&lval;
		r2=*(int*)&rval;
		if(l2<r2)
		{
			//cout<<"matched"<<endl;
			memset(final,0,sizeof(final));
			memcpy(final,lbuff,left_length);
			memcpy(final+left_length,rbuff,right_length);
			memcpy((char*)data,final,sizeof(final));
			return rc;

		}
		else
		{
			goto l2;
		}
	}


	if(atype==TypeChar || atype==TypeVarChar)
	{
		if(strcmp(lval,rval)<0)
		{
			//cout<<"matched"<<endl;
			memset(final,0,sizeof(final));
			memcpy(final,lbuff,left_length);
			memcpy(final+left_length,rbuff,right_length);
			memcpy((char*)data,final,sizeof(final));
			return rc;

		}
		else
		{
			goto l2;
		}
	}
	}


	if(joincd.op==LE_OP)
	{
	if(atype==TypeInt)
	{
		int l1,r1;
		l1=*(int*)&lval;
		r1=*(int*)&rval;
		if(l1<=r1)
		{
			//cout<<"matched"<<endl;
			memset(final,0,sizeof(final));
			memcpy(final,lbuff,left_length);
			memcpy(final+left_length,rbuff,right_length);
			memcpy((char*)data,final,sizeof(final));
			return rc;

		}
		else
		{
			goto l2;
		}
	}

	if(atype==TypeReal)
	{
		float l2,r2;
		l2=*(int*)&lval;
		r2=*(int*)&rval;
		if(l2<=r2)
		{
			//cout<<"matched"<<endl;
			memset(final,0,sizeof(final));
			memcpy(final,lbuff,left_length);
			memcpy(final+left_length,rbuff,right_length);
			memcpy((char*)data,final,sizeof(final));
			return rc;

		}
		else
		{
			goto l2;
		}
	}


	if(atype==TypeChar || atype==TypeVarChar)
	{
		if(strcmp(lval,rval)<=0)
		{
			//cout<<"matched"<<endl;
			memset(final,0,sizeof(final));
			memcpy(final,lbuff,left_length);
			memcpy(final+left_length,rbuff,right_length);
			memcpy((char*)data,final,sizeof(final));
			return rc;

		}
		else
		{
			goto l2;
		}
	}
	}

	if(joincd.op==GT_OP)
		{
		if(atype==TypeInt)
		{
			int l1,r1;
			l1=*(int*)&lval;
			r1=*(int*)&rval;
			if(l1>r1)
			{
				//cout<<"matched"<<endl;
				memset(final,0,sizeof(final));
				memcpy(final,lbuff,left_length);
				memcpy(final+left_length,rbuff,right_length);
				memcpy((char*)data,final,sizeof(final));
				return rc;

			}
			else
			{
				goto l2;
			}
		}

		if(atype==TypeReal)
		{
			float l2,r2;
			l2=*(int*)&lval;
			r2=*(int*)&rval;
			if(l2>r2)
			{
				//cout<<"matched"<<endl;
				memset(final,0,sizeof(final));
				memcpy(final,lbuff,left_length);
				memcpy(final+left_length,rbuff,right_length);
				memcpy((char*)data,final,sizeof(final));
				return rc;

			}
			else
			{
				goto l2;
			}
		}


		if(atype==TypeChar || atype==TypeVarChar)
		{
			if(strcmp(lval,rval)>0)
			{
				//cout<<"matched"<<endl;
				memset(final,0,sizeof(final));
				memcpy(final,lbuff,left_length);
				memcpy(final+left_length,rbuff,right_length);
				memcpy((char*)data,final,sizeof(final));
				return rc;

			}
			else
			{
				goto l2;
			}
		}
		}


	if(joincd.op==GE_OP)
		{
		if(atype==TypeInt)
		{
			int l1,r1;
			l1=*(int*)&lval;
			r1=*(int*)&rval;
			if(l1>=r1)
			{
				//cout<<"matched"<<endl;
				memset(final,0,sizeof(final));
				memcpy(final,lbuff,left_length);
				memcpy(final+left_length,rbuff,right_length);
				memcpy((char*)data,final,sizeof(final));
				return rc;

			}
			else
			{
				goto l2;
			}
		}

		if(atype==TypeReal)
		{
			float l2,r2;
			l2=*(int*)&lval;
			r2=*(int*)&rval;
			if(l2>=r2)
			{
				//cout<<"matched"<<endl;
				memset(final,0,sizeof(final));
				memcpy(final,lbuff,left_length);
				memcpy(final+left_length,rbuff,right_length);
				memcpy((char*)data,final,sizeof(final));
				return rc;

			}
			else
			{
				goto l2;
			}
		}


		if(atype==TypeChar || atype==TypeVarChar)
		{
			if(strcmp(lval,rval)>=0)
			{
				//cout<<"matched"<<endl;
				memset(final,0,sizeof(final));
				memcpy(final,lbuff,left_length);
				memcpy(final+left_length,rbuff,right_length);
				memcpy((char*)data,final,sizeof(final));
				return rc;

			}
			else
			{
				goto l2;
			}
		}
		}


	if(joincd.op==NE_OP)
		{
		if(atype==TypeInt)
		{
			int l1,r1;
			l1=*(int*)&lval;
			r1=*(int*)&rval;
			if(l1!=r1)
			{
				//cout<<"matched"<<endl;
				memset(final,0,sizeof(final));
				memcpy(final,lbuff,left_length);
				memcpy(final+left_length,rbuff,right_length);
				memcpy((char*)data,final,sizeof(final));
				return rc;

			}
			else
			{
				goto l2;
			}
		}

		if(atype==TypeReal)
		{
			float l2,r2;
			l2=*(int*)&lval;
			r2=*(int*)&rval;
			if(l2!=r2)
			{
				//cout<<"matched"<<endl;
				memset(final,0,sizeof(final));
				memcpy(final,lbuff,left_length);
				memcpy(final+left_length,rbuff,right_length);
				memcpy((char*)data,final,sizeof(final));
				return rc;

			}
			else
			{
				goto l2;
			}
		}


		if(atype==TypeChar || atype==TypeVarChar)
		{
			if(strcmp(lval,rval)!=0)
			{
				//cout<<"matched"<<endl;
				memset(final,0,sizeof(final));
				memcpy(final,lbuff,left_length);
				memcpy(final+left_length,rbuff,right_length);
				memcpy((char*)data,final,sizeof(final));
				return rc;

			}
			else
			{
				goto l2;
			}
		}
		}
//}
	if(rc2==RM_EOF)
		goto l3;
	return rc;
}



void NLJoin::getAttributes(vector<Attribute> &attrs) const
{
attrs.clear();
attrs=combined;
}


//index join
INLJoin::INLJoin(Iterator *leftIn,IndexScan *rightIn,const Condition &condition):left_it2(leftIn),right_it2(rightIn)
{
	leftIn->getAttributes(left_attr2);
	rightIn->getAttributes(right_attr2);
	indexcd=condition;
	flag2=0;

	memset(lbuff2,0,sizeof(lbuff2));
	memset(rbuff2,0,sizeof(rbuff2));
	memset(lval2,0,sizeof(lval2));
	memset(rval2,0,sizeof(rval2));
	left_length2=0;
	right_length2=0;


		for(int p=0;p<left_attr2.size();p++)
		{
			indexjoin.insert(indexjoin.end(),left_attr2[p]);
		}
		for(int q=0;q<right_attr2.size();q++)
	   	{
	        indexjoin.insert(indexjoin.end(),right_attr2[q]);
		}


}

INLJoin::~INLJoin()
{

}


RC INLJoin::getNextTuple(void *data)
{

	int rc3,rc4;
	int lmove,lread,rmove,rtread,t1;
	char temp[100],final[200],temp2[100];

	/*
	memset(temp,0,100);
	rc3=left_it2->getNextTuple(temp);
	memcpy(temp2,temp,4);
	cout<<*(int*)&temp2<<endl;
	rc3=left_it2->getNextTuple(temp);
		memcpy(temp2,temp,4);
		cout<<*(int*)&temp2<<endl;
		rc3=left_it2->getNextTuple(temp);
			memcpy(temp2,temp,4);
			cout<<*(int*)&temp2<<endl;*/

	//left
	l4:;
		if(flag2==0)
		{
			flag2=1;
		memset(lbuff2,0,sizeof(lbuff2));
		memset(lval2,0,sizeof(lval2));
		//cout<<"LEFT SIDE"<<endl;
		left_length2=0;
		lread=0;
		lmove=0;
		rc3=left_it2->getNextTuple(lbuff2);
		//cout<<"LEFT RC1 VALUE"<<rc<<endl;
		memset(temp,0,sizeof(temp));
		memcpy(temp,lbuff2,4);
		//cout<<"--->"<<*(int*)&temp<<endl;
		if(rc3==-1)
			return RM_EOF;

		//cout<<"left 1st READ"<<endl;
		for(int r=0;r<left_attr2.size();r++)
		{
		 if(left_attr2[r].type==TypeInt || left_attr2[r].type==TypeReal)
			 {
				 left_length2=left_length2+4;
			 }
			 if(left_attr2[r].type==TypeChar || left_attr2[r].type==TypeVarChar)
			 {
				 memset(temp,0,sizeof(temp));
				 memcpy(temp,lbuff2+left_length2,4);
				 t1=*(int*)&temp;
				 left_length2=left_length2+4+t1;
			 }
		}

		//cout<<"LEFT length"<<left_length2<<endl;


		for(int j=0;j<left_attr2.size();j++)
      	 {
			 if(indexcd.lhsAttr==left_attr2[j].name)
			 {
				 atype2=left_attr2[j].type;
				 if(left_attr2[j].type==TypeInt || left_attr2[j].type==TypeReal)
				 {
					lread=4;
				 }

				 if(left_attr2[j].type==TypeChar || left_attr2[j].type==TypeVarChar)
				 {
					 memset(temp,0,sizeof(temp));
					 memcpy(temp,lbuff2+lmove,4);
					 t1=*(int*)&temp;
					 lread=t1+4;
				 }
				 break;
			 }
			 else
			 {
				 if(left_attr2[j].type==TypeInt || left_attr2[j].type==TypeReal)
				 {
					 lmove=lmove+4;
				 }
				 if(left_attr2[j].type==TypeChar || left_attr2[j].type==TypeVarChar)
				 {
					 memset(temp,0,sizeof(temp));
					 memcpy(temp,lbuff2+lmove,4);
					 t1=*(int*)&temp;
					 lmove=lmove+4+t1;
				 }
			 }
      }
	memcpy(lval2,lbuff2+lmove,lread);
	//cout<<"MOVE "<<lmove<<"READ frm"<<lread<<endl;
	//cout<<"LEFT VALUE "<<lval2<<endl;


		if(atype2==TypeChar || atype2==TypeVarChar)
		{
			right_it2->setIterator(indexcd.op,lval2);
		}
		if(atype2==TypeInt)
		{
			int num;
			memset(temp,0,sizeof(temp));
			memcpy(temp,lval2,4);
			num=*(int*)&lval2;
			right_it2->setIterator(indexcd.op,&num);
		}

		if(atype2==TypeReal)
		{
			float num2;
			memset(temp,0,sizeof(temp));
			memcpy(temp,lval2,4);
			num2=*(float*)&lval2;
			right_it2->setIterator(indexcd.op,&num2);
		}

	}



	l5:;
	memset(rbuff2,0,sizeof(rbuff2));
	rc4=right_it2->getNextTuple(rbuff2);

	if(rc4==RM_EOF)
	{
		flag2=0;
		goto l4;
	}

	right_length2=0;

	for(int q=0;q<right_attr2.size();q++)
	{
	 if(right_attr2[q].type==TypeInt || right_attr2[q].type==TypeReal)
		 {
			 right_length2=right_length2+4;
		 }
		 if(right_attr2[q].type==TypeChar || right_attr2[q].type==TypeVarChar)
		 {
			 memset(temp,0,sizeof(temp));
			 memcpy(temp,rbuff2+right_length2,4);
			 t1=*(int*)&temp;
			 right_length2=right_length2+4+t1;
		 }
	}



	memset(final,0,sizeof(final));
	//cout<<"LEFT : "<<left_length2<<"RIGHT :"<<right_length2<<endl;
	memcpy(final,lbuff2,left_length2);
	memcpy(final+left_length2,rbuff2,right_length2);
	memcpy((char*)data,final,left_length2+right_length2);

	return rc3;

}



void INLJoin::getAttributes(vector<Attribute> &attrs) const
{
attrs.clear();
attrs=indexjoin;
}



//hash join
HashJoin::HashJoin(Iterator *leftIn,Iterator *rightIn,const Condition &condition)
{

}

HashJoin::~HashJoin()
{

}

RC HashJoin::getNextTuple(void *data)
{
return RM_EOF;
}

void HashJoin::getAttributes(vector<Attribute> &attrs) const
{

}




Aggregate::Aggregate(Iterator *input,Attribute aggAttr,AggregateOp op):it5(input)
{
ag=aggAttr;
op1=op;
memset(val,0,sizeof(val));
memset(buff,0,sizeof(buff));
input->getAttributes(agg_attr);
imin=0;
imax=0;
isum=0;
iavg=0;
icount=0;
fmin=0;
fmax=0;
fsum=0;
favg=0;
fcount=0;
}

Aggregate::~Aggregate()
{

}

RC Aggregate::getNextTuple(void *data)
{
	int rc,read,move,t1;
	char temp[100];

	/*
	rc=it5->getNextTuple(buff);
	cout<<"Rc"<<rc<<endl;
	rc=it5->getNextTuple(buff);
	cout<<"Rc"<<rc<<endl;

*/


	rc=it5->getNextTuple(buff);
	if(rc==RM_EOF)
	{
		return rc;
	}

	read=0;
	move=0;

	for(int j=0;j<agg_attr.size();j++)
	{
		 if(ag.name==agg_attr[j].name)
		 {
			 if(agg_attr[j].type==TypeInt || agg_attr[j].type==TypeReal)
			 {
				read=4;
			 }

			 if(agg_attr[j].type==TypeChar || agg_attr[j].type==TypeVarChar)
			 {
				 memset(temp,0,sizeof(temp));
				 memcpy(temp,buff+move,4);
				 t1=*(int*)&temp;
				 read=t1+4;

			 }

			 break;
		 }
		 else
		 {
			 if(agg_attr[j].type==TypeInt || agg_attr[j].type==TypeReal)
			 {
				 move=move+4;
			 }
			 if(agg_attr[j].type==TypeChar || agg_attr[j].type==TypeVarChar)
			 {
				 memset(temp,0,sizeof(temp));
				 memcpy(temp,buff+move,4);
				 t1=*(int*)&temp;
				 move=move+4+t1;
			 }
		 }
	}

	memset(val,0,sizeof(val));
	memcpy(val,buff+move,read);

	//cout<<*(int*)&val<<"[][]"<<op1;
	switch(op1)
	{
	case MIN:
		int n1;
		float f1;
		if(ag.type==TypeInt)
		{
			n1=*(int*)&val;
			if(n1<imin)
				imin=n1;
		}
		if(ag.type==TypeReal)
		{
			f1=*(float*)&val;
			if(f1<fmin)
				fmin=f1;
		}
	break;

	case MAX:
		int n2;
		float f2;
		if(ag.type==TypeInt)
		{

			n2=*(int*)&val;
			//cout<<n2<<endl;
			if(n2>imax)
				imax=n2;
		}
		if(ag.type==TypeReal)
		{
			f2=*(float*)&val;
			if(f2>fmax)
				fmax=f2;
		}
	break;

	case SUM:
		int n3;
		float f3;
		if(ag.type==TypeInt)
		{
			n3=*(int*)&val;
			isum=isum+n3;
		}
		if(ag.type==TypeReal)
		{
			f3=*(float*)&val;
			fsum=fsum+f3;
		}
	break;

	case AVG:
		int n4;
		float f4;
		if(ag.type==TypeInt)
		{
			n4=*(int*)&val;
			icount++;
			isum=isum+n4;
		}
		if(ag.type==TypeReal)
		{
			f4=*(float*)&val;
			fcount++;
			fsum=fsum+f4;
		}
	break;

	case COUNT:
			int n5;
			float f5;
			if(ag.type==TypeInt)
			{
				n5=*(int*)&val;
				icount++;
			}
			if(ag.type==TypeReal)
			{
				f5=*(float*)&val;
				fcount++;
			}
		break;

	}
	memset(buff,0,100);


	switch(op1)
	{
	case MIN:
		if(ag.type==TypeInt)
			memcpy((char*)data,&imin,4);
		if(ag.type==TypeReal)
			memcpy((char*)data,&fmin,4);
		break;

	case MAX:
		if(ag.type==TypeInt)
			memcpy((char*)data,&imax,4);
		if(ag.type==TypeReal)
			memcpy((char*)data,&fmax,4);
		break;

	case SUM:
		if(ag.type==TypeInt)
			memcpy((char*)data,&isum,4);
		if(ag.type==TypeReal)
			memcpy((char*)data,&fsum,4);
		break;

	case AVG:
		if(ag.type==TypeInt)
		{
			favg=isum/icount;
			memcpy((char*)data,&favg,4);
		}
		if(ag.type==TypeReal)
		{
			favg=fsum/fcount;
			memcpy((char*)data,&favg,4);
		}
			break;

	case COUNT:
		if(ag.type==TypeInt)
			memcpy((char*)data,&icount,4);
		if(ag.type==TypeReal)
			memcpy((char*)data,&fcount,4);
		break;
	}

	return rc;

}

void Aggregate::getAttributes(vector<Attribute> &attrs) const
{
attrs.clear();
char s1[20],s2[20],s3[20],s4[20];
memset(s1,0,20);
memset(s2,0,20);
memset(s3,0,20);
memset(s4,0,20);
memcpy(s1,"(",1);
memcpy(s2,")",1);

string tmp="";

Attribute at;
switch(op1)
{
case MIN:

	  tmp += "MIN";
	  tmp+="(";
	  tmp+=ag.name;
	  tmp+=")";
	  at.name=tmp;
	  at.type=ag.type;
	  at.length=ag.length;
	  attrs.insert(attrs.end(),at);
break;

case MAX:

	  tmp += "MAX";
	  tmp+="(";
	  tmp+=ag.name;
	  tmp+=")";
	  at.name=tmp;
	  at.type=ag.type;
	  at.length=ag.length;
	  attrs.insert(attrs.end(),at);
break;


case SUM:

	  tmp += "SUM";
	  tmp+="(";
	  tmp+=ag.name;
	  tmp+=")";
	  at.name=tmp;
	  at.type=ag.type;
	  at.length=ag.length;
	  attrs.insert(attrs.end(),at);
break;

case AVG:

	  tmp += "AVG";
	  tmp+="(";
	  tmp+=ag.name;
	  tmp+=")";
	  at.name=tmp;
	  at.type=ag.type;
	  at.length=ag.length;
	  attrs.insert(attrs.end(),at);
break;

case COUNT:

	  tmp += "COUNT";
	  tmp+="(";
	  tmp+=ag.name;
	  tmp+=")";
	  at.name=tmp;
	  at.type=ag.type;
	  at.length=ag.length;
	  attrs.insert(attrs.end(),at);
break;

}
}


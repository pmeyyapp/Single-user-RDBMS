
#include <fstream>
#include <iostream>
#include<cstring>
#include <cassert>

#include "rm.h"

using namespace std;

void rmTest()
{

RM *rm = new RM();
char *name="student";
vector<char*>attnames;
vector<char*>attnames1;
vector<AttrType>att_type;
vector<AttrLength>att_length;

attnames.insert(attnames.end(),"StudentID");
attnames.insert(attnames.end(),"StudentName");
attnames.insert(attnames.end(),"Grade");
attnames.insert(attnames.end(),"GPA");


att_type.insert(att_type.end(),TypeInt);
att_type.insert(att_type.end(),TypeChar);
att_type.insert(att_type.end(),TypeChar);
att_type.insert(att_type.end(),TypeInt);


att_length.insert(att_length.end(),4);
att_length.insert(att_length.end(),100);
att_length.insert(att_length.end(),100);
att_length.insert(att_length.end(),4);

rm->createTable(name,attnames,att_type,att_length);

RID rid,rid1;
rid.pageNum=1;
rid.slotNum=1;


rid1.pageNum=-1;
rid1.slotNum=-1;

int sid;
char sname[100],grade[100];
int slen,glen,gpa;
int count;
char iptuple[200];

memset(sname,0,100);
memset(grade,0,100);
strcpy(sname,"badri");
strcpy(grade,"A+");
sid=21;
slen=5;
glen=2;
gpa=99;

memset(iptuple,0,200);
count = 0;
memcpy(iptuple,&sid,4);
count += 4;
memcpy((iptuple+count),&slen,4);
count+=4;
memcpy((iptuple+count),sname,strlen(sname));
count += strlen(sname);
memcpy((iptuple+count),&glen,4);
count += 4;
memcpy((iptuple+count),grade,strlen(grade));
count += strlen(grade);
memcpy((iptuple+count),&gpa,4);
count+=4;


void *data;
rm->insertTuple(name,iptuple,rid);


memset(sname,0,100);
memset(grade,0,100);
strcpy(sname,"saiiiiii");
strcpy(grade,"C-");
sid=43;
slen=8;
glen=2;
gpa=76;

memset(iptuple,0,200);
count = 0;
memcpy(iptuple,&sid,4);
count += 4;
memcpy((iptuple+count),&slen,4);
count+=4;
memcpy((iptuple+count),sname,strlen(sname));
count += strlen(sname);
memcpy((iptuple+count),&glen,4);
count += 4;
memcpy((iptuple+count),grade,strlen(grade));
count += strlen(grade);
memcpy((iptuple+count),&gpa,4);
count+=4;
rm->insertTuple(name,iptuple,rid);



memset(sname,0,100);
memset(grade,0,100);
strcpy(sname,"aravind");
strcpy(grade,"B-");
sid=45;
slen=7;
glen=2;
gpa=88;

memset(iptuple,0,200);
count = 0;
memcpy(iptuple,&sid,4);
count += 4;
memcpy((iptuple+count),&slen,4);
count+=4;
memcpy((iptuple+count),sname,strlen(sname));
count += strlen(sname);
memcpy((iptuple+count),&glen,4);
count += 4;
memcpy((iptuple+count),grade,strlen(grade));
count += strlen(grade);
memcpy((iptuple+count),&gpa,4);
count+=4;
rm->insertTuple(name,iptuple,rid);


rid.pageNum=1;
rid.slotNum=2;

//rm->deleteTuple(name,rid);



rid.pageNum=1;
rid.slotNum=3;

//rm->deleteTuple(name,rid);


char buff1[50];
memset(buff1,0,sizeof(buff1));
rm->readTuple(name,rid,buff1);



memset(sname,0,100);
memset(grade,0,100);
strcpy(sname,"shrinikett");
strcpy(grade,"C-");
sid=57;
slen=10;
glen=2;
gpa=85;

memset(iptuple,0,200);
count = 0;
memcpy(iptuple,&sid,4);
count += 4;
memcpy((iptuple+count),&slen,4);
count+=4;
memcpy((iptuple+count),sname,strlen(sname));
count += strlen(sname);
memcpy((iptuple+count),&glen,4);
count += 4;
memcpy((iptuple+count),grade,strlen(grade));
count += strlen(grade);
memcpy((iptuple+count),&gpa,4);
count+=4;
rm->insertTuple(name,iptuple,rid);



attnames1.insert(attnames1.end(),"StudentID");
attnames1.insert(attnames1.end(),"StudentName");
attnames1.insert(attnames1.end(),"Grade");
attnames1.insert(attnames1.end(),"GPA");

RM_ScanIterator *rsc=new RM_ScanIterator();

rm->scan(name,attnames1,*rsc);
cout<<"after scan"<<endl;
rid.pageNum=1;
rid.slotNum=4;
void *data3;
int f=1;
RID rid5;
rid5.pageNum=1;
rid5.slotNum=1;
char buff2[50],buff3[50];
memset(buff2,0,sizeof(buff2));
memset(buff3,0,sizeof(buff3));
while(rsc->getNextTuple(rid5,buff2)!=-1)
{
memcpy(buff3,buff2,4);
cout<<"Stud Id"<<*(int*)&buff3<<endl;
memset(buff2,0,sizeof(buff2));
memset(buff3,0,sizeof(buff3));
}

RID rid9;
rid9.pageNum=1;
rid9.slotNum=2;
char buff[100];
memset(buff,0,sizeof(buff));

rm->readAttribute(name,rid9,"Grade",buff);
cout<<"Atrribute "<<buff<<endl;


memset(iptuple,0,sizeof(iptuple));
memset(sname,0,100);
memset(grade,0,100);
strcpy(sname,"rajnikanth12345");
strcpy(grade,"A+");
sid=25;
slen=15;
glen=2;
gpa=56;
memset(iptuple,0,200);
count = 0;
memcpy(iptuple,&sid,4);
count += 4;
memcpy((iptuple+count),&slen,4);
count+=4;
memcpy((iptuple+count),sname,strlen(sname));
count += strlen(sname);
memcpy((iptuple+count),&glen,4);
count += 4;
memcpy((iptuple+count),grade,strlen(grade));
count += strlen(grade);
memcpy((iptuple+count),&gpa,4);
count+=4;
//rm->insertTuple(name,iptuple,rid);

rid9.pageNum=1;
rid9.slotNum=3;
rm->updateTuple(name,iptuple,rid9);
rid9.pageNum=1;
rid9.slotNum=2;
//rm->deleteTuple(name,rid9);

//rm->reorganizePage(name,1);

delete rm;

RM *rm3=new RM();

attnames.clear();
  	attnames.insert(attnames.end(),"DeptID");
   	attnames.insert(attnames.end(),"Dname");
   	attnames.insert(attnames.end(),"Year");
   	attnames.insert(attnames.end(),"Profit");

   	att_type.clear();
   	att_type.insert(att_type.end(),TypeInt);
   	att_type.insert(att_type.end(),TypeChar);
   	att_type.insert(att_type.end(),TypeInt);
   	att_type.insert(att_type.end(),TypeReal);

   	att_length.clear();
   	att_length.insert(att_length.end(),4);
   	att_length.insert(att_length.end(),100);
   	att_length.insert(att_length.end(),4);
   	att_length.insert(att_length.end(),4);
  	int a1=rm3->createTable("department",attnames,att_type,att_length);


  	char dname[100];
   	int yr,dlen,rc;
   	float profit;
   	memset(dname,0,100);
   	strcpy(dname,"bowling");
		int did=4;
		dlen=7;
		yr=2000;
		profit=789.567;

		memset(iptuple,0,200);
		count = 0;
		memcpy(iptuple,&did,4);
		count += 4;
		memcpy((iptuple+count),&dlen,4);
		count+=4;
		memcpy((iptuple+count),dname,strlen(dname));
		count += strlen(dname);
		memcpy((iptuple+count),&yr,4);
		count+=4;
		memcpy((iptuple+count),&profit,4);
		count+=4;
		rc=rm3->insertTuple("department",iptuple,rid);

		memset(dname,0,100);
	   	strcpy(dname,"batting");
			did=2;
			dlen=7;
		int	yr1=2006;
			profit=978.796;

			memset(iptuple,0,200);
			count = 0;
			memcpy(iptuple,&did,4);
			count += 4;
			memcpy((iptuple+count),&dlen,4);
			count+=4;
			memcpy((iptuple+count),dname,strlen(dname));
			count += strlen(dname);
			memcpy((iptuple+count),&yr1,4);
			count+=4;
			memcpy((iptuple+count),&profit,4);
			count+=4;
			rc=rm3->insertTuple("department",iptuple,rid);

			memset(dname,0,100);
		   	strcpy(dname,"fielding");
				did=3;
				dlen=8;
				int yr2=2003;
				profit=108.09;

				memset(iptuple,0,200);
				count = 0;
				memcpy(iptuple,&did,4);
				count += 4;
				memcpy((iptuple+count),&dlen,4);
				count+=4;
				memcpy((iptuple+count),dname,strlen(dname));
				count += strlen(dname);
				memcpy((iptuple+count),&yr2,4);
				count+=4;
				memcpy((iptuple+count),&profit,4);
				count+=4;
				rc=rm3->insertTuple("department",iptuple,rid);

				char nbuff[100],nbuff2[100];
				memset(nbuff,0,sizeof(nbuff));
				memset(nbuff2,0,sizeof(nbuff2));
				rid.pageNum=1;
				rid.slotNum=3;
				rc=rm3->readAttribute("department",rid,"Year",nbuff);
				cout<<"Vallue profit"<<*(int*)&nbuff<<endl;
delete rm3;
}

/*
int main()
{
  cout << "test..." << endl;
  rmTest();
  // other tests go here
  cout << "OK" << endl;
}
*/


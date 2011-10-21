#include <cstring>
#include <stdlib.h>
#include "rm.h"
#include <iostream>
#include <fstream>
#include <cstdio>

RM::RM()
{
}

char PAGEBUFF[PF_PAGE_SIZE];

RM::~RM()
{
}

RM_ScanIterator::RM_ScanIterator()
{
int i;
for(i=0;i<PF_PAGE_SIZE;i++)
	pslots[i]=0;
for(i=0;i<100;i++)
	p[i]=100;
no_pages=0;
for(i=0;i<100;i++)
	no_rec_in_page[i]=0;
attnames.clear();
memset(tname,0,100);


}

RM_ScanIterator::~RM_ScanIterator()
{

}



struct node
  {  short start;
     short end;
     short prevslot;
     node *nxt;
  };


struct page
{
	int pno;
	int slotid[PF_PAGE_SIZE];
	unsigned int length[PF_PAGE_SIZE];
	int ptr[PF_PAGE_SIZE];
	node *head;
	node *freespace;
	node *temp;
	node *temp3;
}p[30];

int flag,length_record,pgid=1;
int inc=500;
char arr[100];


RC RM::createTable(const char *tableName, 
		     const vector<char *> &attributeNames,
		     const vector<AttrType> &attributeTypes,
		     const vector<AttrLength> &attributeLengths)
{
PF_Manager *pf=new PF_Manager();
pf->CreateFile(tableName);
flag=0;

char tab[100],cat[100],mid[100];
memset(tab,0,sizeof(tab));
memcpy(tab,tableName,strlen(tableName));
memset(cat,0,sizeof(cat));
memcpy(cat,"catalog",7);
memset(mid,0,sizeof(mid));
memcpy(mid,"_",1);
strcat(tab,mid);
strcat(tab,cat);

//cout<<"file to be  "<<tab<<endl;


pf->CreateFile(tab);

vector<char*> attnames=attributeNames;
vector<AttrType> att_type=attributeTypes;
vector<AttrLength> att_length=attributeLengths;

numofattr=attnames.size();

vector<char*>::iterator it1;
vector<AttrType>::iterator it2;
vector<AttrLength>::iterator it3;


fstream fp1;
fp1.open(tab,ios::out);
int j=1;
for(it1=attnames.begin(),it2=att_type.begin(),it3=att_length.begin(); it1!=attnames.end(),it2!=att_type.end(),it3!=att_length.end(); it1++,it2++,it3++)
  {
	 char buff[20];
	 memset(buff,0,sizeof(buff));
	 //itoa(*it3,buff,10);
	 sprintf(buff,"%d",*it3);

	 int len=strlen(*it1)+2+1+strlen(buff)+1+2;
	 int alen=strlen(*it1);

	 char len1[20];
	 char len2[2]="0";
	 char alen1[20];
	 char alen2[20]="0";
	 //itoa(len,len1,10);
	 sprintf(len1,"%d",len);
	 sprintf(alen1,"%d",alen);

	 if(strlen(alen1)!=2)
	 {
		 strcat(alen2,alen1);
		 strcpy(alen1,alen2);
	 }


	 if(strlen(len1)!=2)
	 {
		strcat(len2,len1);
		fp1<<len2<<*it2<<alen1<<*it1<<strlen(buff)<<*it3;

	 }
	 else
	 {
	 fp1<<len<<*it2<<alen1<<*it1<<strlen(buff)<<*it3;
	 }
	 j++;
}
fp1.close();
delete pf;
return 0;
}


RC RM::deleteTable(const char *tableName)
{
PF_Manager *pf=new PF_Manager();
pf->DestroyFile(tableName);
delete pf;
return -1;
}


RC RM::getAttributes(const char *tableName,vector<Attribute> &attrs)
{
	int startpos,length,att_type,att_length,i=0,namelen,leng_len;
	char tempbuff[100],att_name[100];
	Attribute newattr;

	char tab[100],cat[100],mid[100];
	memset(tab,0,sizeof(tab));
	memcpy(tab,tableName,strlen(tableName));
	memset(cat,0,sizeof(cat));
	memcpy(cat,"catalog",7);
	memset(mid,0,sizeof(mid));
	memcpy(mid,"_",1);
	strcat(tab,mid);
	strcat(tab,cat);


	fstream fp;
	fp.open(tab,ios::in);
	while(i<numofattr)
	{
		startpos=fp.tellp();
		memset(tempbuff,0,sizeof(tempbuff));
		fp.read(tempbuff,2);
		length=atoi(tempbuff);
		memset(tempbuff,0,sizeof(tempbuff));
		fp.read(tempbuff,1);
		att_type=atoi(tempbuff);
		memset(tempbuff,0,sizeof(tempbuff));
		fp.read(tempbuff,2);
		namelen=atoi(tempbuff);
		memset(tempbuff,0,sizeof(tempbuff));
		fp.read(tempbuff,namelen);
		memset(att_name,0,sizeof(att_name));
		memcpy(att_name,tempbuff,sizeof(att_name));
		memset(tempbuff,0,sizeof(tempbuff));
		fp.read(tempbuff,1);
		leng_len=atoi(tempbuff);
		memset(tempbuff,0,sizeof(tempbuff));
		fp.read(tempbuff,leng_len);
		att_length=atoi(tempbuff);

		//copying
		newattr.name=att_name;

		if(att_type==0)
			newattr.type=TypeInt;
		if(att_type==1)
			newattr.type=TypeReal;
		if(att_type==2)
			newattr.type=TypeChar;
		if(att_type==3)
			newattr.type=TypeVarChar;

		newattr.length=att_length;

		attrs.insert(attrs.end(),newattr);

		i++;
	}

}

RC RM::insertTuple(const char *tableName, const void *data, RID &rid)
{
l1:;
int t=0,tempor,startpos,length,att_type,vlen,i=0,pq=0;
char buff[100],buffer[PF_PAGE_SIZE],newbuff[PF_PAGE_SIZE];
length_record=0;

memset(buff,0,sizeof(buff));
memset(buffer,0,sizeof(buffer));
memset(newbuff,0,sizeof(newbuff));

char tab[100],cat[100],mid[100];
memset(tab,0,sizeof(tab));
memcpy(tab,tableName,strlen(tableName));
memset(cat,0,sizeof(cat));
memcpy(cat,"catalog",7);
memset(mid,0,sizeof(mid));
memcpy(mid,"_",1);
strcat(tab,mid);
strcat(tab,cat);

fstream fp,fp9;
fp.open("ex1",ios::out|ios::binary);
fp.write((char*)data,300);
fp.close();
fp.open(tab,ios::in);

int p1,p2;
while(i<numofattr)
{
startpos=fp.tellp();
fp.read(buff,2);
length=atoi(buff);
memset(buff,0,sizeof(buff));
fp.read(buff,1);
att_type=atoi(buff);
memset(buff,0,sizeof(buff));
if(att_type==0 || att_type==1)
{
		length_record+=4;

}
/*
else if(att_type==2)
{

	fp.read(buff,1);
	p1=atoi(buff);
	fp.read(buff,p1);
	p2=atoi(buff);
	memcpy(newbuff+length_record,(char*)data+length_record,p2);
	length_record+=p2;
}*/
else if(att_type==2 || att_type==3)
{
	fp9.open("ex1",ios::in | ios::binary);
	fp9.seekp(length_record);
	fp9.read(buff,4);
	fp9.close();
	vlen=*(int*)&buff;
	memset(buff,0,sizeof(buff));
	length_record+=4+vlen;
}
fp.seekp(startpos+length);
i++;
}

PF_Manager *pf=new PF_Manager();
PF_FileHandle *pfh=new PF_FileHandle();
pf->OpenFile(tableName,*pfh);
unsigned int nop=pfh->GetNumberOfPages();
int nop1=(int)nop;

//create the first page
if(nop1==0)
{
	flag=0;
}

//for 1st page
// free space

if(flag==0)
{
pfh->WritePage(pgid-1,buffer);
p[pgid].head=NULL;
flag=2;
p[pgid].freespace=(struct node*)malloc(sizeof(struct node));
p[pgid].freespace->start=0;
p[pgid].freespace->end=PF_PAGE_SIZE;
p[pgid].freespace->prevslot=-1;
p[pgid].freespace->nxt=NULL;
p[pgid].head=p[pgid].freespace;


memset(arr,0,sizeof(arr));
memcpy(arr,&p[pgid].freespace->start,2);
memcpy(arr+2,&p[pgid].freespace->end,2);
memcpy(arr+4,&p[pgid].freespace->prevslot,2);
memset(PAGEBUFF,0,sizeof(PAGEBUFF));
memcpy(arr+6,&p[pgid].freespace->end,6);
memcpy(PAGEBUFF,arr,100);
pfh->WritePage(pgid-1,PAGEBUFF);
}


p[pgid].freespace=(struct node*)malloc(sizeof(struct node));
memset(arr,0,sizeof(arr));
memset(PAGEBUFF,0,sizeof(PAGEBUFF));
pfh->ReadPage(pgid-1,PAGEBUFF);
memcpy(arr,PAGEBUFF,2);
p[pgid].freespace->start=*(short*)&arr;
memcpy(arr,PAGEBUFF+2,2);
p[pgid].freespace->end=*(short*)&arr;
memcpy(arr,PAGEBUFF+4,2);
p[pgid].freespace->prevslot=*(short*)&arr;
p[pgid].freespace->nxt=NULL;
p[pgid].head=p[pgid].freespace;
p[pgid].head->nxt=NULL;

int j=0,count=0,k=6;

while(PAGEBUFF[j]!=0)
{
	j=j+6;
	count++;

}
i=1;

while(i<count)
{
	i++;
	p[pgid].freespace=(struct node*)malloc(sizeof(struct node));
	memcpy(arr,PAGEBUFF+k,2);
	k+=2;
	p[pgid].freespace->start=*(short*)&arr;
	memcpy(arr,PAGEBUFF+k,2);
	k+=2;
	p[pgid].freespace->end=*(short*)&arr;
	memcpy(arr,PAGEBUFF+k,2);
	k+=2;
	p[pgid].freespace->prevslot=*(short*)&arr;
	p[pgid].temp=p[pgid].head;
	while(p[pgid].temp->nxt!=NULL)
	{
	  p[pgid].temp=p[pgid].temp->nxt;
	}
	p[pgid].temp->nxt=p[pgid].freespace;
	p[pgid].freespace->nxt=NULL;
}


p[pgid].temp=p[pgid].head;
while(p[pgid].temp!=NULL)
{
		p[pgid].temp=p[pgid].temp->nxt;
}

int f=0;
p[pgid].temp=p[pgid].head;
while(p[pgid].temp!=NULL)
{

	if(length_record <= ((p[pgid].temp->end)-(p[pgid].temp->start)))
	{
		f=1;
	if(p[pgid].temp->prevslot==-1)
	{
		t=1;
	}
	else
	{
		t=p[pgid].temp->prevslot;
		t++;
	}
	break;
	}
	p[pgid].temp=p[pgid].temp->nxt;
}

if(f==0)
{
pq=1;
}

if(pq==1)
{
pgid++;
flag=0;
goto l1;
}


memset(PAGEBUFF,0,sizeof(PAGEBUFF));
pfh->ReadPage(pgid-1,PAGEBUFF);

if(t==1)
{
	memcpy((PAGEBUFF+inc),(char*)data,length_record);

	p[pgid].pno=pgid;
	p[pgid].slotid[t]=t;
	p[pgid].ptr[t]=inc;
	p[pgid].length[t]=length_record;


	int ct=(PF_PAGE_SIZE)-(12*p[pgid].slotid[t]);
	memcpy(PAGEBUFF+ct,&p[pgid].slotid[t],4);
	ct+=4;
	memcpy(PAGEBUFF+ct,&p[pgid].ptr[t],4);
	ct+=4;
	memcpy(PAGEBUFF+ct,&p[pgid].length[t],4);


				  p[pgid].temp=p[pgid].head;
		          p[pgid].temp->start=p[pgid].ptr[t]+p[pgid].length[t];
		          p[pgid].temp->end=(PF_PAGE_SIZE* p[pgid].pno)-(12*p[pgid].slotid[t]);
		          p[pgid].temp->prevslot=p[pgid].slotid[t];
		          p[pgid].temp->nxt=NULL;
		          p[pgid].head=p[pgid].temp;



	          rid.pageNum=p[pgid].pno;
	          rid.slotNum=p[pgid].slotid[t];

}
else
{
	  j=PF_PAGE_SIZE-12;
	  int counter1=0;
	  int ct3=PF_PAGE_SIZE-12,slt,ptr,len;

	  	while(PAGEBUFF[j]!=0)
	  	{
	  		j=j-12;
	  		counter1++;
	  	}
	  	i=0;
	  	memset(buff,0,sizeof(buff));

	tempor=p[pgid].temp->start;
	memcpy(PAGEBUFF+tempor,(char*)data,length_record);
p[pgid].slotid[t]=t;
p[pgid].ptr[t]=tempor;//tempor;
p[pgid].length[t]=length_record;
fp.close();


counter1++;
int ct=(PF_PAGE_SIZE)-(12*counter1);
memcpy(PAGEBUFF+ct,&p[pgid].slotid[t],4);
ct+=4;
memcpy(PAGEBUFF+ct,&p[pgid].ptr[t],4);
ct+=4;
memcpy(PAGEBUFF+ct,&p[pgid].length[t],4);

         		          p[pgid].temp->start=p[pgid].ptr[t]+p[pgid].length[t];

         		          if(p[pgid].temp->nxt==NULL)
         		          {
         		          p[pgid].temp->end=(PF_PAGE_SIZE)-(12*counter1);
         		          }
         		          p[pgid].temp->prevslot=p[pgid].slotid[t];


          rid.pageNum=p[pgid].pno;
          rid.slotNum=p[pgid].slotid[t];
}

p[pgid].temp=p[pgid].head;
int q=0;;
memset(PAGEBUFF,0,inc);
while(p[pgid].temp!=NULL)
{

	int s1,e1;
	s1=p[pgid].temp->start;
	e1=p[pgid].temp->end;
	if(s1==e1)
		goto last;
   	memcpy(PAGEBUFF+q,&p[pgid].temp->start,2);
   	q+=2;
   	memcpy(PAGEBUFF+q,&p[pgid].temp->end,2);
   	q+=2;
   	memcpy(PAGEBUFF+q,&p[pgid].temp->prevslot,2);
   	q+=2;

   	last:;
	p[pgid].temp=p[pgid].temp->nxt;
}


pfh->WritePage(pgid-1,PAGEBUFF);
delete pf;
delete pfh;
return 0;
}


RC RM::deleteTuples(const char *tableName)
{
	int x1=deleteTable(tableName);
	PF_Manager *pf=new PF_Manager();
	pf->CreateFile(tableName);
	delete pf;
	return -1;
}


RC RM::deleteTuple(const char *tableName, const RID &rid)
{
	  int pgno=rid.pageNum;
	  int slno=rid.slotNum;
	  int counter=0,j,slt,ptr,len,i=0;
	  char buff[100];
	  memset(buff,0,sizeof(buff));


	  PF_Manager *pf=new PF_Manager();
	  PF_FileHandle *pfh=new PF_FileHandle();
	  pf->OpenFile(tableName,*pfh);
	  pfh->ReadPage(pgno-1,PAGEBUFF);

	  j=PF_PAGE_SIZE-12;
	  int ct3=PF_PAGE_SIZE-12;

	  	while(PAGEBUFF[j]!=0)
	  	{
	  		j=j-12;
	  		counter++;
	  	}

	  while(i<counter)
	  {
	   	memcpy(buff,PAGEBUFF+ct3,4);
	  	slt=*(int*)&buff;
	  	memset(buff,0,sizeof(buff));
	  	memcpy(buff,PAGEBUFF+ct3+4,4);
	  	ptr=*(int*)&buff;
	  	memset(buff,0,sizeof(buff));
	  	memcpy(buff,PAGEBUFF+ct3+8,4);
	  	ct3-=12;
	  	len=*(int*)&buff;
	  	memset(buff,0,sizeof(buff));

	  	if(slno==slt)
	  	{
	  		break;
	  	}
	  	i++;
	  }



	  int x=len;
	  if(x==0)
	  {
		  cout<<"Tuple does not exists"<<endl;
		  return -1;

	  }
	  len=0;

	  //directory update
	  	int ct=(PF_PAGE_SIZE* pgno)-(12*slt);
	  	int gt=ct;
		memcpy(PAGEBUFF+ct,&slt,4);
		ct+=4;
		memcpy(PAGEBUFF+ct,&ptr,4);
		ct+=4;
		memcpy(PAGEBUFF+ct,&len,4);

	  //free space update
	  p[pgno].freespace=(struct node*)malloc(sizeof(struct node));
	  p[pgno].freespace->start=ptr;
	  p[pgno].freespace->end=ptr+x;
	  p[pgno].freespace->prevslot=slt-1;

	  int free_length=p[pgno].freespace->end-p[pgno].freespace->start;
	  int currlength;

	  p[pgno].temp=p[pgno].head;
	  while(p[pgno].temp->nxt!=NULL)
	  {

		  currlength=p[pgno].temp->end-p[pgno].temp->start;
		  if(free_length<currlength)
		  {
			  break;
		  }
		  p[pgno].temp3=p[pgno].temp;
		  p[pgno].temp=p[pgno].temp->nxt;
	  }

	  if(p[pgno].temp==p[pgno].head)
	  {
		  p[pgno].freespace->nxt=p[pgno].temp;
		  p[pgno].head=p[pgno].freespace;
	  }
	  else
	  {
		  p[pgno].temp3->nxt=p[pgno].freespace;
		  p[pgno].freespace->nxt=p[pgno].temp;

	  }


	  	  p[pgno].temp=p[pgno].head;
	  	  while(p[pgno].temp!=NULL)
		  {

			  p[pgno].temp=p[pgno].temp->nxt;
			  cout<<endl;
		  }

	  	p[pgid].temp=p[pgid].head;
	  	int q=0;
	  	memset(PAGEBUFF,0,inc);
	  	while(p[pgid].temp!=NULL)
	  	{
	  		int s1,e1,s2,e2;
	  		s2=p[pgid].temp->start;
	  		e2=p[pgid].temp->end;

	  	   	if(e1==s2)
	  		{
	  			q-=4;
	  		   	memcpy(PAGEBUFF+q,&e2,2);
	  		   	q+=4;
	  		   	goto l1;
	  		}

	  	   	memcpy(PAGEBUFF+q,&p[pgid].temp->start,2);
	  	   	q+=2;
	  	   	memcpy(PAGEBUFF+q,&p[pgid].temp->end,2);
	  	   	q+=2;
	  	   	memcpy(PAGEBUFF+q,&p[pgid].temp->prevslot,2);
	  	   	q+=2;

	  	   	s1=p[pgid].temp->start;
	  	  	e1=p[pgid].temp->end;
	  	  	l1:;
	  		p[pgid].temp=p[pgid].temp->nxt;
	  	}


pfh->WritePage(pgid-1,PAGEBUFF);
delete pf;
delete pfh;
return -1;
}




RC RM::updateTuple(const char *tableName, const void *data, const RID &rid)
{
memset(PAGEBUFF,0,sizeof(PAGEBUFF));
PF_Manager *pf=new PF_Manager();
PF_FileHandle *pfh=new PF_FileHandle();
pf->OpenFile(tableName,*pfh);

int pgno=rid.pageNum;
int slno=rid.slotNum;
pfh->ReadPage(pgno-1,PAGEBUFF);
int pos,len1,len,att_type,i=0,vlen,j,counter=0,slt,ptr;
char recout[50],buff[50];
memset(recout,0,sizeof(recout));
memset(buff,0,sizeof(buff));

int lenrec=0,z;
j=PF_PAGE_SIZE-12;
int ct3=PF_PAGE_SIZE-12;

while(PAGEBUFF[j]!=0)
{
	j=j-12;
	counter++;
}

while(i<counter)
{
	   	memcpy(buff,PAGEBUFF+ct3,4);
	  	slt=*(int*)&buff;
	  	memset(buff,0,sizeof(buff));
	  	memcpy(buff,PAGEBUFF+ct3+4,4);
	  	ptr=*(int*)&buff;
	  	memset(buff,0,sizeof(buff));
	  	memcpy(buff,PAGEBUFF+ct3+8,4);
	  	ct3-=12;
	  	len=*(int*)&buff;
	  	memset(buff,0,sizeof(buff));
	  	if(slno==slt)
	  	{
	  		z=i+1;
	  		break;
	  	}
	  	i++;
}


int startpos=ptr;
int reclen=len;

memset(buff,0,sizeof(buff));
fstream fp,fp9;

char tab[100],cat[100],mid[100];
memset(tab,0,sizeof(tab));
memcpy(tab,tableName,strlen(tableName));
memset(cat,0,sizeof(cat));
memcpy(cat,"catalog",7);
memset(mid,0,sizeof(mid));
memcpy(mid,"_",1);
strcat(tab,mid);
strcat(tab,cat);

fp.open("ex2",ios::out|ios::binary);
fp.write((char*)data,300);
fp.close();
fp.open(tab,ios::in);

int k=0;
int p1,p2;

while(k<numofattr)
{
memset(buff,0,sizeof(buff));
pos=fp.tellp();
fp.read(buff,2);
len1=atoi(buff);
memset(buff,0,sizeof(buff));
fp.read(buff,1);
att_type=atoi(buff);
memset(buff,0,sizeof(buff));

if(att_type==0 || att_type==1)
{
	lenrec+=4;
}/*
else if(att_type==2)
{
	fp.read(buff,1);
	p1=atoi(buff);
	fp.read(buff,p1);
	p2=atoi(buff);
	length_record+=p2;
}*/
else if(att_type==3 || att_type==2)
{
	fp9.open("ex2",ios::in | ios::binary);
	fp9.seekg(lenrec);
	fp9.read(buff,4);
	fp9.close();
	vlen=*(int*)&buff;
	lenrec+=4+vlen;

}
fp.seekp(pos+len1);
k++;
}

if(lenrec<=reclen)
{
char newbuff[100];
memset(newbuff,0,100);
memcpy(newbuff,(char*)data,lenrec);
memcpy(PAGEBUFF+startpos,newbuff,lenrec);
}

else if(lenrec>reclen)
{
	RID rid5;
	rid5.pageNum=0;
	rid5.slotNum=0;
	int q=insertTuple(tableName,(char*)data,rid5);
	len=-1;
	int tt=PF_PAGE_SIZE-(12*z)+8;
	memcpy(PAGEBUFF+startpos,&rid5.pageNum,4);
	memcpy(PAGEBUFF+startpos+4,&rid5.slotNum,4);
	memcpy(PAGEBUFF+tt,&len,4);
}

pfh->WritePage(pgno-1,PAGEBUFF);
delete pf;
delete pfh;
return -1;
}


RC RM::readTuple(const char *tableName, const RID &rid, void *data)
{
  int pgno=rid.pageNum;
  int slno=rid.slotNum;
  int len,i=0,type,k,slt,ptr,len1,counter=0,j;
  char recout[50],buff[50];
  memset(buff,0,sizeof(buff));

  memset(PAGEBUFF,0,sizeof(PAGEBUFF));
  PF_Manager *pf=new PF_Manager();
  PF_FileHandle *pfh=new PF_FileHandle();
  pf->OpenFile(tableName,*pfh);
  pfh->ReadPage(pgno-1,PAGEBUFF);

  j=PF_PAGE_SIZE-12;
  int ct3=PF_PAGE_SIZE-12;
  while(PAGEBUFF[j]!=0)
  {
  	j=j-12;
  	counter++;
  }

  while(i<counter)
  {
  	   	memcpy(buff,PAGEBUFF+ct3,4);
  	  	slt=*(int*)&buff;
  	  	memset(buff,0,sizeof(buff));
  	  	memcpy(buff,PAGEBUFF+ct3+4,4);
  	  	ptr=*(int*)&buff;
  	  	memset(buff,0,sizeof(buff));
  	  	memcpy(buff,PAGEBUFF+ct3+8,4);
  	  	ct3-=12;
  	  	len1=*(int*)&buff;
  	  	memset(buff,0,sizeof(buff));
  	  	if(slno==slt)
  	  	{
  	  		break;
  	  	}
  	  	i++;
  }


  int startpos=ptr;
  int reclen=len1;
  int ct=startpos;

  if(reclen==-1)
  {
	 memcpy(buff,PAGEBUFF+startpos,4);
	 int pnum=*(int*)&buff;
	 memcpy(buff,PAGEBUFF+startpos+4,4);
	 int snum=*(int*)&buff;
	 RID ridx;
	 ridx.pageNum=pnum;
	 ridx.slotNum=snum;
	 int x=readTuple(tableName,ridx,data);
	 return -1;
  }

memset(recout,0,sizeof(recout));
memcpy(recout,PAGEBUFF+startpos,reclen);
memcpy(data,recout,sizeof(recout));
delete pf;
delete pfh;
return 0;
}

RC RM::readAttribute(const char *tableName, const RID &rid, 
		       const char *attributeName, void *data)
{
  fstream fp1,fp2;
  char tem_buff[50];
  char	attr[50];
  char table[100],buff[100];
  int val,j,i=0,slt,len1,ptr,counter=0;
  strcpy(table,tableName);

  int pgno=rid.pageNum;
  int slno=rid.slotNum;
  RID rid1;
  rid1=rid;

  memset(PAGEBUFF,0,sizeof(PAGEBUFF));
  PF_Manager *pf=new PF_Manager();
  PF_FileHandle *pfh=new PF_FileHandle();
  pf->OpenFile(tableName,*pfh);
  pfh->ReadPage(pgno-1,PAGEBUFF);

  memset(tem_buff,0,sizeof(tem_buff));
  memset(buff,0,sizeof(buff));

  int length,bytestomove=0,bytestoread=0,k,flag=0,startpos,varlength=0;
  char tab[100],cat[100],mid[100];
  memset(tab,0,sizeof(tab));
  memcpy(tab,tableName,strlen(tableName));
  memset(cat,0,sizeof(cat));
  memcpy(cat,"catalog",7);
  memset(mid,0,sizeof(mid));
  memcpy(mid,"_",1);
  strcat(tab,mid);
  strcat(tab,cat);
  fp1.open(tab,ios::in);
  int p1,p2;

while(flag==0)
{
  startpos=fp1.tellp();
  memset(attr,0,sizeof(attr));
  fp1.read(tem_buff,2);
  length=atoi(tem_buff);
  memset(tem_buff,0,sizeof(tem_buff));
  fp1.read(tem_buff,1);
  k=atoi(tem_buff);
  memset(tem_buff,0,sizeof(tem_buff));
  fp1.read(tem_buff,2);
  memset(tem_buff,0,sizeof(tem_buff));
  fp1.read(tem_buff,strlen(attributeName));
  memcpy(attr,tem_buff,strlen(tem_buff));
  memset(tem_buff,0,sizeof(tem_buff));

  if(strcmp(attributeName,attr)==0)
  {
	  flag=1;
	  if(k==0 || k==1 )
	  {
	   bytestoread=4;
	  }/*
	  else if(k==2)
	  {
			fp1.read(buff,1);
			p1=atoi(buff);
			fp1.read(buff,p1);
			p2=atoi(buff);
		    bytestoread=p2;
	  }*/
	  if(k==3 || k==2)
	  {
		 bytestoread=0;
	  }
  }
  else
  {
	  fp1.seekp(length+startpos);
	  if(k==0 || k==1)
	  {
		  bytestomove+=4;
	  }
	  else if(k==3 || k==2)
	  {
		 varlength=find_length(table,bytestomove,rid1);
		 bytestomove+=4+varlength;
	  }

  }

}

fp1.close();

	j=PF_PAGE_SIZE-12;
  int ct3=PF_PAGE_SIZE-12;
  while(PAGEBUFF[j]!=0)
  {
  	j=j-12;
  	counter++;
  }

  while(i<counter)
  {
  	   	memcpy(buff,PAGEBUFF+ct3,4);
  	  	slt=*(int*)&buff;
  	  	memset(buff,0,sizeof(buff));
  	  	memcpy(buff,PAGEBUFF+ct3+4,4);
  	  	ptr=*(int*)&buff;
  	  	memset(buff,0,sizeof(buff));
  	  	memcpy(buff,PAGEBUFF+ct3+8,4);
  	  	ct3-=12;
  	  	len1=*(int*)&buff;
  	  	memset(buff,0,sizeof(buff));
  	  	if(slno==slt)
  	  	{
  	  		break;
  	  	}
  	  	i++;
  }

int ct=ptr+bytestomove;

if(k==0)
{
	memcpy(tem_buff,PAGEBUFF+ct,4);
	ct+=4;
	val=*(int*)&tem_buff;
	memcpy(buff,&val,sizeof(val));

}
if(k==1)
{

	memcpy(tem_buff,PAGEBUFF+ct,4);
	ct+=4;
	float fval;
	fval=*(float*)&tem_buff;
	memcpy(buff,&fval,sizeof(fval));

}

if(k==3 || k==2)
{
	memcpy(tem_buff,PAGEBUFF+ct,4);
	ct+=4;
	bytestoread=*(int*)&tem_buff;
	memset(tem_buff,0,sizeof(tem_buff));
	memcpy(tem_buff,PAGEBUFF+ct,bytestoread);
	ct+=bytestoread;
	memcpy(buff,tem_buff,strlen(tem_buff));

}

memcpy(data,buff,sizeof(buff));

delete pf;
delete pfh;
return -1;
}


RC RM::readAttribute(const char *tableName, const RID &rid,
		       const char *attributeName,int &b, void *data)
{
  fstream fp1,fp2;
  char tem_buff[50];
  char	attr[50];
  char table[100],buff[100];
  int val,j,i=0,slt,len1,ptr,counter=0;
  float fval;
  strcpy(table,tableName);
  RID rid1;
  rid1=rid;
  int pgno=rid1.pageNum;
  int slno=rid1.slotNum;

  memset(PAGEBUFF,0,sizeof(PAGEBUFF));
  PF_Manager *pf=new PF_Manager();
  PF_FileHandle *pfh=new PF_FileHandle();
  pf->OpenFile(tableName,*pfh);
  pfh->ReadPage(pgno-1,PAGEBUFF);

  memset(tem_buff,0,sizeof(tem_buff));
  memset(buff,0,sizeof(buff));

  int length,bytestomove=0,bytestoread=0,k,flag=0,startpos,varlength=0;
  char tab[100],cat[100],mid[100];
  memset(tab,0,sizeof(tab));
  memcpy(tab,tableName,strlen(tableName));
  memset(cat,0,sizeof(cat));
  memcpy(cat,"catalog",7);
  memset(mid,0,sizeof(mid));
  memcpy(mid,"_",1);
  strcat(tab,mid);
  strcat(tab,cat);

  fp1.open(tab,ios::in);

while(flag==0)
{
  startpos=fp1.tellp();
  memset(attr,0,sizeof(attr));
  fp1.read(tem_buff,2);
  length=atoi(tem_buff);
  memset(tem_buff,0,sizeof(tem_buff));
  fp1.read(tem_buff,1);
  k=atoi(tem_buff);
  memset(tem_buff,0,sizeof(tem_buff));
  fp1.read(tem_buff,2);
  memset(tem_buff,0,sizeof(tem_buff));
  fp1.read(tem_buff,strlen(attributeName));
  memcpy(attr,tem_buff,strlen(tem_buff));
  memset(tem_buff,0,sizeof(tem_buff));

  if(strcmp(attributeName,attr)==0)
  {
	  flag=1;
	  if(k==0 || k==1 )
	  {
	   bytestoread=4;

	  }
	  //else if(k==2)
		//  bytestoread=1;
	  if(k==3 || k==2)
	  {
		 bytestoread=0;
	  }
  }
  else
  {
	  fp1.seekp(length+startpos);
	  if(k==0 || k==1)
	  {
		  bytestomove+=4;
	  }
	  else if(k==3 || k==2)
	  {
		 varlength=find_length(table,bytestomove,rid1);
		 bytestomove+=4+varlength;
	  }

  }

}

fp1.close();

	j=PF_PAGE_SIZE-12;
  int ct3=PF_PAGE_SIZE-12;
  while(PAGEBUFF[j]!=0)
  {
  	j=j-12;
  	counter++;
  }

  while(i<counter)
  {
  	   	memcpy(buff,PAGEBUFF+ct3,4);
  	  	slt=*(int*)&buff;
  	  	memset(buff,0,sizeof(buff));
  	  	memcpy(buff,PAGEBUFF+ct3+4,4);
  	  	ptr=*(int*)&buff;
  	  	memset(buff,0,sizeof(buff));
  	  	memcpy(buff,PAGEBUFF+ct3+8,4);
  	  	ct3-=12;
  	  	len1=*(int*)&buff;
  	  	memset(buff,0,sizeof(buff));
  	  	if(slno==slt)
  	  	{
  	  		break;
  	  	}
  	  	i++;
  }

int ct=ptr+bytestomove;

if(k==0)
{
	memcpy(tem_buff,PAGEBUFF+ct,4);
	ct+=4;
	b=4;
	val=*(int*)&tem_buff;
	memcpy(buff,&val,sizeof(val));

}
if(k==1)
{

	memcpy(tem_buff,PAGEBUFF+ct,4);
	ct+=4;
	b=4;
	fval=*(float*)&tem_buff;
	memcpy(buff,&fval,sizeof(fval));

}

if(k==3 || k==2)
{
	memcpy(tem_buff,PAGEBUFF+ct,4);
	ct+=4;
	bytestoread=*(int*)&tem_buff;
	b=bytestoread+4;
	memcpy(tem_buff+4,PAGEBUFF+ct,bytestoread);
	ct+=bytestoread;
	memcpy(buff,tem_buff,100);
}

memcpy(data,buff,sizeof(buff));

delete pf;
delete pfh;
return -1;
}


RC RM::find_length(const char *tableName,int b,RID &rid)
{
	int len,temp;
	fstream fp;
	char buff[50];
	memset(buff,0,sizeof(buff));
	for(int i=0;i<sizeof(p[rid.pageNum].slotid);i++)
	{
		  if(rid.slotNum==p[rid.pageNum].slotid[i])
		  {
			  temp=i;
			  break;
		  }
	}
	fp.open(tableName,ios::in);
	fp.seekp(p[rid.pageNum].ptr[temp]+b);
	fp.read(buff,4);
	len=*(int*)&buff;
	fp.close();
	return len;
}


RC RM::reorganizePage(const char *tableName, const unsigned pageNumber)
{
	PF_Manager *pf=new PF_Manager();
	PF_FileHandle *pfh=new PF_FileHandle();
	memset(PAGEBUFF,0,sizeof(PAGEBUFF));
	pf->OpenFile(tableName,*pfh);
	pfh->ReadPage(pageNumber-1,PAGEBUFF);

	int slt,ptr,len,nl=inc,i=0,j,counter=0,start,end,pslt;
	char buff[100],NEWBUFF[PF_PAGE_SIZE];
	memset(buff,0,sizeof(buff));
	memset(NEWBUFF,0,sizeof(NEWBUFF));
	int ct=PF_PAGE_SIZE-12;
	int ct1=PF_PAGE_SIZE-12;

	j=PF_PAGE_SIZE-12;
	while(PAGEBUFF[j]!=0)
	{
		j=j-12;
		counter++;
	}

while(i<counter)
{
	l1:;
	memcpy(buff,PAGEBUFF+ct,4);
	slt=*(int*)&buff;
	memset(buff,0,sizeof(buff));
	memcpy(buff,PAGEBUFF+ct+4,4);
	ptr=*(int*)&buff;
	memset(buff,0,sizeof(buff));
	memcpy(buff,PAGEBUFF+ct+8,4);
	ct-=12;
	len=*(int*)&buff;

	memset(buff,0,sizeof(buff));
	if(len==0 || len==-1)
	{
		i++;
		goto l1;

	}

	memcpy(NEWBUFF+nl,PAGEBUFF+ptr,len);
	nl+=len;
	memcpy(NEWBUFF+ct1,&slt,4);
	memcpy(NEWBUFF+ct1+4,&nl,4);
	memcpy(NEWBUFF+ct1+8,&len,4);
	start=nl;
	end=ct1;
	pslt=slt;
	ct1-=12;
	i++;
}
p[pgid].freespace=(struct node*)malloc(sizeof(struct node));
p[pgid].freespace->start=start;
p[pgid].freespace->end=end;
p[pgid].freespace->prevslot=pslt;
p[pgid].freespace->nxt=NULL;
p[pgid].head=p[pgid].freespace;

p[pgid].temp=p[pgid].head;
	  	int q=0;;
	  	memset(NEWBUFF,0,inc);
	  	while(p[pgid].temp!=NULL)
	  	{

	  	   	memcpy(NEWBUFF+q,&p[pgid].temp->start,2);
	  	   	q+=2;
	  	   	memcpy(NEWBUFF+q,&p[pgid].temp->end,2);
	  	   	q+=2;
	  	   	memcpy(NEWBUFF+q,&p[pgid].temp->prevslot,2);
	  	   	q+=2;

	  		p[pgid].temp=p[pgid].temp->nxt;
	  	}

	pfh->WritePage(pageNumber-1,NEWBUFF);
    delete pf;
    delete pfh;

    return -1;
}


int end;



RC RM::scan(const char *tableName, 
	  const vector<char *> &attributeNames,
	  RM_ScanIterator &rm_ScanIterator)
{

PF_Manager *pf=new PF_Manager();
PF_FileHandle *pfh=new PF_FileHandle();
pf->OpenFile(tableName,*pfh);
strcpy(rm_ScanIterator.tname,tableName);
int gn=pfh->GetNumberOfPages();
rm_ScanIterator.no_pages=gn;

for(int l=1;l<=gn;l++)
{
	rm_ScanIterator.p[l]=l;
}

int slt,i=0,j,k,counter=0;

for(int m=1;m<=gn;m++)
{
memset(PAGEBUFF,0,sizeof(PAGEBUFF));
pfh->ReadPage(rm_ScanIterator.p[m]-1,PAGEBUFF);
i=0,k=1,counter=0;
int ct=PF_PAGE_SIZE-12;
char buff[100];
memset(buff,0,sizeof(buff));

j=PF_PAGE_SIZE-12;
while(PAGEBUFF[j]!=0)
{
	j=j-12;
	counter++;
}

rm_ScanIterator.no_rec_in_page[rm_ScanIterator.p[m]]=counter;
i=0;
while(i<counter)
{
	memcpy(buff,PAGEBUFF+ct,4);
	slt=*(int*)&buff;
	rm_ScanIterator.pslots[k]=slt;
	memset(buff,0,sizeof(buff));
	ct-=12;
	i++;
	k++;
}
}


rm_ScanIterator.attnames=attributeNames;
RID rid;
rid.pageNum=1;
rid.slotNum=rm_ScanIterator.pslots[1];

rm_ScanIterator.nrid.pageNum=1;
rm_ScanIterator.nrid.slotNum=rm_ScanIterator.pslots[1];
rm_ScanIterator.end1=0;


char buff[100];
memset(buff,0,sizeof(buff));


int a=readTuple(tableName,rid,buff);
vector<char*>::iterator it1;
int len;
for(it1=rm_ScanIterator.attnames.begin();it1<rm_ScanIterator.attnames.end();it1++)
{
	memset(buff,0,sizeof(buff));
	//int x=readAttribute(rm_ScanIterator.tname,rid,*it1,len,buff);
	cout<<"   ";

}
cout<<endl;

delete pfh;
delete pf;
return -1;
}




RC RM_ScanIterator::getNextTuple(RID &rid, void *data)
{

	if(end1==1)
	{
		int a=close();
		return a;

	}

	int pno=nrid.pageNum;
	int slno=nrid.slotNum;


	rid.pageNum=nrid.pageNum;
	rid.slotNum=nrid.slotNum;

	vector<char*>::iterator it1;
		RM *rm;//=new RM();

		char buff[100],buff4[100];
		memset(buff,0,sizeof(buff));
		memset(buff4,0,sizeof(buff4));
		int ct=0,len=0;
		for(it1=attnames.begin();it1<attnames.end();it1++)
		{
			ct=ct+len;
			int x=rm->readAttribute(tname,nrid,*it1,len,buff);
			memcpy(buff4+ct,buff,len);
		}
		memcpy(data,buff4,sizeof(buff4));



	int k;

	for(int j=1;j<=no_pages;j++)
	{
	for (int i=1;i<no_rec_in_page[p[j]];i++)
	{
		if(slno==pslots[i])
		{
			k=i;
			nrid.slotNum=pslots[k+1];
			nrid.pageNum=p[j];
			break;
		}
	}

	}



	//	rid.pageNum=nrid.pageNum;
	//	rid.slotNum=nrid.slotNum;

	if(slno==pslots[no_rec_in_page[no_pages]])
			{
					end1=1;

			}
	return 0;

}



RC RM_ScanIterator::close()
{
	//cout<<"End of file"<<endl;
	end1=0;
	return RM_EOF;
}

// extra credit
RC RM::dropAttribute(const char *tableName, const char *attributeName)
{
  return -1;
}

RC RM::addAttribute(const char *tableName, 
		      const char *attributeName,
		      const AttrType attributeType,
		      const AttrLength attributeLength)
{
  return -1;
}

RC RM::reorganizeTable(const char *tableName)
{
  return -1;
}

RC RM::scan(const char *tableName,
	  const char *conditionAttribute,
	  const CompOp compOp, // comparision type such as "<" and "="
	  const void *value, // used in the comparison
	  const vector<char *> &attributeNames, // a list of projected attributes
	  RM_ScanIterator &rm_ScanIterator)
{
  return -1;
}

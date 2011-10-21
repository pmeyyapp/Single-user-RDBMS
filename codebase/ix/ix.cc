#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstdio>

#include "ix.h"

//IX_MANAGER CLASS
IX_Manager::IX_Manager()
{
open=0;
}
IX_Manager::~IX_Manager()
{

}


int gnum=0;
int pagenum,flag1=0,initialcase=1,HEIGHT=0;
char PGBUFF[PF_PAGE_SIZE];

//leaf node page
unsigned short no_entries,sibl,par,level;
char rnl;


RC IX_Manager::CreateIndex(const char *tableName,const char *attributeName)
{
PF_Manager *pf=new PF_Manager();
RC rc;

char tab[100],cat[100],mid[100];
memset(tab,0,sizeof(tab));
memcpy(tab,tableName,strlen(tableName));
memset(cat,0,sizeof(cat));
memcpy(cat,attributeName,strlen(attributeName));
memset(mid,0,sizeof(mid));
memcpy(mid,"_",1);
strcat(tab,mid);
strcat(tab,cat);

rc=pf->CreateFile(tab);
open=1;
//if(rc==0)
{
delete pf;
return 0;
}
/*
else
{
	delete pf;
	IX_PrintError(1);
	return 1;
}*/

}


RC IX_Manager::DestroyIndex(const char *tableName,const char *attributeName)
{
	PF_Manager *pf=new PF_Manager();
	fstream fp;
//	fp.open(attributeName,ios::in | ios::out | ios::binary);

	char tab[100],cat[100],mid[100];
	memset(tab,0,sizeof(tab));
	memcpy(tab,tableName,strlen(tableName));
	memset(cat,0,sizeof(cat));
	memcpy(cat,attributeName,strlen(attributeName));
	memset(mid,0,sizeof(mid));
	memcpy(mid,"_",1);
	strcat(tab,mid);
	strcat(tab,cat);

	if(open==1)
	{
	pf->DestroyFile(tab);
	open=0;
	delete pf;
	return 0;
	}
	else
	{
		delete pf;
		IX_PrintError(2);
		return 2;
	}
}

RC IX_Manager::OpenIndex(const char *tableName,const char *attributeName,IX_IndexHandle &indexHandle)
{

	char tab[100],cat[100],mid[100];
	memset(tab,0,sizeof(tab));
	memcpy(tab,tableName,strlen(tableName));
	memset(cat,0,sizeof(cat));
	memcpy(cat,attributeName,strlen(attributeName));
	memset(mid,0,sizeof(mid));
	memcpy(mid,"_",1);
	strcat(tab,mid);
	strcat(tab,cat);

	if(!indexHandle.fp.is_open() && open==1 )
	{

	indexHandle.fp.open(tab,ios::in|ios::out|ios::binary);
	indexHandle.done=1;

	flag1=0;

	strcpy(indexHandle.tname,tableName);
	strcpy(indexHandle.att_name,tab);

	int startpos,length,type,alen,clen,namelen;

	char buff[PF_PAGE_SIZE],tem_buff[PF_PAGE_SIZE];
	int flag=0;

	char tab[100],cat[100],mid[100];
		memset(tab,0,sizeof(tab));
		memcpy(tab,tableName,strlen(tableName));
		memset(cat,0,sizeof(cat));
		memcpy(cat,"catalog",7);
		memset(mid,0,sizeof(mid));
		memcpy(mid,"_",1);
		strcat(tab,mid);
		strcat(tab,cat);


		fstream fp1;
		fp1.open(tab,ios::in);

	while(flag==0)
	{
	  memset(buff,0,sizeof(buff));
	  memset(tem_buff,0,sizeof(tem_buff));
	  startpos=fp1.tellp();
	  fp1.read(tem_buff,2);
	  length=atoi(tem_buff);
	  memset(tem_buff,0,sizeof(tem_buff));
	  fp1.read(tem_buff,1);
	  type=atoi(tem_buff);
	  fp1.read(tem_buff,2);
	  namelen=atoi(tem_buff);
	  memset(tem_buff,0,sizeof(tem_buff));
	  fp1.read(tem_buff,strlen(attributeName));
	  memcpy(buff,tem_buff,strlen(tem_buff));
	  memset(tem_buff,0,sizeof(tem_buff));
	  if(strcmp(attributeName,buff)==0)
	  {

		  flag=1;
		  if(type==0 || type==1 )
		  {
		   alen=4;
		  }
		  if(type==2)
		  {
			 fp1.read(tem_buff,1);
			 clen=atoi(tem_buff);
			 memset(tem_buff,0,sizeof(tem_buff));
			 fp1.read(tem_buff,clen);
			 alen=atoi(tem_buff);
		  }
	  }
	  else
	  {
		  fp1.seekp(length+startpos);
	  }
	}

	fp1.close();
	indexHandle.att_type=type;
	indexHandle.att_length=alen;



	if(indexHandle.att_type==TypeInt || indexHandle.att_type==TypeReal)
	{
	indexHandle.leafnodesize=12;
	indexHandle.nonleafnodesize=8;
	indexHandle.maxentries_l=((PF_PAGE_SIZE-7)/indexHandle.leafnodesize);
	indexHandle.maxentries_nl=((PF_PAGE_SIZE-9)/indexHandle.nonleafnodesize)-2;
	}
	if(indexHandle.att_type==TypeChar)
	{
	indexHandle.leafnodesize=indexHandle.att_length+8;
	indexHandle.nonleafnodesize=indexHandle.att_length+4;
	indexHandle.maxentries_l=((PF_PAGE_SIZE-7)/indexHandle.leafnodesize);
	indexHandle.maxentries_nl=((PF_PAGE_SIZE-9)/indexHandle.nonleafnodesize);
	}
	return 0;
	}

else
{
	IX_PrintError(3);
	return 3;
}
}

RC IX_Manager::CloseIndex(IX_IndexHandle &indexHandle)
{
if(indexHandle.fp.is_open())
{
indexHandle.fp.close();
indexHandle.done=0;

return 0;
}
else
{
IX_PrintError(4);
return 4;
}
}

void IX_Manager::setRM(RM &rm)
{
	rm1=rm;
}


//IX_INDEXHANDLE CLASS
IX_IndexHandle::IX_IndexHandle()
{
memset(tname,0,sizeof(tname));
memset(att_name,0,sizeof(att_name));
done=0;
}

IX_IndexHandle::~IX_IndexHandle ()
{

}


int search(char PGBUFF[],int no_entries,void *newkey,int att_length,int att_type)
{
int k=0,temp=0,mid=0,f1=0,nkey;
float fkey,t1;
char tarr2[PF_PAGE_SIZE],ckey[100],temp1[100];

if(att_type==TypeInt)
{
memset(tarr2,0,sizeof(tarr2));
memcpy(tarr2,newkey,att_length);
nkey=*(int*)&tarr2;

for(int i=0;i<no_entries;i++)
{
	memset(tarr2,0,sizeof(tarr2));
	memcpy(tarr2,PGBUFF+k,att_length);
	temp=*(int*)&tarr2;
	if(nkey<temp)
	{
		mid=k;
		f1=1;
		break;
	}
	k=k+12;
}
if(f1==0)
{
	mid=k;
}
}


if(att_type==TypeReal)
{
memset(tarr2,0,sizeof(tarr2));
memcpy(tarr2,newkey,att_length);
fkey=*(float*)&tarr2;

for(int i=0;i<no_entries;i++)
{
	memset(tarr2,0,sizeof(tarr2));
	memcpy(tarr2,PGBUFF+k,att_length);
	t1=*(float*)&tarr2;
	if(fkey<t1)
	{
		mid=k;
		f1=1;
		break;
	}
	k=k+12;
}
if(f1==0)
{
	mid=k;
}
}


if(att_type==TypeChar)
{
memset(tarr2,0,sizeof(tarr2));
memcpy(tarr2,newkey,att_length);
memset(ckey,0,sizeof(ckey));
memcpy(ckey,tarr2,att_length);

for(int i=0;i<no_entries;i++)
{
	memset(tarr2,0,sizeof(tarr2));
	memcpy(tarr2,PGBUFF+k,att_length);
	//temp=*(int*)&tarr2;
	memcpy(temp1,tarr2,att_length);

	if(strcmp(ckey,temp1)==-1)
	{

		mid=k;
		f1=1;
		break;
	}
	k=k+att_length+8;
}
if(f1==0)
{
	mid=k;

}
}
return mid;
}

int delsearch(char PGBUFF[],int no_entries,void *newkey,int att_length,int att_type)
{
int k=0,temp=0,mid=0,f1=0,nkey;
float fkey,t1;
char tarr2[PF_PAGE_SIZE],ckey[100],temp1[100];

if(att_type==TypeInt)
{
memset(tarr2,0,sizeof(tarr2));
memcpy(tarr2,newkey,att_length);
nkey=*(int*)&tarr2;

for(int i=0;i<no_entries;i++)
{
	memset(tarr2,0,sizeof(tarr2));
	memcpy(tarr2,PGBUFF+k,att_length);
	temp=*(int*)&tarr2;
	if(nkey==temp)
	{
		mid=k;
		f1=1;
		break;
	}
	k=k+12;
}
if(f1==0)
{
	mid=1982;
}
}


if(att_type==TypeReal)
{
memset(tarr2,0,sizeof(tarr2));
memcpy(tarr2,newkey,att_length);
fkey=*(float*)&tarr2;

for(int i=0;i<no_entries;i++)
{
	memset(tarr2,0,sizeof(tarr2));
	memcpy(tarr2,PGBUFF+k,att_length);
	t1=*(float*)&tarr2;
	if(fkey==t1)
	{
		mid=k;
		f1=1;
		break;
	}
	k=k+12;
}
if(f1==0)
{
	mid=1982;
}
}


if(att_type==TypeChar)
{
memset(tarr2,0,sizeof(tarr2));
memcpy(tarr2,newkey,att_length);
memset(ckey,0,sizeof(ckey));
memcpy(ckey,tarr2,att_length);

for(int i=0;i<no_entries;i++)
{
	memset(tarr2,0,sizeof(tarr2));
	memcpy(tarr2,PGBUFF+k,att_length);
	//temp=*(int*)&tarr2;
	memcpy(temp1,tarr2,att_length);
	if(strcmp(ckey,temp1)==0)
	{

		mid=k;
		f1=1;
		break;
	}
	k=k+att_length+8;
}
if(f1==0)
{
	mid=1982;

}
}
return mid;
}


int nonleafsearch(char PGBUFF[],int no_entries,void *newkey,int att_length,int att_type)
{
int k=4,temp=0,mid=0,f1=0,nkey;
float fkey,t1;
char tarr2[PF_PAGE_SIZE],ckey[100],temp1[100];

if(att_type==TypeInt)
{
	memset(tarr2,0,sizeof(tarr2));
	memcpy(tarr2,newkey,att_length);
	nkey=*(int*)&tarr2;
for(int i=0;i<no_entries;i++)
{
	memset(tarr2,0,sizeof(tarr2));
	memcpy(tarr2,PGBUFF+k,att_length);
	temp=*(int*)&tarr2;
	if(nkey<temp)
	{
		mid=k;
		f1=1;
		break;
	}
	k=k+8;
}
if(f1==0)
{
	mid=k;
}
}


if(att_type==TypeReal)
{
	memset(tarr2,0,sizeof(tarr2));
	memcpy(tarr2,newkey,att_length);
	fkey=*(float*)&tarr2;
	for(int i=0;i<no_entries;i++)
	{
	memset(tarr2,0,sizeof(tarr2));
	memcpy(tarr2,PGBUFF+k,att_length);
	t1=*(float*)&tarr2;
	if(fkey<t1)
	{
		mid=k;
		f1=1;
		break;
	}
	k=k+8;
}
if(f1==0)
{
	mid=k;
}
}


if(att_type==TypeChar)
{
	memset(tarr2,0,sizeof(tarr2));
	memcpy(tarr2,newkey,att_length);
	memcpy(ckey,tarr2,att_length);
	for(int i=0;i<no_entries;i++)
	{
	memset(tarr2,0,sizeof(tarr2));
	memcpy(tarr2,PGBUFF+k,att_length);
	memcpy(temp1,tarr2,att_length);
	if(strcmp(ckey,temp1)==-1)
	{
		mid=k;
		f1=1;
		break;
	}
	k=k+4+att_length;
}
if(f1==0)
{
	mid=k;
}
}
return mid;
}


RC IX_IndexHandle::InsertEntry(void *key, const RID &rid)
{
char tarr[PF_PAGE_SIZE],tarr1[PF_PAGE_SIZE],rtnode[PF_PAGE_SIZE];
char nkey[100];
memset(nkey,0,sizeof(nkey));

if(att_type==0 || att_type==1)
memcpy(nkey,key,sizeof(nkey));
if(att_type==2)
memcpy(nkey,(char*)key+4,sizeof(nkey));


//memset(tarr,0,sizeof(tarr));
//memcpy(tarr,nkey,att_length);
//cout<<"nkey"<<tarr<<"len"<<att_length<<endl;
//cout<<"keyyyy valllue"<<*(int*)&tarr<<endl;

memset(tarr,0,sizeof(tarr));
memcpy(tarr,key,50);

int newptr;
int sept=0;


//unsigned int n=pfsh->GetNumberOfPages();
//pagenum=(int)n;

if(flag1==0)//if(pagenum==0)
{
PF_Manager *pf5=new PF_Manager();
PF_FileHandle *pfh5=new PF_FileHandle();
pf5->OpenFile(att_name,*pfh5);
pagenum=0;
flag1=1;
no_entries=0;
level=0;
sibl=500;
par=-1;
memset(PGBUFF,0,sizeof(PGBUFF));
memcpy(PGBUFF+(sizeof(PGBUFF)-1),"L",1);
memcpy(PGBUFF+(sizeof(PGBUFF)-3),&no_entries,2);
memcpy(PGBUFF+(sizeof(PGBUFF)-5),&sibl,2);
memcpy(PGBUFF+(sizeof(PGBUFF)-7),&par,2);
pfh5->WritePage(0,PGBUFF);
pagenum++;
gnum=pagenum;


delete pf5;
delete pfh5;
}

PF_Manager *pf6=new PF_Manager();
PF_FileHandle *pfh6=new PF_FileHandle();
pf6->OpenFile(att_name,*pfh6);

memset(PGBUFF,0,sizeof(PGBUFF));
pfh6->ReadPage(pagenum-1,PGBUFF);
delete pf6;
delete pfh6;

//FIRST CASE FOR INITIAL ROOT
if(PGBUFF[sizeof(PGBUFF)-1]=='L' && initialcase==1)
{
	PF_Manager *pf7=new PF_Manager();
	PF_FileHandle *pfh7=new PF_FileHandle();
	pf7->OpenFile(att_name,*pfh7);
	memset(PGBUFF,0,sizeof(PGBUFF));
	pfh7->ReadPage(pagenum-1,PGBUFF);

	memset(tarr,0,sizeof(tarr));
	memcpy(tarr,PGBUFF+(sizeof(PGBUFF)-3),2);
	no_entries=*(short*)&tarr;
	memset(tarr,0,sizeof(tarr));
	memcpy(tarr,PGBUFF+(sizeof(PGBUFF)-5),2);
	sibl=*(short*)&tarr;
	memset(tarr,0,sizeof(tarr));
	memcpy(tarr,PGBUFF+(sizeof(PGBUFF)-7),2);
	par=*(short*)&tarr;


	if(no_entries<maxentries_l)
	{
		newptr=no_entries*leafnodesize;
		int temp,mid;

		mid=delsearch(PGBUFF,no_entries,nkey,att_length,att_type);
		if(mid!=1982)
		{
			l5:;
			char buff5[100];
			RID ridnew;
			memset(buff5,0,sizeof(buff5));
			memcpy(buff5,PGBUFF+mid+att_length,4);
			ridnew.pageNum=*(int*)&buff5;
			memset(buff5,0,sizeof(buff5));
			memcpy(buff5,PGBUFF+mid+att_length+4,4);
			ridnew.slotNum=*(int*)&buff5;

			if(rid.pageNum==ridnew.pageNum && rid.slotNum==ridnew.slotNum)
			{
			IX_PrintError(5);
			delete pf7;
			delete pfh7;
			return 5;
			}
			else
			{
				memset(buff5,0,sizeof(buff5));
				memcpy(buff5,PGBUFF+mid+att_length+8,4);
				if(att_type==0)
				{
					int num1=*(int*)&buff5;
					int num2=*(int*)&nkey;
					if(num1==num2)
					{
						mid=mid+att_length+8;
						goto l5;
					}
				}

				if(att_type==1)
				{
					float num1=*(float*)&buff5;
					float num2=*(float*)&nkey;
					if(num1==num2)
					{
						mid=mid+att_length+8;
						goto l5;
					}
				}

				if(att_type==2)
				{
					if(strcmp(buff5,nkey)==0)
					{
						mid=mid+att_length+8;
						goto l5;
					}
				}



			}
		}
		mid=0;
		mid=search(PGBUFF,no_entries,nkey,att_length,att_type);

		memset(tarr,0,sizeof(tarr));
		memcpy(tarr,PGBUFF,mid);
		memcpy(tarr+mid,nkey,att_length);
		memcpy(tarr+mid+att_length,&rid.pageNum,4);
		memcpy(tarr+mid+att_length+4,&rid.slotNum,4);
		no_entries++;

		if(mid!=newptr)
		{
		memcpy(tarr+mid+att_length+8,PGBUFF+mid,newptr-mid);
		}
		memset(PGBUFF,0,sizeof(PGBUFF));
		memcpy(PGBUFF,tarr,sizeof(PGBUFF));

		memcpy(PGBUFF+(sizeof(PGBUFF)-1),"L",1);
		memcpy(PGBUFF+(sizeof(PGBUFF)-3),&no_entries,2);
		memcpy(PGBUFF+(sizeof(PGBUFF)-5),&sibl,2);
		memcpy(PGBUFF+(sizeof(PGBUFF)-7),&par,2);
		pfh7->WritePage(gnum-1,PGBUFF);

		delete pf7;
		delete pfh7;

		return 0;
	}
	else
	{
		int mid=0;
		mid=delsearch(PGBUFF,no_entries,nkey,att_length,att_type);
		if(mid!=1982)
		{
		l6:;
		char buff5[100];
		RID ridnew;
		memset(buff5,0,sizeof(buff5));
		memcpy(buff5,PGBUFF+mid+att_length,4);
		ridnew.pageNum=*(int*)&buff5;
		memset(buff5,0,sizeof(buff5));
		memcpy(buff5,PGBUFF+mid+att_length+4,4);
		ridnew.slotNum=*(int*)&buff5;

		if(rid.pageNum==ridnew.pageNum && rid.slotNum==ridnew.slotNum)
		{
			IX_PrintError(5);
		delete pf7;
		delete pfh7;
		return 5;
		}
		else
		{
			memset(buff5,0,sizeof(buff5));
			memcpy(buff5,PGBUFF+mid+att_length+8,4);
			if(att_type==0)
			{
				int num1=*(int*)&buff5;
				int num2=*(int*)&nkey;
				if(num1==num2)
				{
					mid=mid+att_length+8;
					goto l6;
				}
			}

			if(att_type==1)
			{
				float num1=*(float*)&buff5;
				float num2=*(float*)&nkey;
				if(num1==num2)
				{
					mid=mid+att_length+8;
					goto l6;
				}
			}

			if(att_type==2)
			{
				if(strcmp(buff5,nkey)==0)
				{
					mid=mid+att_length+8;
					goto l6;
				}
			}

		}
		}

		HEIGHT=1;
		initialcase=2;
		sept=1;
		par=gnum;
		//pagenum=pfh->GetNumberOfPages();
		//cout<<"pggg"<<pagenum<<endl;
		gnum++;
		int lc=gnum;
		gnum++;
		int rc=gnum;
		int nptr=no_entries*leafnodesize;
		int rval;

		mid=search(PGBUFF,no_entries,nkey,att_length,att_type);

		char tarr3[PF_PAGE_SIZE];
		memset(tarr3,0,sizeof(tarr3));
		memcpy(tarr3,PGBUFF,mid);
		memcpy(tarr3+mid,nkey,att_length);
		memcpy(tarr3+mid+att_length,&rid.pageNum,4);
		memcpy(tarr3+mid+att_length+4,&rid.slotNum,4);
		if(mid!=nptr)
		{
		memcpy(tarr3+mid+att_length+8,PGBUFF+mid,nptr-mid);
		}
		//memset(PGBUFF,0,sizeof(PGBUFF));
		//memcpy(PGBUFF,tarr1,sizeof(tarr1));

		char temp5[100],temp4[100],PGBUF1[PF_PAGE_SIZE],LEFT[PF_PAGE_SIZE],RIGHT[PF_PAGE_SIZE];
		memset(PGBUF1,0,sizeof(PGBUF1));
		memcpy(PGBUF1,tarr3,sizeof(tarr3));

		no_entries++;
		int endofrec=no_entries*leafnodesize;
		int n2=no_entries/2;
		int n3=no_entries-n2;
		newptr=n2*leafnodesize;

		memset(LEFT,0,sizeof(LEFT));
		memset(RIGHT,0,sizeof(RIGHT));
		memcpy(LEFT,PGBUF1,newptr);//left node
		memcpy(RIGHT,PGBUF1+newptr,endofrec-newptr);//right node


		/*
		if(mid<newptr)
		{
			n2++;
		}
		if(mid>=newptr)
		{
			n3++;
		}*/
		memcpy(LEFT+(sizeof(LEFT)-1),"L",1);
		memcpy(LEFT+(sizeof(LEFT)-3),&n2,2);
		sibl=rc;
		memcpy(LEFT+(sizeof(LEFT)-5),&sibl,2);
		memcpy(LEFT+(sizeof(LEFT)-7),&par,2);
		pfh7->WritePage(lc-1,LEFT);

		memcpy(RIGHT+(sizeof(RIGHT)-1),"L",1);
		memcpy(RIGHT+(sizeof(RIGHT)-3),&n3,2);
		sibl=500;
		memcpy(RIGHT+(sizeof(RIGHT)-5),&sibl,2);
		memcpy(RIGHT+(sizeof(RIGHT)-7),&par,2);
		pfh7->WritePage(rc-1,RIGHT);

		memset(tarr1,0,sizeof(tarr1));
		memcpy(tarr1,RIGHT,att_length);
		memset(PGBUFF,0,sizeof(PGBUFF));
		memcpy(PGBUFF,&lc,4);
		memcpy(PGBUFF+4,tarr1,att_length);
		memcpy(PGBUFF+4+att_length,&rc,4);
		no_entries=1;
		int rtparnt=-1;
		memcpy(PGBUFF+(sizeof(PGBUFF)-1),"R",1);
		memcpy(PGBUFF+(sizeof(PGBUFF)-3),&no_entries,2);
		memcpy(PGBUFF+(sizeof(PGBUFF)-7),&rtparnt,2);
		pfh7->WritePage(0,PGBUFF);

		delete pf7;
		delete pfh7;
		return 0;
	}
	delete pf7;
	delete pfh7;
	return 0;

}

//RECURSION THRU NON LEAF NODES

int newnodeptr;
char newchild[100];
memset(newchild,0,sizeof(newchild));

int a;
if(initialcase!=1 && sept==0)
{
	//pfx->OpenFile(att_name,*pfsh);
a=insert(1,nkey,rid,newchild,newnodeptr);
return a;
}
///LEAF NODES
return 0;
}



int f=0;


RC IX_IndexHandle::insert(int pgnode,void *key,const RID &rid,char newchild[],int &newnodeptr)
{

PF_Manager *pf1=new PF_Manager();
PF_FileHandle *pfh1=new PF_FileHandle();
pf1->OpenFile(att_name,*pfh1);

char tarr[PF_PAGE_SIZE],tarr1[PF_PAGE_SIZE],rtnode[PF_PAGE_SIZE];
char lf[20],nlf[20],rt[20];
memset(lf,0,sizeof(lf));
memset(nlf,0,sizeof(nlf));
memset(rt,0,sizeof(rt));
memcpy(lf,"L",1);
memcpy(nlf,"N",1);
memcpy(rt,"R",1);
int mid,newptr,newpg;
int newkey;
memset(tarr,0,sizeof(tarr));

memcpy(tarr,key,50);
newkey=*(int*)&tarr;	//change

memset(PGBUFF,0,sizeof(PGBUFF));
pfh1->ReadPage(pgnode-1,PGBUFF);

//memcpy(newbuff,PGBUFF,PF_PAGE_SIZE);
memset(tarr,0,sizeof(tarr));
memcpy(tarr,PGBUFF+(sizeof(PGBUFF)-1),1);
delete pf1;
delete pfh1;

//IN CASE OF NON LEAF OR ROOT NODE
if(strcmp(tarr,rt)==0 || strcmp(tarr,nlf)==0)
{

PF_Manager *pf2=new PF_Manager();
PF_FileHandle *pfh2=new PF_FileHandle();
pf2->OpenFile(att_name,*pfh2);
char newbuff[PF_PAGE_SIZE],abc[PF_PAGE_SIZE];
memset(abc,0,sizeof(abc));
memcpy(abc,tarr,sizeof(tarr));
int pgfornonleaf=pgnode;
memset(newbuff,0,sizeof(newbuff));
pfh2->ReadPage(pgnode-1,newbuff);
memset(tarr,0,sizeof(tarr));
memcpy(tarr,newbuff+(sizeof(PGBUFF)-3),2);
int no_entries1=*(short*)&tarr;
memset(tarr,0,sizeof(tarr));
memcpy(tarr,newbuff+(sizeof(PGBUFF)-5),2);
int par1=*(short*)&tarr;
mid=nonleafsearch(newbuff,no_entries1,key,att_length,att_type);

memset(tarr,0,sizeof(tarr));
memcpy(tarr,newbuff+mid-4,4);
newpg=*(int*)&tarr;
memset(tarr,0,sizeof(tarr));
memcpy(tarr,newbuff+mid,4);


memset(tarr,0,sizeof(tarr));
pfh2->ReadPage(newpg-1,tarr);

memset(tarr1,0,sizeof(tarr1));
memcpy(tarr1,tarr,4);

int a=insert(newpg,key,rid,newchild,newnodeptr);

if(a==5)
{
	return 5;
}

char ntemp[100];
memset(ntemp,0,sizeof(ntemp));

if(strcmp(newchild,ntemp)==0)
{

}
else
{

	if(no_entries1<maxentries_nl)
	{
		newptr=no_entries1*nonleafnodesize+4;
		//int k=0,temp,mid,f1=0;
		mid=nonleafsearch(newbuff,no_entries1,newchild,att_length,att_type);
		memset(tarr,0,sizeof(tarr));
		memcpy(tarr,newbuff,mid);
		if(mid==4)
		{
			memcpy(tarr+mid-4,&newpg,4);
			memcpy(tarr+mid,newchild,att_length);
			memcpy(tarr+mid+att_length,&newnodeptr,4);
		}
		else
		{
			memcpy(tarr+mid,newchild,att_length);
			memcpy(tarr+mid+att_length,&newnodeptr,4);
		}

		//newchild shud be changed

		if(mid!=newptr)
		{
			//cout<<"mid"<<mid<<"  newptr"<<newptr<<endl;
		memcpy(tarr+mid+att_length+4,newbuff+mid,newptr-mid);
		}
		no_entries1++;
		memset(newbuff,0,sizeof(newbuff));
		memcpy(newbuff,tarr,sizeof(tarr));
		if(HEIGHT<2)
		{

		memcpy(newbuff+(sizeof(newbuff)-1),"R",1);
		}
		else
		{
			if(strcmp(abc,rt)==0)
				memcpy(newbuff+(sizeof(newbuff)-1),"R",1);
			else
				memcpy(newbuff+(sizeof(newbuff)-1),"N",1);
		}
		memcpy(newbuff+(sizeof(newbuff)-3),&no_entries1,2);
		memcpy(newbuff+(sizeof(newbuff)-5),&par,2);

		pfh2->WritePage(pgfornonleaf-1,newbuff);


		memset(newbuff,0,sizeof(newbuff));
		pfh2->ReadPage(pgfornonleaf-1,newbuff);
		memset(tarr,0,sizeof(tarr));
		memcpy(tarr,newbuff+4,4);

		memset(newchild,0,sizeof(newchild));
		newnodeptr=0;

	//	delete pfx;
		//delete pfhx;


	}
	else
	{
		if(strcmp(abc,rt)==0)
		{

			int ln,rn;
			int endptr=(no_entries1*nonleafnodesize)+4;
			ln=++gnum;
			rn=++gnum;
			mid=0;

			mid=nonleafsearch(newbuff,no_entries1,newchild,att_length,att_type);
			memset(tarr1,0,sizeof(tarr1));
			memcpy(tarr1,newbuff,sizeof(tarr1));

			if(mid==4)
				{
					memcpy(tarr1+mid-4,&newpg,4);
					memcpy(tarr1+mid,newchild,att_length);
					memcpy(tarr1+mid+att_length,&newnodeptr,4);
				}
				else
				{
					memcpy(tarr1+mid,newchild,att_length);
					memcpy(tarr1+mid+att_length,&newnodeptr,4);
				}

				if(mid!=endptr)
				{
				memcpy(tarr1+mid+att_length+4,newbuff+mid,endptr-mid);
				}
			memset(newbuff,0,sizeof(newbuff));
			memcpy(newbuff,tarr1,sizeof(tarr1));
			no_entries1++;
			endptr=(no_entries1*nonleafnodesize)+4;
			int nl2=no_entries1/2;
			int nl3=no_entries1-nl2;
			int nlptr=(nl2*nonleafnodesize)+4;
			memset(tarr,0,sizeof(tarr));
			memset(rtnode,0,sizeof(rtnode));
			memcpy(tarr,newbuff,nlptr);
			memcpy(rtnode,newbuff+nlptr-4,endptr-nlptr+4);


			/*
			if(newchild<rval)//search at left node
			{
			mid=nonleafsearch(tarr,nl2,newchild);
			memset(tarr1,0,sizeof(tarr));
			memcpy(tarr1,tarr,mid);
			if(mid==4)
			{
				memcpy(tarr1+mid-4,&newpg,4);
				memcpy(tarr1+mid,&newchild,4);
				memcpy(tarr1+mid+4,&newnodeptr,4);
			}
			else
			{
				memcpy(tarr1+mid,&newchild,4);
				memcpy(tarr1+mid+4,&newnodeptr,4);
			}
			nl2++;
			if(mid!=nlptr)
			{
			memcpy(tarr1+mid+8,tarr+mid,nlptr-mid);
			}
			memset(tarr,0,sizeof(tarr));
		 	memcpy(tarr,tarr1,sizeof(tarr));
			}*/
			par=pgfornonleaf;
			memcpy(tarr+(sizeof(tarr)-1),"N",1);
			memcpy(tarr+(sizeof(tarr)-3),&nl2,2);
			memcpy(tarr+(sizeof(tarr)-7),&par,2);
			pfh2->WritePage(ln-1,tarr);

			/*
			if(newchild>=rval)//search at left node
			{
			mid=nonleafsearch(rtnode,nl3,newchild);
			memset(tarr1,0,sizeof(tarr));
			memcpy(tarr1,rtnode,mid);
			if(mid==4)
			{
				memcpy(tarr1+mid-4,&newpg,4);
				memcpy(tarr1+mid,&newchild,4);
				memcpy(tarr1+mid+4,&newnodeptr,4);
			}
			else
			{
				memcpy(tarr1+mid,&newchild,4);
				memcpy(tarr1+mid+4,&newnodeptr,4);
			}
			nl3++;
			if(mid!=nlptr)
			{
			memcpy(tarr1+mid+8,tarr+mid,nlptr-mid);
			}
			memset(tarr,0,sizeof(tarr));
		 	memcpy(rtnode,tarr1,sizeof(tarr));
			}*/
			par=pgfornonleaf;
			memset(tarr1,0,sizeof(tarr1));
			memcpy(tarr1,rtnode+4,att_length);
			memcpy(newchild,tarr1,att_length);

				memset(tarr1,0,sizeof(tarr1));
				memcpy(tarr1,rtnode+4+att_length,sizeof(tarr1));
				memcpy(rtnode,tarr1,sizeof(tarr1));

			memcpy(rtnode+(sizeof(rtnode)-1),"N",1);
			memcpy(rtnode+(sizeof(rtnode)-3),&nl3,2);
			memcpy(rtnode+(sizeof(rtnode)-7),&par,2);
			pfh2->WritePage(rn-1,rtnode);
			//new root

			char buff[PF_PAGE_SIZE];
			memset(buff,0,sizeof(buff));
			memcpy(buff,&ln,4);
			memcpy(buff+4,newchild,att_length);
			memcpy(buff+4+att_length,&rn,4);
			int no_entries3=1;
			int rtparnt1=-1;
			memcpy(buff+(sizeof(buff)-1),"R",1);
			memcpy(buff+(sizeof(buff)-3),&no_entries3,2);
			memcpy(buff+(sizeof(buff)-7),&rtparnt1,2);
			pfh2->WritePage(0,buff);
			HEIGHT=2;

		}


		if(strcmp(abc,nlf)==0)
		{
		int ln,rn;
		ln=pgfornonleaf;
		rn=++gnum;
		int endptr=(no_entries1*nonleafnodesize)+4;
		memset(tarr1,0,sizeof(tarr1));
		memcpy(tarr1,newbuff,sizeof(tarr1));
		mid=nonleafsearch(tarr1,no_entries1,newchild,att_length,att_type);

		if(mid==4)
		{
			memcpy(tarr1+mid-4,&newpg,4);
			memcpy(tarr1+mid,newchild,att_length);
			memcpy(tarr1+mid+att_length,&newnodeptr,4);
		}
		else
		{
			memcpy(tarr1+mid,newchild,att_length);
			memcpy(tarr1+mid+att_length,&newnodeptr,4);
		}

		if(mid!=endptr)
		{
		memcpy(tarr1+mid+att_length+4,newbuff+mid,endptr-mid);
		}
	memset(newbuff,0,sizeof(newbuff));
	memcpy(newbuff,tarr1,sizeof(tarr1));
	no_entries1++;

		int nl2=no_entries1/2;
		int nl3=no_entries1-nl2;
		int nlptr=(nl2*nonleafnodesize)+4;
		endptr=(no_entries1*nonleafnodesize)+4;
		memset(tarr,0,sizeof(tarr));
		memset(rtnode,0,sizeof(rtnode));
		memcpy(tarr,newbuff,nlptr);
		memcpy(rtnode,newbuff+nlptr-4,endptr-nlptr+4);

		/*
		if(newchild<rval)//search at left node
		{
		mid=nonleafsearch(tarr,nl2,newchild);
		memset(tarr1,0,sizeof(tarr));
		memcpy(tarr1,tarr,mid);
		if(mid==4)
		{
			memcpy(tarr1+mid-4,&newpg,4);
			memcpy(tarr1+mid,&newchild,4);
			memcpy(tarr1+mid+4,&newnodeptr,4);
		}
		else
		{
			memcpy(tarr1+mid,&newchild,4);
			memcpy(tarr1+mid+4,&newnodeptr,4);
		}
		nl2++;
		if(mid!=nlptr)
		{
		memcpy(tarr1+mid+8,tarr+mid,nlptr-mid);
		}
		memset(tarr,0,sizeof(tarr));
	 	memcpy(tarr,tarr1,sizeof(tarr));
		}*/
		par=pgfornonleaf;
		memcpy(tarr+(sizeof(tarr)-1),"N",1);
		memcpy(tarr+(sizeof(tarr)-3),&nl2,2);
		memcpy(tarr+(sizeof(tarr)-7),&par,2);
		pfh2->WritePage(ln-1,tarr);

		/*
		if(newchild>=rval)//search at right node
		{
		mid=nonleafsearch(rtnode,nl3,newchild);
		memset(tarr1,0,sizeof(tarr));
		memcpy(tarr1,rtnode,mid);
		if(mid==4)
		{
			memcpy(tarr1+mid-4,&newpg,4);
			memcpy(tarr1+mid,&newchild,4);
			memcpy(tarr1+mid+4,&newnodeptr,4);
		}
		else
		{
			memcpy(tarr1+mid,&newchild,4);
			memcpy(tarr1+mid+4,&newnodeptr,4);
		}
		nl3++;
		if(mid!=nlptr)
		{
		memcpy(tarr1+mid+8,rtnode+mid,nlptr-mid);
		}
		memset(tarr,0,sizeof(tarr));
	 	memcpy(rtnode,tarr1,sizeof(tarr));
		}*/
		par=pgfornonleaf;

		memset(tarr1,0,sizeof(tarr1));
		memcpy(tarr1,rtnode+4,att_length);
		memcpy(newchild,tarr1,att_length);

		memset(tarr1,0,sizeof(tarr1));
		memcpy(tarr1,rtnode+4+att_length,sizeof(tarr1));
		memcpy(rtnode,tarr1,sizeof(tarr1));
		memcpy(rtnode+(sizeof(rtnode)-1),"N",1);
		memcpy(rtnode+(sizeof(rtnode)-3),&nl3,2);
		memcpy(rtnode+(sizeof(rtnode)-7),&par,2);
		pfh2->WritePage(rn-1,rtnode);


		newnodeptr=rn;
		}

	}
}
delete pf2;
delete pfh2;
return 0;
}

if(strcmp(tarr,lf)==0)
{
PF_Manager *pf3=new PF_Manager();
PF_FileHandle *pfh3=new PF_FileHandle();
pf3->OpenFile(att_name,*pfh3);
//pf->OpenFile(att_name,*pfh);

char leafbuff[PF_PAGE_SIZE];
memset(leafbuff,0,sizeof(leafbuff));
pfh3->ReadPage(pgnode-1,leafbuff);

memset(tarr,0,sizeof(tarr));
memcpy(tarr,leafbuff,4);
memset(tarr,0,sizeof(tarr));
memcpy(tarr,leafbuff+(sizeof(leafbuff)-3),2);
int no_entries2=*(short*)&tarr;
memset(tarr,0,sizeof(tarr));
memcpy(tarr,leafbuff+(sizeof(leafbuff)-5),2);
sibl=*(short*)&tarr;
memset(tarr,0,sizeof(tarr));
memcpy(tarr,leafbuff+(sizeof(leafbuff)-7),2);
par=*(short*)&tarr;


if(no_entries2<maxentries_l)
{
	newptr=no_entries2*leafnodesize;
	//int k=0,temp,mid,f1=0;

	mid=delsearch(leafbuff,no_entries2,key,att_length,att_type);
	if(mid!=1982)
	{
		l7:;
		char buff5[100];
		RID ridnew;
		memset(buff5,0,sizeof(buff5));
		memcpy(buff5,leafbuff+mid+att_length,4);
		ridnew.pageNum=*(int*)&buff5;
		memset(buff5,0,sizeof(buff5));
		memcpy(buff5,leafbuff+mid+att_length+4,4);
		ridnew.slotNum=*(int*)&buff5;

		if(rid.pageNum==ridnew.pageNum && rid.slotNum==ridnew.slotNum)
		{
			IX_PrintError(5);
		delete pf3;
		delete pfh3;
		return 5;
		}
		else
		{
			memset(buff5,0,sizeof(buff5));
			memcpy(buff5,leafbuff+mid+att_length+8,4);
			if(att_type==0)
			{
				int num1=*(int*)&buff5;
				memset(buff5,0,sizeof(buff5));
				memcpy(buff5,key,sizeof(buff5));
				int num2=*(int*)&buff5;
				if(num1==num2)
				{
					mid=mid+att_length+8;
					goto l7;
				}
			}

			if(att_type==1)
			{
				float num1=*(float*)&buff5;
				memset(buff5,0,sizeof(buff5));
				memcpy(buff5,key,sizeof(buff5));
				float num2=*(float*)&buff5;
				if(num1==num2)
				{
					mid=mid+att_length+8;
					goto l7;
				}
			}

			if(att_type==2)
			{
				char buff6[100];
				memset(buff6,0,sizeof(buff6));
				memcpy(buff6,key,sizeof(buff6));

				if(strcmp(buff5,buff6)==0)
				{
					mid=mid+att_length+8;
					goto l7;
				}
			}

		}
	}
	mid=search(leafbuff,no_entries2,key,att_length,att_type);
	memset(tarr,0,sizeof(tarr));
	memcpy(tarr,leafbuff,mid);
	memcpy(tarr+mid,key,att_length);
	memcpy(tarr+mid+att_length,&rid.pageNum,4);
	memcpy(tarr+mid+att_length+4,&rid.slotNum,4);
	no_entries2++;
	if(mid!=newptr)
	{
		//cout<<"mid"<<mid<<"  newptr"<<newptr<<endl;
	memcpy(tarr+mid+att_length+8,leafbuff+mid,newptr-mid);
	}

	memset(leafbuff,0,sizeof(leafbuff));
	memcpy(leafbuff,tarr,sizeof(leafbuff));
	memcpy(leafbuff+(sizeof(leafbuff)-1),"L",1);
	memcpy(leafbuff+(sizeof(leafbuff)-3),&no_entries2,2);
	memcpy(leafbuff+(sizeof(leafbuff)-5),&sibl,2);
	memcpy(leafbuff+(sizeof(leafbuff)-7),&par,2);
	pfh3->WritePage(pgnode-1,leafbuff);



	memset(newchild,0,sizeof(newchild));
	//newchild=0;
	//newnodeptr=0;


	//delete pf;
	//delete pfh;

}
else
{

	mid=delsearch(leafbuff,no_entries2,key,att_length,att_type);
	if(mid!=1982)
	{
		l8:;
		char buff5[100];
		RID ridnew;
		memset(buff5,0,sizeof(buff5));
		memcpy(buff5,leafbuff+mid+att_length,4);
		ridnew.pageNum=*(int*)&buff5;
		memset(buff5,0,sizeof(buff5));
		memcpy(buff5,leafbuff+mid+att_length+4,4);
		ridnew.slotNum=*(int*)&buff5;

		if(rid.pageNum==ridnew.pageNum && rid.slotNum==ridnew.slotNum)
		{
			IX_PrintError(5);
		delete pf3;
		delete pfh3;
		return 5;
		}

		else
		{
			memset(buff5,0,sizeof(buff5));
			memcpy(buff5,leafbuff+mid+att_length+8,4);
			if(att_type==0)
			{
				int num1=*(int*)&buff5;
				memset(buff5,0,sizeof(buff5));
				memcpy(buff5,key,sizeof(buff5));
				int num2=*(int*)&buff5;
				if(num1==num2)
				{
					mid=mid+att_length+8;
					goto l8;
				}
			}

				if(att_type==1)
				{
					float num1=*(float*)&buff5;
					memset(buff5,0,sizeof(buff5));
					memcpy(buff5,key,sizeof(buff5));
					float num2=*(float*)&buff5;
					if(num1==num2)
					{
						mid=mid+att_length+8;
						goto l8;
					}
				}

				if(att_type==2)
				{
					char buff6[100];
					memset(buff6,0,sizeof(buff6));
					memcpy(buff6,key,sizeof(buff6));

					if(strcmp(buff5,buff6)==0)
					{
						mid=mid+att_length+8;
						goto l8;
					}
				}
			}

	}

	par=pgnode;
	int rc=++gnum;
	memset(tarr,0,sizeof(tarr));
	memset(rtnode,0,sizeof(rtnode));
	int nptr=no_entries2*leafnodesize;


	mid=search(leafbuff,no_entries2,key,att_length,att_type);

	char tarr2[PF_PAGE_SIZE];

	memset(tarr2,0,sizeof(tarr2));
	memcpy(tarr2,leafbuff,mid);
	memcpy(tarr2+mid,key,att_length);
	memcpy(tarr2+mid+att_length,&rid.pageNum,4);
	memcpy(tarr2+mid+att_length+4,&rid.slotNum,4);

	if(mid!=nptr)
	{
	memcpy(tarr2+mid+att_length+8,leafbuff+mid,nptr-mid);
	}

	//memset(leafbuff,0,sizeof(leafbuff));
	//memcpy(leafbuff,tarr1,sizeof(tarr1));


	no_entries2++;
	int endofrec=no_entries2*leafnodesize;
	int n2=no_entries2/2;
	int n3=no_entries2-n2;
	newptr=n2*leafnodesize;

	memcpy(tarr,tarr2,newptr);//left node
	memcpy(rtnode,tarr2+newptr,endofrec-newptr);//right node

	memset(tarr1,0,sizeof(tarr1));
	memcpy(tarr1,rtnode,att_length);
	memset(newchild,0,sizeof(newchild));
	float n1;
	if(att_type==1)
	{
		n1=*(float*)&tarr1;
		memcpy(newchild,&n1,att_length);
	}
	else
	{
	memcpy(newchild,rtnode,att_length);
	}
	memcpy(tarr+(sizeof(tarr)-1),"L",1);
	memcpy(tarr+(sizeof(tarr)-3),&n2,2);
	int var=sibl;
	sibl=rc;
	memcpy(tarr+(sizeof(tarr)-5),&sibl,2);
	memcpy(tarr+(sizeof(tarr)-7),&par,2);
	pfh3->WritePage(pgnode-1,tarr);

	memcpy(rtnode+(sizeof(rtnode)-1),"L",1);
	memcpy(rtnode+(sizeof(rtnode)-3),&n3,2);
	sibl=var;
	memcpy(rtnode+(sizeof(rtnode)-5),&sibl,2);
	memcpy(rtnode+(sizeof(rtnode)-7),&par,2);
	pfh3->WritePage(rc-1,rtnode);

	newnodeptr=rc;
	//delete pf;
	//delete pfh;

}
delete pf3;
delete pfh3;
return 0;
}
//delete pfx;
//delete pfhx;
}


RC IX_IndexHandle::DeleteEntry(void *key, const RID &rid)
{
	int pgnode=1,flag;
	char nkey[500];
	memset(nkey,0,sizeof(nkey));

	if(att_type==0 || att_type==1)
	{
		memcpy(nkey,key,sizeof(nkey));
	}
	if(att_type==2)
	{
		memcpy(nkey,(char*)key+4,sizeof(nkey));
	}


	int z=deletekey(pgnode,nkey,rid,flag);
	IX_PrintError(z);
	return z;
}

RC IX_IndexHandle::deletekey(int pgnode,void *key,const RID &rid,int &flag)
{
	char tarr[PF_PAGE_SIZE],tarr1[PF_PAGE_SIZE],temp[PF_PAGE_SIZE],tarr2[PF_PAGE_SIZE];
	char lf[20],nlf[20],rt[20];
	memset(lf,0,sizeof(lf));
	memset(nlf,0,sizeof(nlf));
	memset(rt,0,sizeof(rt));
	memcpy(lf,"L",1);
	memcpy(nlf,"N",1);
	memcpy(rt,"R",1);
	int mid,newptr,newpg,lptr,rptr,no_entries1,par,sib;

	PF_Manager *pf=new PF_Manager();
	PF_FileHandle *pfh=new PF_FileHandle();
	pf->OpenFile(att_name,*pfh);
	memset(PGBUFF,0,sizeof(PGBUFF));
	pfh->ReadPage(pgnode-1,PGBUFF);
	memset(tarr,0,sizeof(tarr));
	memset(temp,0,sizeof(temp));
	memcpy(tarr,PGBUFF+(sizeof(PGBUFF)-1),1);
	memcpy(temp,PGBUFF+(sizeof(PGBUFF)-3),2);
	no_entries1=*(int*)&temp;

	memset(temp,0,sizeof(temp));
	memcpy(temp,PGBUFF+(sizeof(PGBUFF)-5),2);
	sib=*(int*)&temp;
	memset(temp,0,sizeof(temp));
	memcpy(temp,PGBUFF+(sizeof(PGBUFF)-7),2);
	par=*(int*)&temp;


	if(strcmp(tarr,rt)==0 || strcmp(tarr,nlf)==0)
	{
	flag=0;
	char tarr3[PF_PAGE_SIZE];
	memset(tarr3,0,sizeof(tarr3));
	memset(temp,0,sizeof(temp));
	int	mid=nonleafsearch(PGBUFF,no_entries1,key,att_length,att_type);

	memcpy(tarr3,PGBUFF,sizeof(PGBUFF));
	memcpy(temp,PGBUFF+mid-4,4);
	lptr=*(int*)&temp;

	if(lptr==-1)
	{

		return 6;
	}

	int z=deletekey(lptr,key,rid,flag);
	if(z==6)
	{
		return 6;
	}

	if (flag==2)
	{
		int del=-1;
		memcpy(tarr3+mid-4,&del,4);
		pfh->WritePage(pgnode-1,tarr3);
		flag=0;


	}
	delete pf;
	delete pfh;
	}



	if(strcmp(tarr,lf)==0)
	{
	int mid=delsearch(PGBUFF,no_entries1,key,att_length,att_type);

	if(mid==1982)
	{
		//cout<<"sry, value not found"<<endl;
		return 6;
	}
	else
	{
		char buff7[100];
		RID ridold;

		delstart:;
		memset(buff7,0,sizeof(buff7));
		memcpy(buff7,PGBUFF+mid+att_length,4);
		ridold.pageNum=*(int*)&buff7;
		memset(buff7,0,sizeof(buff7));
		memcpy(buff7,PGBUFF+mid+att_length+4,4);
		ridold.slotNum=*(int*)&buff7;
		if(rid.pageNum==ridold.pageNum && rid.slotNum==ridold.slotNum)
		{
			goto start;
		}
		else
		{
			mid=mid+att_length+8;
			goto delstart;
		}


		start:;
		memset(temp,0,sizeof(temp));
		float x;
		memcpy(temp,PGBUFF+mid,att_length);
		x=*(float*)&temp;

		memset(temp,0,sizeof(temp));
		memcpy(temp,PGBUFF+mid+att_length,4);
		int pg=*(int*)&temp;
		memset(temp,0,sizeof(temp));
		memcpy(temp,PGBUFF+mid+att_length+4,4);
		int slot=*(int*)&temp;

		memset(temp,0,sizeof(temp));
		memcpy(temp,PGBUFF,mid);

		int ptr=no_entries1*leafnodesize;


		if((ptr-mid)!=(att_length+8))
		{

		memcpy(temp+mid,PGBUFF+mid+att_length+8,(ptr-mid)-(att_length+8));
		}

		if(no_entries1==1)
		{
			memset(temp,0,sizeof(temp));
		}
		no_entries1=no_entries1-1;

		//rid.pageNum=-1;
		//rid.slotNum=-1;
		if(no_entries1==0)
		{
		flag=2;
		}

		memset(PGBUFF,0,sizeof(PGBUFF));
		memcpy(PGBUFF,temp,sizeof(temp));
		memcpy(PGBUFF+(sizeof(PGBUFF)-1),"L",1);
		memcpy(PGBUFF+(sizeof(PGBUFF)-3),&no_entries1,2);
		memcpy(PGBUFF+(sizeof(PGBUFF)-5),&sib,2);
		memcpy(PGBUFF+(sizeof(PGBUFF)-7),&par,2);
		pfh->WritePage(pgnode-1,PGBUFF);
		delete pf;
		delete pfh;
		return 0;

	}
	delete pf;
	delete pfh;
	}
delete pf;
delete pfh;
return 0;
}


//ix_indexscan class
IX_IndexScan::IX_IndexScan()
{
	memset(RECORDS,0,sizeof(RECORDS));
	memset(att_name1,0,sizeof(att_name1));
	scan=0;
}
IX_IndexScan::~IX_IndexScan()
{

}

RC IX_IndexScan::OpenScan(const IX_IndexHandle &indexHandle, CompOp compOp,void *value)
{

	if(scan==1)
	{
	//	return 7;
	}
	if(indexHandle.done==0)
	{
		IX_PrintError(8);
		return 8;
	}
	else
	{
		scan=1;
	}
	int pg;
	int a=scanfirstpage(1,pg,indexHandle);


	char page[PF_PAGE_SIZE],temp[PF_PAGE_SIZE],nvalue[500];
	memset(page,0,sizeof(page));
	memset(temp,0,sizeof(temp));

	scanSTART:;
	memset(nvalue,0,sizeof(nvalue));
	if(compOp!=NO_OP)
	{
	if(indexHandle.att_type==0 || indexHandle.att_type==0)
	memcpy(nvalue,value,sizeof(nvalue));
	if(indexHandle.att_type==2)
	memcpy(nvalue,(char*)value+4,sizeof(nvalue));
	}


	PF_Manager *pf=new PF_Manager();
	PF_FileHandle *pfh=new PF_FileHandle();
	pf->OpenFile(indexHandle.att_name,*pfh);

	sibling=pg;
	l3:;
	pfh->ReadPage(sibling-1,page);
	memcpy(temp,page+sizeof(page)-3,2);

	no_entry=*(int*)&temp;
	memset(temp,0,sizeof(temp));
	memcpy(temp,page+sizeof(page)-5,2);
	sibling=*(int*)&temp;
	if(no_entry==0)
	{
		goto l3;
	}


	if(compOp==LT_OP || compOp==NO_OP)
	{
	memset(temp,0,sizeof(temp));
	memcpy(temp,page,indexHandle.att_length);
	memset(key,0,sizeof(key));
	memcpy(key,temp,indexHandle.att_length);
	memset(temp,0,sizeof(temp));
	memcpy(temp,page+indexHandle.att_length,4);
	ridscan.pageNum=*(int*)&temp;
	memset(temp,0,sizeof(temp));
	memcpy(temp,page+indexHandle.att_length+4,4);
	ridscan.slotNum=*(int*)&temp;
	curr=1;
	op=compOp;
	}

	if(compOp==LE_OP)
	{
		char temp1[PF_PAGE_SIZE];
		memset(temp1,0,sizeof(temp1));
		memcpy(temp1,page,sizeof(page));
		int mid=delsearch(temp1,no_entry,nvalue,indexHandle.att_length,indexHandle.att_type);

		while(mid==1982 && sibling!=500)
		{
			l5:;
			pfh->ReadPage(sibling-1,temp1);
			memcpy(temp,temp1+sizeof(temp1)-3,2);
			no_entry=*(int*)&temp;
			memset(temp,0,sizeof(temp));
			memcpy(temp,temp1+sizeof(temp1)-5,2);
			sibling=*(int*)&temp;
			if(no_entry==0)
			{
				goto l5;
			}
			mid=delsearch(temp1,no_entry,nvalue,indexHandle.att_length,indexHandle.att_type);
		}

		if(mid==1982)
		{

			compOp=LT_OP;
			delete pf;
			delete pfh;
			goto scanSTART;
		}


	memset(temp,0,sizeof(temp));
	memcpy(temp,page,indexHandle.att_length);
	memset(key,0,sizeof(key));
	memcpy(key,temp,indexHandle.att_length);
	memset(temp,0,sizeof(temp));
	memcpy(temp,page+indexHandle.att_length,4);
	ridscan.pageNum=*(int*)&temp;
	memset(temp,0,sizeof(temp));
	memcpy(temp,page+indexHandle.att_length+4,4);
	ridscan.slotNum=*(int*)&temp;
	curr=1;
	}


	if(compOp==GE_OP)
	{
	int mid=delsearch(page,no_entry,nvalue,indexHandle.att_length,indexHandle.att_type);
	while(mid==1982 && sibling!=500)
	{
		l1:;
		pfh->ReadPage(sibling-1,page);
		memcpy(temp,page+sizeof(page)-3,2);
		no_entry=*(int*)&temp;
		memset(temp,0,sizeof(temp));
		memcpy(temp,page+sizeof(page)-5,2);
		sibling=*(int*)&temp;
		if(no_entry==0)
		{
			goto l1;
		}
		mid=delsearch(page,no_entry,nvalue,indexHandle.att_length,indexHandle.att_type);
	}

	if(mid==1982)
	{
		compOp=GT_OP;
		delete pf;
		delete pfh;
		goto scanSTART;
	}
	memset(temp,0,sizeof(temp));
	memcpy(temp,page+mid,indexHandle.att_length);
	memset(key,0,sizeof(key));
	memcpy(key,temp,indexHandle.att_length);
	memset(temp,0,sizeof(temp));
	memcpy(temp,page+mid+indexHandle.att_length,4);
	ridscan.pageNum=*(int*)&temp;
	memset(temp,0,sizeof(temp));
	memcpy(temp,page+mid+indexHandle.att_length+4,4);
	ridscan.slotNum=*(int*)&temp;
	curr=(mid/indexHandle.leafnodesize)+1;
	}


	if(compOp==EQ_OP)
		{
		int mid=delsearch(page,no_entry,nvalue,indexHandle.att_length,indexHandle.att_type);

		while(mid==1982 && sibling!=500)
		{
			v9:;
			pfh->ReadPage(sibling-1,page);
			memcpy(temp,page+sizeof(page)-3,2);
			no_entry=*(int*)&temp;
			memset(temp,0,sizeof(temp));
			memcpy(temp,page+sizeof(page)-5,2);
			sibling=*(int*)&temp;
			if(no_entry==0)
			{
				goto v9;
			}
			mid=delsearch(page,no_entry,nvalue,indexHandle.att_length,indexHandle.att_type);
		}

		if(mid==1982)
		{
			IX_PrintError(10);
			return 10;

		}
		memset(temp,0,sizeof(temp));
		memcpy(temp,page+mid,indexHandle.att_length);
		memset(key,0,sizeof(key));
		memcpy(key,temp,indexHandle.att_length);
		memset(temp,0,sizeof(temp));
		memcpy(temp,page+mid+indexHandle.att_length,4);
		ridscan.pageNum=*(int*)&temp;
		memset(temp,0,sizeof(temp));
		memcpy(temp,page+mid+indexHandle.att_length+4,4);
		ridscan.slotNum=*(int*)&temp;
		curr=(mid/indexHandle.leafnodesize)+1;
		}


	if(compOp==GT_OP)
	{
	int end=no_entry*indexHandle.leafnodesize;
	int mid=search(page,no_entry,nvalue,indexHandle.att_length,indexHandle.att_type);
	while(mid==end)
	{
		l2:;
		memset(page,0,sizeof(page));
		pfh->ReadPage(sibling-1,page);
		memcpy(temp,page+sizeof(page)-3,2);
		no_entry=*(int*)&temp;
		memset(temp,0,sizeof(temp));
		memcpy(temp,page+sizeof(page)-5,2);
		sibling=*(int*)&temp;
		if(no_entry==0)
		{
			goto l2;
		}
		end=no_entry*indexHandle.leafnodesize;
		mid=search(page,no_entry,nvalue,indexHandle.att_length,indexHandle.att_type);

	}
	memset(temp,0,sizeof(temp));
	memcpy(temp,page+mid,indexHandle.att_length);
	memset(key,0,sizeof(key));
	memcpy(key,temp,indexHandle.att_length);
	memset(temp,0,sizeof(temp));
	memcpy(temp,page+mid+indexHandle.att_length,4);
	ridscan.pageNum=*(int*)&temp;
	memset(temp,0,sizeof(temp));
	memcpy(temp,page+mid+indexHandle.att_length+4,4);
	ridscan.slotNum=*(int*)&temp;
	curr=(mid/indexHandle.leafnodesize)+1;
	}



	memset(val,0,sizeof(val));
	memcpy(val,nvalue,indexHandle.att_length);


	memset(RECORDS,0,sizeof(RECORDS));
	memcpy(RECORDS,page,PF_PAGE_SIZE);


	op=compOp;
	memcpy(att_name1,indexHandle.att_name,sizeof(att_name1));
	att_type1=indexHandle.att_type;
	att_len=indexHandle.att_length;
	size=indexHandle.leafnodesize;

	memset(temp,0,sizeof(temp));
	memcpy(temp,key,sizeof(temp));


	return 0;
	delete pf;
	delete pfh;
}

int flag2=0;


RC IX_IndexScan::GetNextEntry(RID &rid)
{
rid.pageNum=ridscan.pageNum;
rid.slotNum=ridscan.slotNum;

char buff[PF_PAGE_SIZE],temp[PF_PAGE_SIZE];

memset(buff,0,sizeof(buff));

int size1;

//FLOAT
if(att_type1==TypeReal)
{
float k1,v1;
memcpy(buff,key,sizeof(buff));
k1=*(float*)&buff;

memset(buff,0,sizeof(buff));
memcpy(buff,val,sizeof(buff));
v1=*(float*)&buff;

switch(op)
{
case NO_OP:
	if(flag2==0)
	{
	Iterate(flag2);
	return 0;
	}
	else
	{
		//IX_PrintError(9);
		return -1;
	}
	break;


case EQ_OP:
	if(k1==v1 && flag2==0)
	{
		Iterate(flag2);
		return 0;
	}
	else
	{

	//IX_PrintError(9);
	return -1;
	}
	break;


case LT_OP:
if(k1<v1 && flag2==0)
{
	Iterate(flag2);
	return 0;
}
else
{

//IX_PrintError(9);
return -1;
}
break;


case LE_OP:
if(k1<=v1 && flag2==0)
{

	Iterate(flag2);
	return 0;
}
else
{

//IX_PrintError(9);
return -1;
}
break;


case GE_OP:
if(k1>=v1 && flag2==0)
{
	Iterate(flag2);
	return 0;
}
else
{
flag2=0;

//IX_PrintError(9);
return -1;
}
break;


case GT_OP:
if(k1>v1 && flag2==0)
{
	Iterate(flag2);
	return 0;
}
else
{

//IX_PrintError(9);
return -1;
}
break;
}
}


//INTEGER
if(att_type1==TypeInt)
{
int k1,v1;
memcpy(buff,key,sizeof(buff));
k1=*(int*)&buff;

memset(buff,0,sizeof(buff));
memcpy(buff,val,sizeof(buff));
v1=*(int*)&buff;


switch(op)
{

case NO_OP:
	if(flag2==0)
	{
	Iterate(flag2);
	return 0;
	}
	else
	{
		//IX_PrintError(9);
		return -1;
	}
	break;

case EQ_OP:
	if(k1==v1 && flag2==0)
	{
		Iterate(flag2);
		return 0;
	}
	else
	{

	//IX_PrintError(9);
	return -1;
	}
	break;

case LT_OP:
if(k1<v1 && flag2==0)
{
	Iterate(flag2);
	return 0;
}
else
{
	//IX_PrintError(9);
return -1;
}
break;


case LE_OP:
if(k1<=v1 && flag2==0)
{
	Iterate(flag2);
	return 0;
}
else
{
	//IX_PrintError(9);
return -1;
}
break;


case GE_OP:
if(k1>=v1 && flag2==0)
{
	Iterate(flag2);
	return 0;
}
else
{
//flag2=0;
//IX_PrintError(9);
return -1;
}
break;


case GT_OP:
if(k1>v1 && flag2==0)
{
	Iterate(flag2);
	return 0;
}
else
{

//IX_PrintError(9);
return -1;
}
break;
}
}


//CHARACTER

if(att_type1==TypeChar)
{
switch(op)
{

case NO_OP:
	if(flag2==0)
	{
	Iterate(flag2);
	return 0;
	}
	else
	{
		return -1;
	}
	break;


case EQ_OP:
	if(strcmp(key,val)==0 && flag2==0)
	{
		Iterate(flag2);
		return 0;
	}
	else
	{

	//IX_PrintError(9);
	return -1;
	}
	break;


case LT_OP:
if(strcmp(key,val)==-1 && flag2==0)
{
	Iterate(flag2);
	return 0;
}
else
{

return -1;
}
break;


case LE_OP:
if(strcmp(key,val)==-1 || strcmp(key,val)==0)
{
	Iterate(flag2);
	return 0;
}
else
{
return -1;
}
break;


case GE_OP:
if((strcmp(key,val)==1 || strcmp(key,val)==0) && flag2==0)
{
	Iterate(flag2);
	return 0;
}
else
{
flag2=0;
return -1;
}
break;


case GT_OP:
if(strcmp(key,val)==1 && flag2==0)
{
	Iterate(flag2);
	return 0;
}
else
{

return -1;
}
break;
}
}


}


void IX_IndexScan::Iterate(int &flag)
{
	char buff[PF_PAGE_SIZE],temp[PF_PAGE_SIZE];
	memset(buff,0,sizeof(buff));

	PF_Manager *pf=new PF_Manager();
	PF_FileHandle *pfh=new PF_FileHandle();
	pf->OpenFile(att_name1,*pfh);

	int size1;
	if(curr<no_entry)
		{
			size1=size*curr;
			memset(buff,0,sizeof(buff));
			memcpy(buff,RECORDS+size1,att_len);
			memcpy(key,buff,att_len);
			memset(buff,0,sizeof(buff));
			memcpy(buff,RECORDS+size1+att_len,4);
			ridscan.pageNum=*(int*)&buff;
			memset(buff,0,sizeof(buff));
			memcpy(buff,RECORDS+size1+att_len+4,4);
			ridscan.slotNum=*(int*)&buff;
			curr=curr+1;


		}

	else if(sibling==500 && curr==no_entry)
	{

	size1=size*curr;
	memset(buff,0,sizeof(buff));
	memcpy(buff,RECORDS+size1,att_len);
	memcpy(key,buff,att_len);
	memset(buff,0,sizeof(buff));
	memcpy(buff,RECORDS+size1+att_len,4);
	ridscan.pageNum=*(int*)&buff;
	memset(buff,0,sizeof(buff));
	memcpy(buff,RECORDS+size1+att_len+4,4);
	ridscan.slotNum=*(int*)&buff;
		flag=2;
		goto l1;
	}


		else
		{
			l2:;
			memset(buff,0,sizeof(buff));
			pfh->ReadPage(sibling-1,buff);
			memset(RECORDS,0,sizeof(RECORDS));
			memcpy(RECORDS,buff,PF_PAGE_SIZE);
			memset(temp,0,sizeof(temp));
			memcpy(temp,buff+sizeof(buff)-3,2);
			no_entry=*(int*)&temp;
			memset(temp,0,sizeof(temp));
			memcpy(temp,buff+sizeof(buff)-5,2);
			sibling=*(int*)&temp;
			if(no_entry==0)
			{
				goto l2;
			}

			memset(temp,0,sizeof(temp));
			memcpy(temp,buff,att_len);
			memcpy(key,temp,att_len);
			memset(temp,0,sizeof(temp));
			memcpy(temp,buff+att_len,4);
			ridscan.pageNum=*(int*)&temp;
			memset(temp,0,sizeof(temp));
			memcpy(temp,buff+att_len+4,4);
			ridscan.slotNum=*(int*)&temp;
			curr=1;
		}
l1:;
	delete pf;
	delete pfh;
}


RC IX_IndexScan::CloseScan()
{
if(scan!=1)
{
	IX_PrintError(12);
	return 12;
}
else
{
scan=0;
return 0;
}
}

RC IX_IndexScan::scanfirstpage(int pgnode,int &lpage,const IX_IndexHandle &indexHandle)
{
char tarr[PF_PAGE_SIZE],temp[PF_PAGE_SIZE];
char lf[20],nlf[20],rt[20];
memset(lf,0,sizeof(lf));
memset(nlf,0,sizeof(nlf));
memset(rt,0,sizeof(rt));
memcpy(lf,"L",1);
memcpy(nlf,"N",1);
memcpy(rt,"R",1);
int lptr;


PF_Manager *pf1=new PF_Manager();
PF_FileHandle *pfh1=new PF_FileHandle();
pf1->OpenFile(indexHandle.att_name,*pfh1);
memset(PGBUFF,0,sizeof(PGBUFF));
pfh1->ReadPage(pgnode-1,PGBUFF);

memset(tarr,0,sizeof(PGBUFF));
memcpy(tarr,PGBUFF+sizeof(PGBUFF)-1,1);

if(strcmp(tarr,rt)==0 || strcmp(tarr,nlf)==0)
{
memset(temp,0,sizeof(temp));
memcpy(temp,PGBUFF,4);
lptr=*(int*)&temp;

l1:;
if(lptr==-1)
{
	memcpy(temp,PGBUFF+4+indexHandle.att_length,4);
	lptr=*(int*)&temp;
	goto l1;
	//cout<<"Value does not exist...failue"<<endl;
//	return 1;
}


int y=scanfirstpage(lptr,lpage,indexHandle);
delete pf1;
delete pfh1;
return 0;
}


if(strcmp(tarr,lf)==0)
{
lpage=pgnode;
delete pf1;
delete pfh1;
return 0;
}
return 0;
}


//PRINTERROR FN
void IX_PrintError (RC rc)
{
	switch(rc)
	{
	cout<<"****** EXCEPTION CAUGHT ******"<<endl;

	case 1:
	cout<<"INDEX ALREADY EXISTS...."<<endl;
	break;

	case 2:
	cout<<"DESTROYING INDEX FAILED...NO SUCH INDEX EXISTS....."<<endl;
	break;

	case 3:
	cout<<"FAILED OPENING INDEX......"<<endl;
	break;

	case 4:
	cout<<"FAILED CLOSING INDEX........"<<endl;
	break;

	case 5:
	cout<<"TRYING TO INSERT DUPLICATE ENTRY......"<<endl;
	break;

	case 6:
	cout<<"KEY RID PAIR DOES NOT EXISTS..... DELETE FAILED"<<endl;
	break;

	case 7:
	cout<<"Scan opened again"<<endl;
	break;

	case 8:
	cout<<"FAILED OPENING SCAN....."<<endl;
	break;

	case 9:
	cout<<"END OF SCAN...."<<endl;
	break;

	case 12:
	cout<<"FAILED CLOSING SCAN..."<<endl;
	break;
	}
}


#include <fstream>
#include <iostream>
#include <cassert>
#include <cstring>

#include <conio.h>

#include "pf.h"

using namespace std;


void pfTest()
{
   PF_Manager *pf = new PF_Manager();
   PF_FileHandle *pfh=new PF_FileHandle();
   PF_FileHandle *pfh1=new PF_FileHandle();
   int a,ch,pnum;
   void *data;
   char ip[100];
   unsigned int a1;
   char ab[PF_PAGE_SIZE],cd[PF_PAGE_SIZE];

   do
   {
   cout<<endl<<"1.Create File"<<endl<<"2.Destroy File"<<endl<<"3.Open file"<<endl<<"4.No of pages"<<endl<<"5.Read Page"<<endl<<"6.Write Page"<<endl<<"7.Append Page"<<endl<<"8.Close file"<<endl<<"9.Exit"<<endl;
   cout<<"Enter choice:";
   cin>>ch;


   switch(ch)
   {
   case 1:
	   a=pf->CreateFile("file1");

	   break;
   case 2:
	   a=pf->DestroyFile("file1");
	   break;
   case 3:
	   a=pf->OpenFile("file1",*pfh);
	   break;

  case 4:
	   a=pf->OpenFile("file1",*pfh);
	   a1=pfh->GetNumberOfPages();
	   cout<<"Total No of pages in the file : "<<(int)a1<<endl;
	   a=pf->CloseFile(*pfh);
	   break;
  case 5:
	  cout<<"Enter the page number to read"<<endl;
	  cin>>pnum;
	  a=pf->OpenFile("file1",*pfh);
	  a1=pfh->ReadPage(pnum,&data);
	  a=pf->CloseFile(*pfh);
	  break;
  case 6:
	   cout<<"Enter the page number:"<<endl;
	   cin>>pnum;

	   memset(ab,0,PF_PAGE_SIZE);
	   cout<<"Enter data :"<<endl;
	   cin>>ab;


	   ab[PF_PAGE_SIZE] = 0;

	   cout << ab;

	   a=pf->OpenFile("file1",*pfh);
	  // a=pf->OpenFile("file1",*pfh1);
	   a1=pfh->WritePage(pnum,ab);

	   a=pf->CloseFile(*pfh);
	   /*char hj[100];
	   cin>>hj;
	   a1=pfh1->WritePage(pnum,hj);
	   //a=pf->CloseFile(*pfh);
	   a=pf->CloseFile(*pfh1);*/
	   //getch();
	   break;
   case 7:

	   memset(cd,0,PF_PAGE_SIZE);
	   cout<<"Enter data :"<<endl;
	   cin>>cd;
  	   cd[PF_PAGE_SIZE] = 0;
	   a=pf->OpenFile("file1",*pfh);
	   a1=pfh->AppendPage(cd);
	   a=pf->CloseFile(*pfh);
	   break;
   case 8:
	   a=pf->CloseFile(*pfh);
	   break;
   case 9:
	   goto l1;
	   break;
   }
   }while(ch!=10);

  // write your own testing cases here
l1:;
   delete pf;
   delete pfh;
}

/*
int main() 
{

	cout << "test...  integer entered is"<< endl;

  pfTest();
  // other tests go here

  cout << "OK" << endl;
}

*/

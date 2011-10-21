#include <fstream>
#include <iostream>
#include <cassert>
#include <vector>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ix.h"

using namespace std;

//Global Initializations
//IX_Manager *ixManager = new IX_Manager();

//const int success = 0;

/*
void createTable(RM *rm, const char *tablename)
{
    // Functions tested
    // 1. Create Table
    vector<char*> v_name;
    v_name.push_back("EmpName"); 
    v_name.push_back("Age"); 
    v_name.push_back("Height"); 
    v_name.push_back("SSN"); 
    
    vector<AttrType> v_type;
    v_type.push_back(TypeChar); 
    v_type.push_back(TypeInt); 
    v_type.push_back(TypeReal); 
    v_type.push_back(TypeInt); 
    
    vector<AttrLength> v_length;
    v_length.push_back((AttrLength)30); 
    v_length.push_back(4); 
    v_length.push_back(4); 
    v_length.push_back(4); 
    
    rm->createTable(tablename, v_name, v_type, v_length);
    printf("Table created: %s\n", tablename);    
}


void createExtraTable(RM *rm, const char *tablename)
{
    // Functions tested
    // 1. Create Table
    vector<char *> v_name;
    v_name.push_back("EmpName");
    v_name.push_back("Address");

    vector<AttrType> v_type;
    v_type.push_back(TypeChar);
    v_type.push_back(TypeVarChar);

    vector<AttrLength> v_length;
    v_length.push_back(30);
    v_length.push_back(50);

    rm->createTable(tablename, v_name, v_type, v_length);
    printf("Table created: %s\n", tablename);
}


void testCase_1(const char *tablename, const char *attrname)
{    
    // Functions tested
    // 1. Create Index **
    // 2. OpenIndex **
    // 3. CreateIndex -- when index is already created on that key **
    // 4. CloseIndex **
    // NOTE: "**" signifies the new functions being tested in this test case.
    printf("****In Test Case 1****\n");

    RC rc;
    // Test CreateIndex
    rc = ixManager->CreateIndex(tablename, attrname);
    if(rc == success) 
    {
        printf("Index Created!\n");
    }
    else 
    {
        printf("Failed Creating Index...\n");
    }
    
    // Test OpenIndex
    IX_IndexHandle ixHandle;
    rc = ixManager->OpenIndex(tablename, attrname, ixHandle);
    if(rc == success) 
    {
        printf("Index on %s Opened!\n", attrname);
    }
    else 
    {
        printf("Failed Opening Index...\n");
    }  
    
    // Test create duplicate index
    rc = ixManager->CreateIndex(tablename, attrname);
    if(rc != success)
    {
        printf("Duplicate Index not Created -- correct!\n");
    }
    else
    {
        printf("Duplicate Index Created -- failure...\n");
    }
    
    // Test CloseIndex
    rc = ixManager->CloseIndex(ixHandle);
    if(rc == success)
    {
        printf("Index Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Index...\n");
    }  
    return;
}


void testCase_2(const char *tablename, const char *attrname)
{    
    // Functions tested
    // 1. Create Index
    // 2. OpenIndex 
    // 3. Insert entry **
    // 4. Scan entries NO_OP -- open**
    // 5. Scan close **
    // 6. CloseIndex **
    // NOTE: "**" signifies the new functions being tested in this test case.
    printf("****In Test Case 2****\n");
 
    RID rid;
    RC rc;
    // Test CreateIndex
    rc = ixManager->CreateIndex(tablename, attrname);
    if(rc == success) 
    {
        printf("Index Created!\n");
    }
    else 
    {
        printf("Failed Creating Index...\n");
    }
    
    // Test OpenIndex
    IX_IndexHandle ixHandle;
    rc = ixManager->OpenIndex(tablename, attrname, ixHandle);
    if(rc == success) 
    {
        printf("Index on %s Opened!\n", attrname);
    }
    else 
    {
        printf("Failed Opening Index...\n");
    }
    
    // Test InsertEntry
    unsigned numOfTuples = 1000;
    unsigned maxValue = 500;
    for(unsigned i = 0; i <= numOfTuples; i++) 
    {
        unsigned key = (rand() % (maxValue-1))+1;//just in case somebody starts pageNum and recordId from 1
        RID rid;
        rid.pageNum = key;
        rid.slotNum = key+1;
        
        rc = ixHandle.InsertEntry(&key, rid);
        if(rc != success)
        {
            printf("Failed Inserting Entry...\n");
        }     
    }
    
    // Scan
    IX_IndexScan *ixScan = new IX_IndexScan();
    rc = ixScan->OpenScan(ixHandle, NO_OP, NULL);
    if(rc == success)
    {
        printf("Scan Opened Successfully!\n");
    }
    else 
    {
        printf("Failed Opening Scan!\n");
    }  
    
    while(ixScan->GetNextEntry(rid) == success) 
    {
        printf("%u %u\n", rid.pageNum, rid.slotNum);
    }
    
    // Close Scan
    rc = ixScan->CloseScan();
    if(rc == success)
    {
        printf("Scan Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Scan...\n");
    }  
    
    // Close Index
    rc = ixManager->CloseIndex(ixHandle);
    if(rc == success)
    {
        printf("Index Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Index...\n");
    }
    return;  
}


void testCase_3(const char *tablename, const char *attrname)
{    
    // Functions tested
    // 1. OpenIndex 
    // 2. Insert entry 
    // 3. Delete entry **
    // 4. Delete entry -- when the value is not there **
    // 5. Close Index
    // NOTE: "**" signifies the new functions being tested in this test case.
    printf("****In Test Case 3****\n");
    
    RID rid;
    RC rc;
    // Test Open Index
    IX_IndexHandle ixHandle;
    rc = ixManager->OpenIndex(tablename, attrname, ixHandle);
    if(rc == success) 
    {
        printf("Index on %s Opened!\n", attrname);
    }
    else 
    {
        printf("Failed Opening Index...\n");
    }
    
    unsigned numOfTuples = 1;
    unsigned key = 100;
    rid.pageNum = key;
    rid.slotNum = key+1;

    void *payload = malloc(50);
    char *name = (char *)malloc(20);
    strcpy(name, "Thomas");
    *(int *)payload = strlen(name);
    memcpy((char *)payload+4, name, strlen(name));
    
    // Test Insert Entry
    for(unsigned i = 0; i < numOfTuples; i++) 
    {
        rc = ixHandle.InsertEntry(payload, rid);
        if(rc != success)
        {
            printf("Failed Inserting Entry...\n");
        }     
    }
    
    // Test Delete Entry
    rc = ixHandle.DeleteEntry(payload, rid);
    if(rc != success)
    {
        printf("Failed Deleting Entry...\n");
    }      
    
    // Test Delete Entry again
    rc = ixHandle.DeleteEntry(payload, rid);
    if(rc == success) //This time it should NOT give success because entry is not there.
    {
        printf("Entry deleted again...failure\n");
    }
    
    // Test Close Index
    rc = ixManager->CloseIndex(ixHandle);
    if(rc == success)
    {
        printf("Index Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Index...\n");
    }
    
    return;
}


void testCase_4(const char *tablename, const char *attrname)
{    
    // Functions tested
    // 1. Destroy Index **
    // 2. OpenIndex -- should fail
    // 3. Scan  -- should fail
    printf("****In Test Case 4****\n");
    
    RC rc;
    // Test Destroy Index
    rc = ixManager->DestroyIndex(tablename, attrname);
    if(rc != success)
    {
        printf("Failed Destroying Index...\n");
    }     
    
    // Test Open the destroyed index
    IX_IndexHandle ixHandle;
    rc = ixManager->OpenIndex(tablename, attrname, ixHandle);
    if(rc == success) //should not work now
    {
        printf("Index opened again...failure\n");
    }  
    
    IX_IndexScan *ixScan = new IX_IndexScan();
    // Test Open Scan
    rc = ixScan->OpenScan(ixHandle, NO_OP, NULL);
    if(rc == success) 
    {
        printf("Scan opened again...failure\n");
    }  

    return;
}


void testCase_5(const char *tablename, const char *attrname)
{    
    // Functions tested
    // 1. Create Index
    // 2. OpenIndex 
    // 3. Insert entry 
    // 4. Scan entries using GE_OP operator and checking if the values returned are correct. **
    // 5. Scan close 
    // 6. CloseIndex 
    // 7. DestroyIndex
    // NOTE: "**" signifies the new functions being tested in this test case.   
    printf("****In Test Case 5****\n");
        
    RID rid;
    RC rc;
    // Test Create Index
    rc = ixManager->CreateIndex(tablename, attrname);
    if(rc == success) 
    {
        printf("Index Created!\n");
    }
    else 
    {
        printf("Failed Creating Index...\n");
    }
    
    // Test Open Index
    IX_IndexHandle ixHandle;
    rc = ixManager->OpenIndex(tablename, attrname, ixHandle);
    if(rc == success) 
    {
        printf("Index Opened!\n");
    }
    else 
    {
        printf("Failed Opening Index...\n");
    }  
    
    // Test Insert Entry
    unsigned numOfTuples = 100;
    for(unsigned i = 1; i <= numOfTuples; i++) 
    {
        unsigned key = i;
        RID rid;
        rid.pageNum = key;
        rid.slotNum = key+1;
        
        rc = ixHandle.InsertEntry(&key, rid);
        if(rc != success)
        {
            printf("Failed Inserting Keys...\n");
        }     
    }
    
    numOfTuples = 100;
    for(unsigned i = 501; i < numOfTuples+500; i++) 
    {
        unsigned key = i;
        RID rid;
        rid.pageNum = key;
        rid.slotNum = key+1;
        
        rc = ixHandle.InsertEntry(&key, rid);
        if(rc != success)
        {
            printf("Failed Inserting Keys...\n");
        }     
    }
    
    // Test Open Scan
    IX_IndexScan *ixScan = new IX_IndexScan();
    int value = 501;

    rc= ixScan->OpenScan(ixHandle, GE_OP, &value);
    if(rc == success)
    {
        printf("Scan Opened Successfully!\n");
    }
    else 
    {
        printf("Failed Opening Scan...\n");
    }  
    
    // Test IndexScan iterator
    while(ixScan->GetNextEntry(rid) == success) 
    {
        printf("%u %u\n", rid.pageNum, rid.slotNum);
        if (rid.pageNum < 501 || rid.slotNum < 501)
        {
            printf("Wrong entries output...failure\n");
        }
    }
    
    // Test Closing Scan
    rc= ixScan->CloseScan();
    if(rc == success)
    {
        printf("Scan Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Scan...\n");
    }  
    
    // Test Closing Index
    rc = ixManager->CloseIndex(ixHandle);
    if(rc == success)
    {
        printf("Index Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Index...\n");
    }  
    
    // Test Destroying Index
    rc = ixManager->DestroyIndex(tablename, attrname);
    if(rc == success)
    {
        printf("Index Destroyed Successfully!\n");
    }
    else 
    {
        printf("Failed Destroying Index...\n");
    }
    return;
}


void testCase_6(const char *tablename, const char *attrname)
{
	// Functions tested
	// 1. Create Index
	// 2. OpenIndex 
	// 3. Insert entry 
	// 4. Scan entries using LT_OP operator and checking if the values returned are correct. Returned values are part of two separate insertions **
	// 5. Scan close 
	// 6. CloseIndex 
	// 7. DestroyIndex
	// NOTE: "**" signifies the new functions being tested in this test case.
	printf("****In Test Case 6****\n");

    // Test CreateIndex
    RC rc;
    RID rid;
	rc = ixManager->CreateIndex(tablename, attrname);
    if(rc == success) 
    {
        printf("Index Created!\n");
    }
    else 
    {
        printf("Failed Creating Index...\n");
    }

    // Test OpenIndex	
    IX_IndexHandle ixHandle;
    rc = ixManager->OpenIndex(tablename, attrname, ixHandle);
    if(rc == success) 
    {
        printf("Index Opened!\n");
    }
    else 
    {
        printf("Failed Opening Index...\n");
    }  

    // Test InsertEntry	
	unsigned numOfTuples = 2000;
	for(unsigned i = 1; i <= numOfTuples; i++) 
	{
		unsigned key = i;
		rid.pageNum = key;
		rid.slotNum = key;
		
		rc = ixHandle.InsertEntry(&key, rid);
		if(rc != success)
		{
            printf("Failed Inserting Keys...\n");
		} 	
	}

    numOfTuples = 2000;
	for(unsigned i = 6000; i <= numOfTuples+6000; i++) 
	{
		unsigned key = i;
		rid.pageNum = key;
		rid.slotNum = key-(unsigned)500;
		
		rc = ixHandle.InsertEntry(&key, rid);
		if(rc != success)
		{
            printf("Failed Inserting Keys...\n");
		} 	
	}

    // Test Scan	
	IX_IndexScan *ixScan = new IX_IndexScan();
	int compVal = 6500;

	rc = ixScan->OpenScan(ixHandle, LT_OP, &compVal);
    if(rc == success)
    {
        printf("Scan Opened Successfully!\n");
    }
    else 
    {
        printf("Failed Opening Scan...\n");
    }  

    // Test IndexScan Iterator	
	while(ixScan->GetNextEntry(rid) == success) 
	{
        if(rid.pageNum % 500 == 0)
            printf("%u %u\n", rid.pageNum, rid.slotNum);
		if ((rid.pageNum > 2000 && rid.pageNum < 6000) || rid.pageNum >= 6500)
		{
            printf("Wrong entries output...failure\n");
		}
	}

    // Test CloseScan
	rc= ixScan->CloseScan();
    if(rc == success)
    {
        printf("Scan Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Scan...\n");
    }  

    // Test CloseIndex	
	rc = ixManager->CloseIndex(ixHandle);
    if(rc == success)
    {
        printf("Index Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Index...\n");
    }  

    // Test DestroyIndex	
	rc = ixManager->DestroyIndex(tablename, attrname);
    if(rc == success)
    {
        printf("Index Destroyed Successfully!\n");
    }
    else 
    {
        printf("Failed Destroying Index...\n");
    }
	return;
}


void testCase_7(const char *tablename, const char *attrname)
{	
	// Functions tested
	// 1. Create Index
	// 2. OpenIndex 
	// 3. Insert entry 
	// 4. Scan entries. Check if it can handle DUPLICATE entries which are NON-CONTIGUOUS **
	// 5. Scan close 
	// 6. CloseIndex 
	// 7. DestroyIndex
	// NOTE: "**" signifies the new functions being tested in this test case.
	printf("****In Test Case 7****\n");
	
    RC rc;
    RID rid;
    // Test CreateIndex
	rc = ixManager->CreateIndex(tablename, attrname);
    if(rc == success) 
    {
        printf("Index Created!\n");
    }
    else 
    {
        printf("Failed Creating Index...\n");
    }

    // Test OpenIndex
    IX_IndexHandle ixHandle;
	rc = ixManager->OpenIndex(tablename, attrname, ixHandle);
    if(rc == success) 
    {
        printf("Index Opened!\n");
    }
    else 
    {
        printf("Failed Opening Index...\n");
    }  
	
    // Test InsertEntry
	unsigned numOfTuples = 20;
	for(unsigned i = 1; i <= numOfTuples; i++) 
	{
		unsigned key = 1234;
		rid.pageNum = i;
		rid.slotNum = i;
		
		rc = ixHandle.InsertEntry(&key, rid);
		if(rc != success)
		{
            printf("Failed Inserting Keys...\n");
		} 	
	}
    numOfTuples = 20;
	for(unsigned i = 500; i < numOfTuples+500; i++) 
	{
		unsigned key = 1234;
		rid.pageNum = i;
		rid.slotNum = i-5;
		
		rc = ixHandle.InsertEntry(&key, rid);
		if(rc != success)
		{
            printf("Failed Inserting Keys...\n");
		} 
	}
	
    // Test Scan
	IX_IndexScan *ixScan = new IX_IndexScan();
	int compVal = 1234;

	rc = ixScan->OpenScan(ixHandle, EQ_OP, &compVal);
    if(rc == success)
    {
        printf("Scan Opened Successfully!\n");
    }
    else 
    {
        printf("Failed Opening Scan...\n");
    }

    // Test IndexScan Iterator
	while(ixScan->GetNextEntry(rid) == success) 
	{
		printf("%u %u\n", rid.pageNum, rid.slotNum);

		if ((rid.pageNum > 20 && rid.pageNum < 500) || rid.pageNum > 520)
		{
		    printf("Wrong entries output...failure\n");
		}
	}
	printf("\n");

    // Test CloseScan
	rc = ixScan->CloseScan();
    if(rc == success)
    {
        printf("Scan Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Scan...\n");
    }  

    // Test CloseIndex	
	rc = ixManager->CloseIndex(ixHandle);
    if(rc == success)
    {
        printf("Index Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Index...\n");
    }  

    // Test DestroyIndex	
	rc = ixManager->DestroyIndex(tablename, attrname);
    if(rc == success)
    {
        printf("Index Destroyed Successfully!\n");
    }
    else 
    {
        printf("Failed Destroying Index...\n");
    }
		
	return;
}


void testCase_8(const char *tablename, const char *attrname)
{
    // Functions Tested:
    // 1. Create Index -- TypeChar
    // 2. Open Index
    // 3. Insert Entry
    // 4. Scan
    // 5. Close Scan
    // 6. Close Index
    // 7. Destroy Index
    printf("****In Test Case 8****\n");
	
    RC rc;
    RID rid;

    // Test OpenIndex
    IX_IndexHandle ixHandle;
	rc = ixManager->OpenIndex(tablename, attrname, ixHandle);
    if(rc == success) 
    {
        printf("Index Opened!\n");
    }
    else 
    {
        printf("Failed Opening Index...\n");
    }  
	
    // Test InsertEntry
	unsigned numOfTuples = 500;
	for(unsigned i = 1; i <= numOfTuples; i++) 
	{
		void *key = malloc(100);
        unsigned count = ((i-1) % 26) + 1;
        *(int *)key = count;
   
        for(unsigned j = 0; j < count; j++)
        {
            *((char *)key+4+j) = 96+count;
        }

		rid.pageNum = i;
		rid.slotNum = i;
		
		rc = ixHandle.InsertEntry(key, rid);
		if(rc != success)
		{
            printf("Failed Inserting Keys...\n");
		} 	
	}

    // Test Scan
	IX_IndexScan *ixScan = new IX_IndexScan();
    unsigned offset = 20;
    void *key = malloc(100);
	*(int *)key = offset;
    for(unsigned j = 0; j < offset; j++)
    {
        *((char *)key+4+j) = 96+offset;
    }

	rc = ixScan->OpenScan(ixHandle, EQ_OP, key);
    if(rc == success)
    {
        printf("Scan Opened Successfully!\n");
    }
    else 
    {
        printf("Failed Opening Scan...\n");
    }

    // Test IndexScan Iterator
    while(ixScan->GetNextEntry(rid) == success)
    {
        printf("%u %u\n", rid.pageNum, rid.slotNum);
    }
    printf("\n");

    // Test CloseScan
	rc = ixScan->CloseScan();
    if(rc == success)
    {
        printf("Scan Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Scan...\n");
    }  

    // Test CloseIndex	
	rc = ixManager->CloseIndex(ixHandle);
    if(rc == success)
    {
        printf("Index Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Index...\n");
    }  

    // Test DestroyIndex	
	rc = ixManager->DestroyIndex(tablename, attrname);
    if(rc == success)
    {
        printf("Index Destroyed Successfully!\n");
    }
    else 
    {
        printf("Failed Destroying Index...\n");
    }
		
	return;
}


void testCase_9(const char *tablename, const char *attrname)
{
	// Functions tested
	// 1. Create Index
	// 2. OpenIndex 
	// 3. Insert entry 
	// 4. Scan entries, and delete entries
	// 5. Scan close 
	// 6. CloseIndex 
	// 7. DestroyIndex
	// NOTE: "**" signifies the new functions being tested in this test case.
    printf("****In Test Case 9****\n");
	
    RC rc;
    RID rid;
    // Test CreateIndex
	rc = ixManager->CreateIndex(tablename, attrname);
    if(rc == success) 
    {
        printf("Index Created!\n");
    }
    else 
    {
        printf("Failed Creating Index...\n");
    }

    // Test OpenIndex
    IX_IndexHandle ixHandle;
	rc = ixManager->OpenIndex(tablename, attrname, ixHandle);
    if(rc == success) 
    {
        printf("Index Opened!\n");
    }
    else 
    {
        printf("Failed Opening Index...\n");
    }  
	
    // Test InsertEntry
	unsigned numOfTuples = 100;
	for(unsigned i = 1; i <= numOfTuples; i++) 
	{
		unsigned key = i;
		rid.pageNum = key;
		rid.slotNum = key;
		
		rc = ixHandle.InsertEntry(&key, rid);
		if(rc != success)
		{
            printf("Failed Inserting Keys...\n");
		} 	
	}

    // Test Scan
	IX_IndexScan *ixScan = new IX_IndexScan();
	int compVal = 100;

	rc = ixScan->OpenScan(ixHandle, LE_OP, &compVal);
    if(rc == success)
    {
        printf("Scan Opened Successfully!\n");
    }
    else 
    {
        printf("Failed Opening Scan...\n");
    }

    // Test DeleteEntry in IndexScan Iterator
	while(ixScan->GetNextEntry(rid) == success) 
	{
		printf("%u %u\n", rid.pageNum, rid.slotNum);

        unsigned key = rid.pageNum;
        rc = ixHandle.DeleteEntry(&key, rid);
        if(rc != success)
        {
            printf("Failed deleting entry in Scan...\n");
        }
    }
	printf("\n");

    // Test CloseScan
	rc = ixScan->CloseScan();
    if(rc == success)
    {
        printf("Scan Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Scan...\n");
    }

    // Open Scan again

    rc = ixScan->OpenScan(ixHandle, LE_OP, &compVal);

    if(rc == success)
    {
        printf("Scan Opened Successfully!\n");
    }
    else 
    {
        printf("Failed Opening Scan...\n");
    }

    // Test IndexScan Iterator
	while(ixScan->GetNextEntry(rid) == success) 
	{
		printf("Entry returned: %u %u --- failure\n", rid.pageNum, rid.slotNum);

        if(rid.pageNum > 100)
        {
            printf("Wrong entries output...failure\n");
        }
    }

    // Test CloseScan
	rc = ixScan->CloseScan();
    if(rc == success)
    {
        printf("Scan Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Scan...\n");
    }  

    // Test CloseIndex	
	rc = ixManager->CloseIndex(ixHandle);
    if(rc == success)
    {
        printf("Index Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Index...\n");
    }  

    // Test DestroyIndex	
	rc = ixManager->DestroyIndex(tablename, attrname);
    if(rc == success)
    {
        printf("Index Destroyed Successfully!\n");
    }
    else 
    {
        printf("Failed Destroying Index...\n");
    }
		
	return;
}


void testCase_10(const char *tablename, const char *attrname)
{
	// Functions tested
	// 1. Create Index
	// 2. OpenIndex 
	// 3. Insert entry 
	// 4. Scan entries, and delete entries
	// 5. Scan close
    // 6. Insert entries again
    // 7. Scan entries
	// 8. CloseIndex 
	// 9. DestroyIndex
	// NOTE: "**" signifies the new functions being tested in this test case.
    printf("****In Test Case 10****\n");
	
    RC rc;
    RID rid;
    // Test CreateIndex
	rc = ixManager->CreateIndex(tablename, attrname);
    if(rc == success) 
    {
        printf("Index Created!\n");
    }
    else 
    {
        printf("Failed Creating Index...\n");
    }

    // Test OpenIndex
    IX_IndexHandle ixHandle;
	rc = ixManager->OpenIndex(tablename, attrname, ixHandle);
    if(rc == success) 
    {
        printf("Index Opened!\n");
    }
    else 
    {
        printf("Failed Opening Index...\n");
    }  
	
    // Test InsertEntry
	unsigned numOfTuples = 100;
	for(unsigned i = 1; i <= numOfTuples; i++) 
	{
		unsigned key = i;
		rid.pageNum = key;
		rid.slotNum = key;
		
		rc = ixHandle.InsertEntry(&key, rid);
		if(rc != success)
		{
            printf("Failed Inserting Keys...\n");
		} 	
	}

    // Test Scan
	IX_IndexScan *ixScan = new IX_IndexScan();
	int compVal = 100;

	rc = ixScan->OpenScan(ixHandle, LE_OP, &compVal);
    if(rc == success)
    {
        printf("Scan Opened Successfully!\n");
    }
    else 
    {
        printf("Failed Opening Scan...\n");
    }

    // Test DeleteEntry in IndexScan Iterator
	while(ixScan->GetNextEntry(rid) == success) 
	{
		printf("%u %u\n", rid.pageNum, rid.slotNum);

        unsigned key = rid.pageNum;
        rc = ixHandle.DeleteEntry(&key, rid);
        if(rc != success)
        {
            printf("Failed deleting entry in Scan...\n");
        }
    }
	printf("\n");

    // Test CloseScan
	rc = ixScan->CloseScan();
    if(rc == success)
    {
        printf("Scan Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Scan...\n");
    }

    // Test InsertEntry Again
	numOfTuples = 500;
	for(unsigned i = 1; i <= numOfTuples; i++) 
	{
		unsigned key = i;
		rid.pageNum = key;
		rid.slotNum = key;
		
		rc = ixHandle.InsertEntry(&key, rid);
		if(rc != success)
		{
            printf("Failed Inserting Keys...\n");
		} 	
	}

    // Test Scan
	compVal = 450;
	rc = ixScan->OpenScan(ixHandle, GT_OP, &compVal);
    if(rc == success)
    {
        printf("Scan Opened Successfully!\n");
    }
    else 
    {
        printf("Failed Opening Scan...\n");
    }

    while(ixScan->GetNextEntry(rid) == success)
    {
        printf("%u %u\n", rid.pageNum, rid.slotNum);

        if(rid.pageNum <= 450 || rid.slotNum <= 450)
        {
            printf("Wrong entries output...failure\n");
        }
    }
    printf("\n");

    // Test CloseScan
	rc = ixScan->CloseScan();
    if(rc == success)
    {
        printf("Scan Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Scan...\n");
    }  

    // Test CloseIndex	
	rc = ixManager->CloseIndex(ixHandle);
    if(rc == success)
    {
        printf("Index Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Index...\n");
    }  

    // Test DestroyIndex	
	rc = ixManager->DestroyIndex(tablename, attrname);
    if(rc == success)
    {
        printf("Index Destroyed Successfully!\n");
    }
    else 
    {
        printf("Failed Destroying Index...\n");
    }
		
	return;
}


void testCase_extra(const char *tablename, const char *attrname)
{
    // Functions Tested:
    // 1. Create Index
    // 2. Open Index
    // 3. Insert Entry
    // 4. Scan
    // 5. Close Scan
    // 6. Close Index
    // 7. Destroy Index
    printf("****In Extra Test Case****\n");
	
    RC rc;
    RID rid;
    // Test CreateIndex
	rc = ixManager->CreateIndex(tablename, attrname);
    if(rc == success) 
    {
        printf("Index Created!\n");
    }
    else 
    {
        printf("Failed Creating Index...\n");
    }

    // Test OpenIndex
    IX_IndexHandle ixHandle;
	rc = ixManager->OpenIndex(tablename, attrname, ixHandle);
    if(rc == success) 
    {
        printf("Index Opened!\n");
    }
    else 
    {
        printf("Failed Opening Index...\n");
    }  
	
    // Test InsertEntry
	unsigned numOfTuples = 500;
	for(unsigned i = 1; i <= numOfTuples; i++) 
	{
		void *key = malloc(100);
        unsigned count = ((i-1) % 26) + 1;
        *(int *)key = count;
   
        for(unsigned j = 0; j < count; j++)
        {
            *((char *)key+4+j) = 96+count;
        }

		rid.pageNum = i;
		rid.slotNum = i;
		
		rc = ixHandle.InsertEntry(key, rid);
		if(rc != success)
		{
            printf("Failed Inserting Keys...\n");
		} 	
	}

    // Test Scan
	IX_IndexScan *ixScan = new IX_IndexScan();

    unsigned offset = 18;
    void *key = malloc(100);
	*(int *)key = offset;
    for(unsigned j = 0; j < offset; j++)
    {
        *((char *)key+4+j) = 96+offset;
    }

	rc = ixScan->OpenScan(ixHandle, EQ_OP, key);
    if(rc == success)
    {
        printf("Scan Opened Successfully!\n");
    }
    else 
    {
        printf("Failed Opening Scan...\n");
    }

    // Test IndexScan Iterator
    while(ixScan->GetNextEntry(rid) == success)
    {
        printf("%u %u\n", rid.pageNum, rid.slotNum);
    }
    printf("\n");

    // Test CloseScan
	rc = ixScan->CloseScan();
    if(rc == success)
    {
        printf("Scan Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Scan...\n");
    }  

    // Test CloseIndex	
	rc = ixManager->CloseIndex(ixHandle);
    if(rc == success)
    {
        printf("Index Closed Successfully!\n");
    }
    else 
    {
        printf("Failed Closing Index...\n");
    }  

    // Test DestroyIndex	
	rc = ixManager->DestroyIndex(tablename, attrname);
    if(rc == success)
    {
        printf("Index Destroyed Successfully!\n");
    }
    else 
    {
        printf("Failed Destroying Index...\n");
    }
		
	return;
}

/*
int main()
{
    printf("****Starting Test Cases****\n");
    
    RM *rm = new RM();
    createTable(rm, "tbl_employee");
    // Pass the record manager rm to ixManager, you need this function
    ixManager->setRM(*rm);
    
    testCase_1("tbl_employee", "EmpName");
   // testCase_2("tbl_employee", "SSN");
 //   testCase_3("tbl_employee", "EmpName");
  //  testCase_4("tbl_employee", "SSN");
  // testCase_5("tbl_employee", "SSN");

   //testCase_6("tbl_employee", "SSN");
   //testCase_7("tbl_employee", "SSN");
   testCase_8("tbl_employee", "EmpName");
    //testCase_9("tbl_employee", "SSN");
   //testCase_10("tbl_employee", "SSN");

    // Extra Credit Work
    createExtraTable(rm, "tbl_extra");
    ixManager->setRM(*rm);
    testCase_extra("tbl_extra", "Address");
    return 0;
}
*/

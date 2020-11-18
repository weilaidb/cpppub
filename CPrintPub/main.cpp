#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <assert.h>


#include "cprintpub.h"

using namespace std;

int main()
{
    cout << "Hello World!" << endl;
    BYTE uacData[1024] = {0xFD};
    memset(uacData, 0xd,sizeof(uacData));
    CPrintPub::diagSetShowDataSw(1);
    CPrintPub::showData(uacData, sizeof(uacData)/sizeof(uacData[0]));
    CPrintPub::diagSetPageSize(32);
    CPrintPub::diagMemStart(1024 * 1024 * 2);
    WORD32 dwLp =  0;
    for(dwLp = 0;dwLp < 15;dwLp++)
    {
        CPrintPub::diagMemRecord(NULL, 0, "hello, nihaomaxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxyyyyyyyyyzzzzzzzzhello, nihaomaxxxxxxxx11111222333344455556667778889991111111110d 0d 0d 0d 0d 0d 0d 0d  0d 0d 0d 0d 0d 0d 0d 0d 0d 0d 0d 0d 0d 0d 0d 0d  0d 0d 0d 0d 0d 0d 0d 0d 0d 0d 0d 0d 0d 0d 0d 0d  0d 0d 0d 0d 0d 0d 0d 0d 111");
    }

    for(dwLp = 0;dwLp < 5;dwLp++)
    {
        memset(uacData, dwLp+1,sizeof(uacData));
        CPrintPub::diagMemRecord(uacData, sizeof(uacData), "hello, nihaoma");
    }

    CPrintPub::diagMemShow();
    CPrintPub::diagMemStop();
    return 0;
}

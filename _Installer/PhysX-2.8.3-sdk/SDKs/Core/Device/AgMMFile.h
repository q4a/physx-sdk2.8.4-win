 /************************ BEGIN COPYRIGHT NOTICE ***************************\
|*                                                                           *|
|* Copyright 2009 by NVIDIA Corporation.  All rights reserved.  All          *|
|* information contained herein is proprietary and confidential to NVIDIA    *|
|* Corporation.  Any use, reproduction, or disclosure without the written    *|
|* permission of NVIDIA Corporation is prohibited.                           *|
|*                                                                           *|
 \************************** END COPYRIGHT NOTICE ***************************/

#if defined(WIN32)

#ifndef __AG_MM_FILE__
#define __AG_MM_FILE__

#include <windows.h>

class AgMMFile
{
public:
    AgMMFile();
    AgMMFile(char *name, unsigned int size);
    void create(char *name, unsigned int size);
    unsigned int getSize() {return mSize;};
    void * getAddr() {return mAddr;};
    void destroy();
    ~AgMMFile();

private:
    void *mAddr;
    unsigned int mSize;
    HANDLE mFileH;
};


#endif /* __AG_MM_FILE__ */

#endif /* WIN32 */









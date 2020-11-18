#include "cprintpub.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <assert.h>
#include <stdarg.h>

BYTE CPrintPub::m_ucShowDataSw = 0;
CHAR * CPrintPub::m_pDiagMem = NULL;
WORD32 CPrintPub::m_dwPageSize = DIAGPAGESIZEMAX;
WORD32 CPrintPub::m_dwPageCur = 0;
WORD32 CPrintPub::m_dwPageMax = 0;
//func callback
pfn_show CPrintPub::m_pFnShow = printf;
pfn_malloc CPrintPub::m_pFnMalloc = malloc;
pfn_free CPrintPub::m_pFnFree = free;

CPrintPub::CPrintPub()
{

}

void CPrintPub::showData(BYTE *pMsg, WORD32 dwLen)
{
    if(0 == m_ucShowDataSw)
    {
        return;
    }

    if(NULL == m_pFnShow)
    {
        return;
    }

    WORD32 dwLp =  0;
    const char *pFmt = "";
    for(dwLp = 0;dwLp < dwLen;dwLp++)
    {
        if(( dwLp > 0 ) && (0 == (dwLp % 16)))
        {
            pFmt = "\n";
            m_pFnShow(pFmt, pMsg[dwLp]);
        }
        else if(( dwLp > 0 ) && (0 == (dwLp % 8)))
        {
            pFmt = " ";
            m_pFnShow(pFmt, pMsg[dwLp]);
        }
        pFmt = "%02x ";

        m_pFnShow(pFmt, pMsg[dwLp]);
    }
}

BYTE CPrintPub::diagGetShowDataSw ()
{
    return m_ucShowDataSw ;
}

BYTE CPrintPub::diagSetShowDataSw (BYTE _val)
{
    m_ucShowDataSw  = _val;
    return m_ucShowDataSw ;
}

WORD32 CPrintPub::diagGetPageSize ()
{
    return m_dwPageSize ;
}

WORD32 CPrintPub::diagSetPageSize (WORD32 _val)
{
    m_dwPageSize  = _val;
    return m_dwPageSize ;
}


BYTE CPrintPub::diagMemStart(WORD32 dwLen)
{
    BYTE ucError = 0;
    if(0 == dwLen)
    {
        m_pFnShow("mem len invalid\n");
        return ++ucError;
    }

    if(dwLen > DIAGMEMMAX)
    {
        m_pFnShow("mem len overflow:%u\n", DIAGMEMMAX);
        return ++ucError;
    }

    if(NULL == m_pFnMalloc)
    {
        m_pFnShow("mem malloc callback invalid\n");
        return ++ucError;
    }
    if(NULL == m_pFnFree)
    {
        m_pFnShow("mem free callback invalid\n");
        return ++ucError;
    }

    m_pDiagMem = (CHAR *)m_pFnMalloc(dwLen);
    if(NULL == m_pDiagMem)
    {
        m_pFnShow("mem malloc failed\n");
        return ++ucError;
    }

    if(0 == m_dwPageSize)
    {
        m_pFnShow("single page size wrong\n");
        return ++ucError;
    }

    m_dwPageCur = 0;
    m_dwPageMax = dwLen/m_dwPageSize;
    m_pFnShow("diag memstart,malloc len:%u(%uK-%uM),pagesize:%u,maxpage:%u [ok]\n"
              , dwLen
              , dwLen/1024
              , dwLen/1024/1024
              , m_dwPageSize
              , m_dwPageMax
              );
    m_pFnShow("======warning !!!, please stop it when done!!!\n");
    m_pFnShow("======warning !!!, please stop it when done!!!\n");
    m_pFnShow("======warning !!!, please stop it when done!!!\n");
    return 0;
}

BYTE CPrintPub::diagMemStop(void)
{
    BYTE ucError = 0;
    if(NULL == m_pDiagMem)
    {
        m_pFnShow("mem no allocate\n");
        return ++ucError;
    }
    if(NULL == m_pFnMalloc)
    {
        m_pFnShow("mem malloc callback invalid\n");
        return ++ucError;
    }
    if(NULL == m_pFnFree)
    {
        m_pFnShow("mem free callback invalid\n");
        return ++ucError;
    }

    m_pFnFree(m_pDiagMem);
    m_pFnShow("diag memstop[ok]\n");

    return 0;
}

BYTE CPrintPub::diagMemRecord(BYTE *pMsg, WORD32 dwMsgLen, const char *pFmt, ...)
{
    BYTE ucError = 0;

    if(NULL == m_pDiagMem)
    {
        m_pFnShow("mem no allocate\n");
        return ++ucError;
    }

    char uacBuf[DIAGPAGESIZEMAX] = {0};
    va_list args;
    va_start(args, pFmt);

    int iRet = vsnprintf(uacBuf, sizeof(uacBuf), pFmt, args);
    if(iRet < 0)
    {
//        m_pFnShow("format string invalid,actual:%s\n", uacBuf);
        uacBuf[DIAGPAGESIZEMAX - 1] = '\0';
    }
    va_end(args);

    WORD32 dwLen = 0;
    if((NULL == pMsg) || (0 == dwMsgLen))
    {
        dwLen = 0;
    }
    else
    {
        dwLen = dwMsgLen * 2;
    }

    dwLen += strlen(uacBuf) + 1;
    CHAR *pNewBuf = (CHAR *)m_pFnMalloc(dwLen);
    if(NULL == pNewBuf)
    {
        m_pFnShow("mem malloc failed\n");
        return ++ucError;
    }
    memset(pNewBuf, 0, dwLen);



    WORD32 dwLp =  0;
    WORD32 dwTotal =  0;
    sprintf(pNewBuf + dwTotal, "%s", uacBuf);
    dwTotal+= strlen(uacBuf);

    for(dwLp = 0;dwLp < dwMsgLen;dwLp++)
    {
        int iCur = sprintf(pNewBuf + dwTotal, "%02u",  pMsg[dwLp]);
        if(iCur < 0)
        {
            m_pFnShow("sprintf invalid\n");
            m_pFnFree(pNewBuf);
            return ++ucError;
        }
        dwTotal += iCur;
    }


    //记录到内存，同时分片
    WORD32 dwPages =  dwLen/DIAGPAGESIZEMAX;
    if(dwLen%DIAGPAGESIZEMAX > 0)
    {
        dwPages += 1;
    }

    WORD32 dwLp2 =  0;
    char uacBufExt[DIAGPAGESIZEMAX] = {0};
    for(dwLp2 = 0;dwLp2 < dwPages;dwLp2++)
    {
        if(m_dwPageCur >= m_dwPageMax)
        {
            m_dwPageCur = 0;
        }
        //字符串末尾要占用'\0'
        memset(uacBufExt,0, sizeof(uacBufExt));
        memcpy(uacBufExt,pNewBuf + (dwLp2 * (DIAGPAGESIZEMAX - 1)), (sizeof(uacBufExt) - 1));
        int iRet = snprintf((m_pDiagMem + (m_dwPageCur%m_dwPageMax) * DIAGPAGESIZEMAX), DIAGPAGESIZEMAX, "%s",  uacBufExt);
        if(iRet < 0)
        {
            m_pFnShow("snprintf invalid\n");
            m_pFnFree(pNewBuf);
            return ++ucError;
        }
        m_dwPageCur++;
    }

    m_pFnFree(pNewBuf);

    return 0;
}

BYTE CPrintPub::diagMemShow(void)
{
    BYTE ucError = 0;

    if(NULL == m_pDiagMem)
    {
        m_pFnShow("mem no allocate\n");
        return ++ucError;
    }

    if(m_dwPageCur > m_dwPageMax)
    {
        m_pFnShow("current page overflow max page\n");
        return ++ucError;
    }
    m_pFnShow("m_dwPageCur:%u, m_dwPageMax:%u\n", m_dwPageCur, m_dwPageMax);

    WORD32 dwLp =  0;
    for(dwLp = 0;dwLp < m_dwPageCur;dwLp++)
    {
        m_pFnShow("No:%-4u => %s\n" , dwLp, (m_pDiagMem + dwLp * DIAGPAGESIZEMAX));
    }
    return 0;
}


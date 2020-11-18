#ifndef CPRINTPUB_H
#define CPRINTPUB_H

#define DIAGMEMMAX      (1024 * 1024 * 50)
#define DIAGPAGESIZEMAX (128)

typedef unsigned char BYTE;
typedef char CHAR;
typedef unsigned short WORD16;
typedef signed short SWROD16;
typedef unsigned int WORD32;
typedef signed int SWROD32;
typedef unsigned long long WORD64;
typedef signed long long SWORD64;

typedef int (*pfn_show)(const char *fmt, ...);
typedef void *(*pfn_malloc)(WORD32 dwSize);
typedef void (*pfn_free)(void *Ptr);

class CPrintPub
{
public:
    CPrintPub();

    static void showData(BYTE *pMsg, WORD32 dwLen);
    static BYTE diagGetShowDataSw ();
    static BYTE diagSetShowDataSw (BYTE _val);
    static WORD32 diagGetPageSize ();
    static WORD32 diagSetPageSize (WORD32 _val);
    static BYTE diagMemStart(WORD32 dwLen);
    static BYTE diagMemStop(void);
    static BYTE diagMemRecord(BYTE *pMsg, WORD32 dwMsgLen, const char *pFmt, ...);
    static BYTE diagMemShow(void);


    //记录数据的格式:
    /*
     * 1.发生时间
     * 2.避免重复数据
     * 3.快速查询（按key)
     * 4.循环存储
     * 5.清除(按key)
     * 6.记录报文时超过长度时截断到单片
     */
private:
    static BYTE m_ucShowDataSw;
    static CHAR *m_pDiagMem;//诊断内存
    static WORD32 m_dwPageSize; //单页大小
    static WORD32 m_dwPageCur; //当前位置
    static WORD32 m_dwPageMax; //最大页数量

    //func callback
    static pfn_show m_pFnShow;
    static pfn_malloc m_pFnMalloc;
    static pfn_free m_pFnFree;
};

#endif // CPRINTPUB_H

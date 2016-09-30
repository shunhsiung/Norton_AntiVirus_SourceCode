// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef LINUX
	#include <pwd.h>
	#include <sys/types.h>
	#include <string.h>
	#include <unistd.h>
	#include <sys/stat.h>
    #include <syslog.h>

	#include "symnts.h"
	#include "FineTime.h"
#endif

#ifdef GLUE

#ifndef LINUX
	#include <nwfile.h>
	#include <nwtime.h>

	#include <unistd.h>
	#include <sys/stat.h>
#endif

#else // ! GLUE
#include "PSCAN.H"
#ifndef LINUX
	#include <nwnamspc.h>
	#include <nwclxcon.h>
	#include <nwmisc.h>
#endif

#define LOG_LINE_BUF_LEN 1024

#define MAX_GUIDS 16

#define INVALID_FILE_HANDLE_VALUE (-1)

#if defined(LINUX)
#define GLOBAL_VAR
#else
#define GLOBAL_VAR extern "C"
#endif

GLOBAL_VAR BOOL ExitingNormally=TRUE;
GLOBAL_VAR BOOL bAbortFind=FALSE;

GLOBAL_VAR DWORD ThreadCount=0;
GLOBAL_VAR DWORD MessageFileLanguageID = 0;

GLOBAL_VAR unsigned int ProgramDirSize = IMAX_PATH;
GLOBAL_VAR char ProgramDir[IMAX_PATH] = {0};        // directory path where rtvscan.nlm was run from (including volume)
GLOBAL_VAR char NLMRunning=1;
GLOBAL_VAR DEBUGFLAGTYPE debug = 0;
GLOBAL_VAR char DebugLogFile[IMAX_PATH] = {0};

#ifdef SERVER
extern "C" char ProgressCharDone;
extern "C" char ProgressCharNotDone;
#endif

GLOBAL_VAR BOOL FreeTheStringTable = FALSE;
GLOBAL_VAR int languageID=4;
GLOBAL_VAR char *UnknownString = "--Unknown String--";

#if defined(NLMCGI)
        #include "WebMsgs.c"  // default strings
#else
        #include "ACTA.c"  // default strings
#endif
char **TheStringTable = _CompiledStringTable;
DWORD  TheStringTableNoElements = sizeof(_CompiledStringTable)/sizeof(_CompiledStringTable[0]);

#if defined(LINUX_LOCK_DISABLE)
#define LINUX_DISABLE_LOCK
#else
#undef LINUX_DISABLE_LOCK
#endif

#if !defined(LINUX_DISABLE_LOCK)
INIT_LOCK();
#endif // !defined(LINUX_DISABLE_LOCK)

#endif // GLUE

#include "vpstrutils.h"
#include "SymSaferStrings.h"

#if !defined(LINUX)
#include <nit/nwbindry.h>
#include <nwfinfo.h>
#endif // !defined(LINUX)

extern "C" int MySetFileInfo(const char *file,const struct stat *stbuf)
{
#ifdef LINUX
	return SetFileInfo(file,stbuf);
#else	
    struct DOSDateTime
    {
        struct _DOSDate DD;
        struct _DOSTime DT;
    } DC,DLU,DLA,DLa;

    // convert calendar time to DOS style date and time
    _ConvertTimeToDOS(stbuf->st_ctime,&DC.DD,&DC.DT);       // create date and time
    _ConvertTimeToDOS(stbuf->st_atime,&DLA.DD,&DLA.DT);     // Last access date and time
    _ConvertTimeToDOS(stbuf->st_btime,&DLa.DD,&DLa.DT);     // Last archive date and time
    _ConvertTimeToDOS(stbuf->st_mtime,&DLU.DD,&DLU.DT);     // Last update/modify date and time

    return SetFileInfo((char*) file,6 /* Hidden and System */,stbuf->st_attr,(char *)&DC,
                       (char *)&DLA.DD,(char *)&DLU,(char *)&DLa,stbuf->st_uid);
#endif
}

#ifdef NLM
#include "FineTimeImplementation.c"
#endif

#if !defined(LINUX)
/*********************************************************************/
int DeletePurgeFile (const char *file)
{
    SetPurgeFlag( file );
    return unlink( file );
}
/*********************************************************************/
int PurgeFile (const char *file)
{
    SetPurgeFlag( file );
    return unlink( file );
}
/*********************************************************************/
void SetPurgeFlag (const char *file)
{

    struct stat Stats;

    if ( stat( file, &Stats ) !=0 )
        return;

    if ( Stats.st_attr & _A_IMMPURG ) // purge flag is already set
        return;

    Stats.st_attr |= _A_IMMPURG;    // and set them to read/write
    MySetFileInfo( file, &Stats );          // and write to the file
}
#endif // !defined(LINUX)

#ifndef GLUE
#if defined(SERVER) || defined(NLMCGI) || defined(START)
/************************************************************************************/
VOID _printf(const char *format,...)
{

    va_list marker;
    char line[LOG_LINE_BUF_LEN];
#ifdef NLM
    char *outFormat="\nRTVSCAN: %s\a\n";
#else
    char *outFormat="%s";
#endif // NLM

    char *p1, *p2;
    va_start(marker, format);

    ssvsnprintf(line, sizeof(line), format, marker);
#if defined(NLM) && !defined(LINUX)
    // TODO fix this for Netware to not overrun buffer
    p1 = StrRep(line,"\n","\r\n");
#else
	p1 = line;
#endif

#ifdef NLM
    if ( NWversion == 3 && NWsubVersion == 11 )
    {
                #define _outFormat (firstLine ? outFormat : cont)
        char firstLine = 1, *cont="%s\r\n";
        int len;

        while ( (len = strlen(p1)) > 0 )
        {
            Next:           if ( len > 80 )
            {
                char *bs = NULL,        // Last back slash
                *ws = NULL;    // Last white space

                for ( p2 = p1; p2 - p1 < 77; p2 = NextChar(p2) )
                {
                    if ( *p2 == '\\' )
                    {
                        if ( p2 - p1 >= 60 )
                            bs = p2;
                    }

                    else if ( isspace(*p2) )
                    {
                        if ( *p2 == '\r' && *(p2 + 1) == '\n' )
                        {
                            *p2 = 0;
                            ConsolePrintf(_outFormat, p1);
                            firstLine = 0;
                            p1 = p2 + 2;
                            len = strlen(p1);
                            goto Next;
                        }

                        else if ( p2 - p1 >= 60 )
                            ws = p2;
                    }
                }

                if ( (!bs && !ws) )
                {     // break it here, no white space
                    len = *p2;
                    *p2 = 0;
                    ConsolePrintf(_outFormat, p1);
                    firstLine = 0;
                    *p2 = len;
                    p1 = p2;
                }

                else if ( bs > ws )
                {
                    *bs = 0;
                    ConsolePrintf(_outFormat, p1);
                    firstLine = 0;
                    p1 = bs - 2;
                    *p1 = ' ';
                    *(p1 + 1) = ' ';
                    *bs = '\\';
                }

                else
                {
                    *ws = 0;
                    ConsolePrintf(_outFormat, p1);
                    firstLine = 0;
                    p1 = ws + 1;
                }
            }

            else
            {
                ConsolePrintf(_outFormat, p1);
                break;
            }
        }
    }

    else
#else
      REF(p2);
#endif
        ConsolePrintf(outFormat,p1);

    if ( debug&DEBUGLOG )
        LogLine(line,FALSE);

    va_end(marker);
}
/********************************************************************************/
char *GetRealLSPointer(LONG wID)
{

    if ( TheStringTable == NULL )
        return "<<<NO MESSAGES>>>";

    if ( wID < STR_APP_NAME || wID >= IDS_LAST_MESSAGE )
        wID = 998;

    if ((wID - 998) > TheStringTableNoElements)
    {
        ConsolePrintf("ERROR - string table ID %d/Index %d requested but only %d exist!\n", wID, (wID - 998), TheStringTableNoElements);
        wID = TheStringTableNoElements;
    }

    return TheStringTable[wID - 998];
}
/*********************************************************************/
/*The LoadString function loads a string resource from the executable file
associated with a specified module, copies the string into a buffer, and
appends a terminating null character.*/
/*int LoadString(HINSTANCE  hinst,UINT  wID,LPTSTR  lpBuffer,int  cchBuffer) {

        REF(hinst);

        strncpy(lpBuffer,LS(wID),cchBuffer-1);
        lpBuffer[cchBuffer-1] = 0;
        return NumBytes(lpBuffer);
}
*/
/****************************************************************************/
DWORD CloseStringTable()
{

    if ( FreeTheStringTable )
    {
        free(TheStringTable);
        TheStringTableNoElements = 0;
    }

    return 0;
}
/****************************************************************************/
void LoadStringTable( LPCTSTR messageFilename )
{
#ifdef NLM

#define HdrMsg "NetWare Message File: rtvscan.nlm"

    struct
    {
        char    Header[100];
        short   unknown1;
        long    unknown2,
        LanguageID,
        NumberOfMessages,
        unknown3;
    } hdr;

    FILE* fp=NULL;
    char libpath[IMAX_PATH];
    DWORD ccode = 0;
    int i;
    long size;
    DWORD *tmp = NULL;

    if (messageFilename == NULL)
    {
        _printf (LS(IDS_LOADING_LANG), MSG_FILE);
        if ( NWversion == 3 )
        {
            sssnprintf(libpath,sizeof(libpath),"%s\\%s", ProgramDir, MSG_FILE);
        }
        else
        {

            int (*GetLanguageID)(void);

            GetLanguageID = (int (*)(void))ImportSymbol(GetNLMHandle(), "GetCurrentOSLanguageID");
            if ( !GetLanguageID )
            {
                _printf("Cannot get the current language id\n");
                ccode = 1;
                goto Return;
            }

            languageID = GetLanguageID();
            if ( languageID == 4 )
                goto Return;

            sssnprintf(libpath, sizeof(libpath), NW_SYSTEM_DIR sSLASH "NLS" sSLASH "%d" sSLASH "%s", languageID, MSG_FILE);
        }
    }
    else
    {
        strncpy(libpath, messageFilename, sizeof(libpath)/sizeof(libpath[0]));
        _printf(LS(IDS_LOADING_LANG), libpath);
    }

    fp = fopen (libpath,"rb");
    if ( !fp )
    {
        _printf("Cannot open \"%s\": %s\n", libpath, strerror(errno));
        ccode = 1;
        goto Return;
    }

    if ( fseek(fp, 0, SEEK_END) )
    {
        _printf("Cannot determine the size of \"%s\": %s\n", libpath, strerror(errno));
        ccode = 1;
        goto Return;
    }

    size = ftell(fp) - 0x76;
    if ( size < 0 )
    {
        _printf("Cannot determine the size of \"%s\": %s\n", libpath, strerror(errno));
        ccode = 1;
        goto Return;
    }

    if ( fseek(fp, 0, SEEK_SET) )
    {
        _printf("Cannot rewind \"%s\": %s\n", libpath, strerror(errno));
        ccode = 1;
        goto Return;
    }

    if ( fread(&hdr, sizeof(hdr), 1, fp) != 1 )
    {
        _printf("Cannot read \"%s\": %s\n", libpath, strerror(errno));
        ccode = 1;
        goto Return;
    }

    if ( strncmp(hdr.Header, HdrMsg, sizeof(HdrMsg) - 1) )
    {
        _printf("Message file \"%s\" is invalid\n", libpath);
        ccode = 1;
        goto Return;
    }

    if ( static_cast<long>(messageCount) != hdr.NumberOfMessages )
    {
        _printf("The message file \"%s\" does not match the NLM!\n", libpath);
        ccode = 1;
        goto Return;;
    }

    if ( NWversion == 4 && languageID != hdr.LanguageID )
    {
        _printf("The message file \"%s\" language ID is %d, should be %d!\n",
                libpath, hdr.LanguageID, languageID);

        ccode = 1;
        goto Return;;
    }

    MessageFileLanguageID = hdr.LanguageID;

//      read MSG_FILE
    tmp = (DWORD *)malloc(size);
    if ( !tmp )
    {
        _printf("Cannot allocate memory for \"%s\\n", libpath);
        ccode = 1;
        goto Return;
    }

    if ( fread(tmp, size, 1, fp) != 1 )
    {
        _printf("Cannot read \"%s\": %s\n", libpath, strerror(errno));
        ccode = 1;
        goto Return;
    }

    // This Fixes all the offsets in the file just loaded to be pointers in memory.
    for ( i = 0; i < hdr.NumberOfMessages; i++ )
        tmp[i] += (DWORD)tmp;

    FreeTheStringTable = TRUE;
    TheStringTable = (char**) tmp;
    TheStringTableNoElements = hdr.NumberOfMessages;
    tmp = NULL;
#ifdef SERVER
    ProgressCharDone='>';
    ProgressCharNotDone='-';
#endif

    Return:
    if ( fp )
        fclose(fp);

    if ( tmp )
        free(tmp);

    if ( ccode )
        _printf("Unable to load language module: %s\a\nUsing English Instead\a\n", MSG_FILE);

    else
        _printf(LS(IDS_USING_LANG_MODULE), MSG_FILE);
#endif // NLM
}
/********************************************************************************************/
DWORD OpenStringTable( BOOL EnglishOnly, LPCTSTR messageFileOverride )
{
#ifdef NLM
    int NWversion;
    int NWsubVersion;
    FILE_SERV_INFO fsinf;

    GetServerInformation (sizeof(fsinf),&fsinf);
    NWversion=fsinf.netwareVersion;
    NWsubVersion = fsinf.netwareSubVersion;

    if ( NWversion == 4 || NWversion == 5 )
    {
        if ((access(MSG_FILE, 0) == 0) && (messageFileOverride == NULL))
        {
            // Move the message file to sys:\system\nlm\<language ID>
            int msg;
            char newName[100];
            DWORD messageID;

            if ( (msg = open(MSG_FILE, O_RDONLY)) == -1 )
            {
                _printf("Unable to open %s" sSLASH "%s: %s!\n", ProgramDir, MSG_FILE, strerror(errno));
                return 1;
            }

            if ( lseek(msg, 0x6A, SEEK_SET) == -1 )
            {
                _printf("Unable to position %s" sSLASH "%s: %s!\n", ProgramDir, MSG_FILE, strerror(errno));
                close(msg);
                return 1;
            }

            if ( read(msg, &messageID, sizeof(DWORD)) != sizeof(DWORD) )
            {
                _printf("Unable to read %s" sSLASH "%s: %s!\n", ProgramDir, MSG_FILE, strerror(errno));
                close(msg);
                return 1;
            }


            close(msg);

            sssnprintf(newName, sizeof(newName), NW_SYSTEM_DIR sSLASH "NLS" sSLASH "%d" sSLASH "%s", static_cast<int>(messageID), MSG_FILE);
            VerifyPath( newName );

            if ( MyCopyFile(MSG_FILE, newName) )
                _printf("Unable to copy %s" sSLASH "%s: English will be used!\n", ProgramDir, MSG_FILE);

            else if ( DeletePurgeFile(MSG_FILE) )

                _printf("Unable to delete %s" sSLASH "%s: %s!\n", ProgramDir, MSG_FILE, strerror(errno));

            _printf("The language message file \"%s\" has been moved to\n \"%s\"\n", MSG_FILE, newName);
        }

        LoadStringTable(messageFileOverride);
    }
    else if ( !EnglishOnly && !access(MSG_FILE, 0) )
    {
        LoadStringTable(messageFileOverride);
    }

#endif // NLM

    return 0;
}
#ifdef NLM //defined elsewhere
/****************************************************************************/
BOOL StrEqual(char *s1,char *s2)
{// returns TRUE if the strings are the same

    int i;

    if ( strlen(s1)!=strlen(s2) ) return FALSE;

    for ( i=0 ; i<strlen(s1) && i<strlen(s2) ; i++ )
        if ( s1[i]!=s2[i] )
            return FALSE;

    return TRUE;
}
#endif
/***********************************************************************************************/
extern "C" BOOL StrNEqual(char *s1,char *s2,int len)
{// returns TRUE if the strings are the same

    int i, l1 = strlen(s1), l2 = strlen(s2);

    if ( l1 < len || l2 < len ) return FALSE;

    for ( i=0; i<len; i++ )
        if ( s1[i]!=s2[i] )
            return FALSE;

    return TRUE;
}
/************************************************************************************/
// I have removed the code to keep open the log, having a locked open log makes it useless at a debuging tool.
// it's not that big of a deal to open and close it all the time.
// CC :)
DWORD LogLine(char *line, BOOL write)
{

    FILE *fp = NULL;
    int i;

    if ( !debug&DEBUGLOG )
        return 1;

#if !defined(LINUX_DISABLE_LOCK)
    LOCK();
#endif // !defined(LINUX_DISABLE_LOCK)

    for ( i = 0; !fp && i < 5; i++ )
    {
        fp = fopen(DebugLogFile, "at");
        if ( !fp ) NTxSleep(50);
    }

    if ( !fp )
    {
#if !defined(LINUX_DISABLE_LOCK)
        UNLOCK();
#endif // !defined(LINUX_DISABLE_LOCK)
        return 1;
    }

    if ( line )
        fprintf(fp, "%s", line);

    REF(write);

    fclose(fp);

#if !defined(LINUX_DISABLE_LOCK)
    UNLOCK();
#endif // !defined(LINUX_DISABLE_LOCK)

    return 0;
}
#endif // SERVER || NLMCGI || START
/************************************************************************************/
#if defined (SERVER) || defined(NLMCGI) || defined(START)
/************************************************************************************/
DWORD LookupAccountSid(void *computer,PSID sid,
							  char *UserName,DWORD *cbUserName,
							  char *DomainName,DWORD *cbDomainName,
							  DWORD *peUse)
{
#ifdef LINUX
	size_t lHost = HOST_NAME_MAX + 1; char sHost[lHost];
	if (! GetComputerName(sHost,&lHost)) strcpy(sHost,"localhost.localdomain");

//Siebel defect# 1-5IYFWH- getpwuid_r() function opens passwd and other files in /etc directory. This triggers  
//AP to send scan request to rtvscan. When we're updating defs we put all scans on hold. Therefore, a deadlock is possible
//when this call is made during definition update.
//
//We now set the user to root and bypass calling getpwuid_r(). This is valid because all requests are coming from rtvscan
//which is root.
//
//This solution will have to be re-visited when communication and remote management is added to the linux product because
//requests might be coming from sources other than rtvscan having different user IDs.
//begin 1-5IYFWH
#if defined(LINUX_DISABLE_TRANSPORT)
    if( getuid() == 0 )
    {
        strcpy(sid->UserName,"root");
        strcpy(UserName,"root");
    }
    else
    {
        strcpy(sid->UserName,"nobody");
        strcpy(UserName,"nobody");
    }
#else
#error "Fix the deadlock in 1-5IYFWH before enabling this block"
    passwd rPasswd,*pPasswd;
   size_t lBuffer = sysconf(_SC_GETPW_R_SIZE_MAX); char sBuffer[lBuffer];
   if (getpwuid_r(getuid(),&rPasswd,sBuffer,lBuffer,&pPasswd))
        // We will cause the least damage to use the very limited "nobody" user.
        strcpy(rPasswd.pw_name,"nobody");

   strcpy(sid->UserName,rPasswd.pw_name);
   strcpy(UserName,rPasswd.pw_name);
#endif
//end 1-5IYFWH

    sid->ConnectionID = 0;
    *cbUserName = strlen(UserName) + 1;

    char* p = strchr(sHost,'.'); if (! p) p = sHost;
	strcpy(DomainName,p); *cbDomainName = strlen(p) + 1;
#else
    char ComputerName[IMAX_PATH];
    WORD type;
    long id;
    BYTE logTime[32];
    char lUserName[IMAX_PATH];

    REF(computer);
    REF(peUse);

    if ( sid->UserName[0] == 0 )
    {
        GetConnectionInformation((WORD)sid->ConnectionID,lUserName,&type,&id,logTime);
        strncpy(sid->UserName,lUserName,sizeof(sid->UserName));
        sid->UserName[sizeof(sid->UserName)-1] = 0;
    }

    NTSGetComputerName(ComputerName,NULL);

    strncpy(UserName,sid->UserName,*cbUserName);
    UserName[*cbUserName-1] = 0;
    *cbUserName = NumBytes(UserName) + 1;

    strncpy(DomainName,ComputerName,*cbDomainName);
    DomainName[*cbDomainName-1] = 0;
    *cbDomainName = NumBytes(DomainName) + 1;
#endif

    return 0;
}
/*****************************************************************************************************/

// ---------------------------------------
// ksr - NetWare Certification, 10/21/2002
// Procedure Coverage

#if !defined(LINUX)
void FindAllSappingServers(void)
{

    long    objectID;
    char
    objName[48],
    objProp,
    objFlag,
    objSecurity,
    propFlags,
    propVal[128];
    int   ccode,k,cd;
    CBA_NETADDR rawAddress;
    WORD objType;
    unsigned short prot=CBA_PROTOCOL_IPX;

//      FILE *fp=fopen ("SYS:\\BindList.txt","wt");
    objectID = (DWORD) -1;

    do
    {
        ccode = ScanBinderyObject("*",OT_WILD, &objectID,objName,&objType, &objProp, &objFlag, &objSecurity);
//              if (fp) fprintf (fp,"[%d:0x](%x)",ccode,objType);
        if ( ccode == 0 && (objType==0x4 || objType==0x640) )
        { // only get property values for NetWare and NT File servers
//                      if (fp) fprintf (fp,"%s",objType,objName);
            ccode = ReadPropertyValue((const char*) objName,objType,"NET_ADDRESS",1,
												  (BYTE*) propVal,
												  (BYTE*) &objFlag,(BYTE*) &propFlags);
            if ( ccode == 0 )
            {
                memset(&rawAddress,0,sizeof(CBA_NETADDR));
                memcpy(&rawAddress.netAddr.ipx,propVal, sizeof(rawAddress.netAddr.ipx));
                k = 0;
                do
                {
                    cd = CBASendPing(CBA_ID_LDVP,CBA_ID_LDVP,NULL,&rawAddress,(unsigned char)prot,0,NULL,0);
                    if ( cd == -4 )
                        NTxSleep(20);
                } while ( cd == -4 && k++ < 5 && !bAbortFind );
//                              if (fp) fprintf (fp,"<%d>",cd);
            }
//                      if (fp) fprintf (fp,"\n");
        }
    } while ( !ccode );
//      if (fp) fclose (fp);
}
#endif // !defined(LINUX)
// ---------------------------------------

/************************************************************************************/
#ifdef NLM
char ResolveAddress(const char *name,CBA_NETADDR *rawAddress,unsigned short *prot)
{
    char foundAddress = 0;
    BYTE val[129],b1,b2;
    struct hostent *pHE;
    typedef struct hostent he;
    typedef he*(*tghbn)( struct nwsockent *nwsktctx,const char *);
    struct nwsockent nwSocketCtx = {0};
    in_addr*    currAddressPtr          = NULL;

    if ( ReadPropertyValue(name,4,"NET_ADDRESS",1,val,&b1,&b2) == 0 )
    {
        memset(rawAddress,0,sizeof(CBA_NETADDR));
        memcpy(&rawAddress->netAddr.ipx, &val, sizeof(rawAddress->netAddr.ipx));
        *prot = CBA_PROTOCOL_IPX;
        foundAddress = 1;
    }
    else
    {
        // now try IP space
        tghbn ghbn = (tghbn)ImportSymbol(GetNLMHandle(),"NetDBgethostbyname");
        if ( ghbn != NULL )
        {
            memset(rawAddress,0,sizeof(CBA_NETADDR));
            pHE = ghbn(&nwSocketCtx,name);
            if ( pHE )
            {
                currAddressPtr = (in_addr*) pHE->h_addr_list[0];
                if (*((DWORD*) currAddressPtr) != 0)
                {
                    rawAddress->netAddr.ipAddr = currAddressPtr->S_un.S_addr;
                    *prot = CBA_PROTOCOL_IP;
                    foundAddress = 1;
                }
            }
            UnimportSymbol(GetNLMHandle(),"NetDBgethostbyname");
        }
    }

    if( debug )
    {
        if( foundAddress )
        {
            CBA_Addr temp = {0};

            temp.dstAddr = *rawAddress;
            temp.ucProtocol = (unsigned char)*prot;

            dprintfTag3( DEBUGCOMMADDRRESOLUTION, "PKT: Addr: Address for %s found: %s (%hu)\n",
                                                  name,
                                                  MakeCBAAddrString(&temp),
                                                  ntohs(temp.dstAddr.usPort) );
        }
        else
        {
            dprintfTag1( DEBUGCOMMADDRRESOLUTION, "PKT: Addr: Address for %s not found\n", name );
        }
    }

    return foundAddress;
}
#endif // NLM
#endif // SERVER || NLMCGI || START

#if defined(SERVER) || defined(NLMCGI) || defined(START)
/**************************************************************************************************/
DWORD GetFileState(const char *path,void *data)
{

    return stat(path,(struct stat*)data);
}
/****************************************************************************************************/
DWORD SetFileState(const char *path,const void *data)
{

    return MySetFileInfo(path,(struct stat*)data);
}
/*********************************************************************************************************/
DWORD GetFileDate(const char *Path,VTIME *Time)
{

    struct stat stat;
    DWORD ret;
    VTIME t;

    ret = GetFileState(Path,&stat);
    if ( ret == ERROR_SUCCESS )
    {
        t = vtime(stat.st_mtime);
        *Time = t;
    }

    return ret;
}
/*********************************************************************************************************/
DWORD SetFileDate(const char *Path,const VTIME *Time)
{

    DWORD ret;
    struct stat stat;

    ret = GetFileState(Path,&stat);
    if ( ret == ERROR_SUCCESS )
    {
        stat.st_mtime = VTime2TimeT(*Time);
        ret =  SetFileState(Path,&stat);
    }

    return ret;
}
/************************************************************************************/
HANDLE DebugTGID = 0;

#ifdef NLM
void DebugScreen(void *nothing)
{

    int ScreenHan;

    REF(nothing);

    ThreadCount++;
    ScreenHan = CreateScreen("RTVSCAN - Debug",DONT_CHECK_CTRL_CHARS|AUTO_DESTROY_SCREEN);
    if ( ScreenHan==EFAILURE || ScreenHan==NULL )
    {
        RingTheBell();
        ThreadCount--;
        return;
    }
    SetCurrentScreen(ScreenHan);
    SetAutoScreenDestructionMode(TRUE);

    DebugTGID = reinterpret_cast<HANDLE>(GetThreadGroupID());

    RenameThread((int)NTxGetCurrentThreadId(), "RTV Debug");
    ThreadCount--;

    while ( NLMRunning && debug&DEBUGPRINT )
        NTxSleep(500);

    DestroyScreen (ScreenHan);
    ScreenHan=0;
    DebugTGID=0;
}
#endif // NLM

/************************************************************************************/
void Real_dvprintfTagged(DEBUGFLAGTYPE dwTag,const char *format,va_list args)
{

    char line[LOG_LINE_BUF_LEN];
    HANDLE tgid = 0;

    // check skipping untagged lines

    if ( dwTag == 0 && (debug & DEBUGEXCLUDEUNTAGGED) )
    {
        return;
    }

    if ( debug&DEBUGPRINT )
    {
#ifdef NLM
        if ( DebugTGID == 0 )
        {
            BeginThreadGroup(DebugScreen,NULL,STACK_SIZE,NULL);
            NTxSleep(100);
        }
#endif
        if ( DebugTGID )
        {
#ifdef LINUX
            tgid = (HANDLE)SetThreadGroupID(DebugTGID);
#else
            tgid = (HANDLE)SetThreadGroupID(reinterpret_cast<LONG>(DebugTGID));
#endif // LINUX

#if defined(LINUX)
        }
#endif // defined(LINUX)

            DWORD dwFineTime = GetFineLinearTimeEx( ONEDAY_MAX_RES );

            DWORD dwSeconds = dwFineTime / ONEDAY_MAXRES_TICKS_PER_SECOND;
            DWORD dwMicros  = OneDayMaxResToMicroSeconds( dwFineTime % ONEDAY_MAXRES_TICKS_PER_SECOND );
            DWORD dwHours, dwMins, dwSecs;

            SecondsToHoursMinsSecs( dwSeconds, dwHours, dwMins, dwSecs );

            if ( debug&DEBUGTHREADIDS )
            {
                sssnprintf(line, sizeof(line), "%02d:%02d:%02d.%06d[_%p %p_]|", static_cast<int>(dwHours), static_cast<int>(dwMins), static_cast<int>(dwSecs), static_cast<int>(dwMicros), NTxGetCurrentThreadId(), tgid );
            }
            else
            {
                sssnprintf(line, sizeof(line), "%02d:%02d:%02d.%06d|", static_cast<int>(dwHours), static_cast<int>(dwMins), static_cast<int>(dwSecs), static_cast<int>(dwMicros) );
            }
            ssvsnprintfappend(line, sizeof(line), format, args);
#ifdef NLM
            NWprintf("%s",line);
#endif // NLM

#ifdef SERVER
            if ( debug&DEBUGLOG )
                LogLine(line,FALSE);

#if defined(LINUX)
	    if (debug&DEBUGSYSLOG )
		SysLogLine(line);
#endif // LINUX
#endif // SERVER

#if defined(LINUX)
        if ( DebugTGID )
        {
#endif // defined(LINUX)
#ifdef LINUX
            SetThreadGroupID(tgid);
#else
            SetThreadGroupID(reinterpret_cast<LONG>(tgid));
#endif // LINUX
        }
    }
}

void Real_dprintfTagged(DEBUGFLAGTYPE dwTag,const char *format,...)
{
	va_list args;
	va_start(args,format);
	Real_dvprintfTagged(dwTag,format,args);
	va_end(args);
}

/************************************************************************************/

void Real_dvprintf(const char *format,va_list args)
{

    char line[LOG_LINE_BUF_LEN];
    HANDLE tgid = 0;

    // check skipping untagged lines

    if ( debug & DEBUGEXCLUDEUNTAGGED )
    {
        return;
    }

    if ( debug&DEBUGPRINT )
    {
#ifdef NLM
        if ( DebugTGID == 0 )
        {
            BeginThreadGroup(DebugScreen,NULL,STACK_SIZE,NULL);
            NTxSleep(100);
        }
#endif // NLM
        if ( DebugTGID )
        {
#ifdef LINUX
            tgid = (HANDLE)SetThreadGroupID(DebugTGID);
#else
            tgid = (HANDLE)SetThreadGroupID(reinterpret_cast<LONG>(DebugTGID));
#endif // LINUX
#if defined(LINUX)
        }
#endif // defined(LINUX)
            DWORD dwFineTime = GetFineLinearTimeEx( ONEDAY_MAX_RES );

            DWORD dwSeconds = dwFineTime / ONEDAY_MAXRES_TICKS_PER_SECOND;
            DWORD dwMicros  = OneDayMaxResToMicroSeconds( dwFineTime % ONEDAY_MAXRES_TICKS_PER_SECOND );
            DWORD dwHours, dwMins, dwSecs;

            SecondsToHoursMinsSecs( dwSeconds, dwHours, dwMins, dwSecs );

            if ( debug&DEBUGTHREADIDS )
            {
                sssnprintf(line, sizeof(line), "%02d:%02d:%02d.%06d[_%p %p_]|", static_cast<int>(dwHours), static_cast<int>(dwMins), static_cast<int>(dwSecs), static_cast<int>(dwMicros), NTxGetCurrentThreadId(), tgid );
            }
            else
            {
                sssnprintf(line, sizeof(line), "%02d:%02d:%02d.%06d|", static_cast<int>(dwHours), static_cast<int>(dwMins), static_cast<int>(dwSecs), static_cast<int>(dwMicros) );
            }

            ssvsnprintfappend(line, sizeof(line), format, args);
#ifdef NLM
            NWprintf("%s",line);
#endif // NLM

#if defined(SERVER) || defined(START)
            if ( debug&DEBUGLOG )
                LogLine(line,FALSE);

#if defined(LINUX)
	    if (debug&DEBUGSYSLOG )
		SysLogLine(line);
#endif // LINUX
#endif // SERVER

#if defined(LINUX)
        if ( DebugTGID )
        {
#endif // defined(LINUX)
#ifdef LINUX
            SetThreadGroupID(tgid);
#else
            SetThreadGroupID(reinterpret_cast<LONG>(tgid));
#endif // LINUX
        }
    }
}

void Real_dprintf(const char *format,...)
{
	va_list args;
	va_start(args,format);
	Real_dvprintf(format,args);
	va_end(args);
}

#if (defined(NLM) || defined(LINUX))
/***************************************************************************************************/
DWORD random(DWORD max)
{

    DWORD r1,r2;

    r1=rand();
    r2=(rand()<<16)+r1;
    return r2%max;
}

/*************************************************************************************/
/*******************************************************************************************/
void MakeWriteable (const char *file,DWORD mask)
{

    struct stat Stats;
    int cc, han;

    han = open(file,O_RDONLY);
    if ( han != -1 )
    {
        cc = fstat(han,&Stats);
        close (han);

        if ( !cc )
        {              // so get the current attributes
#ifndef LINUX
            Stats.st_attr &= mask;  // and set them to read/write
#endif
            MySetFileInfo (file,&Stats);            // and write to the file
        }
        else
        {
            dprintf("Cannot get attributes for %s: %s!", file, strerror(errno));
        }
    }
}
#endif // (defined(NLM) || defined(LINUX))

#ifdef NLM
// Provide a reference counted LoadLibrary / FreeLibrary implementation.
typedef std::map<std::string, unsigned int> REF_COUNT_MAP;

static REF_COUNT_MAP    s_mapLibraryRefCounts;
static cCriticalSection s_objLibraryRefCountSentry;

#ifdef NLM
#include "LaunchNLM.cpp"
/*********************************************************************/
HANDLE _LoadLibrary(const char *szName, bool bLoad)
{
    if ( bLoad )
        dprintf ("Attempting to Load: %s\n", szName);

    long hNLM = 0;

    try
    {
        char szTempBuf[MAX_PATH + 1];
        szTempBuf[0] = '\0';

        // Determine if the specified name includes launch parameters. Parse
        // out the parameters and remove them from name.
        const char *szParameters = NULL;
        const char *szSpace      = strchr (szName,' ');
        if (szSpace)
        {
            // Setup pointer to the parameter list.
            szParameters = NextChar(szSpace);

            // Copy the name of the NLM to a temporary buffer while truncating
            // the parameters off of the end of the NLM name specification.
            ssStrnCpyn(szTempBuf, szName, sizeof(szTempBuf) ,szSpace - szName + 1);
            szName = szTempBuf;
        }

        // Look for the beginning of the filename by walking backwards from the
        // end of the string.
        const char *q = szName + NumBytes(szName) - 1;
        while ( q > szName && *q != ':' && *q != '/' && *q != '\\' )
            q=NWPrevChar(szName,const_cast<char*>(q));
        /* not enabled for multi byte*/

        std::string strFileOnly = q;

        // Convert the string to lower case. That way we are doing
        // case-insensitive lookups. This is not DBCS enabled because
        // all of our NLM filenames are single-byte filenames.
        std::string::iterator iter = strFileOnly.begin();
        for (; iter != strFileOnly.end(); ++iter)
            *iter = tolower (*iter);

        hNLM = FindNLMHandle(q==szName?q:q+1);

        if ( hNLM )
        {
           	dprintf ("\t%s Already Loaded\n", szName);
            return reinterpret_cast<HANDLE>(hNLM);
        }

        if ( !bLoad )
            return 0;

        int nSpawnResult = LaunchNLM (const_cast<char*>(szName),
                                      const_cast<char*>(szName),
                                      const_cast<char*>(szParameters));

        if (nSpawnResult)
        {
            dprintf("\t%s spawnlp failed\n",szName);
            return 0;
        }

        dprintf("looking for '%s' \n",q==szName?q:q+1);
        for (int i=0; i < 60 && !hNLM; i++)
        {
            NTxSleep(100);
            hNLM = FindNLMHandle(q==szName?q:q+1);
        }

       	dprintf ("\t%s Loaded\n", szName);
    }
    catch (std::bad_alloc&)
    {
        // _TODO: JimBr: Behzad, should we call nlm_FreeLibrary or _FreeLibrary if this method did a LaunchNLM()?
    }

    NTxSleep(2000); // Trend's nlm will crash if we call VEInit before waiting a little after load.

    return reinterpret_cast<HANDLE>(hNLM);
}
/****************************************************************************************/
void nlm_FreeLibrary(HANDLE hNLM)
{

    typedef struct
    {
        char stuff[48];
        char len;
        char name[1];
    } HNLM;

    HNLM *h = (HNLM *)hNLM;
    if ( h->len == strlen(h->name) )
    {
        _FreeLibrary(h->name);
    }
}
/************************************************************************************************/
void _FreeLibrary(const char *szName)
{
    const char *szFileOnly = StrRChar(szName, cSLASH);
    if ( szFileOnly == NULL )
        szFileOnly = szName;
    else
        szFileOnly++;

    try
    {
        std::string strFileOnly = szFileOnly;

        // Convert the string to lower case. That way we are doing
        // case-insensitive lookups. This is not DBCS enabled because
        // all of our NLM fileszNames are single-byte filenames.
        std::string::iterator iter = strFileOnly.begin();
        for (; iter != strFileOnly.end(); ++iter)
            *iter = tolower (*iter);
        char line[64];
        vpstrncpy    (line, "UNLOAD ",  sizeof (line));
        vpstrnappend (line, szFileOnly, sizeof (line));

        system(line);

        // Wait for the NLM to fully unload
        int cnt=150;
        do
        {
            NTxSleep(1000);
        } while ( _LoadLibrary(szFileOnly, false) && cnt-- > 0 );

       dprintf ("\t%s UnLoaded\n", szName);
    }
    catch (std::bad_alloc&)
    {
    }
}
#endif // NLM

/*********************************************************************/

BOOL MyCopyFile(const char *from, const char *to)
{

    struct stat
    sStat;
    int
    sHan=-1,
    dHan=-1,
    ccode=-1;

    if ( !to || !from )
        return FALSE;

    memset (&sStat,0,sizeof(struct stat));

    dprintf ("Copying File %s -> %s\n",from,to);
    ThreadSwitchWithDelay();

    sHan = open(from,O_BINARY|O_RDONLY);
    if ( sHan != INVALID_FILE_HANDLE_VALUE )
    {
        dHan = open(to,O_WRONLY|O_TRUNC|O_BINARY|O_CREAT,S_IWRITE|S_IREAD);
        if ( dHan == INVALID_FILE_HANDLE_VALUE )
        {             // maybe the file is read-only
            dprintf ("\tMyCopyFile Open 1 %s Failed: %d-Destination file may be read only\n",to,errno);
            MakeWriteable (to,0xfffffffc);
            dHan = open(to,O_WRONLY|O_TRUNC|O_BINARY|O_CREAT,S_IWRITE|S_IREAD);
            if ( dHan == INVALID_FILE_HANDLE_VALUE )
            {
                dprintf ("\tUnable to open file: %s\n",to);
            }
        }
        if ( dHan != INVALID_FILE_HANDLE_VALUE )
        {
            LONG size = lseek(sHan,0,SEEK_END);
            LONG out=0;
            lseek(sHan,0,SEEK_SET);
//                      ccode=MasterFileCopy(NULL, from , NULL, to, COPY_ALWAYS|COPY_SAVE_DATE);
            ccode=FileServerFileCopy(sHan,dHan,0,0,size,&out);
            switch ( ccode )
            {
                case ERROR_SUCCESS:
                    break;
                default:
#ifdef START
                    LogInstallMessage("File Copy failed with error code of %d (%02x)\n",ccode,ccode);
#endif
                    close(dHan);
                    close(sHan);
                    return ccode;
            }
            close(dHan);
        }
        else
        {
#ifdef START
            LogInstallMessage("\tMyCopyFile open 2 %s failed with errno: %d\n",to,errno);
#endif
            ccode=errno;
        }
        close(sHan);
    }
    else
    {
#ifdef START
        LogInstallMessage("\tMyCopyFile open 3 %s failed with errno: %d\n",from,errno);
#endif
        ccode=errno;
    }
    sHan = open(from,O_BINARY|O_RDONLY); // set the file time on the destination to the same as the
    if ( sHan != INVALID_FILE_HANDLE_VALUE )
    {
        if ( fstat(sHan,&sStat)==0 )
        {
//                      sStat.st_attr |= 0x01;
            MySetFileInfo (to,&sStat);
        }
        close (sHan);
    }
    return ccode;
}
/***************************************************************************************/

// This is already provided by Enterprise_Systems_SESACollectors
// Collector_Framework_Plugins\Common\Connector\directory.cpp. As of 9/18/2003,
// these are almost exactly the same functions. The only difference is in
// DIRToFD(). The StrNCopy() is replaced with their version.
#ifndef DO_NOT_INCLUDE_FINDFIRSTFILE

/*****************************************************************************************/
#ifndef START
char *_strncpy(char *d,const char *s,int n)
{
    char *org=d;
    n--;
    while ( n&&*s )
    {
        *d++=*s++;
        n--;
    }
    *d=0;
    return org;
}       //MLR Fixed
#endif
/***************************************************************************************/
static void DIRToFD( DIR* dir, WIN32_FIND_DATA* fd )
{
    // save away all the important attributes
    fd->dwFileAttributes = dir->d_attr;
    fd->ftCreationTime   = dir->d_cdatetime;
    fd->ftLastAccessTime = dir->d_adatetime;

    // this conversion isn't clear, but I am not using this anyway
    fd->ftLastWriteTime  = dir->d_date << 16 | dir->d_time;
    fd->nFileSizeHigh    = 0;  // don't have files this big;
    fd->nFileSizeLow     = dir->d_size;

    // porting from changelist 134119 from the trunk
    memset( fd->cFileName, 0, sizeof(fd->cFileName) );
    strncpy( fd->cFileName, dir->d_name, sizeof(fd->cFileName) );
    fd->cFileName[sizeof(fd->cFileName) - 1] = 0;

    memset( fd->cAlternateFileName, 0, sizeof(fd->cAlternateFileName) );
    strncpy( fd->cAlternateFileName, dir->d_name, sizeof(fd->cAlternateFileName) );
    fd->cAlternateFileName[sizeof(fd->cAlternateFileName) - 1] = 0;
}

/***************************************************************************************/
HANDLE FindFirstFile(const char *path,WIN32_FIND_DATA *fd)
{
    DIR *dir;
 	BYTE	oldNameSpace;

    dir = opendir(path);
 	if (!dir)	// If opendir is unsuccessful try using the other name space either DOS (0) or LONG (4)
 	{
 		oldNameSpace = SetCurrentNameSpace(0);
 		dprintf("FINDFIRSTFILE: SetCurrentNameSpace(0): %d\n", oldNameSpace);
 		if (0 == oldNameSpace)
 			SetCurrentNameSpace(4);
 
     	dir = opendir(path);	// The result of this call will be tested below.
 		if (oldNameSpace != 255)
 			SetCurrentNameSpace(oldNameSpace);
 	}
    if ( dir )
    {
        fd->dir = readdir(dir);
        if ( fd->dir )
            DIRToFD( fd->dir, fd );
        else
        {
            closedir(dir);
            dir = (DIR *)INVALID_HANDLE_VALUE;
        }
    }
    else
        dir = (DIR *)INVALID_HANDLE_VALUE;

    return(HANDLE)dir;
}
#endif //DO_NOT_INCLUDE_FINDFIRSTFILE
#if defined(LINUX) || !defined(DO_NOT_INCLUDE_FINDFIRSTFILE)
/*********************************************************************/
BOOL FindNextFile(HANDLE han,WIN32_FIND_DATA *fd)
{
    BOOL ret = FALSE;

    fd->dir = readdir((DIR *)han);
    if ( fd->dir )
    {
        DIRToFD( fd->dir, fd );
        ret = TRUE;
    }
    return ret;
}
/*********************************************************************/
BOOL FindClose(HANDLE han)
{

    DIR *dir = (DIR *)han;

    closedir(dir);

    return 1;
}
#endif // defined(LINUX) || !defined(DO_NOT_INCLUDE_FINDFIRSTFILE)
#endif // SERVER || NLMCGI || START
#endif // NLM

#ifndef LINUX
/************************************************************************/
void InitializeCriticalSection(LPCRITICAL_SECTION lock)
{

    lock->ThreadID = 0;
    lock->inuse = 0;
}
/************************************************************************/
void EnterCriticalSection(LPCRITICAL_SECTION lock)
{

    int id = (int) NTxGetCurrentThreadId();

    while ( lock->inuse && lock->ThreadID != id )
        NTxSleep (20);

    lock->ThreadID = id;
    lock->inuse++;
}
/************************************************************************/
void LeaveCriticalSection(LPCRITICAL_SECTION lock)
{

    // The fact that LeaveCriticalSection is thread neutral is leveraged in various
    // places.  This allows a NetWare Critical Section to be used like a WIN32 Event,
    // where one thread sets it and another thread resets it.

    lock->inuse--;
}
/************************************************************************/
void DeleteCriticalSection(LPCRITICAL_SECTION lock)
{
    lock->inuse = 0;
    lock->ThreadID = 0;
}
#endif // ! LINUX

#endif // ! GLUE
/************************************************************************/
BOOL IsWin9x( void )
{
    return( FALSE );
}
/****************************************************************************/
BOOL IsWinNT( void )
{
    return( FALSE );
}
/****************************************************************************/
void UuidCreateFake( GUID* guid )
{
    // Generate a fake GUID -- just random data.
    // This GUID will have no guarantee of uniqueness.

    if( guid != NULL )
    {
        DWORD* iter = (DWORD*)guid;

        iter[0] = random( 0xFFFFFFFF );
        iter[1] = random( 0xFFFFFFFF );
        iter[2] = random( 0xFFFFFFFF );
        iter[3] = random( 0xFFFFFFFF );
    }
}

/************************************************************************************/
#ifdef NLM
void CreateGuid( GUID* guid )
{
	NWCCODE        ccode;
	NW_GUID        guidlist[MAX_GUIDS];
	NWCONN_HANDLE  connHandle;
    int             j;
    char*           myguid = (char*)guid;

	/* Initialize libraries */
	ccode = NWCallsInit(NULL, NULL);
	if(ccode)
	{
		dprintf("\nNWCallsInit returned %04X", ccode);
        return;
	}

	ccode = NWCCOpenConnByName(0, FileServerName, NWCC_NAME_FORMAT_BIND, NWCC_OPEN_LICENSED, NWCC_TRAN_TYPE_WILD, &connHandle);
	if(ccode)
	{
		dprintf("NWCCOpenConnByName returned %x\n", ccode);
        NWCallsTerm(NULL);
        return;
	}

	ccode = NWGenerateGUIDs(connHandle, sizeof(guidlist) / sizeof(NW_GUID), guidlist);
	if(ccode)
	{
		dprintf("NWGenerateGUIDs returned %x\n", ccode);
        NWCCCloseConn(connHandle);
        NWCallsTerm(NULL);
        return;
	}
	else
	{
        for(j = 0; j < sizeof(guidlist->GUID); j++)
        {
            myguid[j] = (char)guidlist[0].GUID[j];
        }
	}

	NWCCCloseConn(connHandle);
	NWCallsTerm(NULL);
}
//for client conversion, cleanup scheduled scans. we'll get them from our new parent server
void ClearOldServerScanList()
{
    char name[NAME_SIZE];
    int index;
    HKEY hkey,hpkey,lsKey;

    memset (name,0,sizeof(name));
    if(RegOpenKey(hMainKey,"LocalScans",&lsKey)!=ERROR_SUCCESS)
        return;

    for (index=0 ; RegEnumKey(lsKey,index++,name,sizeof(name)) == ERROR_SUCCESS ; ) {

        if (RegOpenKey(lsKey,name,&hpkey) != ERROR_SUCCESS) 
            continue;

        if (RegOpenKey(hpkey,"Schedule",&hkey) == ERROR_SUCCESS) { // if it has a "schedule" subkey then it is a scheduled scan

            RegCloseKey(hkey);
            RegDeleteKey(hpkey, "Schedule");
        }
        RegCloseKey(hpkey);
    }
}
#endif
/************************************************************************************/
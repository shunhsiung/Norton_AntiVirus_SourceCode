// Dec2.h : Decomposer 2 Interface
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2_H)
#define DEC2_H

/////////////////////////////////////////////////////////////////////////////
// Forward Declarations

class IDecomposer;
class IDecEngine;
class IDecObject;
class IDecContainerObject;
class IDecEventSink;


/////////////////////////////////////////////////////////////////////////////
// DEC_TYPE_xxx Codes

enum DecTypes
{
    DEC_TYPE_UNKNOWN,
    DEC_TYPE_AMG,
    DEC_TYPE_ARC,
    DEC_TYPE_ARJ,
    DEC_TYPE_CAB,
    DEC_TYPE_GHO,
    DEC_TYPE_GZIP,
    DEC_TYPE_HQX,
    DEC_TYPE_HTTP,
    DEC_TYPE_JAR,
    DEC_TYPE_LHA,
    DEC_TYPE_LN,
    DEC_TYPE_LZ,
    DEC_TYPE_MIME,
    DEC_TYPE_OLESS,
    DEC_TYPE_RAR,
    DEC_TYPE_RTF,
    DEC_TYPE_TAR,
    DEC_TYPE_UUE,
    DEC_TYPE_XBO,
    DEC_TYPE_ZIP,
    DEC_TYPE_ZOO
};

/////////////////////////////////////////////////////////////////////////////
// DECRESULT Codes

enum DecResult
{
    DECRESULT_UNCHANGED = 0,
    DECRESULT_CHANGED = 1,
    DECRESULT_TO_BE_REPLACED = 2,
    DECRESULT_TO_BE_DELETED = 3
};


/////////////////////////////////////////////////////////////////////////////
// HRESULT Codes
//
// All Decomposer HRESULT error values use the facility FACILITY_ITF
// and error codes in the range 0x0200 - 0xFFFF.  This is in accordance
// with Microsoft's recommendations in the Platform SDK.
//
// Once created, an error code's value must not change.
// Engine authors should resist adding new error codes whenever possible.
// Applications should properly handle unlisted error codes.

#if defined(UNIX)
#define MAKE_HRESULT(sev,fac,code) ((HRESULT)(((unsigned long)(sev)<<31)|((unsigned long)(fac)<<16)|((unsigned long)(code))))
#define FACILITY_ITF 4 
#endif

#define DECERR(code)  MAKE_HRESULT(1, FACILITY_ITF, 0x0200 + code)

enum DecErr
{
    DEC_OK                          = 0,

    DECERR_ENGINE_NOT_SUPPORTED     = DECERR(10),
    DECERR_ENGINE_FILE_MISSING      = DECERR(11),

    DECERR_OBJECT_FILE_INVALID      = DECERR(20),

    DECERR_CONTAINER_OPEN           = DECERR(30),
    DECERR_CONTAINER_ACCESS         = DECERR(31),
    DECERR_CONTAINER_UPDATE         = DECERR(32),
    DECERR_CONTAINER_CLOSE          = DECERR(33),

    DECERR_CHILD_EXTRACT            = DECERR(40),
    DECERR_CHILD_ACCESS             = DECERR(41),
    DECERR_CHILD_DECRYPT            = DECERR(42),
    DECERR_CHILD_SPLIT              = DECERR(43),
    DECERR_CHILD_SPLIT_PART         = DECERR(44),
    DECERR_CHILD_SIZE_UNKNOWN       = DECERR(45),

    DECERR_TEMPDIR_INVALID          = DECERR(50),

    DECERR_TEMPFILE_CREATE          = DECERR(61),
    DECERR_TEMPFILE_OPEN            = DECERR(62),
    DECERR_TEMPFILE_READ            = DECERR(63),
    DECERR_TEMPFILE_WRITE           = DECERR(64),
    DECERR_TEMPFILE_CLOSE           = DECERR(65),
    DECERR_TEMPFILE_DELETE          = DECERR(66),

    DECERR_DISK_FULL                = DECERR(70),
    DECERR_DISK_NOT_READY           = DECERR(71),

    DECERR_OUT_OF_MEMORY            = DECERR(80),

    DECERR_INVALID_PARAMS           = DECERR(90),
    DECERR_INVALID_CHANGE           = DECERR(91),
    DECERR_INVALID_REPLACE          = DECERR(92),
    DECERR_INVALID_DELETE           = DECERR(93),

    DECERR_USER_CANCEL              = DECERR(100),

    DECERR_ENGINE_EXCEPTION         = DECERR(150),

    DECERR_GENERIC                  = DECERR(200),
};


/////////////////////////////////////////////////////////////////////////////
// DEC_CHARSET_xxx codes

enum DecCharset
{
    DEC_CHARSET_CP850,          // MS-DOS Latin1 (OEM)
    DEC_CHARSET_CP1252,         // Windows-1252 WinLatin1 (ANSI)
    DEC_CHARSET_US_ASCII,
    DEC_CHARSET_ISO_8859_1,     // sub-set of CP1252
    DEC_CHARSET_ISO_8859_2,
    DEC_CHARSET_ISO_8859_3,
    DEC_CHARSET_ISO_8859_4,
    DEC_CHARSET_ISO_8859_5,
    DEC_CHARSET_ISO_8859_6,
    DEC_CHARSET_ISO_8859_7,
    DEC_CHARSET_ISO_8859_8,
    DEC_CHARSET_ISO_8859_9,
    DEC_CHARSET_ISO_8859_10,
    DEC_CHARSET_ISO_8859_11,
    DEC_CHARSET_ISO_8859_12,
    DEC_CHARSET_ISO_8859_13,
    DEC_CHARSET_ISO_8859_14,
    DEC_CHARSET_ISO_8859_15,
    DEC_CHARSET_ISO_2022_JP
};

#define DEC_CHARSET_LAST    DEC_CHARSET_ISO_2022_JP


/////////////////////////////////////////////////////////////////////////////
// Interface IDecomposer

class IDecomposer
{
public:
    // IDecomposer methods:
    virtual HRESULT AddEngine(IDecEngine* pEngine) = 0;
    virtual HRESULT RemoveEngines() = 0;
    virtual HRESULT SetTempDir(LPCTSTR szDir) = 0;
    virtual HRESULT SetTrustFileExtensions(BOOL bEnabled) = 0;
    virtual HRESULT Process(IDecObject* pObject, IDecEventSink* pSink, WORD* pwResult, LPTSTR pszNewDataFile) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecEngine

class IDecEngine
{
public:
    // IDecEngine methods:
    virtual HRESULT SetTempDir(LPCTSTR szDir) = 0;
    virtual HRESULT SetTrustFileExtensions(BOOL bEnabled) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecObject

class IDecObject
{
public:
    // IDecObject methods:
    virtual HRESULT GetName(LPTSTR pszName) = 0;
    virtual HRESULT GetNameCharacterSet(DWORD *pdwCharset) = 0;
    virtual HRESULT GetParent(IDecContainerObject** ppParentObject) = 0;
    virtual HRESULT GetDepth(DWORD* pdwDepth) = 0;
    virtual HRESULT GetDataFile(LPTSTR pszDataFile) = 0;
    virtual HRESULT GetCanDelete(BOOL* pbCanDelete) = 0;
    virtual HRESULT GetCanDeleteAncestor(BOOL* pbCanDelete, DWORD* pdwDepth) = 0;
    virtual HRESULT GetCanModify(BOOL* pbCanModify) = 0;
    virtual HRESULT GetCanReplace(BOOL* pbCanReplace) = 0;
    virtual HRESULT GetProperty(LPCTSTR szName, void** ppValue) = 0;
    virtual HRESULT SetProperty(LPCTSTR szName, void* pValue) = 0;
    virtual HRESULT RemoveProperty(LPCTSTR szName) = 0;
    virtual HRESULT GetNormalizedName(LPTSTR pszInput, LPTSTR pszOutput, DWORD dwOutputSize, BOOL bANSI) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecContainerObject

class IDecContainerObject :
    public IDecObject
{
public:
    // IDecObject methods:
    virtual HRESULT GetName(LPTSTR pszName) = 0;
    virtual HRESULT GetNameCharacterSet(DWORD *pdwCharset) = 0;
    virtual HRESULT GetParent(IDecContainerObject** ppParentObject) = 0;
    virtual HRESULT GetDepth(DWORD* pdwDepth) = 0;
    virtual HRESULT GetDataFile(LPTSTR pszDataFile) = 0;
    virtual HRESULT GetCanDelete(BOOL* pbCanDelete) = 0;
    virtual HRESULT GetCanDeleteAncestor(BOOL* pbCanDelete, DWORD* pdwDepth) = 0;
    virtual HRESULT GetCanModify(BOOL* pbCanModify) = 0;
    virtual HRESULT GetCanReplace(BOOL* pbCanReplace) = 0;
    virtual HRESULT GetProperty(LPCTSTR szName, void** ppValue) = 0;
    virtual HRESULT SetProperty(LPCTSTR szName, void* pValue) = 0;
    virtual HRESULT RemoveProperty(LPCTSTR szName) = 0;
    virtual HRESULT GetNormalizedName(LPTSTR pszInput, LPTSTR pszOutput, DWORD dwOutputSize, BOOL bANSI) = 0;

    // IDecContainerObject methods:
    virtual HRESULT GetType(DWORD* pdwType) = 0;
    virtual HRESULT GetUncompressedSize(DWORDLONG* pdwlSize) = 0;
    virtual HRESULT GetCompressedSize(DWORDLONG* pdwlSize) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecEventSink

class IDecEventSink
{
public:
    // IDecEventSink methods:

    virtual HRESULT OnNew(
        IDecObject* pObject, 
        WORD* pwResult, 
        LPTSTR pszNewDataFile, 
        BOOL* pbContinue) = 0;

    virtual HRESULT OnIdentified(
        IDecContainerObject* pObject, 
        BOOL* pbContinue) = 0;

    virtual HRESULT OnChildFound(
        IDecContainerObject* pObject, 
        LPCTSTR pszChildName,
        BOOL* pbContinue) = 0;

    virtual HRESULT OnBadChild(
        IDecContainerObject* pObject, 
        HRESULT hr,
        LPCTSTR pszChildName,
        BOOL* pbContinue) = 0;

    virtual HRESULT OnNotIdentified(
        IDecObject* pObject, 
        WORD* pwResult, 
        LPTSTR pszNewDataFile) = 0; 

    virtual HRESULT OnBusy() = 0;

    virtual HRESULT OnEngineError(
        IDecContainerObject* pObject, 
        HRESULT hr,
        WORD* pwResult, 
        LPTSTR pszNewDataFile) = 0; 
};


/////////////////////////////////////////////////////////////////////////////
// Class CDecEventSink
//
// Standard implementation of a Decomposer event sink.

class CDecEventSink : 
    public IDecEventSink
{
public:
    virtual HRESULT OnNew(
        IDecObject* pObject, 
        WORD* pwResult, 
        LPTSTR pszNewDataFile, 
        BOOL* pbContinue) 
    { 
        *pwResult = DECRESULT_UNCHANGED;
        *pszNewDataFile = '\0';
        *pbContinue = TRUE;
        return DEC_OK; 
    }

    virtual HRESULT OnIdentified(
        IDecContainerObject* pObject, 
        BOOL* pbContinue) 
    { 
        *pbContinue = TRUE;
        return DEC_OK; 
    }

    virtual HRESULT OnChildFound(
        IDecContainerObject* pObject, 
        LPCTSTR pszChildName,
        BOOL* pbContinue) 
    { 
        *pbContinue = TRUE;
        return DEC_OK; 
    }

    virtual HRESULT OnBadChild(
        IDecContainerObject* pObject, 
        HRESULT hr,
        LPCTSTR pszChildName,
        BOOL* pbContinue) 
    { 
        *pbContinue = FALSE;
        return hr; 
    }

    virtual HRESULT OnNotIdentified(
        IDecObject* pObject, 
        WORD* pwResult, 
        LPTSTR pszNewDataFile) 
    { 
        *pwResult = DECRESULT_UNCHANGED;
        *pszNewDataFile = '\0';
        return DEC_OK; 
    } 

    virtual HRESULT OnBusy() 
    { 
        return DEC_OK; 
    }

    virtual HRESULT OnEngineError(
        IDecContainerObject* pObject, 
        HRESULT hr,
        WORD* pwResult, 
        LPTSTR pszNewDataFile) 
    { 
        *pwResult = DECRESULT_UNCHANGED;
        *pszNewDataFile = '\0';
        return hr; 
    } 
};


/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2_CPP
#define DEC2IE __declspec(dllexport) 
#else
#define DEC2IE __declspec(dllimport) 
#endif
#endif

#if defined(_WINDOWS)
DEC2IE HRESULT DecNewDecomposer(IDecomposer** ppDecomposer);
DEC2IE HRESULT DecDeleteDecomposer(IDecomposer* pDecomposer);
DEC2IE HRESULT DecNewFileObject(LPCTSTR szName, LPCTSTR szDataFile, IDecObject** ppObject);
DEC2IE HRESULT DecDeleteFileObject(IDecObject* pObject);
#endif

#if defined(UNIX)
EXTERN_C HRESULT DecNewDecomposer(IDecomposer** ppDecomposer);
EXTERN_C HRESULT DecDeleteDecomposer(IDecomposer* pDecomposer);
EXTERN_C HRESULT DecNewFileObject(LPCTSTR szName, LPCTSTR szDataFile, IDecObject** ppObject);
EXTERN_C HRESULT DecDeleteFileObject(IDecObject* pObject);
#endif

#endif  // DEC2_H

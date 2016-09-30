/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#ifndef _DATAOBJ_H_
#define _DATAOBJ_H_

#include "atlsnap.h"  
#include "qscon.h"  

///////////////////////////////////////////////////////////////////////////////////
// Our data object class.
//

class ATL_NO_VTABLE CQSDataObject : public CSnapInDataObjectImpl,
									public IQCScopeItem
{
public:
BEGIN_COM_MAP(CQSDataObject)
	COM_INTERFACE_ENTRY(IDataObject)
    COM_INTERFACE_ENTRY(IQCScopeItem)
    COM_INTERFACE_ENTRY_CHAIN(CSnapInDataObjectImpl)
END_COM_MAP()
    CQSDataObject(){};
    
    static void Init()
        {
        m_CCF_COMPUTER_NAME = (CLIPFORMAT) RegisterClipboardFormat( _T("MMC_SNAPIN_MACHINE_NAME" ) );
        }
// 
// Overrides
// 
    STDMETHOD(GetDataHere)(FORMATETC* pformatetc, STGMEDIUM* pmedium);
public:
	STDMETHOD(AvisConfigGetValue)(/*[in]*/ BSTR bstrFieldName, 
								  /*[in]*/ BSTR bstrKeyName,
								  /*[out]*/ VARIANT *v );	

	STDMETHOD(AvisConfigSetValue)(/*[in]*/ BSTR bstrFieldName, 
								  /*[in]*/ BSTR bstrKeyName,
								  /*[in]*/ VARIANT *v );	

	STDMETHOD(GetQSVersion)(/*[out]*/ ULONG *ulVersion);

	STDMETHOD(GetQSDefVersion)(/*[out]*/ BSTR *bstrQSVersion);
	STDMETHOD(GetQSDefDate)(/*[out]*/ BSTR *bstrQSDate);
    // 
    // Computer name clipboard format.
    // 
    static CLIPFORMAT       m_CCF_COMPUTER_NAME;


private:
	HRESULT GetMachineName( LPSTREAM pStream );

// 
// Private data.
// 
private:
    
};

_declspec( selectany ) CLIPFORMAT CQSDataObject::m_CCF_COMPUTER_NAME = 0;


class ATL_NO_VTABLE CQSResultDataObject : public CSnapInDataObjectImpl,
                                          public IQCResultItem
{
public:
BEGIN_COM_MAP(CQSResultDataObject)
	COM_INTERFACE_ENTRY(IDataObject)
    COM_INTERFACE_ENTRY(IQCResultItem)
    COM_INTERFACE_ENTRY_CHAIN(CSnapInDataObjectImpl)
END_COM_MAP()

    CQSResultDataObject(){m_pComponent = NULL;};
    ~CQSResultDataObject();

// 
// IQCResultItem 
// 
    STDMETHOD(GetItemIDs)( 
        /*[out]*/ ULONG*  ulCount,
		/*[out]*/ ULONG** ulItemArray );

	STDMETHOD(GetQserverItem)( 
        /*[in]*/ ULONG ulItemID,
        /*[out]*/ IUnknown** pItem,
		/*[out]*/ ULONG *pIdentity);

public:
    // 
    // Pointer to our component data object.
    //     
    IComponentData*             m_pComponent;

// 
// Private data.
// 
private:
    
};



#endif
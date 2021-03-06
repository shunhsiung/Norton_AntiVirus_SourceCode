#ifndef OPENAPPSDIALOG_H
#define OPENAPPSDIALOG_H

///////////////////////////////////////////////////////////////////////////////
//
// CResultsLog
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _S
#define _S(x)(CString)(LPCTSTR)(x)
#endif

class COpenAppsDialog : 
	public CAxDialogImpl<COpenAppsDialog>
{
public:
	COpenAppsDialog()
	{
	};

	~COpenAppsDialog(){};

	enum { IDD = IDD_OPEN_APPS_DIALOG };

BEGIN_MSG_MAP(CLogDialog)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	CHAIN_MSG_MAP(CAxDialogImpl<COpenAppsDialog>)
END_MSG_MAP()

private:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CenterWindow(NULL);

		return 0;
	}

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}
	
	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}
};

#endif

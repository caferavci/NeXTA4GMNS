#pragma once
#include "afxwin.h"
#include "TLiteDoc.h"

// CDlg_NodeProperties dialog

class CDlg_NodeProperties : public CDialog
{
	DECLARE_DYNAMIC(CDlg_NodeProperties)

public:
	CDlg_NodeProperties(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_NodeProperties();

// Dialog Data
	enum { IDD = IDD_DIALOG_NodeProperties };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

		CTLiteDoc* m_pDoc;
	long NodeNo;
	CString NodeName;
	long ZoneID;

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();

	CString m_ControlType;
	CString m_NodeType;
	CStatic AddField1;
	CStatic AddField2;
	CStatic AddField3;
	CStatic AddField4;
	CStatic AddField5;
	CString AddFieldValue1;
	CString AddFieldValue2;
	CString AddFieldValue3;
	CString AddFieldValue4;
	CString AddFieldValue5;
};

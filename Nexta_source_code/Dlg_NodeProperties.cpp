// Dlg_NodeProperties.cpp : implementation file
//

#include "stdafx.h"
#include "TLite.h"
#include "Dlg_NodeProperties.h"


// CDlg_NodeProperties dialog

IMPLEMENT_DYNAMIC(CDlg_NodeProperties, CDialog)

CDlg_NodeProperties::CDlg_NodeProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_NodeProperties::IDD, pParent)
	, NodeNo(0)
	, NodeName(_T(""))
	, ZoneID(0)
	, m_ControlType(_T(""))
	, m_NodeType(_T(""))
	, AddFieldValue1(_T(""))
{

}

CDlg_NodeProperties::~CDlg_NodeProperties()
{
}

void CDlg_NodeProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NODE_ID, NodeNo);
	DDX_Text(pDX, IDC_EDIT_STREET_NAME, NodeName);
	DDV_MaxChars(pDX, NodeName, 100);
	DDX_Text(pDX, IDC_EDIT_ZoneID, ZoneID);
	DDV_MinMaxLong(pDX, ZoneID, -1, 1000000);
	DDX_Text(pDX, IDC_EDIT_ControlType, m_ControlType);
	DDX_Text(pDX, IDC_EDIT_NodeType, m_NodeType);
	DDX_Control(pDX, ID_NODE_ADDITIONAL_FIELD6, AddField1);
	DDX_Control(pDX, ID_NODE_ADDITIONAL_FIELD2, AddField2);
	DDX_Control(pDX, ID_NODE_ADDITIONAL_FIELD3, AddField3);
	DDX_Control(pDX, ID_NODE_ADDITIONAL_FIELD4, AddField4);
	DDX_Control(pDX, ID_NODE_ADDITIONAL_FIELD5, AddField5);

	DDX_Text(pDX, IDC_EDIT_NodeAddField1, AddFieldValue1);
	DDX_Text(pDX, IDC_EDIT_NodeAddField2, AddFieldValue2);
	DDX_Text(pDX, IDC_EDIT_NodeAddField3, AddFieldValue3);
	DDX_Text(pDX, IDC_EDIT_NodeAddField3, AddFieldValue4);
	DDX_Text(pDX, IDC_EDIT_NodeAddField4, AddFieldValue5);
}


BEGIN_MESSAGE_MAP(CDlg_NodeProperties, CDialog)
	ON_BN_CLICKED(IDOK, &CDlg_NodeProperties::OnBnClickedOk)
	ON_BN_CLICKED(ID_CANCEL, &CDlg_NodeProperties::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlg_NodeProperties message handlers

void CDlg_NodeProperties::OnBnClickedOk()
{
	UpdateData();


	OnOK();
}

BOOL CDlg_NodeProperties::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(theApp.m_Node_Additional_Field[1].GetLength() > 0) 
	{
		AddField1.SetWindowText(theApp.m_Node_Additional_Field[1]);
	}
	else
	{
		CWnd* tLabel = GetDlgItem(IDC_EDIT_NodeAddField1);
		tLabel->EnableWindow(0);
	}

	if (theApp.m_Node_Additional_Field[2].GetLength() > 0)
		AddField2.SetWindowText(theApp.m_Node_Additional_Field[2]);
	else
	{
		CWnd* tLabel = GetDlgItem(IDC_EDIT_NodeAddField2);
		tLabel->EnableWindow(0);
	}

	if (theApp.m_Node_Additional_Field[3].GetLength() > 0)
		AddField3.SetWindowText(theApp.m_Node_Additional_Field[3]);
	else
	{
		CWnd* tLabel = GetDlgItem(IDC_EDIT_NodeAddField3);
		tLabel->EnableWindow(0);
	}

	if (theApp.m_Node_Additional_Field[4].GetLength() > 0)
		AddField4.SetWindowText(theApp.m_Node_Additional_Field[4]);
	else
	{
		CWnd* tLabel = GetDlgItem(IDC_EDIT_NodeAddField4);
		tLabel->EnableWindow(0);
	}

	if (theApp.m_Node_Additional_Field[5].GetLength() > 0)
		AddField5.SetWindowText(theApp.m_Node_Additional_Field[5]);
	else
	{
		CWnd* tLabel = GetDlgItem(IDC_EDIT_NodeAddField5);
		tLabel->EnableWindow(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_NodeProperties::OnBnClickedCancel()
{
	OnCancel();
}







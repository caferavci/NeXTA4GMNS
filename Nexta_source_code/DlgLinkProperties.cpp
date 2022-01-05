// DlgLinkProperties.cpp : implementation file
//

#include "stdafx.h"
#include "TLite.h"
#include "DlgLinkProperties.h"


// CDlgLinkProperties dialog

IMPLEMENT_DYNAMIC(CDlgLinkProperties, CDialog)

CDlgLinkProperties::CDlgLinkProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLinkProperties::IDD, pParent)
	, FromNode(0)
	, ToNode(0)
	, LinkLength(0)
	, FreeSpeed(0)
	, FreeFlowTravelTime(0)
	, LaneCapacity(0)
	, nLane(0)
	, m_LinkID("")
	, StreetName(_T(""))
	, m_cycle_length(0)
	, m_start_green(0)
	, m_end_of_green(0)
	, m_AddFieldValue1(_T(""))
	, m_AddFieldValue2(_T(""))
	, m_AddFieldValue3(_T(""))
	, m_AddFieldValue4(_T(""))
	, m_AddFieldValue5(_T(""))
	, LinkType(0)
{
m_bEditChange = false;
}

CDlgLinkProperties::~CDlgLinkProperties()
{
}

void CDlgLinkProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FROM_NODE, FromNode);
	DDX_Text(pDX, IDC_EDIT_To_NODE, ToNode);
	DDX_Text(pDX, IDC_EDIT_LINK_TYPE_INDEX, LinkType);
	DDX_Text(pDX, IDC_EDIT_LENGTH, LinkLength);
	DDV_MinMaxFloat(pDX, LinkLength, 0.0001f, 10000);
	DDX_Text(pDX, IDC_EDIT_SPEEDLIMIT, FreeSpeed);
	DDV_MinMaxFloat(pDX, FreeSpeed, 5, 120);
	DDX_Text(pDX, IDC_EDIT_FFTT, FreeFlowTravelTime);
	DDX_Text(pDX, IDC_EDIT_LANE_CAPACITY, LaneCapacity);
	DDV_MinMaxInt(pDX, LaneCapacity, 0, 300000);
	DDX_Text(pDX, IDC_EDIT_NUMLANES, nLane);

	DDV_MinMaxInt(pDX, nLane, 0, 10);
	DDX_Text(pDX, IDC_EDIT_LINKID, m_LinkID);
	DDX_Text(pDX, IDC_EDIT_STREET_NAME, StreetName);
	DDX_Text(pDX, IDC_EDIT_MAIN_NODE_ID, main_node_id);
	DDX_Text(pDX, IDC_EDIT_MOVMENT_STR, mvmt_txt_id);
	DDV_MaxChars(pDX, StreetName, 100);

	DDX_Text(pDX, IDC_EDIT_CYCLE_LENGTH, m_cycle_length);
	DDX_Text(pDX, IDC_EDIT_Start_of_Green, m_start_green);
	DDX_Text(pDX, IDC_EDIT_End_of_Green, m_end_of_green);
	DDX_Control(pDX, IDC_LINK_ADDFIELD1, m_AddField1);
	DDX_Control(pDX, IDC_LINK_ADDFIELD2, m_AddField2);
	DDX_Control(pDX, IDC_LINK_ADDFIELD3, m_AddField3);
	DDX_Control(pDX, IDC_LINK_ADDFIELD4, m_AddField4);
	DDX_Control(pDX, IDC_LINK_ADDFIELD5, m_AddField5);
	DDX_Text(pDX, IDC_EDIT_LINK_ADD_FILED1, m_AddFieldValue1);
	DDX_Text(pDX, IDC_EDIT_LINK_ADD_FILED2, m_AddFieldValue2);
	DDX_Text(pDX, IDC_EDIT_LINK_ADD_FILED3, m_AddFieldValue3);
	DDX_Text(pDX, IDC_EDIT_LINK_ADD_FILED4, m_AddFieldValue4);
	DDX_Text(pDX, IDC_EDIT_LINK_ADD_FILED5, m_AddFieldValue5);

}


BEGIN_MESSAGE_MAP(CDlgLinkProperties, CDialog)
	ON_CBN_EDITCHANGE(IDC_COMBO1, &CDlgLinkProperties::OnCbnEditchangeCombo1)
	ON_BN_CLICKED(IDOK, &CDlgLinkProperties::OnBnClickedOk)
	ON_BN_CLICKED(IDSAVEASDEFAULT, &CDlgLinkProperties::OnBnClickedSaveasdefault)
	ON_BN_CLICKED(ID_CANCEL, &CDlgLinkProperties::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CDlgLinkProperties::OnCbnSelchangeCombo1)
	ON_EN_CHANGE(IDC_EDIT_LENGTH, &CDlgLinkProperties::OnEnChangeEditLength)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CDlgLinkProperties::OnBnClickedButtonUpdate)
	ON_EN_CHANGE(IDC_EDIT_STREET_NAME, &CDlgLinkProperties::OnEnChangeEditStreetName)
	ON_EN_CHANGE(IDC_EDIT_SPEEDLIMIT, &CDlgLinkProperties::OnEnChangeEditSpeedlimit)
	ON_EN_CHANGE(IDC_EDIT_NUMLANES, &CDlgLinkProperties::OnEnChangeEditNumlanes)
	ON_EN_CHANGE(IDC_EDIT_LANE_CAPACITY, &CDlgLinkProperties::OnEnChangeEditLaneCapacity)
	ON_EN_CHANGE(IDC_EDIT6, &CDlgLinkProperties::OnEnChangeEdit6)
	ON_EN_CHANGE(IDC_EDIT7, &CDlgLinkProperties::OnEnChangeEdit7)
	ON_EN_CHANGE(IDC_EDIT1, &CDlgLinkProperties::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT5, &CDlgLinkProperties::OnEnChangeEdit5)
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_EDIT8, &CDlgLinkProperties::OnEnChangeEdit8)
	ON_BN_CLICKED(IDC_CHECK2, &CDlgLinkProperties::OnBnClickedCheck2)
END_MESSAGE_MAP()


// CDlgLinkProperties message handlers

BOOL CDlgLinkProperties::OnInitDialog()
{
	CDialog::OnInitDialog();
	std::map<int, DTALinkType>:: const_iterator itr;


	EnableDataBasedOnLinkType();

	if (theApp.m_Link_Additional_Field[1].GetLength() > 0)
	{
		m_AddField1.SetWindowText(theApp.m_Link_Additional_Field[1]);
	}
	else
	{
		CWnd* tLabel = GetDlgItem(IDC_EDIT_LINK_ADD_FILED1);
		tLabel->EnableWindow(0);
	}

	if (theApp.m_Link_Additional_Field[2].GetLength() > 0)
	{
		m_AddField2.SetWindowText(theApp.m_Link_Additional_Field[2]);
	}
	else
	{
		CWnd* tLabel = GetDlgItem(IDC_EDIT_LINK_ADD_FILED2);
		tLabel->EnableWindow(0);
	}

	if (theApp.m_Link_Additional_Field[3].GetLength() > 0)
	{
		m_AddField3.SetWindowText(theApp.m_Link_Additional_Field[3]);
	}
	else
	{
		CWnd* tLabel = GetDlgItem(IDC_EDIT_LINK_ADD_FILED3);
		tLabel->EnableWindow(0);
	}

	if (theApp.m_Link_Additional_Field[4].GetLength() > 0)
	{
		m_AddField3.SetWindowText(theApp.m_Link_Additional_Field[4]);
	}
	else
	{
		CWnd* tLabel = GetDlgItem(IDC_EDIT_LINK_ADD_FILED4);
		tLabel->EnableWindow(0);
	}

	if (theApp.m_Link_Additional_Field[5].GetLength() > 0)
	{
		m_AddField3.SetWindowText(theApp.m_Link_Additional_Field[5]);
	}
	else
	{
		CWnd* tLabel = GetDlgItem(IDC_EDIT_LINK_ADD_FILED5);
		tLabel->EnableWindow(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgLinkProperties::EnableDataBasedOnLinkType()
{
	
}
void CDlgLinkProperties::OnCbnEditchangeCombo1()
{

}

void CDlgLinkProperties::OnBnClickedOk()
{
	OnOK();
}

void CDlgLinkProperties::OnBnClickedSaveasdefault()
{
	UpdateData(false);

	DefaultSpeedLimit = FreeSpeed;
	DefaultCapacity = LaneCapacity;
	DefaultnLane = nLane;
    DefaultLinkType = LinkType;

}

void CDlgLinkProperties::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgLinkProperties::OnCbnSelchangeCombo1()
{

	UpdateData(true);
}

void CDlgLinkProperties::OnEnChangeEditLength()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnBnClickedButtonUpdate()
{
	UpdateData(1);
	FreeFlowTravelTime = LinkLength/1000.0 / max(0.001,FreeSpeed);
	UpdateData(0);

}

void CDlgLinkProperties::OnEnChangeEditStreetName()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEditSpeedlimit()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEditNumlanes()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEditLaneCapacity()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEditSaturationFlowRate()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEditEffectiveGreenTime()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEdit6()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEdit7()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEdit1()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEdit2()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEdit4()
{
	m_bEditChange = true;
}

void CDlgLinkProperties::OnEnChangeEdit5()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEditAadt()
{
	m_bEditChange = true;
}

void CDlgLinkProperties::OnEnChangeEditPeakhourlyvolume()
{
	m_bEditChange = true;
}

void CDlgLinkProperties::OnClose()
{
	// TODO: Add your message handler code here and/or call default

//	CDialog::OnClose();

   int nRet = 5; 
   EndDialog(nRet); 

}

void CDlgLinkProperties::OnEnChangeEditNumlanesLeftTurn()
{
	m_bEditChange = true;
}

void CDlgLinkProperties::OnEnChangeEditNumlanesRightTurn()
{
	m_bEditChange = true;
}

void CDlgLinkProperties::OnEnChangeEdit8()
{
	m_bEditChange = true;
}

void CDlgLinkProperties::OnBnClickedCheck2()
{
	m_bEditChange = true;

}



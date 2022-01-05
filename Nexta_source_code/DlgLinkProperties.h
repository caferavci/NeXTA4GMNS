#pragma once
#include "afxwin.h"
#include "TLiteDoc.h"

// CDlgLinkProperties dialog

class CDlgLinkProperties : public CDialog
{
	DECLARE_DYNAMIC(CDlgLinkProperties)

public:

	int m_NumLeftTurnLanes, m_NumRightTurnLanes;
	bool m_bEditChange;
	CDlgLinkProperties(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLinkProperties();

	void EnableDataBasedOnLinkType();
// Dialog Data
	enum { IDD = IDD_DIALOG_LinkProperties };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTLiteDoc* m_pDoc;

	float m_LinkLocationX;
	float m_LinkLocationY;


	int FromNode;
	int ToNode;
	float LinkLength;
	float FreeSpeed;
	float FreeFlowTravelTime;
	int LaneCapacity;
	int nLane;
	int LinkType;


	float DefaultSpeedLimit;
	int DefaultCapacity;
	int DefaultnLane;
    int DefaultLinkType;

	virtual BOOL OnInitDialog();
	afx_msg void OnCbnEditchangeCombo1();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSaveasdefault();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeCombo1();
	CString m_LinkID;
	CString StreetName;
	CString mvmt_txt_id;
	CString main_node_id;
	CString nema_phase;

	afx_msg void OnEnChangeEditLength();
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnEnChangeEditStreetName();
	afx_msg void OnEnChangeEditSpeedlimit();
	afx_msg void OnEnChangeEditNumlanes();
	afx_msg void OnEnChangeEditLaneCapacity();
	afx_msg void OnEnChangeEditSaturationFlowRate();
	afx_msg void OnEnChangeEditEffectiveGreenTime();
	afx_msg void OnEnChangeEdit6();
	afx_msg void OnEnChangeEdit7();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnEnChangeEdit5();
	afx_msg void OnEnChangeEditAadt();
	afx_msg void OnEnChangeEditPeakhourlyvolume();
	afx_msg void OnClose();
	afx_msg void OnEnChangeEditNumlanesLeftTurn();
	afx_msg void OnEnChangeEditNumlanesRightTurn();
	afx_msg void OnEnChangeEdit8();
	afx_msg void OnBnClickedCheck2();
	int m_cycle_length;
	int m_start_green;
	int m_end_of_green;
	CStatic m_AddField1;
	CStatic m_AddField2;
	CStatic m_AddField3;
	CStatic m_AddField4;
	CStatic m_AddField5;
	CString m_AddFieldValue1;
	CString m_AddFieldValue2;
	CString m_AddFieldValue3;
	CString m_AddFieldValue4;
	CString m_AddFieldValue5;
};

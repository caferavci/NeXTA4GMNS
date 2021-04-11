#pragma once
#include "afxcmn.h"
#include "Network.h"
#include "TLiteDoc.h"
#include "BaseDialog.h"
#include "CGridListCtrlEx\\CGridListCtrlEx.h"
#include "afxwin.h"

// CDlgTrajectoryList dialog
enum eTrajectorySelectionMode {eTrajectorySelection_AllLinks = 0,
eTrajectorySelection_SelectedLinksOnly,
eTrajectorySelection_WithinSubarea,
eTrajectorySelection_FreewayOnly,eTrajectorySelection_HighwayOnly,eTrajectorySelection_RampOnly,eTrajectorySelection_ArterialOnly,eTrajectorySelection_NoConnectors};

class CDlgTrajectoryList : public CBaseDialog
{
	DECLARE_DYNAMIC(CDlgTrajectoryList)
	CTLiteDoc* m_pDoc;
	CTLiteDoc* m_pDoc2;
	bool m_bDoc2Ready;


	bool ExportDataToCSVFile(CString csv_file);

public:

	std::vector <int> m_ListedLinkNoVector;
	eTrajectorySelectionMode m_TrajectorySelectionMode;
	CDlgTrajectoryList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTrajectoryList();

	void ReloadData();
// Dialog Data
	enum { IDD = IDD_DIALOG_Trajectory_LIST	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CGridListCtrlEx m_ListCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTrajectorySelectionShowalllinks();
	afx_msg void OnTrajectorySelectionShowhighwaylinksonly();
	afx_msg void OnTrajectorySelectionShowramplinksonly();
	afx_msg void OnTrajectorySelectionShowarteriallinksonly();
	afx_msg void OnTrajectorySelectionShowalllinksexceptconnectors();
	afx_msg void OnUpdateTrajectorySelectionShowalllinks(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTrajectorySelectionShowhighwaylinksonly(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTrajectorySelectionShowramplinksonly(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTrajectorySelectionShowarteriallinksonly(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTrajectorySelectionShowalllinksexceptconnectors(CCmdUI *pCmdUI);
	BOOL m_ZoomToSelectedLink;
	CString m_StrDocTitles;

	afx_msg void OnBnClickedCheckZoomToSelectedLink();
	afx_msg void OnBnClickedBarchartpiechart();
	afx_msg void OnTrajectorySelectionShowselectedlinksonly();
	afx_msg void OnUpdateTrajectorySelectionShowselectedlinksonly(CCmdUI *pCmdUI);
	CComboBox m_ComboBox;
	afx_msg void OnCbnSelchangeComboTrajectorySelection();
	afx_msg void OnBnClickedExport();
	CComboBox m_StartHour;
	CComboBox m_EndHour;
	afx_msg void OnCbnSelchangeComboStarttime();
	afx_msg void OnCbnSelchangeComboEndtime();
	afx_msg void OnStnClickedDocTitle();
};

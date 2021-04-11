// Page_Node_Movement.cpp : implementation file
//
//  Portions Copyright 2010 Xuesong Zhou (xzhou99@gmail.com) 

//   If you help write or modify the code, please also list your names here.
//   The reason of having Copyright info here is to ensure all the modified version, as a whole, under the GPL 
//   and further prevent a violation of the GPL.

// More about "How to use GNU licenses for your own software"
// http://www.gnu.org/licenses/gpl-howto.html


//    This file is part of NeXTA Version 0.9.12022020 (Open-source).

//    NEXTA is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    NEXTA is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with NEXTA.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "TLite.h"
#include "Page_Node_Phase.h"

#include "CGridListCtrlEx\CGridColumnTraitEdit.h"
#include "CGridListCtrlEx\CGridColumnTraitCombo.h"
#include "CGridListCtrlEx\CGridRowTraitXP.h"
#include "MainFrm.h"
#include <string>
#include <sstream>
// CPage_Node_Phase dialog

extern CPen g_PenProhibitedMovement;

IMPLEMENT_DYNAMIC(CPage_Node_Phase, CPropertyPage)

CPage_Node_Phase::CPage_Node_Phase()
: CPropertyPage(CPage_Node_Phase::IDD)
, m_CurrentNode_Name(_T(""))
, m_MovementMsg(_T(""))
, m_CycleLength(0)
, m_Offset(0)
, m_bHideRightTurnMovement(TRUE)
{
	m_bColumnWidthIncludeHeader = true;
	m_SelectedMovementIndex = -1;
	m_bModifiedFlag = false;
	m_PeakHourFactor = 1.0;

	m_SelectedTimingPlanNo = 0;

	m_SelectedPhaseNumber = 0;

}

CPage_Node_Phase::~CPage_Node_Phase()
{
}

void CPage_Node_Phase::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CURRENT_NodeNo, m_CurrentNodeID);

	DDX_Control(pDX, IDC_GRID_Phasing, m_PhasingGrid);

	DDX_Control(pDX, IDC_COMBO1, m_ControlTypeComboBox);
	DDX_Control(pDX, IDC_COMBO_Timing_Plan, m_TimingPlanComboBox);
	DDX_Check(pDX, IDC_EDIT_MODE2, m_bHideRightTurnMovement);
}


BEGIN_MESSAGE_MAP(CPage_Node_Phase, CPropertyPage)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_LBN_SELCHANGE(IDC_LIST1, &CPage_Node_Phase::OnLbnSelchangeList1)
	ON_LBN_DBLCLK(IDC_LIST1, &CPage_Node_Phase::OnLbnDblclkList1)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_Phasing, OnGridEndSelChange)
	ON_WM_LBUTTONDBLCLK()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CPage_Node_Phase::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CPage_Node_Phase::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()



BOOL CPage_Node_Phase::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	for(int p = 1; p <= _max_phase_number; p++)
	{
		m_bAvailablePhaseVector [p] = false; 
		m_EffectiveGreenTime [p]= 0;
	}

	m_CurrentNodeNo =  m_pDoc->m_SelectedNodeNo ;
	m_CurrentNodeID = m_pDoc->m_NodeNoMap [m_CurrentNodeNo]->m_NodeID ;

	// Give better margin to editors
	CString str;
	for (int tp = 1; tp <= _max_timing_plans; tp++)
	{
		str.Format("%d", tp);
		m_TimingPlanComboBox.AddString(str);
		m_TimePlanVector.push_back(tp);
	}
	

	m_TimingPlanComboBox.SetCurSel(0);

	string timing_plan_name = "1";


	UpdateData(0);

	if(m_pDoc->m_NodeNoMap [m_CurrentNodeNo]->m_bSignalData == false)
	{

		return true;
	}



	m_PhasingGrid.SetDoubleBuffering(1);

	UpdatePhaseData();
	UpdateData(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CPage_Node_Phase::UpdatePhaseData()
{

	DisplayPhasingGrid();

}

void CPage_Node_Phase::UpdateList()
{

}

void CPage_Node_Phase::DisplayList()
{


}
void CPage_Node_Phase::OnPaint()
{
	CString str;
	m_TimingPlanComboBox.GetLBText(m_TimingPlanComboBox.GetCurSel(), str);

	std::string timing_plan_name = m_pDoc->CString2StdString(str);

	CPaintDC dc(this); // device context for painting
	CRect PlotRect;
	GetClientRect(PlotRect);
	m_PlotRect = PlotRect;

	m_PlotRect.top += 35;
	m_PlotRect.bottom -= 250;
	m_PlotRect.left += 50;
	m_PlotRect.right -= 50;

	int GreenTimeDiagramHeight = 25;
	DrawMovements(&dc,m_PlotRect,false);

}

void CPage_Node_Phase::DrawPhaseGreenTimeBand(CPaintDC* pDC,CRect PlotRect, int CycleLength, float PhaseStartTime,float PhaseEndTime, float Yellow, float AllRed )
{

	CBrush  GreenBrush(RGB(0,255,0)); 
	CBrush  YellowBrush(RGB(255,255,0)); 

	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(&GreenBrush);

	float width = 	PlotRect.Width ();

	float GreenEndTime = PhaseEndTime - AllRed - Yellow;
	CRect PlotRectNew = PlotRect;

	PlotRectNew.left = PlotRect.left + PhaseStartTime/CycleLength * width;
	PlotRectNew.right  =  PlotRect.left + GreenEndTime/CycleLength *width;


	pDC->Rectangle (PlotRectNew);

	CString str;

	str.Format("P%d [%.1f s]",m_SelectedPhaseNumber,PhaseEndTime - PhaseStartTime );

	pDC->TextOutA(PlotRectNew.left+5,PlotRectNew.top +5,str);

	// yellow block
	pDC->SelectObject(&YellowBrush);

	PlotRectNew.left = PlotRectNew.right; // next to previous green block 
	PlotRectNew.right  =  PlotRectNew.left + Yellow/CycleLength *width;


	pDC->Rectangle (PlotRectNew);

}


void CPage_Node_Phase::DrawMovements(CPaintDC* pDC,CRect PlotRect, bool bPhaseWindow =false)
{
	CString str;


	m_MovementBezierVector.clear();

	CPen NormalPen(PS_SOLID,2,RGB(0,0,0));
	CPen TimePen(PS_DOT,1,RGB(0,0,0));

	CPen DASHPen(PS_SOLID,1,RGB(255,178,102));

	CPen DataPen(PS_SOLID,0,RGB(0,0,0));
	CPen SelectedPen(PS_SOLID,3,RGB(255,0,0));
	CPen SelectedPhasePen(PS_SOLID,4,RGB(0,0,255));

	CBrush  WhiteBrush(RGB(255,255,255)); 

	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(&DataPen);
	pDC->SelectObject(&WhiteBrush);

	pDC->Rectangle (PlotRect);

	if(bPhaseWindow == true )
	{
		str.Format("Phase %d",m_SelectedPhaseNumber);
		pDC->TextOutA(PlotRect.left+10,PlotRect.top +5,str);

	}


	CBrush  BrushLinkBand(RGB(152,245,255)); 
	pDC->SelectObject(&BrushLinkBand);

	DTANode* pNode  = m_pDoc->m_NodeNoMap [m_CurrentNodeNo];

	int node_size = 10;
	int node_set_back = 50;

	int link_length = 100;
	int lane_width = 10;
	int text_length = link_length+ 20;

	str.Format("%d",m_CurrentNodeID);

	if(bPhaseWindow == false)
		pDC->TextOutA( PlotRect.CenterPoint().x-5, PlotRect.CenterPoint().y-5,str);

	for (unsigned int i=0;i< pNode->m_MovementVector .size();i++)
	{
		DTANodeMovement movement = pNode->m_MovementVector[i];

		if( m_pDoc->m_hide_non_specified_movement_on_freeway_and_ramp && movement.bNonspecifiedTurnDirectionOnFreewayAndRamps && i != m_SelectedMovementIndex)
			continue;


		DTALink* pInLink  = m_pDoc->m_LinkNoMap [movement.IncomingLinkNo];
		DTALink* pOutLink  = m_pDoc->m_LinkNoMap [movement.OutgoingLinkNo ];

		GDPoint p1, p2, p3;
		// 1: fetch all data
		p1  = m_pDoc->m_NodeIDMap[movement.in_link_from_node_id ]->pt;
		p2  = m_pDoc->m_NodeIDMap[movement.in_link_to_node_id ]->pt;
		p3  = m_pDoc->m_NodeIDMap[movement.out_link_to_node_id]->pt;

		double DeltaX = p2.x - p1.x ;
		double DeltaY = p2.y - p1.y ;
		double theta = atan2(DeltaY, DeltaX);

		GDPoint p1_new, p2_new, p3_new;
		GDPoint p1_text, p3_text;
		GDPoint pt_movement[3];


		// 2. set new origin
		p2_new.x = (-1)*node_set_back*cos(theta);  
		p2_new.y = (-1)*node_set_back*sin(theta);


		int link_mid_offset  = (pInLink->m_NumberOfLanes/2 +1)*lane_width;  // mid

		pt_movement[0].x = p2_new.x + link_mid_offset* cos(theta-PI/2.0f);
		pt_movement[0].y = p2_new.y + link_mid_offset* sin(theta-PI/2.0f);

		// 3 determine the control point for  PolyBezier
		float control_point_ratio = 0;
		if(movement.movement_turn == DTA_Through ) 
		{
			control_point_ratio = 0;
		}else if(movement.movement_turn == DTA_LeftTurn ) 
		{
			control_point_ratio = 1.2;
		}else
		{
			control_point_ratio = 0.5;
		}

		pt_movement[1].x = pt_movement[0].x + node_set_back*control_point_ratio*cos(theta);
		pt_movement[1].y = pt_movement[0].y + node_set_back*control_point_ratio*sin(theta);


		if(bPhaseWindow == true)
		{
			link_length = 1;
			text_length = node_set_back+10;
		}

		p1_new.x = (-1)*link_length*cos(theta);
		p1_new.y = (-1)*link_length*sin(theta);


		p1_text.x= (-1)*(text_length)*cos(theta);
		p1_text.y= (-1)*(text_length)*sin(theta);

		// 4: draw from node name

		str.Format("%d",m_pDoc->m_NodeIDMap [movement.in_link_from_node_id]->m_NodeID );


		if(p1_text.y < -50)
			p1_text.y +=10;

		CPoint pt_text = NPtoSP(p1_text);

		if(bPhaseWindow == false)
		{
			pDC->SetTextColor(RGB(0,0,255));
			pDC->TextOutA(pt_text.x-10,pt_text.y,str);
			pDC->SetTextColor(RGB(0,0,0));
		}

		if(bPhaseWindow == false)
			DrawLink(pDC,p1_new,p2_new,pInLink->m_NumberOfLanes,theta,lane_width);

		////////////////////////////////////////////
		//5: outgoing link
		DeltaX = p3.x - p2.x ;
		DeltaY = p3.y - p2.y ;
		theta = atan2(DeltaY, DeltaX);

		// set new origin
		p2_new.x = node_set_back*cos(theta);  
		p2_new.y = node_set_back*sin(theta);

		link_mid_offset  = (pOutLink->m_NumberOfLanes/2+1)*lane_width;
		pt_movement[2].x = p2_new.x + link_mid_offset* cos(theta-PI/2.0f);
		pt_movement[2].y = p2_new.y + link_mid_offset* sin(theta-PI/2.0f);


		p3_new.x = link_length*cos(theta);
		p3_new.y = link_length*sin(theta);

		p3_text.x= text_length*cos(theta);
		p3_text.y= text_length*sin(theta);

		if(bPhaseWindow == false)
			DrawLink(pDC,p2_new,p3_new,pOutLink->m_NumberOfLanes,theta,lane_width);

		DTALink * pRevLink = NULL; //reversed link
		unsigned long ReversedLinkKey = m_pDoc->GetLinkKey(pOutLink->m_ToNodeNo, pOutLink->m_FromNodeNo);

		int reversed_link_id = 0;
		if ( m_pDoc->m_NodeIDtoLinkMap.find ( ReversedLinkKey) == m_pDoc->m_NodeIDtoLinkMap.end())
		{
			str.Format("%d",m_pDoc->m_NodeIDMap [movement.out_link_to_node_id ]->m_NodeID );

			if(p3_text.y < -50)
				p3_text.y +=10;

			pt_text = NPtoSP(p3_text);

			if(bPhaseWindow == false )
				pDC->TextOutA(pt_text.x-10,pt_text.y,str);

		}



		// draw movement 

		CPoint Point_Movement[4];



		if(i == m_SelectedMovementIndex)
		{
			pDC->SelectObject(&SelectedPen);
		}
		else
		{
			//if(m_ListCtrl.GetItemText (i,4).Find("Prohibited") != -1) 			// select prohibited movement pen: 
			//	pDC->SelectObject(&g_PenProhibitedMovement);
			//else
			//	pDC->SelectObject(&NormalPen);
		}

		if(movement.movement_turn == DTA_LeftTurn )
		{
			float weight = 0.9;
			pt_movement[1].x = (1-weight)*(pt_movement[0].x + pt_movement[2].x)/2 +0*weight;
			pt_movement[1].y = (1-weight)*(pt_movement[0].y + pt_movement[2].y)/2 +0*weight;
		}

		Point_Movement[0]= NPtoSP(pt_movement[0]);
		Point_Movement[1]= NPtoSP(pt_movement[1]);
		Point_Movement[2]= NPtoSP(pt_movement[1]);
		Point_Movement[3]= NPtoSP(pt_movement[2]);

		Point_Movement[0]= NPtoSP(pt_movement[0]);
		Point_Movement[1]= NPtoSP(pt_movement[1]);
		Point_Movement[2]= NPtoSP(pt_movement[1]);
		Point_Movement[3]= NPtoSP(pt_movement[2]);

		MovementBezier element(Point_Movement[0], Point_Movement[1],Point_Movement[3]);

		m_MovementBezierVector.push_back (element);

		if(m_bHideRightTurnMovement &&( pNode->m_MovementVector[i].movement_turn == DTA_RightTurn ||  pNode->m_MovementVector[i].movement_turn == DTA_RightTurn2))
			continue;

		bool bMovementIncluded = false;

		if(m_SelectedPhaseNumber >=1)
		{
			bMovementIncluded = m_pDoc->IfMovementIncludedInPhase(m_CurrentNodeID ,m_SelectedPhaseNumber-1, movement.in_link_from_node_id,movement.out_link_to_node_id );
		}

		if(m_SelectedPhaseNumber <=0 ||  /* all phases*/
			m_SelectedPhaseNumber>=1 && ( bMovementIncluded == true ) )  // seleted phase
		{

		pDC->SelectObject(&NormalPen);

			if(m_SelectedPhaseNumber>=1 )
			{
				if(  bMovementIncluded == true)
					pDC->SelectObject(&NormalPen);
				else
					pDC->SelectObject(&DASHPen);

			}

			if(i == m_SelectedMovementIndex )
			{
				pDC->SelectObject(&SelectedPen);
			}

			//overwrite
			if(i == m_SelectedMovementIndex)
			{
				pDC->SelectObject(&SelectedPen);
			}

			pDC->PolyBezier(Point_Movement,4);

			bool bShowArrow = false;

			if(m_SelectedPhaseNumber>=1 &&  bMovementIncluded == true)
			{
				bShowArrow = true;

			}
			if(bShowArrow)
			{
				CPoint FromPoint = Point_Movement[2] ; 
				CPoint ToPoint = Point_Movement[3];


				CPoint arrow_pts[3];
				double slopy = atan2((double)(FromPoint.y - ToPoint.y), (double)(FromPoint.x - ToPoint.x));
				double cosy = cos(slopy);
				double siny = sin(slopy);   
				double display_length  = sqrt((double)(FromPoint.y - ToPoint.y)*(FromPoint.y - ToPoint.y)+(double)(FromPoint.x - ToPoint.x)*(FromPoint.x - ToPoint.x));
				double arrow_size = min(10,display_length/3.0);

				if(arrow_size>0.2)
				{

					arrow_pts[0] = ToPoint;
					arrow_pts[1].x = ToPoint.x + (int)(arrow_size * cosy - (arrow_size / 2.0 * siny) + 0.5);
					arrow_pts[1].y = ToPoint.y + (int)(arrow_size * siny + (arrow_size / 2.0 * cosy) + 0.5);
					arrow_pts[2].x = ToPoint.x + (int)(arrow_size * cosy + arrow_size / 2.0 * siny + 0.5);
					arrow_pts[2].y = ToPoint.y - (int)(arrow_size / 2.0 * cosy - arrow_size * siny + 0.5);

					pDC->Polygon(arrow_pts, 3);

				}
			}



		}
		//restore pen
		pDC->SelectObject(&DataPen);

	}
}

void CPage_Node_Phase::DrawLink(CPaintDC* pDC,GDPoint pt_from, GDPoint pt_to,int NumberOfLanes, double theta, int lane_width)
{
	CPoint DrawPoint[4];

	//then offset
	int link_offset = lane_width;

	pt_from.x += link_offset* cos(theta-PI/2.0f);
	pt_to.x += link_offset* cos(theta-PI/2.0f);

	pt_from.y += link_offset* sin(theta-PI/2.0f);
	pt_to.y += link_offset* sin(theta-PI/2.0f);

	DrawPoint[0] = NPtoSP(pt_from);
	DrawPoint[1] = NPtoSP(pt_to);

	link_offset = min(5,NumberOfLanes)*lane_width ;
	pt_from.x += link_offset* cos(theta-PI/2.0f);
	pt_to.x += link_offset* cos(theta-PI/2.0f);

	pt_from.y += link_offset* sin(theta-PI/2.0f);
	pt_to.y += link_offset* sin(theta-PI/2.0f);

	DrawPoint[2] = NPtoSP(pt_to);
	DrawPoint[3] = NPtoSP(pt_from);

	pDC->Polygon(DrawPoint, 4);

}
// CPage_Node_Phase message handlers

void CPage_Node_Phase::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	unsigned int i;

	DTANode* pNode  = m_pDoc->m_NodeNoMap [m_CurrentNodeNo];


	m_SelectedMovementIndex =  FindClickedMovement(point);

	CPropertyPage::OnLButtonDown(nFlags, point);
	Invalidate();

}


void CPage_Node_Phase::SaveData()
{

}

void CPage_Node_Phase::OnOK( )
{
	CPropertyPage::OnOK();
}

void CPage_Node_Phase::OnCancel( )
{
	CPropertyPage::OnCancel();
}

void CPage_Node_Phase::OnBnClickedButtonSave()
{


}



void CPage_Node_Phase::OnLbnSelchangeList1()
{
	Invalidate();
}
void CPage_Node_Phase::OnLbnDblclkList1()
{
	Invalidate();
}

void CPage_Node_Phase::DisplayPhasingGrid()
{

	m_PhasingGrid.SetRowCount(13);  // Phase Number, Green Time 
	m_PhasingGrid.SetColumnCount(_max_phase_number+1);  // Title, green time value

	m_PhasingGrid.SetFixedColumnCount(1);
	//	m_PhasingGrid.SetFixedRowCount(1);

	m_PhasingGrid.SetFixedColumnSelection(true);

	CString str;

	int p;
	int i = 0;  // Phase Number
	for(p = 1; p <= _max_phase_number; p++)
	{
		str.Format("Phase %d", p);
		SetItemTextInPhasingGrid(i,p,str,true);
		m_PhasingGrid.SetItemBkColour(i,p,RGB(240,240,240));
	}


	i=1;  // Green Time
	p = 0;

	SetItemTextInPhasingGrid(i++, 0, "min green");
	SetItemTextInPhasingGrid(i++, 0, "max green");
	SetItemTextInPhasingGrid(i++, 0, "yellow");

	SetItemTextInPhasingGrid(i++,0,"Movement 1");
	SetItemTextInPhasingGrid(i++, 0, "Movement 2");
	SetItemTextInPhasingGrid(i++, 0, "Movement 3");
	SetItemTextInPhasingGrid(i++, 0, "Movement 4");
	SetItemTextInPhasingGrid(i++, 0, "Movement 5");
	SetItemTextInPhasingGrid(i++, 0, "Movement 6");
	SetItemTextInPhasingGrid(i++, 0, "Movement 7");
	SetItemTextInPhasingGrid(i++, 0, "Movement 8");
	SetItemTextInPhasingGrid(i++, 0, "Movement 9");

	DTANode* pNode = m_pDoc->m_NodeNoMap[m_CurrentNodeNo];


	for(p = 0; p < min(pNode->m_node_phase_vector.size(), _max_phase_number); p++)
	{
		i = 1;  // Green Time

		str.Format("%d", pNode->m_node_phase_vector[p].min_green);
		SetItemTextInPhasingGrid(i, p+1, str);
		i++;

		str.Format("%d", pNode->m_node_phase_vector[p].max_green);
		SetItemTextInPhasingGrid(i, p + 1, str);
		i++;

		str.Format("%d", pNode->m_node_phase_vector[p].amber);
		SetItemTextInPhasingGrid(i, p+1, str);
		i++;

		for (int m = 0; m < min(8, pNode->m_node_phase_vector[p ].movement_vector.size()); m++)
		{
			SetItemTextInPhasingGrid(i, p+1, pNode->m_node_phase_vector[p].movement_vector[m]);
			i++;

		}

	}



}


void CPage_Node_Phase::UpdatePhasingDataInGrid()
{

}

void CPage_Node_Phase::OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	TRACE(_T("End Selection Change on row %d, col %d \n"),
		pItem->iRow, pItem->iColumn);

	if(pItem->iRow ==0) // column selecetd
	{
		m_SelectedPhaseNumber = pItem->iColumn;

		m_SelectedMovementIndex = -1;
		UpdatePhasingDataInGrid();
		Invalidate();
	}

}



void CPage_Node_Phase::OnContextMenu(CWnd* pWnd, CPoint point)
{

}
void CPage_Node_Phase::OnLButtonDblClk(UINT nFlags, CPoint point)
{


	CPropertyPage::OnLButtonDblClk(nFlags, point);
}




void CPage_Node_Phase::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
}

void CPage_Node_Phase::OnCbnSelchangeCombo2()
{
	// TODO: Add your control notification handler code here
}





//TLiteDoc.cpp : implementation of the CTLiteDoc class
//

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
#include "Geometry.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "CSVParser.h"

#include "TLite.h"
#include "Network.h"
#include "TLite.h"
#include "TLiteDoc.h"
#include "TLiteView.h"
#include "DlgMOE.h"
#include "DlgFileLoading.h"
#include "MainFrm.h"
#include "Shellapi.h"
#include "DlgAssignmentSettings.h"
#include "CGridListCtrlEx\\CGridListCtrlEx.h"
#include "CDlg_UserInput.h"
#include "DlgLinkList.h"
#include "DlgTrajectoryList.h"
#include "DlgPathList.h"


#include "Shellapi.h"

#include "DlgNetworkAlignment.h"
#include "Dlg_VehEmissions.h"
#include "DlgMOETabView.h"



#include "Dlg_VehicleClassification.h"
#include "Dlg_Find_Vehicle.h"

#include "Dlg_Legend.h"

#include "Page_Node_Movement.h"

#include "AssignmentSimulationSettingDlg.h"
#include "NetworkDataDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define _max_number_of_movements  32

extern std::vector<float> g_link_speed_vector;

extern void g_SetCursor(_cursor_type cursor_type);


extern int g_ParserIntSequence(std::string str, std::vector<int> &vect); 
extern int g_ParserFloatSequence(std::string str, std::vector<float> &vect); 
extern int g_ParserStringSequence(std::string str, std::vector<string> &vect);

CDlgMOE *g_LinkMOEDlg = NULL;
CDlg_Legend* g_pLegendDlg = NULL;
CDlgLinkList* g_pLinkListDlg = NULL;
CDlgTrajectoryList* g_pTrajectoryListDlg = NULL;
CDlg_VehPathAnalysis* g_pAgentPathDlg = NULL;
CDlgPathList* g_pPathListDlg = NULL;

bool g_bShowLinkList = false;
bool g_bShowAgentPathDialog = false;



extern float g_Simulation_Time_Stamp;
bool g_LinkMOEDlgShowFlag = false;

std::list<s_link_selection>	g_LinkDisplayList;
std::list<CTLiteDoc*>	g_DocumentList;

std::vector<CDlg_AgentClassification*>	g_SummaryDialogVector;

bool g_bValidDocumentChanged = false;
std::list<CTLiteView*>	g_ViewList;

void g_ClearLinkSelectionList()
{
	//std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
	//while (iDoc != g_DocumentList.end())
	//{

	//	if((*iDoc)!=NULL)
	//	{
	//	for (std::list<DTALink*>::iterator iLink = (*iDoc)->m_LinkSet.begin(); iLink != (*iDoc)->m_LinkSet.end(); iLink++)
	//	{
	//		if((*iLink)!=NULL)
	//		{
	//		(*iLink)->m_DisplayLinkID = -1;
	//		}
	//	}
	//	}
	//	iDoc++;
	//}

	for (std::list<s_link_selection>::iterator iLinkDisplay = g_LinkDisplayList.begin(); iLinkDisplay != g_LinkDisplayList.end();iLinkDisplay++ )
	{

		if((*iLinkDisplay).pLink !=NULL)
			(*iLinkDisplay).pLink -> m_DisplayLinkID =  -1;
	}


	g_LinkDisplayList.clear ();
}


void g_ClearTrajectorySelectionList()
{
	//std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
	//while (iDoc != g_DocumentList.end())
	//{

	//	if((*iDoc)!=NULL)
	//	{
	//	for (std::list<DTALink*>::iterator iLink = (*iDoc)->m_LinkSet.begin(); iLink != (*iDoc)->m_LinkSet.end(); iLink++)
	//	{
	//		if((*iLink)!=NULL)
	//		{
	//		(*iLink)->m_DisplayLinkID = -1;
	//		}
	//	}
	//	}
	//	iDoc++;
	//}

	for (std::list<s_link_selection>::iterator iLinkDisplay = g_LinkDisplayList.begin(); iLinkDisplay != g_LinkDisplayList.end(); iLinkDisplay++)
	{

		if ((*iLinkDisplay).pLink != NULL)
			(*iLinkDisplay).pLink->m_DisplayLinkID = -1;
	}


	g_LinkDisplayList.clear();
}

void g_AddLinkIntoSelectionList(DTALink* pLink, int link_no, int document_no, bool b_SelectOtherDocuments, double x, double y)
{
	s_link_selection element;
	element.link_no = link_no;
	element.document_no = document_no;
	element.pLink = pLink;

	g_LinkDisplayList.push_back(element);

	std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
	while (iDoc != g_DocumentList.end())
	{
		if((*iDoc)->m_DocumentNo != document_no)  // not the current document
		{
			GDPoint point;
			point.x = x;
			point.y = y;
			double matching_distance = 0;
			int sel_link_no = (*iDoc)->SelectLink(point, matching_distance);
			if(sel_link_no>=0)  //select link
			{
				element.link_no = sel_link_no;
				element.document_no = (*iDoc)->m_DocumentNo ;
				g_LinkDisplayList.push_back(element);
			}
			//update anyway
			(*iDoc)->UpdateAllViews (0);

		}

		iDoc++;
	}

}
// CTLiteDoc
string g_time_coding(float time_stamp)
{
	int hour = time_stamp / 60;
	int minute = time_stamp - hour * 60;

	int second = (time_stamp - hour * 60 - minute) * 60;

	ostringstream strm;
	strm.fill('0');
	strm << setw(2) << hour << setw(2) << minute << ":" << setw(2) << second;

	return strm.str();
} // transform hhmm to minutes 


vector<float> g_time_parser(string str)
{
	vector<float> output_global_second;

	int string_lenghth = str.length();

	const char *string_line = str.data(); //string to char*

	int char_length = strlen(string_line);

	char ch, buf_ddhhmm[32] = { 0 }, buf_SS[32] = { 0 }, buf_sss[32] = { 0 };
	char dd1, dd2, hh1, hh2, mm1, mm2, SS1, SS2, sss1, sss2, sss3;
	float ddf1, ddf2, hhf1, hhf2, mmf1, mmf2, SSf1, SSf2, sssf1, sssf2, sssf3;
	float global_minute = 0;
	float dd = 0, hh = 0, mm = 0, SS = 0, sss = 0;
	int i = 0;
	int buffer_i = 0, buffer_k = 0, buffer_j = 0;
	int num_of_colons = 0;

	//DDHHMM:SS:sss or HHMM:SS:sss

	while (i < char_length)
	{
		ch = string_line[i++];

		if (num_of_colons == 0 && ch != '_' && ch != ':') //input to buf_ddhhmm until we meet the colon
		{
			buf_ddhhmm[buffer_i++] = ch;
		}
		else if (num_of_colons == 1 && ch != ':') //start the Second "SS"
		{
			buf_SS[buffer_k++] = ch;
		}
		else if (num_of_colons == 2 && ch != ':') //start the Millisecond "sss"
		{
			buf_sss[buffer_j++] = ch;
		}

		if (ch == '_' || ch == ';' || i == char_length) //start a new time string
		{
			if (buffer_i == 4) //"HHMM"
			{
				//HHMM, 0123
				hh1 = buf_ddhhmm[0]; //read each first
				hh2 = buf_ddhhmm[1];
				mm1 = buf_ddhhmm[2];
				mm2 = buf_ddhhmm[3];

				hhf1 = ((float)hh1 - 48); //convert a char to a float
				hhf2 = ((float)hh2 - 48);
				mmf1 = ((float)mm1 - 48);
				mmf2 = ((float)mm2 - 48);

				dd = 0;
				hh = hhf1 * 10 * 60 + hhf2 * 60;
				mm = mmf1 * 10 + mmf2;
			}
			else if (buffer_i == 6) //"DDHHMM"
			{
				//DDHHMM, 012345
				dd1 = buf_ddhhmm[0]; //read each first
				dd2 = buf_ddhhmm[1];
				hh1 = buf_ddhhmm[2];
				hh2 = buf_ddhhmm[3];
				mm1 = buf_ddhhmm[4];
				mm2 = buf_ddhhmm[5];

				ddf1 = ((float)dd1 - 48); //convert a char to a float
				ddf2 = ((float)dd2 - 48);
				hhf1 = ((float)hh1 - 48);
				hhf2 = ((float)hh2 - 48);
				mmf1 = ((float)mm1 - 48);
				mmf2 = ((float)mm2 - 48);

				dd = ddf1 * 10 * 24 * 60 + ddf2 * 24 * 60;
				hh = hhf1 * 10 * 60 + hhf2 * 60;
				mm = mmf1 * 10 + mmf2;
			}

			if (num_of_colons == 1 || num_of_colons == 2)
			{
				//SS, 01
				SS1 = buf_SS[0]; //read each first
				SS2 = buf_SS[1];

				SSf1 = ((float)SS1 - 48); //convert a char to a float
				SSf2 = ((float)SS2 - 48);

				SS = (SSf1 * 10 + SSf2) / 60;
			}

			if (num_of_colons == 2)
			{
				//sss, 012
				sss1 = buf_sss[0]; //read each first
				sss2 = buf_sss[1];
				sss3 = buf_sss[2];

				sssf1 = ((float)sss1 - 48); //convert a char to a float
				sssf2 = ((float)sss2 - 48);
				sssf3 = ((float)sss3 - 48);

				sss = (sssf1 * 100 + sssf2 * 10 + sssf3) / 1000;
			}

			float global_minute = dd + hh + mm + SS + sss;
			float global_second = (dd + hh + mm + SS + sss)*60.0;

			output_global_second.push_back(global_second);

			//initialize the parameters
			buffer_i = 0;
			buffer_k = 0;
			buffer_j = 0;
			num_of_colons = 0;
		}

		if (ch == ':')
		{
			num_of_colons += 1;
		}
	}

	return output_global_second;
}

IMPLEMENT_DYNCREATE(CTLiteDoc, CDocument)

BEGIN_MESSAGE_MAP(CTLiteDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CTLiteDoc::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE_PROJECT, &CTLiteDoc::OnFileSaveProject)
	ON_COMMAND(ID_FILE_SAVE_PROJECT_AS, &CTLiteDoc::OnFileSaveProjectAs)
	ON_COMMAND(ID_FILE_DATALOADINGSTATUS, &CTLiteDoc::OnFileDataloadingstatus)
	ON_COMMAND(ID_MOE_VOLUME, &CTLiteDoc::OnMoeVolume)
	ON_COMMAND(ID_MOE_SPEED, &CTLiteDoc::OnMoeSpeed)
	ON_UPDATE_COMMAND_UI(ID_MOE_VOLUME, &CTLiteDoc::OnUpdateMoeVolume)
	ON_UPDATE_COMMAND_UI(ID_MOE_SPEED, &CTLiteDoc::OnUpdateMoeSpeed)
	ON_COMMAND(ID_MOE_NONE, &CTLiteDoc::OnMoeNone)
	ON_UPDATE_COMMAND_UI(ID_MOE_NONE, &CTLiteDoc::OnUpdateMoeNone)
	ON_COMMAND(ID_MOE_VC_Ratio, &CTLiteDoc::OnMoeVcRatio)
	ON_UPDATE_COMMAND_UI(ID_MOE_VC_Ratio, &CTLiteDoc::OnUpdateMoeVcRatio)
	ON_COMMAND(ID_MOE_TRAVELTIME, &CTLiteDoc::OnMoeTraveltime)
	ON_UPDATE_COMMAND_UI(ID_MOE_TRAVELTIME, &CTLiteDoc::OnUpdateMoeTraveltime)
	ON_COMMAND(ID_MOE_CAPACITY, &CTLiteDoc::OnMoeCapacity)
	ON_UPDATE_COMMAND_UI(ID_MOE_CAPACITY, &CTLiteDoc::OnUpdateMoeCapacity)
	ON_COMMAND(ID_MOE_SPEEDLIMIT, &CTLiteDoc::OnMoeSpeedlimit)
	ON_UPDATE_COMMAND_UI(ID_MOE_SPEEDLIMIT, &CTLiteDoc::OnUpdateMoeSpeedlimit)
	ON_COMMAND(ID_MOE_FREEFLOWTRAVLETIME, &CTLiteDoc::OnMoeFreeflowtravletime)
	ON_UPDATE_COMMAND_UI(ID_MOE_FREEFLOWTRAVLETIME, &CTLiteDoc::OnUpdateMoeFreeflowtravletime)
	ON_COMMAND(ID_EDIT_DELETESELECTEDLINK, &CTLiteDoc::OnEditDeleteselectedlink)
	ON_COMMAND(ID_EDIT_SETDEFAULTLINKPROPERTIESFORNEWLINKS, &CTLiteDoc::OnEditSetdefaultlinkpropertiesfornewlinks)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETDEFAULTLINKPROPERTIESFORNEWLINKS, &CTLiteDoc::OnUpdateEditSetdefaultlinkpropertiesfornewlinks)
	ON_COMMAND(ID_TOOLS_PROJECTFOLDER, &CTLiteDoc::OnToolsProjectfolder)
	ON_COMMAND(ID_TOOLS_OPENNEXTAPROGRAMFOLDER, &CTLiteDoc::OnToolsOpennextaprogramfolder)
	ON_COMMAND(ID_ODTABLE_IMPORT_OD_TRIP_FILE, &CTLiteDoc::OnOdtableImportOdTripFile)
	ON_COMMAND(ID_SEARCH_LINKLIST, &CTLiteDoc::OnSearchLinklist)
	ON_COMMAND(ID_MOE_Agent, &CTLiteDoc::OnMoeAgent)
	ON_UPDATE_COMMAND_UI(ID_MOE_Agent, &CTLiteDoc::OnUpdateMoeAgent)
	ON_COMMAND(ID_TOOLS_VIEWSIMULATIONSUMMARY, &CTLiteDoc::OnToolsViewsimulationsummary)
	ON_COMMAND(ID_TOOLS_VIEWASSIGNMENTSUMMARYLOG, &CTLiteDoc::OnToolsViewassignmentsummarylog)
	ON_COMMAND(ID_HELP_VISITDEVELOPMENTWEBSITE, &CTLiteDoc::OnHelpVisitdevelopmentwebsite)

	ON_COMMAND(ID_MOE_AgentPATHANALAYSIS, &CTLiteDoc::OnMoeAgentpathanalaysis)
	ON_COMMAND(ID_IMPORT_NGSIM_FILE, &CTLiteDoc::OnImportNgsimFile)
	ON_COMMAND(ID_LINK_AgentSTATISTICSANALAYSIS, &CTLiteDoc::OnLinkAgentstatisticsanalaysis)
	ON_COMMAND(ID_SUBAREA_DELETESUBAREA, &CTLiteDoc::OnSubareaDeletesubarea)
	ON_COMMAND(ID_LINK_LINKBAR, &CTLiteDoc::OnLinkLinkbar)
	ON_COMMAND(ID_LINK_INCREASEOFFSETFORTWO, &CTLiteDoc::OnLinkIncreaseoffsetfortwo)
	ON_COMMAND(ID_LINK_DECREASEOFFSETFORTWO, &CTLiteDoc::OnLinkDecreaseoffsetfortwo)
	ON_COMMAND(ID_VIEW_SHOWHIDE_LEGEND, &CTLiteDoc::OnViewShowhideLegend)
	ON_COMMAND(ID_MOE_VIEWLINKMOESUMMARYFILE, &CTLiteDoc::OnMoeViewlinkmoesummaryfile)
	ON_COMMAND(ID_VIEW_CALIBRATIONVIEW, &CTLiteDoc::OnViewCalibrationview)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CALIBRATIONVIEW, &CTLiteDoc::OnUpdateViewCalibrationview)
	ON_COMMAND(ID_MOE_VIEWODDEMANDESTIMATIONSUMMARYPLOT, &CTLiteDoc::OnMoeViewoddemandestimationsummaryplot)
	ON_COMMAND(ID_DELETE_SELECTED_LINK, &CTLiteDoc::OnDeleteSelectedLink)
	ON_COMMAND(ID_EXPORT_GENERATEZONE, &CTLiteDoc::OnExportGenerateTravelTimeMatrix)
	ON_COMMAND(ID_EXPORT_GENERATESHAPEFILES, &CTLiteDoc::OnExportGenerateshapefiles)
	ON_COMMAND(ID_MOETYPE1_QUEUELENGTH, &CTLiteDoc::OnLinkmoedisplayQueueLengthRatio)
	ON_UPDATE_COMMAND_UI(ID_MOETYPE1_QUEUELENGTH, &CTLiteDoc::OnUpdateLinkmoedisplayQueueLengthRatio)


	ON_COMMAND(ID_MOE_PATHLIST, &CTLiteDoc::OnMoePathlist)
	ON_COMMAND(ID_VIEW_SHOWMOE, &CTLiteDoc::OnViewShowmoe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWMOE, &CTLiteDoc::OnUpdateViewShowmoe)
	ON_COMMAND(ID_FILE_UPLOADLINKDATATOGOOGLEFUSIONTABLE, &CTLiteDoc::OnFileUploadlinkdatatogooglefusiontable)
	ON_COMMAND(ID_3_VIEWDATAINEXCEL, &CTLiteDoc::On3Viewdatainexcel)
	ON_COMMAND(ID_5_VIEWDATAINEXCEL, &CTLiteDoc::On5Viewdatainexcel)
	ON_COMMAND(ID_MOE_VIEWNETWORKTIMEDEPENDENTMOE, &CTLiteDoc::OnMoeViewnetworktimedependentmoe)
	ON_COMMAND(ID_2_VIEWDATAINEXCEL33398, &CTLiteDoc::On2Viewdatainexcel33398)
	ON_COMMAND(ID_2_VIEWNETWORKDATA, &CTLiteDoc::On2Viewnetworkdata)
	ON_COMMAND(ID_3_VIEWODDATAINEXCEL, &CTLiteDoc::On3Viewoddatainexcel)
	ON_COMMAND(ID_MOE_OPENALLMOETABLES, &CTLiteDoc::OnMoeOpenallmoetables)
	ON_BN_CLICKED(IDC_BUTTON_Database, &CTLiteDoc::OnBnClickedButtonDatabase)
	ON_COMMAND(ID_TOOLS_UNITTESTING, &CTLiteDoc::OnToolsUnittesting)
	ON_COMMAND(ID_NODE_INCREASENODETEXTSIZE, &CTLiteDoc::OnNodeIncreasenodetextsize)
	ON_COMMAND(ID_NODE_DECREASENODETEXTSIZE, &CTLiteDoc::OnNodeDecreasenodetextsize)
	ON_COMMAND(ID_IMPORT_SYNCHROUTDFCSVFILES, &CTLiteDoc::OnImportSynchroutdfcsvfiles)
	ON_COMMAND(ID_PROJECT_EDITMOESETTINGS, &CTLiteDoc::OnProjectEditmoesettings)
	ON_COMMAND(ID_PROJECT_12, &CTLiteDoc::OnProject12)
	ON_COMMAND(ID_VIEW_MOVEMENT_MOE, &CTLiteDoc::OnViewMovementMoe)
	ON_COMMAND(ID_PROJECT_TIME_DEPENDENT_LINK_MOE, &CTLiteDoc::OnProjectTimeDependentLinkMoe)
	ON_COMMAND(ID_LINKATTRIBUTEDISPLAY_LINKNAME, &CTLiteDoc::OnLinkattributedisplayLinkname)
	ON_UPDATE_COMMAND_UI(ID_LINKATTRIBUTEDISPLAY_LINKNAME, &CTLiteDoc::OnUpdateLinkattributedisplayLinkname)
	ON_COMMAND(ID_PROJECT_1_NETWORK, &CTLiteDoc::OnProjectNetworkData)
	ON_COMMAND(ID_MOE_MOE, &CTLiteDoc::OnMoeTableDialog)
	ON_COMMAND(ID_SENSORTOOLS_CONVERTTOHOURCOUNT, &CTLiteDoc::OnSensortoolsConverttoHourlyVolume)
	ON_COMMAND(ID_IMPORT_INRIXSHAPEFILEANDSPEEDDATA, &CTLiteDoc::OnImportInrixshapefileandspeeddata)
	ON_COMMAND(ID_SUBAREA_CREATEZONEFROMSUBAREA, &CTLiteDoc::OnSubareaCreatezonefromsubarea)
	ON_COMMAND(ID_NODE_VIEWNODEDATA, &CTLiteDoc::OnNodeViewnodedata)
	ON_COMMAND(ID_LINK_VIEWLINKDATA, &CTLiteDoc::OnLinkViewlinkdata)
	ON_COMMAND(ID_MOVEMENT_VIEWMOVEMENTDATATABLE, &CTLiteDoc::OnMovementViewmovementdatatable)
	ON_COMMAND(ID_ODMATRIX_ODDEMANDMATRIX, &CTLiteDoc::OnOdmatrixOddemandmatrix)
	ON_COMMAND(ID_SHOW_MOE_PATHLIST, &CTLiteDoc::OnShowMoePathlist)
	ON_COMMAND(ID_DETECTOR_EXPORTLINKFLOWPROPORTIONMATRIXTOCSVFILE, &CTLiteDoc::OnDetectorExportlinkflowproportionmatrixtocsvfile)
	ON_COMMAND(ID_MOVEMENT_HIDENON, &CTLiteDoc::OnMovementHidenon)
	ON_UPDATE_COMMAND_UI(ID_MOVEMENT_HIDENON, &CTLiteDoc::OnUpdateMovementHidenon)
	ON_COMMAND(ID_CHANGELINKTYPECOLOR_FREEWAY, &CTLiteDoc::OnChangelinktypecolorFreeway)
	ON_COMMAND(ID_CHANGELINKTYPECOLOR_RAMP, &CTLiteDoc::OnChangelinktypecolorRamp)
	ON_COMMAND(ID_CHANGELINKTYPECOLOR_ARTERIAL, &CTLiteDoc::OnChangelinktypecolorArterial)
	ON_COMMAND(ID_CHANGELINKTYPECOLOR_CONNECTOR, &CTLiteDoc::OnChangelinktypecolorConnector)
	ON_COMMAND(ID_CHANGELINKTYPECOLOR_TRANSIT, &CTLiteDoc::OnChangelinktypecolorTransit)
	ON_COMMAND(ID_CHANGELINKTYPECOLOR_WALKINGMODE, &CTLiteDoc::OnChangelinktypecolorWalkingmode)
	ON_COMMAND(ID_CHANGELINKTYPECOLOR_RESETTODEFAULTCOLORSCHEMA, &CTLiteDoc::OnChangelinktypecolorResettodefaultcolorschema)
	ON_COMMAND(ID_NODE_CHANGENODECOLOR, &CTLiteDoc::OnNodeChangenodecolor)
	ON_COMMAND(ID_NODE_CHANGENODEBACKGROUNDCOLOR, &CTLiteDoc::OnNodeChangenodebackgroundcolor)
	ON_COMMAND(ID_ZONE_CHANGEZONECOLOR, &CTLiteDoc::OnZoneChangezonecolor)
	ON_COMMAND(ID_EDIT_UNDO33707, &CTLiteDoc::OnEditUndo33707)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO33707, &CTLiteDoc::OnUpdateEditUndo33707)
	ON_COMMAND(ID_EDIT_REDO33709, &CTLiteDoc::OnEditRedo33709)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO33709, &CTLiteDoc::OnUpdateEditRedo33709)
	ON_COMMAND(ID_MOVEMENT_SETUPNUMBEROFLEFTTURNLANESFORSIGNALIZEDNODES, &CTLiteDoc::OnMovementSetupnumberofleftturnlanesforsignalizednodes)
	ON_COMMAND(ID_MOVEMENT_OPTIMIZEPHASINGANDTIMINGDATAFORALLTRAFFICSIGNALSTHROUGHQEM, &CTLiteDoc::OnMovementOptimizephasingandtimingdataforalltrafficsignalsthroughqem)
	ON_COMMAND(ID_LINKMOEDISPLAY_BOTTLENECK, &CTLiteDoc::OnLinkmoedisplayBottleneck)
	ON_UPDATE_COMMAND_UI(ID_LINKMOEDISPLAY_BOTTLENECK, &CTLiteDoc::OnUpdateLinkmoedisplayBottleneck)
	ON_COMMAND(ID_GISPLANNINGDATASET_CONFIGUREIMPORTINGSETTINGFILE, &CTLiteDoc::OnGisplanningdatasetConfigureimportingsettingfile)
	ON_COMMAND(ID_SUBAREA_EXPORTTOTALNUMBEROFAgentSINSUBAREA, &CTLiteDoc::OnSubareaExporttotalnumberofAgentsinsubarea)
	ON_COMMAND(ID_TOOLS_CONFIGURATION, &CTLiteDoc::OnToolsConfiguration)
	ON_COMMAND(ID_PATH_CLEARALLPATHDISPLAY, &CTLiteDoc::OnPathClearallpathdisplay)
		ON_COMMAND(ID_TOOLS_IMPORTSYNCHRONCOMBINEDFILE, &CTLiteDoc::OnToolsImportsynchroncombinedfile)
		ON_COMMAND(ID_ODMATRIX_RAPID, &CTLiteDoc::OnOdmatrixRapid)
		ON_COMMAND(ID_ODMATRIX_RAPID_Decrease, &CTLiteDoc::OnOdmatrixRapidDecrease)
		ON_COMMAND(ID_BUTTON_ABM, &CTLiteDoc::OnButtonAbm)
		ON_COMMAND(ID_HELP_VISITDEVELOPMENTWEBSITE_DTALite, &CTLiteDoc::OnHelpVisitdevelopmentwebsiteDtalite)
		ON_COMMAND(ID_TOOLS_RUNTRAFFICASSIGNMENT33023, &CTLiteDoc::OnToolsRunSimulation)
		ON_COMMAND(ID_TOOLS_SIMULATIONSETTINGS, &CTLiteDoc::OnToolsSimulationsettings)
		ON_COMMAND(ID_TOOLS_IMPORTGTFSDATA, &CTLiteDoc::OnToolsImportgtfsdata)
		ON_COMMAND(ID_AGENT_CHANGEAGENTCOLOR, &CTLiteDoc::OnAgentChangeagentcolor1)
		ON_COMMAND(ID_AGENT_CHANGEAGENTCOLOR2, &CTLiteDoc::OnAgentChangeagentcolor2)
		ON_COMMAND(ID_TOOLS_GENERATEZONEGRID, &CTLiteDoc::OnToolsGeneratezonegrid)
		ON_COMMAND(ID_BACKGROUNDIMAGE_RELOADBACKGROUNDIMAGE, &CTLiteDoc::OnBackgroundimageReloadbackgroundimage)
		END_MESSAGE_MAP()


// CTLiteDoc construction/destruction

CTLiteDoc::CTLiteDoc()
{
	m_DemandAlpha = 1.0;
	bSynchroImportReadyToSaveFlag = false;

	m_calibration_data_start_time_in_min = 0;
	m_calibration_data_end_time_in_min = 1440;

	m_ImageMoveSize = 0.0001;
	m_SensorMapX = 0;
	m_SensorMapY = 0;
	
	m_SensorMapXResolution = 1;
	m_SensorMapYResolution = 1;
	m_SensorMapMoveSize = 0.0001;


	m_CurrentDisplayTimingPlanName = "0";



	m_MovementTextBoxSizeInDistance = 50;
	m_bShowSignalNodeMovementOnly = true;

	m_hide_non_specified_movement_on_freeway_and_ramp = true;

	m_sensor_data_aggregation_type = 0;  // no processing, 1:  hourly data: 2: hourly and lane-based data 

	m_TurnDirectionStringMap["NBL2"] =  DTA_NBL2;
	m_TurnDirectionStringMap["NBL"] =  DTA_NBL;
	m_TurnDirectionStringMap["NBT"] =  DTA_NBT;
	m_TurnDirectionStringMap["NBR"] =  DTA_NBR;
	m_TurnDirectionStringMap["NBR2"] =  DTA_NBR2;

	m_TurnDirectionStringMap["SBL2"] =  DTA_SBL2;
	m_TurnDirectionStringMap["SBL"] =  DTA_SBL;
	m_TurnDirectionStringMap["SBT"] =  DTA_SBT;
	m_TurnDirectionStringMap["SBR"] =  DTA_SBR;
	m_TurnDirectionStringMap["SBR2"] =  DTA_SBR2;

	m_TurnDirectionStringMap["EBL2"] =  DTA_EBL2;
	m_TurnDirectionStringMap["EBL"] =  DTA_EBL;
	m_TurnDirectionStringMap["EBT"] =  DTA_EBT;
	m_TurnDirectionStringMap["EBR"] =  DTA_EBR;
	m_TurnDirectionStringMap["EBR2"] =  DTA_EBR2;

	m_TurnDirectionStringMap["WBL2"] =  DTA_WBL2;
	m_TurnDirectionStringMap["WBL"] =  DTA_WBL;
	m_TurnDirectionStringMap["WBT"] =  DTA_WBT;
	m_TurnDirectionStringMap["WBR"] =  DTA_WBR;
	m_TurnDirectionStringMap["WBR2"] =  DTA_WBR2;

	m_PeakHourFactor = 1.0;
	m_bIdentifyBottleneckAndOnOffRamps = false;
	m_ScreenWidth_InMile = 10;

	m_bUseMileVsKMFlag = true;
	m_bRightHandTrafficFlag = 1;
	m_ImageWidthInMile = 1;
	m_PointA_x = m_PointA_y = m_PointB_x = m_PointB_y = 0;
	m_PointA_long = m_PointA_lat = m_PointB_long = m_PointB_lat= 0;


	m_bSummaryDialog = false;
	m_StartNodeIDForNewNodes = 1;
	m_demand_multiplier = 1;

	m_number_of_iterations = 20;
	m_number_of_reporting_days = 1;
	m_traffic_flow_model = 1;
	m_signal_reresentation_model = 0;
	m_traffic_analysis_method = 0;

	m_ActivityLocationCount = 0;
	m_SearchMode= efind_node;
	m_LongLatFlag = false; // default, we do not know if the coordinate system is long or lat
	g_bValidDocumentChanged = true;

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();

	m_DefaultDataFolder.Format ("%s\\default_data_folder\\",pMainFrame->m_CurrentDirectory);

	m_ZoneNoSize  = 0;
	m_bRunCrashPredictionModel = false;
	m_ZoomToSelectedObject = true;
	m_max_walking_distance = 0.25;
	m_max_accessible_transit_time_in_min = 15;

	m_AgentTypeSize = 1; // just one demand trip type:
	m_ODMOEMatrix = NULL;

	m_bMovementAvailableFlag = false;

	m_ImportNetworkAlready = false;
	m_DemandLoadingStartTimeInMin = 420;
	m_DemandLoadingEndTimeInMin = 480;

	m_bSaveProjectFromSubareaCut = false;
	m_bSaveProjectFromImporting = false;
	m_bExport_Link_MOE_in_input_link_CSF_File = false;
	m_bBezierCurveFlag = true;

	m_CriticalOriginZone = -1;
	m_CriticalDestinationZone = -1;

	m_ControlType_UnknownControl = 0;
	m_ControlType_NoControl = 1;
	m_ControlType_YieldSign = 2;
	m_ControlType_2wayStopSign = 3;
	m_ControlType_4wayStopSign = 4;
	m_ControlType_PretimedSignal = 5;
	m_ControlType_ActuatedSignal = 6;
	m_ControlType_Roundabout = 7;
	m_ControlType_ExternalNode = 100;


	m_LinkTypeMap[1].link_type = 1;
	m_LinkTypeMap[1].link_type_name = "Freeway";
	m_LinkTypeMap[1].type_code = "f";

	m_LinkTypeMap[2].link_type = 2;
	m_LinkTypeMap[2].link_type_name = "Highway";
	m_LinkTypeMap[2].type_code = "h";

	m_LinkTypeMap[3].link_type = 3;
	m_LinkTypeMap[3].link_type_name = "Principal arterial";
	m_LinkTypeMap[3].type_code = "a";

	m_LinkTypeMap[4].link_type = 4;
	m_LinkTypeMap[4].link_type_name = "Major arterial";
	m_LinkTypeMap[4].type_code = "a";

	m_LinkTypeMap[5].link_type = 5;
	m_LinkTypeMap[5].link_type_name = "Minor arterial";
	m_LinkTypeMap[5].type_code = "a";

	m_LinkTypeMap[6].link_type = 6;
	m_LinkTypeMap[6].link_type_name = "Collector";
	m_LinkTypeMap[6].type_code = "a";

	m_LinkTypeMap[7].link_type = 7;
	m_LinkTypeMap[7].link_type_name = "Local";
	m_LinkTypeMap[7].type_code = "a";

	m_LinkTypeMap[8].link_type = 8;
	m_LinkTypeMap[8].link_type_name = "Frontage road";
	m_LinkTypeMap[8].type_code = "a";

	m_LinkTypeMap[9].link_type = 9;
	m_LinkTypeMap[9].link_type_name = "Ramp";
	m_LinkTypeMap[9].type_code = "r";

	m_LinkTypeMap[10].link_type = 10;
	m_LinkTypeMap[10].link_type_name = "Zonal connector";
	m_LinkTypeMap[10].type_code = "c";

	m_LinkTypeMap[100].link_type = 100;
	m_LinkTypeMap[100].link_type_name = "Transit link";
	m_LinkTypeMap[100].type_code = "t";

	m_LinkTypeMap[200].link_type = 200;
	m_LinkTypeMap[200].link_type_name = "Walking link";
	m_LinkTypeMap[200].type_code = "w";

	m_LinkTypeFreeway = 1;
	m_LinkTypeArterial = 3;
	m_LinkTypeHighway = 2;

	m_OriginOnBottomFlag = 1;

	m_DocumentNo = g_DocumentList.size();
	g_DocumentList.push_back (this);

	m_RandomRoutingCoefficient = 0.0f;
	m_bGPSDataSet = false;
	m_bEmissionDataAvailable = false;
	m_AdjLinkSize = 20;  // initial value
	m_ColorDirection = -1;
	m_colorLOS[0] = RGB(190,190,190);
	m_colorLOS[1] = RGB(0,255,0);
	m_colorLOS[2] = RGB(255,250,117);
	m_colorLOS[3] = RGB(255,250,0);
	m_colorLOS[4] = RGB(255,216,0);
	m_colorLOS[5] = RGB(255,153,0);
	m_colorLOS[6] = RGB(255,0,0);


	m_ColorDirectionVector[DTA_South] = RGB(255,69,0 );  // orange red
	m_ColorDirectionVector[DTA_North] = RGB(255,255,255); // white
	m_ColorDirectionVector[DTA_East] = RGB(192,192,192 ); // Silver
	m_ColorDirectionVector[DTA_West] = RGB(169,169,169); // gray

	m_ColorDirectionVector[DTA_NorthEast] = RGB(255,69,0);  //orange red
	m_ColorDirectionVector[DTA_NorthWest] = RGB(255,69,0); //orange red
	m_ColorDirectionVector[DTA_SouthEast] = RGB(255,255,255 ); // white
	m_ColorDirectionVector[DTA_SouthWest] = RGB(255,255,255); // white
	m_ColorDirectionVector[DTA_NotDefined] = RGB(169,169,169); // grey



	m_MaxLinkWidthAsLinkVolume = 1000;

	m_AgentSelectionMode = CLS_network;
	m_bLoadNetworkDataOnly = false;

	m_bSimulationDataLoaded  = false;
	m_EmissionDataFlag = false;
	m_bLinkToBeShifted = true;
	m_SimulationStartTime_in_min = 0;  // 6 AM
	m_SimulationEndTime_in_min = 1440;


	m_LinkMOEMode = MOE_none;
	m_PrevLinkMOEMode = MOE_fftt;  // make sure the document gets a change to initialize the display view as the first mode is MOE_none
	m_ODMOEMode = odnone;

	MaxNodeKey = 60000;  // max: unsigned short 65,535;
	m_BackgroundBitmapLoaded  = false;
	m_LongLatCoordinateFlag = false;
	m_ColorFreeway = RGB(198,226,255);
	m_ColorHighway = RGB(100,149,237);
	m_ColorArterial = RGB(0,0,0);
	m_pNetwork = NULL;
	m_ONodeNo = -1;

	m_DNodeNo = -1;
	m_NodeSizeSP = 0;


	m_PathMOEDlgShowFlag = false;
	m_SelectPathNo = -1;

	m_ImageX1 = 0;
	m_ImageX2 = 1000;
	m_ImageY1 = 0;
	m_ImageY2 = 1000;

	m_ZoneTextColor = RGB(0,191,255);

	m_bLoadMovementData = true;

	if(theApp.m_VisulizationTemplate == e_traffic_assignment)
	{

		m_NodeDisplaySize = 0.01;  // in km
		m_BottleneckDisplaySize = 1;
		m_AgentDisplaySize = 0.001; // in km
		theApp.m_BackgroundColor =  RGB(255,255,255);  //white
		m_NodeTextDisplayRatio = 4;

	}

	

	char CurrentDirectory[MAX_PATH + 1];
	GetCurrentDirectory(MAX_PATH, CurrentDirectory);

	CString NEXTASettingsPath;
	NEXTASettingsPath.Format("%s\\NEXTA_Settings.ini", CurrentDirectory);

	m_NetworkRect.top  = g_GetPrivateProfileDouble("Grid", "top", 50, NEXTASettingsPath);
	m_NetworkRect.bottom = g_GetPrivateProfileDouble("Grid", "bottom", 0, NEXTASettingsPath);

	m_NetworkRect.left   = g_GetPrivateProfileDouble("Grid", "left", 0, NEXTASettingsPath);
	m_NetworkRect.right = g_GetPrivateProfileDouble("Grid", "right", 100, NEXTASettingsPath);

	m_UnitDistance = 1;


	m_OffsetInDistance = 0.1;
	m_LaneWidthInKM = 4/1000.0;
	m_bFitNetworkInitialized = false; 

	m_DefaultNumLanes = 1;
	m_DefaultSpeedLimit = 65.0f;
	m_DefaultCapacity = 1900.0f;
	m_DefaultLinkType = 1;

	m_ODSize = 0;
	m_PreviousODSize = -1;
	m_SelectedLinkNo = -1;
	m_SelectedNodeNo = -1;
	m_SelectedZoneID = -1;
	m_SelectedOZoneID = -1;
	m_SelectedDZoneID = -1;

	m_bSetView = false;
	m_bShowLegend = false;
	m_bShowPathList = true;

	for(int i=0; i<40;i++)
	{
		for(int los= 0; los < MAX_LOS_SIZE; los++)
		{
			m_LOSBound[i][los] = 0;
		}
	}

	// speed LOS bound
	m_LOSBound[MOE_speed][1] = 100;
	m_LOSBound[MOE_speed][2] = 80;
	m_LOSBound[MOE_speed][3] = 65;
	m_LOSBound[MOE_speed][4] = 50;
	m_LOSBound[MOE_speed][5] = 40;
	m_LOSBound[MOE_speed][6] = 33;
	m_LOSBound[MOE_speed][7] = 0;

	m_LOSBound[MOE_density][1] = 0;
	m_LOSBound[MOE_density][2] = 11;
	m_LOSBound[MOE_density][3] = 17;
	m_LOSBound[MOE_density][4] = 25;
	m_LOSBound[MOE_density][5] = 35;
	m_LOSBound[MOE_density][6] = 45;
	m_LOSBound[MOE_density][7] = 999;


	m_LOSBound[MOE_bottleneck][7] = 100;
	m_LOSBound[MOE_bottleneck][6] = 83.3;
	m_LOSBound[MOE_bottleneck][5] = 66.7;
	m_LOSBound[MOE_bottleneck][4] = 50.0;
	m_LOSBound[MOE_bottleneck][3] = 33.3;
	m_LOSBound[MOE_bottleneck][2] = 16.7;
	m_LOSBound[MOE_bottleneck][1] = 0;


	float max_value[10];
	max_value[1]= 2.0000;
	max_value[2]= 5.0000;
	max_value[3]= 5.0000;
	max_value[4]= 0.6;

	
	m_TrafficFlowModelFlag = 1;  // static traffic assignment as default
	m_Doc_Resolution = 1;
	m_bShowCalibrationResults = false;

	m_SampleExcelNetworkFile = "\\sample_data_sets\\2. Importing_Creating_Traffic_Modeling_Data_Sets\\0. Import_Excel_files\\";
	//	m_SampleExcelSensorFile = "\\Sample_Import_Excel_Files\\input_Portland_sensor_data.xls";

	TDDemandSOVMatrix = NULL;
	TDDemandHOVMatrix = NULL;
	TDDemandTruckMatrix = NULL;

}

static bool DeleteLinkPointer( DTALink * theElement ) { delete theElement; return true; }

void CTLiteDoc::ClearNetworkData()
{
	m_LinkKeyMap.clear ();
	m_SpeedSensorIDMap.clear ();
	m_CountSensorIDMap.clear ();

	m_NodeSet.clear ();
	m_LinkSet.clear ();

	m_NodeIDtoZoneNameMap.clear();
	m_NodeIDtoLinkMap.clear();

	m_NodeIDtoNodeNoMap.clear();
	m_ODSize = 0;
	m_PathDisplayList.clear();
	m_LinkIDRecordVector.clear();
	m_MessageStringVector.clear();

	m_DTAPointSet.clear();
	m_DTALineSet.clear();


	m_SubareaNodeSet.clear();
	m_SubareaLinkSet.clear();
	m_ZoneMap.clear();

	m_DemandFileVector.clear();




}

CTLiteDoc::~CTLiteDoc()
{
	
	g_bValidDocumentChanged = true;
	CWaitCursor wait;
	if(m_ODMOEMatrix !=NULL)
		Deallocate3DDynamicArray<AgentStatistics>(m_ODMOEMatrix,	m_PreviousAgentTypeSize, m_PreviousZoneNoSize);


	if(g_LinkMOEDlg!=NULL && g_LinkMOEDlg->m_pDoc == this)
	{
		delete g_LinkMOEDlg;
		g_LinkMOEDlg= NULL;

	}

	

	if(g_pLegendDlg!=NULL &&  g_pLegendDlg->m_pDoc == this)
	{
		delete g_pLegendDlg;
		g_pLegendDlg= NULL;

	}

	if(g_pLinkListDlg!=NULL && g_pLinkListDlg->m_pDoc == this)
	{
		delete g_pLinkListDlg;
		g_pLinkListDlg= NULL;

	}

	if(g_pAgentPathDlg!=NULL && g_pAgentPathDlg->m_pDoc == this)
	{
		delete g_pAgentPathDlg;
		g_pAgentPathDlg= NULL;

	}
	if(g_pPathListDlg!=NULL && g_pPathListDlg->m_pDoc == this)
	{
		delete g_pPathListDlg;
		g_pPathListDlg= NULL;

	}


	std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
	while (iDoc != g_DocumentList.end())
	{
		if((*iDoc) == this)
		{
			g_DocumentList.erase (iDoc);  // remove the document to be deleted
			break;

		}
		iDoc++;
	}

	m_NEXTALOGFile.close();

	/*if(m_pNetwork!=NULL)
		delete m_pNetwork;*/


	//	m_LinkSet.remove_if (DeleteLinkPointer);

	/*
	std::list<DTANode*>::iterator iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
	DTANode* pNode = (*iNode);

	if(pNode!=NULL)
	delete pNode;

	}
	*/
}

BOOL CTLiteDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CTLiteDoc serialization

void CTLiteDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CTLiteDoc diagnostics

#ifdef _DEBUG
void CTLiteDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTLiteDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG



void CTLiteDoc::SetStatusText(CString StatusText)
{
	CTLiteView* pView = 0;
	POSITION pos = GetFirstViewPosition();
	if(pos != NULL)
	{
		pView = (CTLiteView*) GetNextView(pos);
		if(pView!=NULL)
			pView->SetStatusText(StatusText);
	}


}

bool CTLiteDoc::ReadSimulationLinkMOEData_Parser(LPCTSTR lpszFileName)
{


	int error_count = 0;
	CCSVParser parser;

	int i= 0;
	if (parser.OpenCSVFile(lpszFileName))
	{

		for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			(*iLink)->ResetMOEAry(g_Simulation_Time_Horizon);  // use one day horizon as the default value
		}

		while(parser.ReadRecord())
		{

			string link_id;
			if(parser.GetValueByFieldName("link_id", link_id) == false)
				break;

			DTALink* pLink = FindLinkWithLinkID(link_id);

			if(pLink!=NULL)
			{


					string time_period;

					if (parser.GetValueByFieldName("time_period", time_period) == false)
						break;

					vector<float> time_stamp_vector;
					time_stamp_vector = g_time_parser(time_period);


					if (time_stamp_vector.size() != 2)
						continue; 

						float travel_time_in_min = 0;
						float link_volume = 0;
						float density = 0;
						float speed = 60.0; 
						float queue_length_ratio = 0;
						float number_of_queued_agents = 0;
						float VOC = 0;

						parser.GetValueByFieldName("travel_time", travel_time_in_min);
						parser.GetValueByFieldName("volume", link_volume);
						parser.GetValueByFieldName("density", density);
						parser.GetValueByFieldName("speed", speed);
						parser.GetValueByFieldName("queue", number_of_queued_agents);  // virtual queue
						parser.GetValueByFieldName("VOC", VOC);  // virtual queue

						float K_jam = 200;
						if(number_of_queued_agents>=1)
						{
							queue_length_ratio = number_of_queued_agents / (pLink->m_Length * pLink->m_NumberOfLanes * K_jam);
						}

						if (travel_time_in_min < 0.01 && speed>0.1)  // travel time data invalid, but with speed data
						{
							travel_time_in_min = pLink->m_Length / speed * 60;
						}

						if (travel_time_in_min > 0.01 && speed<0.01)  // travel time data valid, but without speed data
						{
							speed = pLink->m_Length / (travel_time_in_min / 60);
						}


						pLink->m_total_link_volume += link_volume;
						pLink->m_total_speed += speed;
						pLink->m_total_speed_count += 1;
						pLink->m_MeanSpeed = pLink->m_total_speed / pLink->m_total_speed_count;
						pLink->m_MeanSpeed = pLink->m_total_speed / pLink->m_total_speed_count;
						pLink->m_VoCRatio = VOC;

						pLink->m_hourly_link_volume = pLink->m_total_link_volume / (max(1,m_DemandLoadingEndTimeInMin - m_DemandLoadingStartTimeInMin)/60);

						float time_duration_per_hour = max(0.01,(time_stamp_vector[1] - time_stamp_vector[0]) / 60.0);

						if (m_DemandLoadingStartTimeInMin > time_stamp_vector[0])
							m_DemandLoadingStartTimeInMin = time_stamp_vector[0];

						if (m_DemandLoadingEndTimeInMin < time_stamp_vector[1])
							m_DemandLoadingEndTimeInMin = time_stamp_vector[1];


						for (int tt = time_stamp_vector[0]; tt < time_stamp_vector[1]; tt++)
						{
							if (tt < pLink ->m_LinkMOEArySize)
							{
								pLink->m_LinkMOEAry[tt].TravelTime = travel_time_in_min;
								pLink->m_LinkMOEAry[tt].LinkFlow = link_volume/ time_duration_per_hour;
								pLink->m_LinkMOEAry[tt].Density = density;
								pLink->m_LinkMOEAry[tt].Speed = speed;
								pLink->m_LinkMOEAry[tt].QueueLengthRatio = queue_length_ratio;

							}
						}

						i++;
				

			}else
			{
				error_count++;
				if(error_count<=4)
				{ 
				CString msg;
				msg.Format ("Please check if link_id %s at link_performance.csv is defined in link.csv.", link_id.c_str());  // +2 for the first field name line
				AfxMessageBox(msg);
				}
				continue;
			}

		}

		m_bSimulationDataLoaded = true;



		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

		if (pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingStartTimeInMin == -1)
		{
			pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingStartTimeInMin = m_DemandLoadingStartTimeInMin;
		}
		else if (pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingStartTimeInMin > m_DemandLoadingStartTimeInMin)
		{
			pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingStartTimeInMin = m_DemandLoadingStartTimeInMin;

		}

		if (pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingEndTimeInMin == -1)
		{
			pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingEndTimeInMin = m_DemandLoadingEndTimeInMin;
		}
		else if (pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingEndTimeInMin < m_DemandLoadingEndTimeInMin)
		{
			pMainFrame->m_wndPlayerSeekBar.m_DemandLoadingEndTimeInMin = m_DemandLoadingEndTimeInMin;

		}

		pMainFrame->m_wndPlayerSeekBar.Invalidate();



		g_Simulation_Time_Stamp = 0; // reset starting time
		g_SimulationStartTime_in_min = 0;

		m_SimulationLinkTDMOEDataLoadingStatus.Format ("%d link time-dependent MOE records are loaded from file %s. %d records do not have valid link_id.",i,lpszFileName, error_count);
		return true;
	}
	else
	{
		return false;
	}
}





BOOL CTLiteDoc::OnOpenTrafficNetworkDocument(CString ProjectFileName, bool bNetworkOnly, bool bImportShapeFiles)
{


	m_NodeDisplaySize = 0.01;  // in KM


	CTime LoadingStartTime = CTime::GetCurrentTime();

	m_bLoadNetworkDataOnly = bNetworkOnly;
	FILE* st = NULL;
	//	cout << "Reading file node.csv..."<< endl;

	CString directory;
	m_ProjectFile = ProjectFileName;
	directory = m_ProjectFile.Left(ProjectFileName.ReverseFind('\\') + 1);

	m_ProjectDirectory = directory;
	m_ProjectTitle = GetWorkspaceTitleName(directory);
	SetTitle(m_ProjectTitle);

	// read users' prespecified control type
	ReadLinkTypeCSVFile(directory+"settings.csv");

 // we need to check the data consistency here
	ReadAgentTypeCSVFile(directory+"settings.csv");

	CWaitCursor wc;
	OpenWarningLogFile(directory);

	ClearNetworkData();
	m_ODSize = 0;


	if (ReadNodeCSVFile(directory + "node.csv") || m_BackgroundBitmapLoaded == true)
	{
	
		ReadLinkCSVFile(directory + "link.csv", false, false);
		ReadZoneCSVFile(directory + "zone.csv");
		ReadDemandCSVFile(directory + "demand.csv");

	}


	ConstructMovementVector();
	ConstructMovementVector();
	ReadAMSMovementCSVFile(directory + "movement.csv", -1);
//	ReadAMSSignalControlCSVFile(directory + "input_timing.csv");

	LoadSimulationOutput();

	CalculateDrawingRectangle(false);
	m_bFitNetworkInitialized  = false;

	CTime LoadingEndTime = CTime::GetCurrentTime();

	CTimeSpan ts = LoadingEndTime  - LoadingStartTime;
	CString str_running_time;

	str_running_time.Format ("Network loading time: %d min(s) %d sec(s)...",ts.GetMinutes(), ts.GetSeconds());

	SetStatusText(str_running_time);

	OffsetLink(); 

	ReadBackgroundImageFile();


	m_AMSLogFile.close();
	return true;
}


BOOL CTLiteDoc::OnOpenDocument(CString ProjectFileName, bool bLoadNetworkOnly )
{
	CWaitCursor wait;


	CTime LoadingStartTime = CTime::GetCurrentTime();

	if(ProjectFileName.Find("csv")>=0)  //Transportation network project format
	{

		OnOpenTrafficNetworkDocument(ProjectFileName,bLoadNetworkOnly);
	}else
	{
		AfxMessageBox("The selected file type is not selected.");
		return false;	

	}


	CTime LoadingEndTime = CTime::GetCurrentTime();

	CTimeSpan ts = LoadingEndTime  - LoadingStartTime;

	m_StrLoadingTime.Format ("Overall loading time: %d min(s) %d sec(s)...",ts.GetMinutes(), ts.GetSeconds());

	CDlgFileLoading dlg;
	dlg.m_pDoc = this;
	dlg.DoModal ();

	UpdateAllViews(0);
	return true;

}

bool CTLiteDoc::ReadBackgroundImageFile()
{
	//read impage file Background.bmp
	
	
	
	if(m_BackgroundBitmapLoaded)
		m_BackgroundBitmap.Detach ();

	m_BackgroundBitmap.Load(m_ProjectDirectory + "image.bmp");


	m_BackgroundBitmapLoaded = !(m_BackgroundBitmap.IsNull ());
	//	m_BackgroundBitmapLoaded = true;

	m_ImageXResolution = 1;
	m_ImageYResolution = 1;

	if(m_BackgroundBitmapLoaded)
	{

		CString SettingsPath;
		SettingsPath.Format("%s\\image.bpw", m_ProjectDirectory);

		FILE* st = fopen(SettingsPath, "r");
		if (st != NULL)
		{

			// Number of matrices and the multiplication factor
			float pixelX = g_read_float(st);
			g_read_float(st);
			g_read_float(st);
			float pixelY = g_read_float(st);
			float X1 = g_read_float(st);
			float Y1 = g_read_float(st);

			float real_world_width = (int)g_GetPrivateProfileDouble("image", "real_world_width", 10, SettingsPath);

			float width = m_BackgroundBitmap.GetWidth();
			float height = m_BackgroundBitmap.GetHeight();

			//because pixelX = deltaX / image_width
			//	deltaX = pixelX * image_width
			//	X2 = X1 + deltaX


			m_ImageX1 = X1;
			m_ImageY1 = Y1;
			m_ImageX2 = X1+ pixelX * width;
			m_ImageY2 = Y1 + pixelY * height;
			m_ImageWidthInMile = real_world_width;

			m_ImageWidth = fabs(m_ImageX2 - m_ImageX1);
			m_ImageHeight = fabs(m_ImageY2 - m_ImageY1);

			m_ImageMoveSize = m_ImageWidth / 2000.0f;

			m_BackgroundImageFileLoadingStatus.Format("Optional background image file is loaded.");

			m_NetworkRect.left = m_ImageX1;
			m_NetworkRect.right = m_ImageX2;
			m_NetworkRect.top = m_ImageY1;
			m_NetworkRect.bottom = m_ImageY2;

			m_UnitDistance = fabs(pixelX * width) / max(0.0001, real_world_width);
			fclose(st);
		}
	}
	else
		m_BackgroundImageFileLoadingStatus.Format ("Optional background image file is not loaded.");



	return m_BackgroundBitmapLoaded;
}
// CTLiteDoc commands

void CTLiteDoc::OnFileOpen()
{

	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_LONGNAMES|OFN_ENABLESIZING,
		_T("GMNS Data Files (node.csv)|node.csv|"),NULL,0,true);
	if(dlg.DoModal() == IDOK)
	{
		OnOpenDocument(dlg.GetPathName());
	}
}



COLORREF CTLiteDoc::GetLinkTypeColor(int LinkType)
{
	COLORREF color;
	switch (LinkType)
	{
	case 1: color = m_ColorFreeway; break;
	case 2: color = m_ColorHighway; break;
	default: color = m_ColorArterial;

	}
	return color;
}

void CTLiteDoc::OnShowShowpathmoe()
{

}

void CTLiteDoc::OnUpdateShowShowpathmoe(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(g_LinkMOEDlgShowFlag);
}



bool CTLiteDoc::ReadNodeCSVFile(LPCTSTR lpszFileName, int LayerNo)
{



	CCSVParser parser_test;

	if (parser_test.OpenCSVFile(lpszFileName))
	{

		//test
		parser_test.ReadRecord();
		long node_id;
		string name;
		long zone_id = 0;
		string control_type;
		double X;
		double Y;
		node_id = 0;


		if (parser_test.GetValueByFieldName("node_id", node_id) == False)
		{
			AfxMessageBox("Error: Field node_id does not exist in node.csv.");
			return false;
		}



			if (parser_test.GetValueByFieldName("x_coord", X, false) == FALSE)
			{
				AfxMessageBox("Error: Field x_coord does not exist in node.csv.");
				return false;
			}

			if (parser_test.GetValueByFieldName("y_coord", Y, false) == FALSE)
			{
				AfxMessageBox("Error: Field y_coord does not exist in node.csv.");
				return false;
			}

			parser_test.CloseCSVFile();
	
	}

CCSVParser parser;

	if (parser.OpenCSVFile(lpszFileName))
	{
		int i= m_NodeSet.size();
		while(parser.ReadRecord())
		{
			int node_id;
			string name;
			int zone_id = 0;
			DTANode* pNode = 0;
			string control_type;
			double X;
			double Y;

			node_id =0;

			int level_no = 0;


			parser.GetValueByFieldName("node_id", node_id);
			
			parser.GetValueByFieldName("name", name);
			parser.GetValueByFieldName("zone_id", zone_id);
			string node_type;
			parser.GetValueByFieldName("node_type", node_type);
			parser.GetValueByFieldName("ctrl_type", control_type);


			bool bFieldX_Exist = parser.GetValueByFieldName("x_coord",X,false);
			parser.GetValueByFieldName("y_coord",Y,false);

			string geo_string;

			if(parser.GetValueByFieldName("geometry",geo_string))
			{
				// overwrite when the field "geometry" exists
				CGeometry geometry(geo_string);

				std::vector<CCoordinate> CoordinateVector = geometry.GetCoordinateList();

				if(CoordinateVector.size()>0)
				{
					X = CoordinateVector[0].X;
					Y = CoordinateVector[0].Y;
				}

			}

			if(m_NodeIDtoNodeNoMap.size()!= 0 && m_NodeIDtoNodeNoMap.find(node_id) != m_NodeIDtoNodeNoMap.end())
			{
				CString error_message;
				error_message.Format ("Node %d in node.csv has been defined twice. Please check.", node_id);
				AfxMessageBox(error_message);
				return 0;
			}
			float production = 0.1;
			float attraction = 0.1;
			parser.GetValueByFieldName("production", production);
			parser.GetValueByFieldName("attraction", attraction);

			pNode = new DTANode;
			pNode->m_NodeID = node_id;
			pNode->m_NodeProduction = production;
			pNode->m_NodeAttraction = attraction;

			pNode->m_ctrl_type = control_type;
			pNode->m_node_type = node_type;
			//pNode->m_SignalOffsetInSecond = offset_in_second;
			pNode->m_LayerNo = level_no;


			pNode->pt.x = X;
			pNode->pt.y = Y;

			pNode->m_NodeNo = i;
			pNode->m_ZoneID = zone_id;

			//m_NodeIDtoZoneNameMap[m_NodeIDtoNodeNoMap[node_id]] = zone_number;
			//m_NodeIDMap[node_id]]->m_ZoneID = zone_number;


			//if (m_ODSize < zone_number)
			//	m_ODSize = zone_number;

			m_NodeSet.push_back(pNode);
			m_NodeNoMap[i] = pNode;
			m_NodeIDMap[node_id] = pNode;
			m_NodeIDtoNodeNoMap[node_id] = i;

			i++;

			//			cout << "node = " << node << ", X= " << X << ", Y = " << Y << endl;

		}

		m_ZoneNoSize = m_ZoneID2ZoneNoMap.size();

		std::map<int, int>	::const_iterator itr_o;

		int z = 0;
		for (itr_o = m_ZoneID2ZoneNoMap.begin(); itr_o != m_ZoneID2ZoneNoMap.end(); itr_o++)
		{
			m_ZoneID2ZoneNoMap[itr_o->first] = z;
			z++;
		}
		

		

		m_NodeDataLoadingStatus.Format ("%d nodes and %d zones are loaded from file %s.",m_NodeSet.size(), m_ZoneID2ZoneNoMap.size(), lpszFileName);
		return true;
	}else
	{
		if (LayerNo == 0)
		{
			AfxMessageBox("Error: File node.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
		}return false;
		//		g_ProgramStop();
	}

}
bool CTLiteDoc::ReadZoneCSVFile(LPCTSTR lpszFileName)
{
		bool bNodeNonExistError = false;
		int lineno = 0;

		CCSVParser parser;

		m_ZoneMap.clear();

		if (parser.OpenCSVFile(lpszFileName))
		{
			int i = 0;
			while (parser.ReadRecord())
			{
				int zone_id;
				float production = 0;
				float attraction = 0;

				if (parser.GetValueByFieldName("zone_id", zone_id) == false)
				{
					if (i == 0) // first line
					{
						AfxMessageBox("Field zone_id has not been defined in file input_activity_location.csv. Please check.");
					}
					return false;
				}

				string name;
				//parser.GetValueByFieldName("production", production);
				//parser.GetValueByFieldName("attraction", attraction);

				if (parser.GetValueByFieldName("name", name))
					m_ZoneMap[zone_id].name = name;

				std::vector<CCoordinate> CoordinateVector;
				string geo_string;

				if (parser.GetValueByFieldName("geometry", geo_string))
				{
					// overwrite when the field "geometry" exists
					CGeometry geometry(geo_string);
					CoordinateVector = geometry.GetCoordinateList();

					int si;
					for (si = 0; si < CoordinateVector.size(); si++)
					{
						GDPoint	pt;
						pt.x = CoordinateVector[si].X;
						pt.y = CoordinateVector[si].Y;
						m_ZoneMap[zone_id].m_ShapePoints.push_back(pt);

					}

				}

				lineno++;
			}

			m_ZoneDataLoadingStatus.Format("%d zone info records are loaded from file %s.", lineno, lpszFileName);
			return true;
		}
		else
		{
			m_ZoneDataLoadingStatus.Format("File zone.csv does not exist.");

			return false;
		}

}
bool CTLiteDoc::ReadDemandCSVFile(LPCTSTR lpszFileName)
{
	bool bNodeNonExistError = false;
	int lineno = 0;

	CCSVParser parser;

	if (parser.OpenCSVFile(lpszFileName))
	{
		int i = 0;
		while (parser.ReadRecord())
		{
			int o_zone_id, d_zone_id;
			float volume = 0;

			if (parser.GetValueByFieldName("o_zone_id", o_zone_id))
			{
				if (parser.GetValueByFieldName("d_zone_id", d_zone_id))
				{
					if (parser.GetValueByFieldName("volume", volume))
					{
						CString label;
						label.Format("%d,%d", o_zone_id, d_zone_id);

						if (m_ODMatrixMap.find(label) == m_ODMatrixMap.end())
						{
							m_ODMatrixMap[label].Origin = o_zone_id;
							m_ODMatrixMap[label].Destination = d_zone_id;
							m_ODMatrixMap[label].demand_volume = volume;
							i++;

						}
					}

				}



			}

		}

		m_ZoneDataLoadingStatus.Format("%d demand info records are loaded from file %s.", m_ODMatrixMap.size(), lpszFileName);
		return true;
	}
	else
	{
		m_ZoneDataLoadingStatus.Format("File demand.csv does not exist.");

		return false;
	}

}
void CTLiteDoc::BuildGridSystem()
{

	bool bRectInitialized = false;
	m_AdjLinkSize = 0;

	for (std::list<DTANode*>::iterator iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		if ((*iNode)->m_LayerNo == 0)  //base line network only
		{
			if (!bRectInitialized)
			{
				m_GridRect.left = (*iNode)->pt.x;
				m_GridRect.right = (*iNode)->pt.x;
				m_GridRect.top = (*iNode)->pt.y;
				m_GridRect.bottom = (*iNode)->pt.y;
				bRectInitialized = true;
			}

			m_GridRect.Expand((*iNode)->pt);
		}
	}

	m_GridXStep = max(0.0001,m_GridRect.Width () /_MAX_GRID_SIZE);

	m_GridYStep = max(0.0001,m_GridRect.Height ()/_MAX_GRID_SIZE);

	int time_interval_no = 0;
	for (std::list<DTANode*>::iterator iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		if ((*iNode)->m_LayerNo == 0)  //base line network only
		{
			int x_key = ((*iNode)->pt.x - m_GridRect.left) / m_GridXStep;
			int y_key = ((*iNode)->pt.y - m_GridRect.bottom) / m_GridYStep;

			//feasible region
			x_key = max(0, x_key);
			x_key = min(_MAX_GRID_SIZE - 1, x_key);

			y_key = max(0, y_key);
			y_key = min(_MAX_GRID_SIZE - 1, y_key);

			m_GridMatrix[x_key][y_key].m_NodeVector.push_back((*iNode)->m_NodeID);
			m_GridMatrix[x_key][y_key].m_NodeX.push_back((*iNode)->pt.x);
			m_GridMatrix[x_key][y_key].m_NodeY.push_back((*iNode)->pt.y);

			(*iNode)->m_AreaCode = x_key*(100) + y_key;
			(*iNode)->m_GateFlag = 0;
		}
	}

	int m_GateFlagCount[10] = { 0 };

	for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		if ((*iLink)->m_LayerNo == 0)  //base line network only
		{
			int x_key = ((*iLink)->m_FromPoint.x  - m_GridRect.left) / m_GridXStep;
			int y_key = ((*iLink)->m_FromPoint.y - m_GridRect.bottom) / m_GridYStep;

			//feasible region
			x_key = max(0, x_key);
			x_key = min(_MAX_GRID_SIZE - 1, x_key);

			y_key = max(0, y_key);
			y_key = min(_MAX_GRID_SIZE - 1, y_key);

			m_GridMatrix[x_key][y_key].m_LinkNoVector.push_back((*iLink)->m_LinkNo);

			(*iLink)->From_xGridNo = x_key;
			(*iLink)->From_yGridNo = y_key;
			(*iLink)->From_xyGridNo = x_key*(100)+ y_key;


			x_key = ((*iLink)->m_ToPoint.x - m_GridRect.left) / m_GridXStep;
			y_key = ((*iLink)->m_ToPoint.y - m_GridRect.bottom) / m_GridYStep;

			//feasible region
			x_key = max(0, x_key);
			x_key = min(_MAX_GRID_SIZE - 1, x_key);

			y_key = max(0, y_key);
			y_key = min(_MAX_GRID_SIZE - 1, y_key);

			(*iLink)->To_xGridNo = x_key;
			(*iLink)->To_yGridNo = y_key;
			(*iLink)->To_xyGridNo = x_key*(100) + y_key;

	
			if ((*iLink)->From_xyGridNo == (*iLink)->To_xyGridNo)
			{
				(*iLink)->m_GateFlag = 0;
				m_GateFlagCount[0] +=1;
			}
			else
			{
				if (m_LinkTypeMap[(*iLink)->m_link_type].IsFreeway() ||
					m_LinkTypeMap[(*iLink)->m_link_type].IsHighway())  // freeway or highway
				{
					(*iLink)->m_GateFlag = 2;
					m_NodeNoMap[(*iLink)->m_FromNodeNo]->m_GateFlag = 2;
					m_NodeNoMap[(*iLink)->m_ToNodeNo]->m_GateFlag = 2;

					m_GateFlagCount[2] += 1;

				}
				else
				{
					(*iLink)->m_GateFlag = 1;
					m_NodeNoMap[(*iLink)->m_FromNodeNo]->m_GateFlag = 1;
					m_NodeNoMap[(*iLink)->m_ToNodeNo]->m_GateFlag = 1;
					m_GateFlagCount[1] += 1;
				}
	
			}
		
		}
	}
	m_GateFlagCount[9] = 0;
}


int CTLiteDoc::FindClosestNode(double x, double y, double min_distance,  int step_size, double time_stamp_in_min)
{

	step_size = int(min_distance/m_GridXStep+1);

	int x_key = (x - m_GridRect.left)/ m_GridXStep;
	int y_key = (y -m_GridRect.bottom)/ m_GridYStep;

	//feasible region
	x_key = max(0,x_key);
	x_key = min(99,x_key);

	y_key = max(0,y_key);
	y_key = min(99,y_key);

	int NodeNo = -1;


	for(int x_i = max(0,x_key- step_size); x_i <= min(99,x_key+ step_size); x_i++)
		for(int y_i = max(0,y_key-step_size); y_i <= min(99,y_key+step_size ); y_i++)
		{

			GridNodeSet element = m_GridMatrix[x_i][y_i];

			for(unsigned int i = 0; i < element.m_NodeVector.size(); i++)
			{

				double distance = sqrt( (x-element.m_NodeX[i])*(x-element.m_NodeX[i]) + (y-element.m_NodeY[i])*(y-element.m_NodeY[i]))/m_UnitDistance;

				if(distance < min_distance)
				{

					min_distance = distance;

					NodeNo =  element.m_NodeVector[i];

				}



			}	// per node in a grid cell

		} // for nearby cell

		return NodeNo;
}

int CTLiteDoc::FindClosestZone(double x, double y, double min_distance,  int step_size)
{

	step_size = int(min_distance/m_GridXStep+1);

	int x_key = (x - m_GridRect.left)/ m_GridXStep;
	int y_key = (y -m_GridRect.bottom)/ m_GridYStep;

	//feasible region
	x_key = max(0,x_key);
	x_key = min(99,x_key);

	y_key = max(0,y_key);
	y_key = min(99,y_key);

	int NodeNo = -1;
	int ZoneNumber = -1;


	for(int x_i = max(0,x_key- step_size); x_i <= min(99,x_key+ step_size); x_i++)
		for(int y_i = max(0,y_key-step_size); y_i <= min(99,y_key+step_size ); y_i++)
		{

			GridNodeSet element = m_GridMatrix[x_i][y_i];

			for(unsigned int i = 0; i < element.m_NodeVector.size(); i++)
			{
				if( m_NodeIDMap[element.m_NodeVector[i]]->m_ZoneID >0)

				{
					double distance = sqrt( (x-element.m_NodeX[i])*(x-element.m_NodeX[i]) + (y-element.m_NodeY[i])*(y-element.m_NodeY[i]));

					if(distance < min_distance)
					{

						min_distance = distance;

						NodeNo =  element.m_NodeVector[i];
						ZoneNumber  = m_NodeNoMap[NodeNo]->m_ZoneID;
					}
				}



			}	// per node in a grid cell

		} // for nearby cell

		return ZoneNumber;
}


float CTLiteDoc::GetLinkBandWidth(float Value)
{ //output: m_BandWidthValue for each link
	//if(m_MaxLinkWidthAsLinkVolume < 10)
	//	m_MaxLinkWidthAsLinkVolume = 10;

	float VolumeRatio = 1/m_MaxLinkWidthAsLinkVolume;  // 1000 Agents flow rate as 1 lanes

	float LaneVolumeEquivalent = 2000;

	float link_volume = 0;
	// default mode
	float BandWidthValue = 0;

	BandWidthValue =  max(0,Value*LaneVolumeEquivalent*VolumeRatio);

	BandWidthValue = Value*VolumeRatio; 

	return BandWidthValue;

}

void CTLiteDoc::ReCalculateLinkBandWidth()
{ //output: m_BandWidthValue for each link
	std::list<DTALink*>::iterator iLink;

	//if(m_MaxLinkWidthAsLinkVolume < 10)
	//	m_MaxLinkWidthAsLinkVolume = 10;

	float VolumeRatio = 1/m_MaxLinkWidthAsLinkVolume;  // 1000 Agents flow rate as 1 lanes

	float LaneVolumeEquivalent = 2000;  // 200 = 1 lane.

	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		DTALink* pLink = (*iLink);
		if (pLink->m_FromNodeID == 31 && pLink->m_ToNodeID == 30)
			TRACE("");

		float link_volume = 0;
		// default mode
		pLink->m_BandWidthValue = max(0, pLink->m_NumberOfLanes);

			if (g_Simulation_Time_Stamp >= 1) // dynamic traffic assignment mode
			{

				GetLinkMOE((*iLink), MOE_volume, g_Simulation_Time_Stamp, g_MOEAggregationIntervalInMin, link_volume);

				pLink->m_BandWidthValue = link_volume * VolumeRatio;
			}
			else  // total volume
			{

				GetLinkMOE((*iLink), MOE_volume, m_DemandLoadingStartTimeInMin, m_DemandLoadingEndTimeInMin - m_DemandLoadingStartTimeInMin, link_volume);

				pLink->m_BandWidthValue = link_volume * VolumeRatio;
			}

		

	}
}
void CTLiteDoc::GenerateOffsetLinkBand()
{


	ReCalculateLinkBandWidth();

	std::list<DTALink*>::iterator iLink;

//	m_UnitDistance = max(0.00001,m_UnitDistance);

	m_LaneWidthInKM = 0.4;
	double lane_offset = m_UnitDistance*m_LaneWidthInKM;  // 20 feet per lane

	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		(*iLink)->m_BandLeftShapePoints.clear();
		(*iLink)->m_BandRightShapePoints.clear();

		(*iLink)->m_ReferenceBandLeftShapePoints.clear();
		(*iLink)->m_ReferenceBandRightShapePoints.clear();

		if((*iLink)->m_ShapePoints.size() ==0)
			continue;

		int last_shape_point_id = (*iLink) ->m_ShapePoints .size() -1;
		double DeltaX = (*iLink)->m_ShapePoints[last_shape_point_id].x - (*iLink)->m_ShapePoints[0].x;
		double DeltaY = (*iLink)->m_ShapePoints[last_shape_point_id].y - (*iLink)->m_ShapePoints[0].y;
		double theta = 0;

		if(fabs(DeltaY)>0.0000001)
			theta = atan2(DeltaY, DeltaX);


		for(unsigned int si = 0; si < (*iLink) ->m_ShapePoints .size(); si++)
		{

			// calculate theta for each feature point segment
			// longer than 0.5 miles
			if(si>= 1 && (*iLink) ->m_Length > 0.5 &&((*iLink) ->m_ShapePoints .size() >4 || m_LinkTypeMap[(*iLink)->m_link_type].IsRamp ()))  // ramp or >4 feature points
			{
				last_shape_point_id = si;
				DeltaX = (*iLink)->m_ShapePoints[last_shape_point_id].x - (*iLink)->m_ShapePoints[si-1].x;
				DeltaY = (*iLink)->m_ShapePoints[last_shape_point_id].y - (*iLink)->m_ShapePoints[si-1].y;

				if(fabs(DeltaY)>0.00001)
					theta = atan2(DeltaY, DeltaX);
				else {
					if (DeltaX > 0)
						theta = 0;
					else 
						theta = PI;
					}
			}

			GDPoint pt;

			pt.x = (*iLink)->m_ShapePoints[si].x ;
			pt.y = (*iLink)->m_ShapePoints[si].y ;

			(*iLink)->m_BandLeftShapePoints.push_back (pt);

			double x_offset = (*iLink)->m_BandWidthValue*lane_offset* cos(theta - PI / 2.0f);
			double y_offset = (*iLink)->m_BandWidthValue*lane_offset* sin(theta - PI / 2.0f);
			pt.x = (*iLink)->m_ShapePoints[si].x + x_offset;
			pt.y = (*iLink)->m_ShapePoints[si].y + y_offset;

			(*iLink)->m_BandRightShapePoints.push_back (pt);

			if((*iLink)->m_bSensorData) // refernece band
			{

				pt.x = (*iLink)->m_ShapePoints[si].x ;
				pt.y = (*iLink)->m_ShapePoints[si].y ;

				(*iLink)->m_ReferenceBandLeftShapePoints.push_back (pt);

				pt.x  = (*iLink)->m_ShapePoints[si].x + (*iLink)->m_ReferenceBandWidthValue*lane_offset* cos(theta-PI/2.0f);
				pt.y = (*iLink)->m_ShapePoints[si].y + (*iLink)->m_ReferenceBandWidthValue*lane_offset* sin(theta-PI/2.0f);
				(*iLink)->m_ReferenceBandRightShapePoints.push_back (pt);
			}

		}

	}

}

void CTLiteDoc::OffsetLink()
{
	CWaitCursor wait;
	std::list<DTALink*>::iterator iLink;

	double length_sum_in_mile = 0;
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		length_sum_in_mile += (*iLink) ->m_Length;

		if( (*iLink) ->m_Original_ShapePoints .size() ==0)// no original shape points
		{

			(*iLink) ->m_Original_ShapePoints = (*iLink) ->m_ShapePoints;
		}

		// comment it out for future examination
		//if( (*iLink) ->m_Original_ShapePoints .size() ==2)// two shape points only, overwite them using upsteram node and downstream node number 
		//{

		//		if((*iLink)->m_FromNodeID == 53718 && (*iLink)->m_ToNodeID == 53810)
		//		{
		//		TRACE("");
		//		}

		//	(*iLink) ->m_Original_ShapePoints.clear ();
		//	(*iLink) ->m_Original_ShapePoints.push_back (m_NodeNoMap[(*iLink) ->m_FromNodeNo]->pt);
		//	(*iLink) ->m_Original_ShapePoints.push_back (m_NodeNoMap[(*iLink) ->m_ToNodeNo]->pt );

		//}
	}

	if(m_bLinkToBeShifted)
	{
		std::list<DTALink*>::iterator iLink;

		double minimum_link_length = 999999;
		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			if((*iLink) -> m_bToBeShifted)
			{
				if((*iLink)->m_Length <  minimum_link_length)
					minimum_link_length = (*iLink)->m_Length ;
			}
		}
		double link_offset = max(minimum_link_length*0.025*m_UnitDistance, m_UnitDistance*m_OffsetInDistance);  // 80 feet

		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			if ((*iLink)->m_FromNodeID == 31 && (*iLink)->m_ToNodeID == 30)
				TRACE("");


			if((*iLink) -> m_bToBeShifted)
			{
				//Test if an opposite link exits
				unsigned long OppositeLinkKey = GetLinkKey((*iLink)->m_ToNodeNo,(*iLink)->m_FromNodeNo);
				if ( m_NodeIDtoLinkMap.find(OppositeLinkKey) != m_NodeIDtoLinkMap.end())
				{
					// set two links as two-way links
					(*iLink)->m_bOneWayLink = false;

					DTALink* pLink = m_NodeIDtoLinkMap[OppositeLinkKey];
					if(pLink!=NULL)
						pLink->m_bOneWayLink = false;

				}
			}
		}



		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			if ((*iLink)->m_FromNodeID == 31 && (*iLink)->m_ToNodeID == 30)
			{

				TRACE("");
			}

			if( (*iLink)->m_bOneWayLink == false)// apply link split to two way links
			{


				int last_shape_point_id = (*iLink) ->m_Original_ShapePoints .size() -1;

				ASSERT(last_shape_point_id>=1);

				double DeltaX = (*iLink)->m_Original_ShapePoints[last_shape_point_id].x - (*iLink)->m_Original_ShapePoints[0].x;
				double DeltaY = (*iLink)->m_Original_ShapePoints[last_shape_point_id].y - (*iLink)->m_Original_ShapePoints[0].y;
				double theta = 0;			
				if(fabs(DeltaY)>0.00001)
					theta= atan2(DeltaY, DeltaX);
				else {
					if (DeltaX > 0)
						theta = 0;
					else
						theta = PI;
					}

				for(unsigned int si = 0; si < (*iLink) ->m_Original_ShapePoints .size(); si++)
				{
					// calculate theta for each feature point segment
					if(si>= 1 && ( (*iLink) ->m_Original_ShapePoints .size() >4 || m_LinkTypeMap[(*iLink)->m_link_type].IsRamp ()))  // ramp or >4 feature points
					{
						last_shape_point_id = si;
						DeltaX = (*iLink)->m_Original_ShapePoints[last_shape_point_id].x - (*iLink)->m_Original_ShapePoints[si-1].x;
						DeltaY = (*iLink)->m_Original_ShapePoints[last_shape_point_id].y - (*iLink)->m_Original_ShapePoints[si-1].y;


						//if(fabs(DeltaY)>0.001)
						//	theta= atan2(DeltaY, DeltaX);
					}
					double cos_offset =  cos(theta-PI/2.0);
					double sin_offset = sin(theta-PI/2.0);
					(*iLink)->m_ShapePoints[si].x = (*iLink)->m_Original_ShapePoints[si].x + m_bRightHandTrafficFlag*link_offset* cos_offset;
					(*iLink)->m_ShapePoints[si].y = (*iLink)->m_Original_ShapePoints[si].y + m_bRightHandTrafficFlag*link_offset* sin_offset;
				}
			}


		}
	}

	m_bBezierCurveFlag = false;

	if(m_bBezierCurveFlag)  //do not apply m_bBezierCurveFlag 
	{
		BezierCurve bezier_smoothing;

		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			// bezier smoothing
			std::vector<GDPoint> NewShapePoints;

			if((*iLink) ->m_ShapePoints .size() >2)
			{
				// general smooth curve
				bezier_smoothing.Bezier2D((*iLink) ->m_ShapePoints, NewShapePoints);

				// re-write to data
				(*iLink) ->m_ShapePoints = NewShapePoints;

			}
		}

	}
}

bool CTLiteDoc::ReadLinkCSVFile(LPCTSTR lpszFileName, bool bCreateNewNodeFlag = false,  int LayerNo = 0)
{

	long i = m_LinkSet.size();
	DTALink* pLink = 0;
	float default_coordinate_distance_sum=0;
	float length_sum_in_mile = 0;

	CString error_message;
	CString warning_message = "";
	int warning_message_no = 0;

	bool bTwoWayLinkFlag = false;

	bool length_field_warning = false;
	bool number_of_lanes_field_warning = false;
	bool speed_limit_field_warning = false;
	bool capacity_field_warning = false;
	bool type_field_warning = false;


	CCSVParser parser;
	if (parser.OpenCSVFile(lpszFileName))
	{
		bool bNodeNonExistError = false;
		while (parser.ReadRecord())
		{
			string link_id;
			long from_node_id = 0;
			long to_node_id = 0;
			int direction = 0;
			double length_in_mile;
			int number_of_lanes = 1;
			int free_speed = 60;
			double capacity_in_pcphpl = 2000;
			string link_type_str;
			string name;
			float k_jam = 200;


			float grade = 0;

			if (!parser.GetValueByFieldName("link_id", link_id))  // no value
			{
				// mark it as 0 first, and we then find a new unique link id after reading all links
			}

			if (!parser.GetValueByFieldName("name", name))
				name = "";


			if (!parser.GetValueByFieldName("from_node_id", from_node_id))
			{
				AfxMessageBox("Field from_node_id does not exist in link.csv. Please check.");
				break;
			}
			if (!parser.GetValueByFieldName("to_node_id", to_node_id))
			{
				AfxMessageBox("Field to_node_id does not exist in link.csv. Please check.");
				break;
			}


			if (m_NodeIDtoNodeNoMap.find(from_node_id) == m_NodeIDtoNodeNoMap.end())
			{
				if (bCreateNewNodeFlag == false)  // not create new node
				{
					CString warning;
					warning.Format("from_node_id %d in link.csv has not been defined in node.csv.\n", from_node_id);
					bNodeNonExistError = true;

					if (warning_message.GetLength() < 3000)  // not adding and showing too many links
					{
						warning_message += warning;
					}

					continue;
				}
				else  // need to create a new node with geometry information
				{
					string geo_string;
					std::vector<CCoordinate> CoordinateVector;
					if (parser.GetValueByFieldName("geometry", geo_string))
					{
						// overwrite when the field "geometry" exists
						CGeometry geometry(geo_string);
						CoordinateVector = geometry.GetCoordinateList();
						if (CoordinateVector.size() >= 2)
						{
							GDPoint	pt;
							pt.x = CoordinateVector[0].X;
							pt.y = CoordinateVector[0].Y;
							AddNewNode(pt, from_node_id, LayerNo);

						}
						else
						{

							error_message.Format("Field geometry in Link %s has less than 2 feature points, coordinate information of upstream node cannot be extracted. Please check.", name.c_str());
							AfxMessageBox(error_message);
							return false;
						}

					}
					else
					{
						AfxMessageBox("Field geometry cannot be found in link.csv. Please check.");
						return false;
					}

				}
			}

			if (m_NodeIDtoNodeNoMap.find(to_node_id) == m_NodeIDtoNodeNoMap.end())
			{
				if (bCreateNewNodeFlag == false)  // not create new node
				{
					CString warning;
					warning.Format("to_node_id %d in link.csv has not been defined in node.csv\n", to_node_id);
					bNodeNonExistError = true;

					if (warning_message.GetLength() < 3000)  // not adding and showing too many links
					{
						warning_message += warning;
					}
					continue;
				}
				else  // create new node
				{
					string geo_string;
					std::vector<CCoordinate> CoordinateVector;
					if (parser.GetValueByFieldName("geometry", geo_string))
					{
						// overwrite when the field "geometry" exists
						CGeometry geometry(geo_string);
						CoordinateVector = geometry.GetCoordinateList();
						if (CoordinateVector.size() >= 2)
						{
							GDPoint	pt;
							pt.x = CoordinateVector[CoordinateVector.size() - 1].X;
							pt.y = CoordinateVector[CoordinateVector.size() - 1].Y;
							AddNewNode(pt, to_node_id, LayerNo);

						}
						else
						{
							error_message.Format("Field geometry in Link %s has less than 2 feature points, coordinate information of upstream node cannot be extracted. Please check.", name.c_str());
							AfxMessageBox(error_message);
							return false;
						}

					}
					else
					{
						AfxMessageBox("Field geometry cannot be found in link.csv. Please check.");
						return false;
					}


				}

			}


			DTALink* pExistingLink = FindLinkWithNodeIDs(from_node_id, to_node_id);
			bool bDonotAllowDuplicatedLinks = false;
			if (bDonotAllowDuplicatedLinks && pExistingLink)
			{

				CString warning;
				warning.Format("Link %d-> %d is duplicated.\n", from_node_id, to_node_id);
				if (warning_message.GetLength() < 3000)  // not adding and showing too many links
				{
					warning_message += warning;
				}
				continue;
			}


			if (!parser.GetValueByFieldName("length", length_in_mile))
			{
				if (!length_field_warning)
				{
					AfxMessageBox("Field length has not been defined in file link.csv. Please check.");
					length_field_warning = true;
				}

				length_in_mile = 1;
			}

			if (!parser.GetValueByFieldName("dir_flag", direction,false))
				direction = 1;



			if (!parser.GetValueByFieldName("lanes", number_of_lanes))
			{
				if (!number_of_lanes_field_warning)
				{
					AfxMessageBox("Field lanes has not been defined in file link.csv. Please check.");
					number_of_lanes_field_warning = true;
				}
				number_of_lanes = 1;

			}


			if (!parser.GetValueByFieldName("free_speed", free_speed))
			{
				if (!speed_limit_field_warning)
				{
					error_message.Format("Link %s: Field free_speed has not been defined in file link.csv. Please check.", name.c_str());
					AfxMessageBox(error_message);
					speed_limit_field_warning = true;
				}
				free_speed = 60;
			}

			if (!parser.GetValueByFieldName("capacity", capacity_in_pcphpl))
			{
				if (!capacity_field_warning)
				{
					error_message.Format("Link %s: Field capacity has not been defined in file link.csv. Please check.", name.c_str());
					AfxMessageBox(error_message);
					capacity_field_warning = true;
				}

			}

			parser.GetValueByFieldName("facility_type", link_type_str);
			int link_type = 1;
			parser.GetValueByFieldName("link_type", link_type);

			string link_key;
			parser.GetValueByFieldName("link_key", link_key);

			string main_node_id;
			string movement_str;
			string NEMA_phase_number;
			parser.GetValueByFieldName("main_node_id", main_node_id);
			parser.GetValueByFieldName("movement_str", movement_str);
			parser.GetValueByFieldName("NEMA_phase_number", NEMA_phase_number);

			string geo_string;

			std::vector<CCoordinate> Original_CoordinateVector;
			if (parser.GetValueByFieldName("geometry", geo_string))
			{
				// overwrite when the field "geometry" exists
				CGeometry geometry(geo_string);
				Original_CoordinateVector = geometry.GetCoordinateList();
			}

			bool bToBeShifted = true;
			bool bWithCoordinateVector = false;
			std::vector<CCoordinate> CoordinateVector;
			if (parser.GetValueByFieldName("geometry", geo_string))
			{
				// overwrite when the field "geometry" exists
				CGeometry geometry(geo_string);
				CoordinateVector = geometry.GetCoordinateList();
				if (CoordinateVector.size() >= 2)
				{
					m_bLinkToBeShifted = false;
					bToBeShifted = false;
					bWithCoordinateVector = true;
				}
			}
			if (bWithCoordinateVector == false)
			{
				// no geometry information
				CCoordinate cc_from, cc_to;
				cc_from.X = m_NodeIDMap[from_node_id]->pt.x;
				cc_from.Y = m_NodeIDMap[from_node_id]->pt.y;

				cc_to.X = m_NodeIDMap[to_node_id]->pt.x;
				cc_to.Y = m_NodeIDMap[to_node_id]->pt.y;

				CoordinateVector.push_back(cc_from);
				CoordinateVector.push_back(cc_to);
			}


			int link_code_start = 1;
			int link_code_end = 1;

			if (direction == -1) // reversed
			{
				link_code_start = 2; link_code_end = 2;
			}

			if (direction == 0) // two-directional link
			{
				link_code_start = 1; link_code_end = 2;
				bTwoWayLinkFlag = true;
			}

			string SpeedSensorID;
			parser.GetValueByFieldName("sensor_id", SpeedSensorID);

			int layer_no = 0;

			for (int link_code = link_code_start; link_code <= link_code_end; link_code++)
			{

				bool bNodeNonExistError = false;
				int m_SimulationHorizon = 1440;
				pLink = new DTALink(m_SimulationHorizon);

				pLink->m_LayerNo = layer_no;
				pLink->m_LinkNo = i;

				if (i == 44645)
				{
					int i_debug = 1;
				}

				pLink->m_Name = name;
				pLink->m_OrgDir = direction;
				pLink->m_LinkID = link_id;
				pLink->m_LinkKey = link_key.c_str();

				m_LinkKeyMap[pLink->m_LinkKey] = pLink;

				pLink->m_geo_string = geo_string;

				if (link_code == 1)  //AB link
				{
					pLink->m_FromNodeID = from_node_id;

					pLink->m_ToNodeID = to_node_id;

					if (pLink->m_ToNodeID == 90002800)
					{
						int idebug = 1;
					}
					pLink->m_Direction = 1;

					pLink->m_FromNodeNo = m_NodeIDtoNodeNoMap[from_node_id];
					pLink->m_ToNodeNo = m_NodeIDtoNodeNoMap[to_node_id];

					int si;

					if (LayerNo <= 1)  // not the mapping layer 
					{
						int step = 1;
						if (CoordinateVector.size() > 900)
						{
							step = CoordinateVector.size() / 900 +1;
						}


						for (si = 0; si < CoordinateVector.size(); si+= step)
						{
							GDPoint	pt;
							pt.x = CoordinateVector[si].X;
							pt.y = CoordinateVector[si].Y;
							pLink->m_ShapePoints.push_back(pt);

						}
					}

				}

				if (link_code == 2)  //BA link
				{
					pLink->m_FromNodeID = to_node_id;
					pLink->m_ToNodeID = from_node_id;

					if (pLink->m_ToNodeID == 90002800)
					{
						int idebug = 1;
					}
					pLink->m_Direction = 1;

					pLink->m_FromNodeNo = m_NodeIDtoNodeNoMap[to_node_id];
					pLink->m_ToNodeNo = m_NodeIDtoNodeNoMap[from_node_id];

					for (int si = CoordinateVector.size() - 1; si >= 0; si--)
					{
						GDPoint	pt;
						pt.x = CoordinateVector[si].X;
						pt.y = CoordinateVector[si].Y;
						pLink->m_ShapePoints.push_back(pt);
					}

					pLink->m_bToBeShifted = bToBeShifted;
				}
				// original geometry

				if (pLink->m_ShapePoints.size() == 0)
				{
					TRACE("");
				}


				if (Original_CoordinateVector.size() > 0) // data available
				{
					for (unsigned int si = 0; si < Original_CoordinateVector.size(); si++)
					{
						GDPoint	pt;
						pt.x = Original_CoordinateVector[si].X;
						pt.y = Original_CoordinateVector[si].Y;
						pLink->m_Original_ShapePoints.push_back(pt);

					}
				}
				else
				{  //   data not available, we might use the previously offet shape points to construct the "base-line" shape points
					pLink->m_Original_ShapePoints = pLink->m_ShapePoints;

					m_OffsetInDistance = 0;

				}
				pLink->m_NumberOfLanes = number_of_lanes;

				pLink->main_node_id = main_node_id;
				pLink->movement_str = movement_str;
				pLink->NEMA_phase_number = NEMA_phase_number;

				pLink->m_FreeSpeed = max(10, free_speed);  // minimum Free Speed is 1 mph
				//	pLink->m_Length= max(length_in_mile, pLink->m_FreeSpeed*0.1f/60.0f);  // minimum distance, special note: we do not consider the minimum constraint here, but a Agent cannot travel longer then 0.1 seconds
				pLink->m_Length = length_in_mile;
				pLink->m_FreeFlowTravelTime = pLink->m_Length / pLink->m_FreeSpeed*60.0f;  // convert from hour to min
				pLink->m_StaticTravelTime = pLink->m_FreeFlowTravelTime;



				pLink->m_MaximumServiceFlowRatePHPL = capacity_in_pcphpl;
				pLink->m_LaneCapacity = pLink->m_MaximumServiceFlowRatePHPL;
				pLink->m_link_type_str = link_type_str;
				pLink->m_link_type = link_type;
				pLink->m_Grade = grade;
				
				pLink->m_Kjam = k_jam;
				m_NodeNoMap[pLink->m_FromNodeNo]->m_Connections += 1;
				m_NodeNoMap[pLink->m_ToNodeNo]->m_Connections += 1;


				if (m_LinkTypeMap[pLink->m_link_type].IsFreeway() || m_LinkTypeMap[pLink->m_link_type].IsRamp())
				{
					m_NodeNoMap[pLink->m_FromNodeNo]->m_bConnectedToFreewayORRamp = true;
					m_NodeNoMap[pLink->m_ToNodeNo]->m_bConnectedToFreewayORRamp = true;

				}

				m_NodeNoMap[pLink->m_FromNodeNo]->m_OutgoingLinkVector.push_back(pLink->m_LinkNo);
				m_NodeNoMap[pLink->m_ToNodeNo]->m_IncomingLinkVector.push_back(pLink->m_LinkNo);

				if (m_LinkTypeMap[pLink->m_link_type].IsConnector() == false)
					m_NodeNoMap[pLink->m_ToNodeNo]->m_IncomingNonConnectors++;


				unsigned long LinkKey = GetLinkKey(pLink->m_FromNodeNo, pLink->m_ToNodeNo);

				m_NodeIDtoLinkMap[LinkKey] = pLink;

				__int64  LinkKey2 = GetLink64Key(pLink->m_FromNodeID, pLink->m_ToNodeID);
				m_NodeIDtoLinkMap[LinkKey2] = pLink;

				m_LinkIDtoLinkMap[link_id] = pLink;


				pLink->m_FromPoint = m_NodeNoMap[pLink->m_FromNodeNo]->pt;
				pLink->m_ToPoint = m_NodeNoMap[pLink->m_ToNodeNo]->pt;

				if (pLink->m_Length >= 0.00001)
				{
					default_coordinate_distance_sum += pLink->DefaultDistance();
					length_sum_in_mile += pLink->m_Length;
				}

			


				if (!bNodeNonExistError)
				{
					//TRACE("\nAdd link no.%d,  %d -> %d",i,pLink->m_FromNodeID, pLink->m_ToNodeID );
					m_LinkSet.push_back(pLink);
					m_LinkNoMap[i] = pLink;
					i++;
				}
			}

		}


		m_UnitDistance = 1.0f;

		if (length_sum_in_mile > 0.000001f)
		{
			m_UnitDistance = default_coordinate_distance_sum / length_sum_in_mile;

		}


		double AvgLinkLength = length_sum_in_mile / max(1, m_LinkSet.size());
			   		 

		m_MovementTextBoxSizeInDistance = AvgLinkLength * 0.2; // 20% of average link length

		for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			if ((*iLink)->m_Length <= 0.00001)  //reset zero length link
			{
				(*iLink)->m_Length = pLink->DefaultDistance() / m_UnitDistance;

			}

		}

		// count of # of transit and walking links

		int transit_link_count = 0;
		int walking_link_count = 0;
		std::list<DTALink*>::iterator iLink;

		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{


			if (m_LinkTypeMap[(*iLink)->m_link_type].IsTransit())
				transit_link_count++;

			if (m_LinkTypeMap[(*iLink)->m_link_type].IsWalking())
				walking_link_count++;

		}

		if (walking_link_count > 0 || transit_link_count > 0)
		{
			m_LinkDataLoadingStatus.Format("%d links (%d transit links, %d walking links) are loaded from file %s.",
				m_LinkSet.size(), transit_link_count, walking_link_count, lpszFileName);

		}
		else
		{
			m_LinkDataLoadingStatus.Format("%d links are loaded from file %s.", m_LinkSet.size(), lpszFileName);
		}


		if (bTwoWayLinkFlag == true)
			m_bLinkToBeShifted = true;

		GenerateOffsetLinkBand();

		BuildGridSystem();

		if (warning_message.GetLength() >= 1)
		{
			CString final_message;
			final_message = "Warning messages:\n" + warning_message + "\n\nIf you save the network in *.tnp again, then duplicated links and links with non-existings nodes will be automatically removed.\n";
			AfxMessageBox(final_message);
		}
		return true;
		}
		else
		{
			if (LayerNo == 0)
			{
				AfxMessageBox("Error: File link.csv cannot be opened.\n It might be currently used and locked by EXCEL.");
			}
			return false;
			//		g_ProgramStop();
		}


}




bool CTLiteDoc::ReadAgentTypeCSVFile(LPCTSTR lpszFileName)
{
	long lineno = 1;
	CCSVParser parser;

	int agent_type_no = 1;  // start from 1, 0 is reserved for all types 

	if (parser.OpenCSVFile(lpszFileName))
	{
		m_AgentTypeMap.clear();

		while(parser.ReadRecord())
		{
			string agent_type;
			float averageVOT;

			if (parser.GetValueByFieldName("agent_type", agent_type) == false)
				break;
				
			string agent_type_name;
			parser.GetValueByFieldName("name",agent_type_name);


			DTAAgentType element;
			element.agent_type = agent_type;
			element.agent_type_name  = agent_type_name.c_str ();
			element.agent_type_no = agent_type_no++;

			m_AgentTypeMap[agent_type] = element;

			lineno++;
		}
		m_AMSLogFile << "Read " << m_AgentTypeMap.size() << " demand types from file "  << lpszFileName << endl; 

		CString msg;
		msg.Format("Imported %d demand types from file %s",m_AgentTypeMap.size(),lpszFileName);
		m_MessageStringVector.push_back (msg);

		return true;
	}else
	{
		CString msg;
		msg.Format("Imported 0 demand types from file %s",lpszFileName);
		m_MessageStringVector.push_back (msg);

		return false;
	}

}


bool CTLiteDoc::ReadLinkTypeCSVFile(LPCTSTR lpszFileName)
{
	long lineno = 0;
	CCSVParser parser;

	if (parser.OpenCSVFile(lpszFileName))
	{
		m_LinkTypeMap.clear();
		while(parser.ReadRecord_Section())
		{
			if (parser.SectionName == "[link_type]")
			{

			DTALinkType element;

			if(parser.GetValueByFieldName("link_type",element.link_type ) == false)
			{
				if(lineno==0)
				{
					AfxMessageBox("Field link_type cannot be found in link_type.csv.");
				}
				break;
			}



			if(element.link_type<0)
			{
				CString str;
				str.Format ("Field link_type in file link_type.csv has an invalid value of %d at line %d. Please check.",element.link_type, lineno);
				AfxMessageBox(str);
				break;
			}

			parser.GetValueByFieldName("name", element.link_type_name);


			if (parser.GetValueByFieldName("type_code", element.type_code))
			{

				if (element.type_code.find('f') != string::npos)
				{
					m_LinkTypeFreeway = element.link_type;
				}

				if (element.type_code.find('a') != string::npos)
				{
					m_LinkTypeArterial = element.link_type;
				}

				if (element.type_code.find('h') != string::npos)
				{
					m_LinkTypeHighway = element.link_type;
				}
			}

			m_LinkTypeMap[element.link_type] = element;

			}
			lineno++;
			
		}

		CString msg;
		msg.Format("Imported %d link types from file %s",m_LinkTypeMap.size(),lpszFileName);
		m_MessageStringVector.push_back (msg);
		m_AMSLogFile << "Read " << m_LinkTypeMap.size() << " link types from file "  << lpszFileName << endl; 

		return true;
	}else
	{
		return false;

	}

}

void CTLiteDoc::SendTexttoStatusBar(CString str, int index)
{
	POSITION pos = GetFirstViewPosition();
	CView *p = (CView *)this->GetNextView(pos);
	CStatusBar* bar = (CStatusBar*)(p->GetParentFrame()->GetMessageBar());
	bar->SetPaneText(index, str);
}




BOOL CTLiteDoc::SaveLinkData()
{
	CWaitCursor wait;

	// reidentify bottlenecks;

	int StartHour = m_DemandLoadingStartTimeInMin/60;
	int EndHour = int((m_DemandLoadingEndTimeInMin+59)/60);

	int Start15Min =  0;
	int End15Min =  1440;

	CString  directory = m_ProjectDirectory;

	FILE* st;
	fopen_s(&st, directory + "link.csv", "w");
	if(st!=NULL)
	{
		std::list<DTALink*>::iterator iLink;

		fprintf(st,"link_id,name,from_node_id,to_node_id,facility_type,link_type,dir_flag,length,lanes,free_speed,capacity,main_node_id,movement_str,NEMA_phase_number,geometry,");
		fprintf(st,"\n");	

		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			if( (*iLink)->m_FromNodeNo != (*iLink)->m_ToNodeNo)
			{
				int ToNodeNo = (*iLink)->m_ToNodeNo ;
				DTANode* pNode = m_NodeNoMap[ToNodeNo];
	
				CString link_type_name = " ";

				if(m_LinkTypeMap.find((*iLink)->m_link_type) != m_LinkTypeMap.end())
				{
					link_type_name = m_LinkTypeMap[(*iLink)->m_link_type].link_type_name.c_str ();
				}

				int sign_flag = 1;

				std::replace( (*iLink)->m_Name.begin(), (*iLink)->m_Name.end(), ',', ' '); 

				fprintf(st, "%s,%s,%d,%d,%s,%d,%d,",
					(*iLink)->m_LinkID.c_str(),
					(*iLink)->m_Name.c_str(),
					(*iLink)->m_FromNodeID,
					(*iLink)->m_ToNodeID,
					(*iLink)->m_link_type_str.c_str(),
					(*iLink)->m_link_type,
					(*iLink)->m_Direction);

				fprintf(st, "%.5f,%d,%.1f,%.1f,",
					(*iLink)->m_Length,
					(*iLink)->m_NumberOfLanes,
					(*iLink)->m_FreeSpeed,
					(*iLink)->m_LaneCapacity
					);


					fprintf(st, "%s,%s,%s,",
						(*iLink)->main_node_id.c_str(),
						(*iLink)->movement_str.c_str(),
						(*iLink)->NEMA_phase_number.c_str());
				// geometry
				fprintf(st,"\"LINESTRING (");

				for(unsigned int si = 0; si< (*iLink)->m_ShapePoints.size(); si++)
				{
					fprintf(st,"%f %f",(*iLink)->m_ShapePoints[si].x, (*iLink)->m_ShapePoints[si].y);

					if(si!=(*iLink)->m_ShapePoints.size()-1)
						fprintf(st,", ");
				}

				fprintf(st,")\"");

				fprintf(st,"\n");
			}  // layer no ==0
		}

		fclose(st);
	}else
	{
		CString message;
		message.Format("Error: File link.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
		AfxMessageBox(message);
		return false;
	}
}
BOOL CTLiteDoc::SaveNodeFile()
{

	CString  directory = m_ProjectDirectory;
	FILE* st = NULL;


	fopen_s(&st,directory+"node.csv","w");
	if(st!=NULL)
	{
		std::list<DTANode*>::iterator iNode;
		fprintf(st, "node_id,name,x_coord,y_coord,node_type,ctrl_type,zone_id,production,attraction,geometry\n");
		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

				CString control_type_name="";

				if( m_NodeTypeMap.find((*iNode)->m_ControlType)!= m_NodeTypeMap.end())				{
					control_type_name = m_NodeTypeMap[(*iNode)->m_ControlType].c_str() ;
				}

				

				std::replace( (*iNode)->m_Name.begin(), (*iNode)->m_Name.end(), ',', ' '); 

				fprintf(st, "%d,%s,%f,%f,%s,%s,%d,%f,%f,",
					(*iNode)->m_NodeID,
					(*iNode)->m_Name.c_str(),
					(*iNode)->pt.x, (*iNode)->pt.y, 
					(*iNode)->m_node_type.c_str(),
					(*iNode)->m_ctrl_type.c_str(),
					(*iNode)->m_ZoneID,
					(*iNode)->m_NodeProduction ,
					(*iNode)->m_NodeAttraction
				);

				fprintf(st, "\"POINT (%f %f)\"\n",
					(*iNode)->pt.x, (*iNode)->pt.y);

		}

		fclose(st);
	}else
	{
		AfxMessageBox("Error: File node.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
		return false;
	}
}

BOOL CTLiteDoc::SaveZoneFile()
{
	CString  directory = m_ProjectDirectory;
	FILE* st = NULL;

	fopen_s(&st, directory + "zone.csv", "w");
	if (st != NULL)
	{
		fprintf(st, "zone_id,name,geometry\n");
		std::map<int, DTAZone>	::const_iterator itr;

		for (itr = m_ZoneMap.begin(); itr != m_ZoneMap.end(); itr++)
		{
				//create activity location for empty zones

				fprintf(st, "%d,%s,",
					itr->first, itr->second.name.c_str());


				fprintf(st, "\"POLYGON ((");
				for (unsigned int si = 0; si < itr->second.m_ShapePoints.size(); si++)
				{
					fprintf(st, "%f %f", itr->second.m_ShapePoints[si].x, itr->second.m_ShapePoints[si].y);

					if (si != itr->second.m_ShapePoints.size() - 1)
						fprintf(st, ", ");
				}

				fprintf(st, "))\"");
				fprintf(st, "\n");
		}

		fclose(st);

	}
	else
	{
		AfxMessageBox("Error: File zone.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
		return false;
	}
}
void CTLiteDoc::FileSaveColorScheme()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	CString NEXTASettingsPath;
	NEXTASettingsPath.Format("%s\\NEXTA_Settings.ini", pMainFrame->m_CurrentDirectory);


	CString color_str;
	color_str.Format("%d", theApp.m_FreewayColor);
	WritePrivateProfileString("initialization", "FreewayColor", color_str, NEXTASettingsPath);

	color_str.Format("%d", theApp.m_RampColor);
	WritePrivateProfileString("initialization", "RampColor", color_str, NEXTASettingsPath);

	color_str.Format("%d", theApp.m_ArterialColor);
	WritePrivateProfileString("initialization", "ArterialColor", color_str, NEXTASettingsPath);

	color_str.Format("%d", theApp.m_ConnectorColor);
	WritePrivateProfileString("initialization", "ConnectorColor", color_str, NEXTASettingsPath);

	color_str.Format("%d", theApp.m_TransitColor);
	WritePrivateProfileString("initialization", "TransitColor", color_str, NEXTASettingsPath);

	color_str.Format("%d", theApp.m_WalkingColor);
	WritePrivateProfileString("initialization", "WalkingColor", color_str, NEXTASettingsPath);

	color_str.Format("%d", theApp.m_BackgroundColor);
	WritePrivateProfileString("initialization", "BackgroundColor", color_str, NEXTASettingsPath);

	color_str.Format("%d", theApp.m_NodeColor);
	WritePrivateProfileString("initialization", "NodeColor", color_str, NEXTASettingsPath);

	color_str.Format("%d", theApp.m_NodeBrushColor);
	WritePrivateProfileString("initialization", "NodeBackgroundColor", color_str, NEXTASettingsPath);

	color_str.Format("%d", theApp.m_ZoneColor);
	WritePrivateProfileString("initialization", "ZoneColor", color_str, NEXTASettingsPath);

	char lpbuffer[64];

	sprintf_s(lpbuffer, "%f", m_NodeDisplaySize);

	sprintf_s(lpbuffer, "%f", m_NodeDisplaySize);

	WritePrivateProfileString("GUI", "node_display_size", lpbuffer, NEXTASettingsPath);

	sprintf_s(lpbuffer, "%f", m_NodeTextDisplayRatio);
	WritePrivateProfileString("GUI", "node_text_display_ratio", lpbuffer, NEXTASettingsPath);

	CString agent_str;
	CString agent_str_color;
	for(int agent_type = 1; agent_type <=4; agent_type++)
	{
		agent_str_color.Format("AgentColor%d", agent_type);
		agent_str.Format("%d", theApp.m_AgentColor[agent_type]);
	WritePrivateProfileString("initialization", agent_str_color, agent_str, NEXTASettingsPath);
	}


}

BOOL CTLiteDoc::SaveProject(LPCTSTR lpszPathName, int SelectedLayNo)
{
	Modify(false);
	FILE* st = NULL;
	CString directory;
	CString prj_file = lpszPathName;
	directory = prj_file.Left(prj_file.ReverseFind('\\') + 1);

	CWaitCursor wc;

	CString OldDirectory = m_ProjectDirectory;
	// update m_ProjectDirectory
	m_ProjectDirectory = directory;

	char lpbuffer[64];

	SaveNodeFile();

	SaveLinkData();
	SaveMovementData();
	
	if (bSynchroImportReadyToSaveFlag)
	{
		SaveTimingData();
		bSynchroImportReadyToSaveFlag = false;
	}

	//fopen_s(&st,directory+"input_phase.csv","w");
	//if(st!=NULL)
	//{
	//	std::list<DTANode*>::iterator iNode;
	//	fprintf(st, "node_name,node_id,phase_index,incoming_link_from_node_id,outgoing_link_to_node_id,turning_direction\n");
	//	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	//	{
	//		if((*iNode)->m_LayerNo == SelectedLayNo) 
	//		{
	//			for(unsigned int p = 0; p< (*iNode)->m_PhaseVector .size(); p++)
	//			{
	//				for(unsigned int m = 0; m< (*iNode)->m_PhaseVector[p].movement_index_vector.size() ; m++)
	//				{
	//					int movement_index = (*iNode)->m_PhaseVector[p].movement_index_vector[m];;
	//					DTANodeMovement movement = (*iNode)->m_MovementVector[movement_index];


	//					fprintf(st,"%s,%d,%d,%d,%d,%s\n",(*iNode)->m_Name.c_str (), 
	//						(*iNode)->m_NodeID ,
	//						p+1,
	//						m_NodeNotoIDMap[movement.in_link_from_node_id],
	//						m_NodeNotoIDMap[movement.out_link_to_node_id ],
	//						GetTurnString(movement.movement_turn));
	//				}
	//			}
	//		}
	//	}

	//	fclose(st);
	//}else
	//{
	//	AfxMessageBox("Error: File input_phase.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
	//	return false;
	//}
	if(m_bLoadNetworkDataOnly)  // only network data are loaded, no need to save the other data.
		return true; 


	return true;
}
void CTLiteDoc::OnFileSaveProject()
{
	if(theApp.m_VisulizationTemplate == e_train_scheduling)
		return;

	if(m_ProjectDirectory.GetLength ()>3 && m_ProjectFile.GetLength()>3)
		SaveProject(m_ProjectFile);
	else
		OnFileSaveProjectAs();
}

void CTLiteDoc::OnFileSaveProjectAs()
{
	Modify(false);

	int number_of_signals  = 0;

	for (std::list<DTANode*>::iterator iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{


		if((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal)
		{
			number_of_signals++;
		}

	}

	try{
		CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();

		CFileDialog fdlg (FALSE, "node.csv", "node.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |OFN_ENABLESIZING,
			"Transportation Network Project (node.csv)|*.csv|",pMainFrame,0,true);


		if(fdlg.DoModal()==IDOK)
		{
			CString path = fdlg.GetPathName  ();
			CWaitCursor wait;
			m_ProjectFile = path;

			m_ProjectTitle = GetWorkspaceTitleName(m_ProjectFile);

			bool bSavedFlag = false;
			if(path.Find("csv")>=0)  //Transportation network project format
			{
				bSavedFlag = SaveProject(path);
			}

			if(bSavedFlag)
			{
				CString msg;
				if(m_NodeSet.size()>0)
				{
					msg.Format ("Files node.csv and link.csv  have been successfully saved with %d nodes and %d links.",
						m_NodeSet.size(), 
						m_LinkSet.size());
					AfxMessageBox(msg,MB_OK|MB_ICONINFORMATION);
				}
				SetTitle(m_ProjectTitle);


			}
		}

	} catch (char *str  ) 
	{
		AfxMessageBox(str,MB_ICONINFORMATION);
	}
	// TODO: Add your command handler code here
}

void CTLiteDoc::CalculateDrawingRectangle(bool NodeLayerOnly)
{   

	CWaitCursor wait;

	std::list<DTALink*>::iterator iLink;
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		(*iLink)->CalculateShapePointRatios();

	}

	bool bRectInitialized = false;
	m_AdjLinkSize = 0;

	for (std::list<DTANode*>::iterator iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		if(NodeLayerOnly || (*iNode)->m_Connections >0 )  // we might try import node layer only from shape file, so all nodes have no connected links. 
		{
			if(!bRectInitialized)
			{
				m_NetworkRect.left = (*iNode)->pt.x ;
				m_NetworkRect.right = (*iNode)->pt.x;
				m_NetworkRect.top = (*iNode)->pt.y;
				m_NetworkRect.bottom = (*iNode)->pt.y;
				bRectInitialized = true;
			}

			if((*iNode)->m_Connections > m_AdjLinkSize)
				m_AdjLinkSize = (*iNode)->m_Connections;

			m_NetworkRect.Expand((*iNode)->pt);
		}

	}

	for (std::list<DTAPoint*>::iterator iPoint = m_DTAPointSet.begin(); iPoint != m_DTAPointSet.end(); iPoint++)
	{
		if(!bRectInitialized)
		{
			m_NetworkRect.left = (*iPoint)->pt.x ;
			m_NetworkRect.right = (*iPoint)->pt.x;
			m_NetworkRect.top = (*iPoint)->pt.y;
			m_NetworkRect.bottom = (*iPoint)->pt.y;
			bRectInitialized = true;
		}

		m_NetworkRect.Expand((*iPoint)->pt);

	}

	if(m_BackgroundBitmapLoaded)  // if there is a bitmap background
	{

		if(!bRectInitialized)  // there is no node or link layer
		{
			m_NetworkRect.left = m_ImageX1;
			m_NetworkRect.right = m_ImageX2;
			m_NetworkRect.top = m_ImageY2;
			m_NetworkRect.bottom = m_ImageY1;
			bRectInitialized = true;

		}

	}

	for (std::list<DTALine*>::iterator iLine = m_DTALineSet.begin(); iLine != m_DTALineSet.end(); iLine++)
	{
		for(unsigned int i = 0; i< (*iLine)->m_ShapePoints .size(); i++)
		{
			if(!bRectInitialized)
			{
				m_NetworkRect.left = (*iLine)->m_ShapePoints[i].x ;
				m_NetworkRect.right = (*iLine)->m_ShapePoints[i].x;
				m_NetworkRect.top = (*iLine)->m_ShapePoints[i].y;
				m_NetworkRect.bottom = (*iLine)->m_ShapePoints[i].y;

				bRectInitialized = true;
			}else
			{
				m_NetworkRect.Expand((*iLine)->m_ShapePoints[i]);
			}
		}

	}



	if(theApp.m_VisulizationTemplate == e_train_scheduling)
	{

		CString str;
		str.Format("%d nodes and %d links are loaded.", m_NodeSet.size(), m_LinkSet.size());
		AfxMessageBox(str,MB_ICONINFORMATION);
	}


	IdentifyBottleNeckAndOnOffRamps();
}


bool CTLiteDoc::WriteSelectAgentDataToCSVFile(LPCTSTR lpszFileName, std::vector<DTAAgent*> AgentVector)
{

	// Agent_id,  origin_zone_id, destination_zone_id, departure_time,
	//	arrival_time, complete_flag, trip_time, agent_type, occupancy, information_type,
	//	value_of_time, path_min_cost,distance_in_mile, number_of_nodes,
	//	node id, node arrival time

	ofstream AgentDataFile;
	AgentDataFile.open (lpszFileName, ios::out);
	if (AgentDataFile.is_open())
	{
		AgentDataFile.width(12);
		AgentDataFile.precision(4) ;
		AgentDataFile.setf(ios::fixed);
		AgentDataFile << "agent_id,o_zone_id,d_zone_id,departure_time,trip_time,complete_flag, agent_type,pricing_type,Agent_type, information_class,value_of_time, toll_dollar_cost,emissions,distance_in_mile, number_of_nodes, <node id;node arrival time>" << endl;

		for(int v = 0; v < AgentVector.size(); v++)
		{

			DTAAgent* pAgent = AgentVector[v] ;

			if(pAgent!=NULL)
			{
				AgentDataFile << pAgent->m_AgentID << ",";
				AgentDataFile << pAgent->m_o_ZoneID << ",";
				AgentDataFile << pAgent->m_d_ZoneID << ",";
				AgentDataFile << pAgent->m_DepartureTime << ",";
				AgentDataFile << pAgent->m_TripTime << ",";
				if(pAgent->m_bComplete == 0)
					AgentDataFile << "0,";
				else
					AgentDataFile << "1,";

				AgentDataFile << pAgent->m_AgentType << ",";
				AgentDataFile << pAgent->m_Distance << ",";
				AgentDataFile << pAgent->m_NodeSize << ",\"";

				//for(int i=0; i< pAgent->m_NodeSize-1; i++)
				//{
				//	AgentDataFile << "<"; 

				//	if(pAgent->m_NodeAry[i].LinkNo!=-1 && m_LinkNoMap.find (pAgent->m_NodeAry[i].LinkNo) != m_LinkNoMap.end())
				//	{
				//	DTALink* pLink = m_LinkNoMap[pAgent->m_NodeAry[i].LinkNo];
				//
				//	AgentDataFile << pLink->m_FromNodeID << ";";
				//	AgentDataFile << pAgent->m_NodeAry[i].ArrivalTimeOnDSN << ">";
				//	}
				//}
				//AgentDataFile << pAgent->m_NodeSize << "\"";
				AgentDataFile << endl;
			}
		}
		AgentDataFile.close();
		return true;
	}

	return false;
}
void CTLiteDoc::ReadAgentCSVFile_Parser(LPCTSTR lpszFileName)
{

	//   cout << "Read Agent file... "  << endl;
	// Agent_id,  origin_zone_id, destination_zone_id, departure_time,
	//	arrival_time, complete_flag, trip_time, agent_type, occupancy, information_type,
	//	value_of_time, path_min_cost,distance_in_mile, number_of_nodes,
	//	node id, node arrival time

	float LengthinMB;
	FILE* pFile;
	fopen_s(&pFile,lpszFileName,"rb");
	if(pFile!=NULL)
	{
		fseek(pFile, 0, SEEK_END );
		int Length = ftell(pFile);
		fclose(pFile);
		LengthinMB= Length*1.0/1024/1024;
		if(LengthinMB>50)
		{
			CString msg;
			msg.Format("The %s file is %5.1f MB in size.\nIt could take quite a while to load this file.\nWould you like to load the Agent file?",lpszFileName,LengthinMB);
			if(AfxMessageBox(msg,MB_YESNO|MB_ICONINFORMATION)==IDNO)
				return;
		}
	}

	
	
		CCSVParser parser_test;
		if (parser_test.OpenCSVFile(lpszFileName))
		{
			parser_test.ReadRecord();

			DTAAgent* pAgent = 0;
			pAgent = new DTAAgent;

				string m_AgentID;
				if (!parser_test.GetValueByFieldName("agent_id", m_AgentID))
				{
					AfxMessageBox("Field agent_id does not exist in agent.csv");
					return;
				}


				if (!parser_test.GetValueByFieldName("o_zone_id", pAgent->m_o_ZoneID))
				{
					AfxMessageBox("Field o_zone_id does not exist in agent.csv");
					return;
				}
				if (!parser_test.GetValueByFieldName("d_zone_id", pAgent->m_d_ZoneID))
				{
					AfxMessageBox("Field d_zone_id does not exist in agent.csv");
						return;
				}


				//if (!parser_test.GetValueByFieldName("agent_type", pAgent->m_AgentType))
				//{
				//	AfxMessageBox("Field agent_type does not exist in agent.csv");
				//	return;

				//}

			
				delete pAgent;
				parser_test.CloseCSVFile();
		}
	CCSVParser parser;
	if (parser.OpenCSVFile(lpszFileName))
	{
		m_AgentSet.clear();
		int count = 0;
		int error_count = 0;
		while(parser.ReadRecord())
		{


			string m_AgentID;
			parser.GetValueByFieldName("agent_id",m_AgentID);
			if(m_AgentID.size()==0)
				break;

			DTAAgent* pAgent = 0;
			pAgent = new DTAAgent;
			pAgent->m_AgentID		= m_AgentID;
			pAgent->m_o_ZoneID = 0;
			pAgent->m_d_ZoneID = 0;
			pAgent->m_TripTime = 60;
			pAgent->m_Distance = 60;
			pAgent->m_Volume = 1;
			pAgent->m_AgentType = '0';

			parser.GetValueByFieldName("o_zone_id",pAgent->m_o_ZoneID);
			parser.GetValueByFieldName("d_zone_id",pAgent->m_d_ZoneID);

			m_ZoneMap[pAgent->m_o_ZoneID].m_OriginAgentSize += 1;
			m_ZoneMap[pAgent->m_d_ZoneID].m_OriginAgentSize += 1;

			if (m_ZoneID2ZoneNoMap.find(pAgent->m_o_ZoneID) == m_ZoneID2ZoneNoMap.end())  // not defined
			{
				int m_ZoneNoSize = m_ZoneID2ZoneNoMap.size();
				m_ZoneID2ZoneNoMap[pAgent->m_o_ZoneID] = m_ZoneNoSize;
				m_ZoneNo2ZoneIDMap[m_ZoneNoSize] = pAgent->m_o_ZoneID;
			}

			if (m_ZoneID2ZoneNoMap.find(pAgent->m_d_ZoneID) == m_ZoneID2ZoneNoMap.end())  // not defined
			{
				int m_ZoneNoSize = m_ZoneID2ZoneNoMap.size();
				m_ZoneID2ZoneNoMap[pAgent->m_d_ZoneID] = m_ZoneNoSize;
				m_ZoneNo2ZoneIDMap[m_ZoneNoSize] = pAgent->m_d_ZoneID;

			}

			
			pAgent->m_bComplete = true;
			pAgent->m_DepartureTime = 0;

			string time_period;
			vector<float> timestamp_vector;

			parser.GetValueByFieldName("travel_time",pAgent->m_TripTime );

			parser.GetValueByFieldName("distance",pAgent->m_Distance  );
			parser.GetValueByFieldName("volume", pAgent->m_Volume);
			parser.GetValueByFieldName("agent_type",pAgent->m_AgentType );

			if (m_AgentTypeMap.find(pAgent->m_AgentType) != m_AgentTypeMap.end())
			{

				pAgent->m_AgentTypeNo = m_AgentTypeMap[pAgent->m_AgentType].agent_type_no;

			}
			else
			{
				int agent_type_size = m_AgentTypeMap.size()+1;

				DTAAgentType element;
				element.agent_type = pAgent->m_AgentType;
				element.agent_type_name = pAgent->m_AgentType;
				element.agent_type_no = agent_type_size;

				m_AgentTypeMap[pAgent->m_AgentType] = element;
				pAgent->m_AgentTypeNo = agent_type_size;

			}

			std::string activity_node_sequence_str;
			parser.GetValueByFieldName("activity_node_sequence", activity_node_sequence_str);
			  //physcial node sequence

			std::string path_node_sequence, path_time_sequence, path_state_sequence;
			parser.GetValueByFieldName("node_sequence",path_node_sequence );
			string time_sequence_str;
			parser.GetValueByFieldName("time_sequence", time_sequence_str);

			std::vector<int> node_sequence;
			std::vector<float> time_sequence;
			std::vector<string> state_sequence;
			g_ParserIntSequence(path_node_sequence, node_sequence);


			pAgent->m_NodeSize = node_sequence.size();
			time_sequence = g_time_parser(time_sequence_str);

			std::string node_sequence_node_timestamp;
			std::vector<float> node_time_sequence;


			if (pAgent->m_NodeSize >= 1)  // in case reading error
			{

				pAgent->m_NodeAry = new SAgentLink[pAgent->m_NodeSize];
				pAgent->m_NodeIDSum = 0;
				for (int i = 0; i < pAgent->m_NodeSize; i++)
				{

					m_PathNodeVectorSP[i] = node_sequence[i];
					pAgent->m_NodeIDSum += m_PathNodeVectorSP[i];
					if (i >= 1)
					{
						DTALink* pLink = FindLinkWithNodeIDs(m_PathNodeVectorSP[i - 1], m_PathNodeVectorSP[i]);
						if (pLink == NULL)
						{
							if (error_count == 0)
							{
								CString msg;
								msg.Format("Agent file has an invalid link sequence %d->%d in the field node_sequence for agent %s. Please check.",
									m_PathNodeVectorSP[i - 1], m_PathNodeVectorSP[i], m_AgentID.c_str());
								AfxMessageBox(msg);
							}
							error_count++;
						}

						if (pLink != NULL)
						{

							pAgent->m_NodeAry[i].LinkNo = pLink->m_LinkNo;
							pAgent->m_NodeAry[i].from_pt = pLink->m_FromPoint;
							pAgent->m_NodeAry[i].to_pt = pLink->m_ToPoint;
							pLink->m_TotalTravelTime += pAgent->m_NodeAry[i].ArrivalTimeOnDSN - pAgent->m_NodeAry[i - 1].ArrivalTimeOnDSN;
						}

					}

					// random error beyond 6 seconds for better ainimation

					float random_value = g_RNNOF() * 0.01; // 0.1 min = 6 seconds

					if (time_sequence.size() >= pAgent->m_NodeSize)
					{
						pAgent->m_NodeAry[i].ArrivalTimeOnDSN = max(0,time_sequence[i] + random_value);
					}
				}

				pAgent->m_DepartureTime = pAgent->m_NodeAry[0].ArrivalTimeOnDSN;

				pAgent->m_ArrivalTime = pAgent->m_NodeAry[pAgent->m_NodeSize - 1].ArrivalTimeOnDSN;

			}else
			{

				string geo_string;
				std::vector<CCoordinate> CoordinateVector;
				if (parser.GetValueByFieldName("geometry", geo_string))
				{
					// overwrite when the field "geometry" exists
					CGeometry geometry(geo_string);
					CoordinateVector = geometry.GetCoordinateList();
					if (CoordinateVector.size() >=2 && CoordinateVector.size() == time_sequence.size() )
					{
						for(int j = 0 ; j < CoordinateVector.size(); j++)
						{
						GDPoint	pt;
						pt.x = CoordinateVector[j].X;
						pt.y = CoordinateVector[j].Y;

						AgentLocationRecord element;
						element.time_stamp_in_second = time_sequence[j] * 60;  // min to second

						element.x = pt.x;
						element.y = pt.y;
						AddLocationRecord(element);
						m_NetworkRect.Expand(pt);
						}
					}
				}
			}
		
			m_AgentSet.push_back(pAgent);
		}

		UpdateMovementDataFromAgentTrajector();

		m_SimulationAgentDataLoadingStatus.Format ("%d agents are loaded from file %s.", m_AgentSet.size(),lpszFileName);

	}
}




int CTLiteDoc::ReadAMSMovementCSVFile(LPCTSTR lpszFileName, int NodeNo = -1)
{

	int number_of_nodes = 0;

	m_MovementPointerMap.clear();

	std::list<DTALink*>::iterator iLink;

	std::list<DTANode*>::iterator iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{

				for(unsigned int m = 0; m< (*iNode)->m_MovementVector .size(); m++)
				{

					DTANodeMovement movement = (*iNode)->m_MovementVector[m];

					CString label;
					string ib_link_id = m_LinkNoMap[movement.IncomingLinkNo]->m_LinkID;
					string ob_link_id = m_LinkNoMap[movement.OutgoingLinkNo]->m_LinkID;
					label.Format("%s;%d;%s", ib_link_id.c_str (),(*iNode)->m_NodeID, ob_link_id.c_str());

					m_MovementPointerMap[label] = &((*iNode)->m_MovementVector[m]); // store pointer

					// set default value for through turns

					DTALink* pLink = m_LinkNoMap[movement.IncomingLinkNo];
					if(pLink !=NULL)
					{


						switch ((*iNode)->m_MovementVector[m].movement_turn)
						{
						case DTA_Through: (*iNode)->m_MovementVector[m].QEM_Lanes = pLink->m_NumberOfLanes; break;
						case DTA_LeftTurn: 
						case DTA_LeftTurn2:
							(*iNode)->m_MovementVector[m].QEM_Lanes = max((*iNode)->m_MovementVector[m].QEM_Lanes,
								pLink->m_NumberOfLeftTurnLanes) ; break;


						case DTA_RightTurn:
						case DTA_RightTurn2: 
							(*iNode)->m_MovementVector[m].QEM_Lanes = max( pLink->m_NumberOfRightTurnLanes,
								(*iNode)->m_MovementVector[m].QEM_Lanes); break;
						}

						(*iNode)->m_MovementVector[m].QEM_Speed  = pLink->m_FreeSpeed ;
					}else
					{
						return 0;
					}


		}
	}

	CCSVParser parser_movement;

	int count = 0;

	int current_node_id = -1;
	if (parser_movement.OpenCSVFile(lpszFileName))
	{
		while(parser_movement.ReadRecord())
		{
			string ib_link_id;
			int node_id = 0;
			string ob_link_id;


			if(parser_movement.GetValueByFieldName("node_id",node_id) == false)
				break;

			if(current_node_id != node_id)
			{
				current_node_id = node_id;
				number_of_nodes++;
			}


			parser_movement.GetValueByFieldName("ib_link_id", ib_link_id);
			parser_movement.GetValueByFieldName("ob_link_id", ob_link_id);


			DTANode* pNode = FindNodeWithNodeID (node_id);


			if(m_LinkIDtoLinkMap.find(ib_link_id)== m_LinkIDtoLinkMap.end()
				|| m_NodeIDtoNodeNoMap.find(node_id)== m_NodeIDtoNodeNoMap.end() 
				|| m_LinkIDtoLinkMap.find(ob_link_id)== m_LinkIDtoLinkMap.end() )
			{
			continue;
			
			}
			//CString movement_index = pNode->GetMovementIndex(m_LinkIDtoLinkMap[ib_link_id]->m_FromNodeNo ,
			//	m_NodeIDtoNodeNoMap[node_id], m_LinkIDtoLinkMap[ob_link_id]->m_ToNodeNo);


			CString label;
			label.Format("%s;%d;%s", ib_link_id.c_str(),node_id, ob_link_id.c_str());

			if(m_MovementPointerMap.find(label) != m_MovementPointerMap.end())
			{
				DTANodeMovement* pMovement = m_MovementPointerMap[label];

				float penalty = 0;
				parser_movement.GetValueByFieldName ("penalty", penalty);
				if (penalty >= 99)
				{
					pMovement->turning_prohibition_flag = 1;
				}

				int ib_lanes = 0;
				parser_movement.GetValueByFieldName("ib_lanes", ib_lanes);
				pMovement->QEM_Lanes = ib_lanes;

				
				std::string turn_type;

				std::string QEM_dir_string;
				parser_movement.GetValueByFieldName ("turn_type",turn_type );
				parser_movement.GetValueByFieldName ("turn_direction", QEM_dir_string);

				pMovement->movement_approach_turn  = GetTurnDirectionFromString(QEM_dir_string.c_str ());

			count++;

			}else
			{
				// if the movement has not been defined in the construction part, ignore the data. 
			}
		}
	}




	m_MovementDataLoadingStatus.Format ("%d movements are loaded from file %s.",count,lpszFileName);
	return number_of_nodes;

}

int CTLiteDoc::SaveMovementData()
{
	CWaitCursor wait;

	CString  directory = m_ProjectDirectory;

	FILE* st;
	int mvmt_count = 1;
	fopen_s(&st, directory + "movement.csv", "w");
	if (st != NULL)
	{
		fprintf(st, "mvmt_id,node_id,name,ib_link_id,ib_lanes,ob_link_id,type,penalty,capacity,ctrl_type,geometry,");

		std::list<DTANode*>::iterator iNode;
		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			for (unsigned int m = 0; m < (*iNode)->m_MovementVector.size(); m++)
			{

				DTANodeMovement movement = (*iNode)->m_MovementVector[m];

				fprintf(st, "mvmt_id,node_id,name,ib_link_id,ob_link_id,ib_lanes,type,penalty,geometry,");
				CString label;
				string ib_link_id = m_LinkNoMap[movement.IncomingLinkNo]->m_LinkID;
				string ob_link_id = m_LinkNoMap[movement.OutgoingLinkNo]->m_LinkID;
				label.Format("%s;%d;%s", ib_link_id.c_str(), (*iNode)->m_NodeID, ob_link_id.c_str());

				m_MovementPointerMap[label] = &((*iNode)->m_MovementVector[m]); // store pointer
				fprintf(st, "%d,%d,%s,%s,%d,%s,%d,",
					mvmt_count, (*iNode)->m_NodeID, (*iNode)->m_Name .c_str(), ib_link_id.c_str(), ob_link_id.c_str(),
					movement.QEM_Lanes, GetTurnDirectionString(movement.movement_approach_turn),
					movement.turning_prohibition_flag);

				GDPoint p1, p2, p3, p_text;
				// 1: fetch all data
				p1 = m_NodeIDMap[movement.in_link_from_node_id]->pt;
				p2 = m_NodeIDMap[movement.in_link_to_node_id]->pt;
				p3 = m_NodeIDMap[movement.out_link_to_node_id]->pt;

				// reserved direction
				double DeltaX = p2.x - p1.x;
				double DeltaY = p2.y - p1.y;
				double theta = atan2(DeltaY, DeltaX);

				double movement_approach_turnection_theta = atan2(p3.y - p1.y, p3.x - p1.x);


				GDPoint p1_new, p2_new, p3_new;
				GDPoint p1_text, p3_text;
				GDPoint pt_movement[3];

				double size_ratio = m_MovementTextBoxSizeInDistance / 250;

				double node_set_back = size_ratio * 150 * m_UnitDistance;

				double link_length = size_ratio * 250 * m_UnitDistance;
				// 2. set new origin
				p2_new.x = (*iNode)->pt.x + (-1) * node_set_back * cos(theta);
				p2_new.y = (*iNode)->pt.y + (-1) * node_set_back * sin(theta);

				p1_new.x = (*iNode)->pt.x + (-1) * link_length * cos(theta);
				p1_new.y = (*iNode)->pt.y + (-1) * link_length * sin(theta);



				// 3 determine the control point for  PolyBezier
				double lane_width = size_ratio * 3 * m_LaneWidthInKM * m_UnitDistance;
				double movement_offset = 0;
				double movement_length = size_ratio * 100 * m_UnitDistance;
				double text_offset = 0.7 * lane_width;
				int mid_lane_number = 4 + 1;
				float control_point_ratio = 0;

				float MovementTextFontSize = 8;

				GDPoint pt_from, pt_to, pt_text;
				pt_from.x = p1_new.x + movement_offset * cos(theta - PI / 2.0f);
				pt_from.y = p1_new.y + movement_offset * sin(theta - PI / 2.0f);


				double alpha = 0.3;

				pt_to.x = p2_new.x + movement_offset * cos(theta - PI / 2.0f);
				pt_to.y = p2_new.y + movement_offset * sin(theta - PI / 2.0f);

				// direction

				DeltaX = p2.x - p3.x;
				DeltaY = p2.y - p3.y;
				theta = atan2(DeltaY, DeltaX);


				if (movement.movement_turn == DTA_Through)
				{
					p3_new.x = pt_to.x - movement_length * cos(theta);
					p3_new.y = pt_to.y - movement_length * sin(theta);

				}
				else if (movement.movement_turn == DTA_LeftTurn)
				{
					p3_new.x = pt_to.x - movement_length * cos(theta - PI / 4.0f);
					p3_new.y = pt_to.y - movement_length * sin(theta - PI / 4.0f);
				}
				else
				{
					p3_new.x = pt_to.x - movement_length * cos(theta + PI / 4.0f);
					p3_new.y = pt_to.y - movement_length * sin(theta + PI / 4.0f);
				}

				// geometry
				fprintf(st, "\"LINESTRING (");

				fprintf(st, "%f %f,", pt_from.x, pt_from.y);
				fprintf(st, "%f %f,", pt_to.x, pt_to.y);
				fprintf(st, "%f %f", p3_new.x, p3_new.y);
				fprintf(st, ")\"");

				fprintf(st, "\n");


				mvmt_count ++;

			}
		}

		fclose(st);
	}


	return 0;

}

int CTLiteDoc::ReadAMSSignalControlCSVFile(LPCTSTR lpszFileName)
{

	CCSVParser parser_signal;

	if (parser_signal.OpenCSVFile(lpszFileName, true))  // not required
	{
		int up_node_id = 0;
		int dest_node_id = 0;

		while (parser_signal.ReadRecord())
		{
			int node_id = -1;
			int phase_id = -1;
			int next_phase_id = -1;
			int green_duration = -1;
			std::string  movement_str, movement_dir_str;

			parser_signal.GetValueByFieldName("int_id", node_id);

			DTANode* pNode = FindNodeWithNodeID(node_id);

			if (pNode != NULL)
			{
				pNode->m_withSignalPlans = true;
				//parser_signal.GetValueByFieldName("timing_plan_no", timing_plan_name);
				parser_signal.GetValueByFieldName("movement_str", movement_str);
				parser_signal.GetValueByFieldName("green_duration", green_duration);
				parser_signal.GetValueByFieldName("next_phase_id", next_phase_id,false);
				parser_signal.GetValueByFieldName("phase_id", phase_id);

				SetupPhaseData(node_id,  phase_id, PHASE_GreenDuration, green_duration);
				SetupPhaseData(node_id,  phase_id, PHASE_NextPhaseNo, next_phase_id);
				SetupPhaseData(node_id,  phase_id, PHASE_MOVEMENT_VECTOR, movement_str);

				parser_signal.GetValueByFieldName("movement_dir_str", movement_dir_str);
				SetupPhaseData(node_id,  phase_id, PHASE_MOVEMENT_DIR_VECTOR, movement_dir_str);


				if (movement_str.size() == 0 && movement_dir_str.size() > 0)
				{
					for (unsigned int m = 0; m < pNode->m_MovementVector.size(); m++)
					{
						bool bMovementIncluded = IfMovementDirIncludedInPhase(node_id,
							 phase_id, m);

						if (bMovementIncluded)
						{

							CString sub_movement_str;

							sub_movement_str.Format(";%d_%d_%s", m_NodeIDMap[pNode->m_MovementVector[m].in_link_from_node_id]->m_NodeID,
								m_NodeIDMap[pNode->m_MovementVector[m].out_link_to_node_id]->m_NodeID,
								GetTurnShortString(pNode->m_MovementVector[m].movement_turn));
							//select
							movement_str += sub_movement_str;
						}

					}

					SetupPhaseData(node_id,  phase_id, PHASE_MOVEMENT_VECTOR, movement_str);

				}

			}
		}
	}


	/// auto generate the phasing data
	std::list<DTANode*>::iterator iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		DTANode* pNode = (*iNode);
		if (pNode != NULL && pNode->m_withSignalPlans!=true && pNode->m_ControlType == 5)
		{
			int node_id = pNode->m_NodeID;
			int phase_id = -1;
			int next_phase_id = -1;
			int green_duration = 30;

			for (unsigned int m = 0; m < pNode->m_MovementVector.size(); m++)
			{
				int NEMAPhase_no = 0;
				NEMAPhase_no = GetNEMAPhase_from_TurnDirectionString(pNode->m_MovementVector[m].movement_approach_turn);
				if (NEMAPhase_no > 0)
				{
					if(NEMAPhase_no%2==1) // left turn
						green_duration = 10;
					else {
						 green_duration = 30;

						}


					phase_id = NEMAPhase_no;

					std::string timing_plan_name = "1";
					std::string movement_str, movement_dir_str;
					movement_dir_str += ";";
					SetupPhaseData(node_id,  phase_id, PHASE_GreenDuration, green_duration);
					SetupPhaseData(node_id,  phase_id, PHASE_NextPhaseNo, next_phase_id);
					SetupPhaseData(node_id,  phase_id, PHASE_MOVEMENT_VECTOR, movement_str);
					movement_dir_str += GetTurnDirectionString(pNode->m_MovementVector[m].movement_approach_turn);

					SetupPhaseData(node_id,  phase_id, PHASE_MOVEMENT_DIR_VECTOR, movement_dir_str);
					CString sub_movement_str;
					sub_movement_str.Format(";%d_%d_%s", m_NodeIDMap[pNode->m_MovementVector[m].in_link_from_node_id]->m_NodeID,
						m_NodeIDMap[pNode->m_MovementVector[m].out_link_to_node_id]->m_NodeID,
						GetTurnShortString(pNode->m_MovementVector[m].movement_turn));
					//select
						movement_str += sub_movement_str;

					SetupPhaseData(node_id,  phase_id, PHASE_MOVEMENT_VECTOR, movement_str);
				}
			}

		}
	}
	return true;
}



void  CTLiteDoc::UpdateMovementDataFromAgentTrajector()
{
	// movement count
	std::list<DTANode*>::iterator iNode;

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{
			(*iNode)->ResetMovementMOE ();		
		}

	std::list<DTAAgent*>::iterator iAgent;
	for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++)
	{
		DTAAgent* pAgent = (*iAgent);

		if(pAgent==NULL)
			break;


		if(pAgent ->m_bComplete  == false)
			continue;

		for(int i= 2; i<pAgent->m_NodeSize; i++)
		{

			if(pAgent->m_NodeAry == NULL)
				return;

			int link = pAgent->m_NodeAry[i-1].LinkNo;
			if( m_LinkNoMap.find(link)== m_LinkNoMap.end())
				return;  // error 

			DTALink* pLink0 = m_LinkNoMap[pAgent->m_NodeAry[i-1].LinkNo];
			DTALink* pLink1 = m_LinkNoMap[pAgent->m_NodeAry[i].LinkNo];

			if(pLink0==NULL || pLink1== NULL)
				break;

			float traveltime = pAgent->m_NodeAry[i-1].ArrivalTimeOnDSN -  max(pAgent->m_DepartureTime , pAgent->m_NodeAry[i-2].ArrivalTimeOnDSN);
			float delay = traveltime - pLink0->m_FreeFlowTravelTime ;

			if(delay <0)
				delay = 0;

			if(traveltime < pLink0->m_FreeFlowTravelTime)
				traveltime = pLink0->m_FreeFlowTravelTime;


			if(pLink0->m_FromNodeID == 1 && pLink0->m_ToNodeID == 5 && pLink1->m_ToNodeID == 3)
			{
				TRACE("delay: %f\n", delay);
			}

			DTANode* pNode = m_NodeNoMap[ pLink0->m_ToNodeNo ];

			if(pNode->m_NodeID == 36)
			{
				TRACE("");
			}

			if(pLink0 !=NULL && pLink1!=NULL)
			{
				pLink0->m_TotalVolumeForMovementCount++;
				pLink0->m_total_travel_time +=traveltime;



				//
				////TO DO: fetch timing_plan no according to day no and time period
				//DTATimingPlan timing_plan = FindTimingPlanFromStartTime( pAgent->m_NodeAry[i].ArrivalTimeOnDSN);

				//pNode->AddMovementCountAndDelay(pLink0->m_FromNodeNo, pLink1->m_ToNodeNo, delay , timing_plan);

				//{
				//	DTATimingPlan timing_plan_all_day;

				//	timing_plan_all_day.timing_plan_name = "0";

				//	timing_plan_all_day.start_time_in_min = m_DemandLoadingStartTimeInMin;
				//	timing_plan_all_day.end_time_in_min = m_DemandLoadingEndTimeInMin;

				//	pNode->AddMovementCountAndDelay(pLink0->m_FromNodeNo, pLink1->m_ToNodeNo, delay);
				//}

			}

		}
	}

	// turning percentage
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		for(unsigned int m = 0; m< (*iNode)->m_MovementVector .size(); m++)
		{
			DTANodeMovement* pMovement = &((*iNode)->m_MovementVector[m]);
			DTALink* pLink0 = m_LinkNoMap[pMovement->IncomingLinkNo  ];

			if(pLink0->m_FromNodeID == 1)
			{
				TRACE("");
			}

			int total_link_count = 0;
			for(unsigned int j = 0; j< (*iNode)->m_MovementVector .size(); j++)
			{

				if((*iNode)->m_MovementVector[j].IncomingLinkNo == pMovement->IncomingLinkNo )
				{
					total_link_count+= (*iNode)->m_MovementVector[j].sim_turn_count ;
				}

			}

			if (pMovement->sim_turn_count >=1)
			{
				pMovement->turning_percentage = 
					pMovement->sim_turn_count * 100.0f / max(1,total_link_count);

				pMovement->sim_turn_delay  = 
					pMovement->sim_turn_delay/ max(1,pMovement->sim_turn_count);

			}else
			{
				pMovement->sim_turn_delay  = 0 ;
			}
		}

	}




}
int CTLiteDoc::GetVehilePosition(DTAAgent* pAgent, double CurrentTime, int &link_sequence_no, float& ratio)
{
	GDPoint pt;
	pt.x = 0;
	pt.y = 0;

	int search_counter = 0;
	int beg, end, mid;
	beg = 1;
	end = pAgent->m_NodeSize-1;
	mid = (beg+end)/2;
	int i = mid;
	ratio = 0;
	float link_travel_time;
	float traveling_time;

	link_sequence_no = -1;

	while(beg<=end)
	{
		if(CurrentTime >= pAgent->m_NodeAry [i-1].ArrivalTimeOnDSN &&
			CurrentTime <= pAgent->m_NodeAry [i].ArrivalTimeOnDSN )	// find the link between the time interval
		{

			link_travel_time = pAgent->m_NodeAry [i].ArrivalTimeOnDSN - pAgent->m_NodeAry [i-1].ArrivalTimeOnDSN;

			traveling_time = CurrentTime - pAgent->m_NodeAry [i-1].ArrivalTimeOnDSN;

			ratio = traveling_time/link_travel_time;

			if(ratio <0)
				ratio = 0;

			if(ratio >1)
				ratio = 1;

			if(pAgent->m_NodeAry [i].LinkNo  == 457)
				TRACE("");

			link_sequence_no = i;
			return pAgent->m_NodeAry [i].LinkNo ;

			//			pt.x =  ratio*pLink->m_FromPoint .x  +  (1-ratio)*ratio*pLink->m_ToPoint .x;
			//			pt.y =  ratio*pLink->m_FromPoint .y  +  (1-ratio)*ratio*pLink->m_ToPoint .y;

		}else  // not found
		{
			if( CurrentTime>= pAgent->m_NodeAry [i].ArrivalTimeOnDSN)  // time stamp is after i
			{
				// shift to the right
				beg=mid+1;
				mid=(beg+end)/2;
				i = mid;
			}else //CurrentTime < pAgent->m_NodeAry[i].ArrivalTimeOnDSN// time stamp is before i
			{   //shift to the left
				end=mid-1;
				mid=(beg+end)/2;
				i = mid;
			}

			search_counter++;

			if(search_counter > pAgent->m_NodeSize)  // exception handling
			{
				//				ASSERT(false);
				return 0;
			}

		}
	}

	return 0;
}

bool CTLiteDoc::GetAgentPosition(string agent_id, double CurrentTime_in_min, GDPoint& pt)
{
	float ratio = 0;
	double CurrentTime = CurrentTime_in_min * 60;  // in seconds
	pt.x = 0;
	pt.y = 0;

	int search_counter = 0;
	int beg, end, mid;
	beg = 1;
	end = m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector.size() - 1;
	mid = (beg + end) / 2;
	int i = mid;
	ratio = 0;
	float link_travel_time;
	float traveling_time;

	while (beg <= end)
	{
		if (CurrentTime >= m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i - 1].time_stamp_in_second &&
			CurrentTime <= m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i].time_stamp_in_second)	// find the link between the time interval
		{

			link_travel_time = m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i].time_stamp_in_second 
				- m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i].time_stamp_in_second;

			traveling_time = CurrentTime - m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i - 1].time_stamp_in_second;

			ratio = traveling_time / link_travel_time;

			if (ratio <0)
				ratio = 0;

			if (ratio >1)
				ratio = 1;


			int LinkNo = m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i].link_no;

			if (LinkNo >= 0)
			{
				DTALink* pLink = m_LinkNoMap[LinkNo];
				pt = pLink->GetRelativePosition(ratio);
				return true;
			}
			else
			{
				
				pt.x = m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i - 1].x +

					m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i].x;
				return true;

			}
		}
		else  // not found
		{
			if (CurrentTime >= m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector[i].time_stamp_in_second)  // time stamp is after i
			{
				// shift to the right
				beg = mid + 1;
				mid = (beg + end) / 2;
				i = mid;
			}
			else //CurrentTime < pAgent->m_NodeAry[i].ArrivalTimeOnDSN// time stamp is before i
			{   //shift to the left
				end = mid - 1;
				mid = (beg + end) / 2;
				i = mid;
			}

			search_counter++;

			if (search_counter > m_AgentWithLocationVectorMap[agent_id].AgentLocationRecordVector.size())  // exception handling
			{
				//				ASSERT(false);
				return false;
			}

		}
	}

	return false;
}
bool CTLiteDoc::GetAgentPosition(DTAAgent* pAgent, double CurrentTime, GDPoint& pt)
{

	pt = pAgent->m_NodeAry[0].from_pt;


	int search_counter = 0;
	int beg, end, mid;
	beg = 1;
	end = pAgent->m_NodeSize-1;
	mid = (beg+end)/2;
	int i = mid;
	double ratio = 0;
	float link_travel_time;
	float remaining_time;

	while(beg<=end)
	{
		if(CurrentTime >= pAgent->m_NodeAry[i-1].ArrivalTimeOnDSN &&
			CurrentTime <= pAgent->m_NodeAry[i].ArrivalTimeOnDSN )	// find the link between the time interval
		{

			link_travel_time = pAgent->m_NodeAry [i].ArrivalTimeOnDSN - pAgent->m_NodeAry [i-1].ArrivalTimeOnDSN;

			remaining_time = CurrentTime - pAgent->m_NodeAry [i-1].ArrivalTimeOnDSN;

			ratio = 1-remaining_time/link_travel_time;

			if(ratio <0)
				ratio = 0;

			if(ratio >1)
				ratio = 1;

			pt.x = ratio * pAgent->m_NodeAry [i-1].from_pt.x + (1- ratio)* pAgent->m_NodeAry [i].from_pt.x ;
			pt.y = ratio * pAgent->m_NodeAry [i-1].from_pt.y + (1- ratio)* pAgent->m_NodeAry [i].from_pt.y ;

			return true ;

		}else  // not found
		{
			if( CurrentTime>= pAgent->m_NodeAry [i].ArrivalTimeOnDSN)  // time stamp is after i
			{
				// shift to the right
				beg=mid+1;
				mid=(beg+end)/2;
				i = mid;
			}else //CurrentTime < pAgent->m_NodeAry[i].ArrivalTimeOnDSN// time stamp is before i
			{   //shift to the left
				end=mid-1;
				mid=(beg+end)/2;
				i = mid;
			}

			search_counter++;

			if(search_counter > pAgent->m_NodeSize)  // exception handling
			{
				//				ASSERT(false);
				return false;
			}

		}
	}

	return false;
}




void CTLiteDoc::OnFileDataloadingstatus()
{
	CDlgFileLoading dlg;
	dlg.m_pDoc = this;
	dlg.DoModal ();
}

void CTLiteDoc::OnMoeVolume()
{
	m_LinkMOEMode = MOE_volume;
	ShowLegend(true);

	GenerateOffsetLinkBand();
	UpdateAllViews(0);}

void CTLiteDoc::OnMoeSpeed()
{
	m_LinkMOEMode = MOE_speed;
	ShowLegend(true);
	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}

void CTLiteDoc::OnMoeDensity()
{
	m_LinkMOEMode = MOE_density;
	ShowLegend(true);

	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}

void CTLiteDoc::OnMoeQueueLengthRatio()
{
	m_LinkMOEMode = MOE_QueueLengthRatio;

	GenerateOffsetLinkBand();
	UpdateAllViews(0);}
void CTLiteDoc::OnUpdateMoeVolume(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_volume);
}

void CTLiteDoc::OnUpdateMoeSpeed(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_speed);
}

void CTLiteDoc::OnUpdateMoeDensity(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_density);
}

void CTLiteDoc::OnUpdateMoeQueueLengthRatio(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_QueueLengthRatio);
}





void CTLiteDoc::OnMoeNone()
{
	m_LinkMOEMode = MOE_none;

	ShowLegend(false);

	// visualization configuration
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeNone(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_none);
}

float CTLiteDoc::GetLinkMOE(DTALink* pLink, Link_MOE LinkMOEMode,int CurrentTime, int AggregationIntervalInMin, float &value)
{

	float power = 0.0f;
	float max_link_volume = 8000.0f;
	float max_speed_ratio = 2.0f; 
	float max_density = 45.0f;

	switch (LinkMOEMode)
	{
		//case MOE_volume:  power = pLink->m_total_link_volume/max_link_volume; 
		//	value = pLink->m_total_link_volume;
		//	break;


	case MOE_volume:   
		value = pLink->m_total_link_volume;
		break;
		


	case MOE_density: power = 0;
		value = 0;
		break;
	case MOE_traveltime:  
		value = pLink->m_StaticTravelTime;
		break;

	case MOE_capacity:  
		value = pLink->m_LaneCapacity * pLink->m_NumberOfLanes ;
		break;

	case MOE_speedlimit:  
		value = pLink->m_FreeSpeed  ;
		break;

	case MOE_fftt: 
		value = pLink->m_FreeFlowTravelTime ;
		break;

	case MOE_length: 
		value = pLink->m_Length  ;
		break;

	case MOE_queue_length:
		value =  0;
		break;


	default: power = 0.0;

	}

	
		int StartTime = CurrentTime;
		int EndTime  = CurrentTime + AggregationIntervalInMin;

		float max_power = 0; 
		float min_power = 1;

		float total_value = 0;
		int total_measurement_count = 0;

		//		value = pLink->GetWithinDayLinkMOE(LinkMOEMode, m_PrimaryDataSource, CurrentTime);

		for(CurrentTime  = StartTime; CurrentTime < EndTime; CurrentTime ++)
		{


			if(pLink->IsSimulatedDataAvailable (CurrentTime) )
			{

				total_measurement_count++;

				switch (LinkMOEMode)
				{
				case MOE_volume:  

					total_value+= pLink->GetDynamicLinkVolume (CurrentTime);

					break;
				case MOE_speed: 
					total_value+= pLink->GetDynamicSpeed(CurrentTime);
					break;
				case MOE_traveltime:
					total_value+= pLink->GetDynamicTravelTime (CurrentTime);
					break;
				case MOE_density:
					total_value+= pLink->GetSimulatedDensity(CurrentTime);
					break;
				case MOE_queue_length: 
					total_value+= pLink->GetQueueLengthRatioPercentage(CurrentTime);
					break;


				}

			}
		}  // end of for loop

		//update value when there is at one valid data point
		if(total_measurement_count>=1)
		{
			value = total_value/max(1,total_measurement_count);
		}


		switch (LinkMOEMode)
		{
		case MOE_volume:  
			power = value/max_link_volume;
			break;
		case MOE_speed: 
			power =  min(100,value / pLink->m_FreeSpeed*100);
			break;
		case MOE_traveltime:
			if(value <=0.1)  // no data
				power = 0;
			else 
				power = pLink->m_FreeSpeed / max(1,value)*100; 

			break;
		case MOE_density:
			power = value; 
			break;
		case MOE_queue_length: 
			power = value  ; 
			break;



		}

		if(LinkMOEMode == MOE_speed && CurrentTime >= 1020&& pLink->m_FromNodeID == 1354 &&  pLink->m_ToNodeID == 756)
		{
			int LOS = GetLOSCode(power);
			TRACE("");
		}

	

	return power;
}

void CTLiteDoc::OnToolsCarfollowingsimulation()
{
	if(m_SelectedLinkNo==-1)
	{
		AfxMessageBox("Please select a link first.");
		return;
	}


}

DWORD CTLiteDoc::ProcessWait(DWORD PID) 
{
	DWORD dwRetVal = DWORD(-1);
	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, PID);
	if(hProcess) {
		do 
		{
			::Sleep(500);
			::GetExitCodeProcess(hProcess, &dwRetVal);
		} while(dwRetVal == STILL_ACTIVE);
		CloseHandle(hProcess);
	}
	return dwRetVal;
}

DWORD CTLiteDoc::ProcessExecute(CString & strCmd, CString & strArgs,  CString & strDir, BOOL bWait)
{

	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
	StartupInfo.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION ProcessInfo;
	DWORD dwRetVal = DWORD(-1);

	/*
	TCHAR szExe[1024];
	if( int(FindExecutable(
	strPath,									// pointer to filename
	strDir.IsEmpty() ? 0 : LPCTSTR(strDir),					// pointer to default directory
	szExe										// result-buffer
	)) <= 32 ) {
	OnExecError(GetLastError(), strCmd);
	return DWORD(-1);
	}
	*/
	BOOL bStat =	CreateProcess(
		strCmd,						// pointer to name of executable module
		0,			// pointer to command line string
		0,						// pointer to process security attributes
		0,						// pointer to thread security attributes
		TRUE,						// handle inheritance flag
		0,						// creation flags
		0,						// pointer to new environment block
		strDir.IsEmpty() ? 0 : LPCTSTR(strDir),
		&StartupInfo,				// pointer to STARTUPINFO
		&ProcessInfo				// pointer to PROCESS_INFORMATION
		);
	if( bStat ) 
	{
		if( bWait ) 
		{
			::WaitForInputIdle(ProcessInfo.hProcess, INFINITE);
			dwRetVal = ProcessWait(ProcessInfo.dwProcessId);
		} else {
			// before we return to the caller, we wait for the currently
			// started application until it is ready to work.
			::WaitForInputIdle(ProcessInfo.hProcess, INFINITE);
			dwRetVal = ProcessInfo.dwProcessId;
		}

		::CloseHandle(ProcessInfo.hThread);
	}

	return dwRetVal;
}


void CTLiteDoc::OnToolsRunSimulation()
{
	STARTUPINFO si = { 0 };  
	PROCESS_INFORMATION pi = { 0 };  

	si.cb = sizeof(si); 

	CWaitCursor curs;
	if(!SetCurrentDirectory(m_ProjectDirectory))
	{
		if(m_ProjectDirectory.GetLength()==0)
		{
			AfxMessageBox("The project directory has not been specified.");
			return;
		}
		CString msg;
		msg.Format ("The specified project directory %s does not exist.", m_ProjectDirectory);
		AfxMessageBox(msg);
		return;
	}

	if(IsModified())
		SaveProject(m_ProjectFile);


	CString sCommand;
	CString strParam;
	CTime ExeStartTime = CTime::GetCurrentTime();

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();


		sCommand.Format("%s\\%s", pMainFrame->m_CurrentDirectory,theApp.m_SimulatorString_64);

	ProcessExecute(sCommand, strParam, m_ProjectDirectory, true);


	FILE* pFile;

	int OutputAgentFileSize = 0;
	fopen_s(&pFile,m_ProjectDirectory + "\\link_performance.csv","rb");
	if(pFile!=NULL)
	{
		fseek(pFile, 0, SEEK_END );
		OutputAgentFileSize = ftell(pFile);
		fclose(pFile);
	}

	CTime ExeEndTime = CTime::GetCurrentTime();

	CTimeSpan ts = ExeEndTime  - ExeStartTime;
	CString str_running_time;

	FILE* st = NULL;

	CString directory = m_ProjectDirectory;

	if (OutputAgentFileSize >= 1 && ts.GetTotalSeconds() >= 1)
	{

		str_running_time.Format("Simulation program execution has completed.\nProgram execution time: %d hour(s) %d min(s) %d sec(s).\nDo you want to load the simulation results now?",
		ts.GetHours(), ts.GetMinutes(), ts.GetSeconds() );

				if (AfxMessageBox(str_running_time, MB_YESNO | MB_ICONINFORMATION) == IDYES)
				{
					OnToolsViewassignmentsummarylog();
				}
				LoadSimulationOutput();
			
		UpdateAllViews(0);


	}

}



void CTLiteDoc::LoadSimulationOutput()
{

	CString DTASettingsPath = m_ProjectDirectory + "Settings.txt";
	g_Simulation_Time_Horizon = 1440;
	SetStatusText("Loading output link_performance data");

	CCSVParser parser;

	bool b_link_MOE_data_flag = true; 
	b_link_MOE_data_flag = ReadSimulationLinkMOEData_Parser(m_ProjectDirectory + "link_performance.csv");

	ReadAgentCSVFile_Parser(m_ProjectDirectory+ "agent.csv");
	ReadAgentTrajectory(m_ProjectDirectory + "input_agent.csv");
	//RecalculateLinkMOEFromAgentTrajectoryFile();

	int speed_data_aggregation_interval = 15;

	
	SetStatusText("Generating OD statistics...");

	ResetODMOEMatrix();
}


void CTLiteDoc::OnMoeVcRatio()
{
	m_LinkMOEMode = MOE_density;
	ShowLegend(true);
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeVcRatio(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_density);
}

void CTLiteDoc::OnMoeTraveltime()
{
	m_LinkMOEMode = MOE_traveltime;

	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeTraveltime(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_traveltime);
}

void CTLiteDoc::OnMoeCapacity()
{
	m_LinkMOEMode = MOE_capacity;
	ShowTextLabel();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeCapacity(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_capacity);
}

void CTLiteDoc::OnMoeSpeedlimit()
{
	m_LinkMOEMode = MOE_speedlimit;
	ShowTextLabel();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeSpeedlimit(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_speedlimit);
}

void CTLiteDoc::OnMoeFreeflowtravletime()
{
	m_LinkMOEMode = MOE_fftt;
	ShowTextLabel();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeFreeflowtravletime(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_fftt);
}


CString CTLiteDoc::GetLocalFileName(CString strFullPath)
{
	int len = strFullPath.GetLength();
	/////////////////////////////////

	CString	strPathTitleName;

	strPathTitleName.Empty ();

	for(int k=len-1;k>=0;k--)
	{
		if(strFullPath[k]=='\\')
			break;

			strPathTitleName+=strFullPath[k];

	}

	strPathTitleName.MakeReverse();

	return strPathTitleName;

}

CString CTLiteDoc::GetWorkspaceTitleName(CString strFullPath)
{
	int len = strFullPath.GetLength();
	/////////////////////////////////

	CString	strPathTitleName;

	strPathTitleName.Empty ();
	bool StartFlag = false;

	int count = 0;

	for(int k=len-1;k>=0;k--)
	{
		if (strFullPath[k] == '\\')
		{
			count++;

			if(count == 2)
			break;
		}

		if(strFullPath[k]=='\\' && StartFlag == false)
		{
			StartFlag = true;
			continue;
		}

		if(StartFlag == true)
			strPathTitleName+=strFullPath[k];

	}

	strPathTitleName.MakeReverse();

	return strPathTitleName;

}
void CTLiteDoc::OnEditDeleteselectedlink()
{

	// TODO: Add your command handler code here
}


void CTLiteDoc::OnMoeLength()
{
	m_LinkMOEMode = MOE_length;
	ShowTextLabel();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeLength(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_length);
}

void CTLiteDoc::OnEditSetdefaultlinkpropertiesfornewlinks()
{

	
}

void CTLiteDoc::OnUpdateEditSetdefaultlinkpropertiesfornewlinks(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

void CTLiteDoc::OnToolsProjectfolder()
{
	if(m_ProjectDirectory.GetLength()==0)
	{
		AfxMessageBox("The project directory has not been specified.");
		return;
	}

	ShellExecute( NULL,  "explore", m_ProjectDirectory, NULL,  NULL, SW_SHOWNORMAL );
}

void CTLiteDoc::OnToolsOpennextaprogramfolder()
{
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	ShellExecute( NULL,  "explore", pMainFrame->m_CurrentDirectory, NULL,  NULL, SW_SHOWNORMAL );
}

void CTLiteDoc::OnMoeNoodmoe()
{
	m_ODMOEMode = odnone;
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeNoodmoe(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_ODMOEMode == MOE_none);
}


void CTLiteDoc::OnOdtableImportOdTripFile()
{

}

void CTLiteDoc::OnToolsEditassignmentsettings()
{
}


void CTLiteDoc::OnSearchLinklist()
{

	g_bShowLinkList = !g_bShowLinkList;

	if(g_bShowLinkList)
	{
		if(g_pLinkListDlg==NULL)
		{
			g_pLinkListDlg = new CDlgLinkList();
			g_pLinkListDlg->Create(IDD_DIALOG_LINK_LIST);
		}

		// update using pointer to the active document; 

		if(g_pLinkListDlg->GetSafeHwnd())
		{
			// we udpate the pointer list for document every time we open this link list window
			std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
			while (iDoc != g_DocumentList.end())
			{
				if ((*iDoc)->m_NodeSet.size()>0)
				{

					if(g_pLinkListDlg->m_pDoc ==NULL)
						g_pLinkListDlg->m_pDoc = (*iDoc);
					else if((*iDoc)!= g_pLinkListDlg->m_pDoc)
					{
						g_pLinkListDlg->m_pDoc2 = (*iDoc);
						g_pLinkListDlg->m_bDoc2Ready = true;
					}

				}
				iDoc++;

			}

			g_pLinkListDlg->ReloadData ();
			g_pLinkListDlg->ShowWindow(SW_HIDE);
			g_pLinkListDlg->ShowWindow(SW_SHOW);
		}
	}else
	{
		if(g_pLinkListDlg!=NULL && g_pLinkListDlg->GetSafeHwnd())
		{
			g_pLinkListDlg->ShowWindow(SW_HIDE);
		}
	}

}

void CTLiteDoc::OnMoeAgent()
{
	m_LinkMOEMode = MOE_Agent;
	GenerateOffsetLinkBand();
	ShowLegend(false);

	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateMoeAgent(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_Agent);
}
LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
	HKEY hkey;
	LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

	if (retval == ERROR_SUCCESS) {
		long datasize = MAX_PATH;
		TCHAR data[MAX_PATH];
		RegQueryValue(hkey, NULL, data, &datasize);
		lstrcpy(retdata,data);
		RegCloseKey(hkey);
	}

	return retval;
}
HINSTANCE g_OpenDocument(LPCTSTR url, int showcmd)
{
	TCHAR key[MAX_PATH + MAX_PATH];

	// First try ShellExecute()
	HINSTANCE result = ShellExecute(NULL, _T("open"), url, NULL,NULL, showcmd);

	// If it failed, get the .htm regkey and lookup the program
	if ((UINT)result <= HINSTANCE_ERROR) {

		if (GetRegKey(HKEY_CLASSES_ROOT, _T(".csv"), key) == ERROR_SUCCESS) {
			lstrcat(key, _T("\\shell\\open\\command"));

			if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS) {
				TCHAR *pos;
				pos = _tcsstr(key, _T("\"%1\""));
				if (pos == NULL) {			   // No quotes found
					pos = strstr(key, _T("%1"));	   // Check for %1, without quotes
					if (pos == NULL)			   // No parameter at all...
						pos = key+lstrlen(key)-1;
					else
						*pos = '\0';			 // Remove the parameter
				}
				else
					*pos = '\0';			  // Remove the parameter

				lstrcat(pos, _T(" "));
				lstrcat(pos, url);
				result = (HINSTANCE) WinExec(key,showcmd);
			}
		}
	}

	return result;
}

HINSTANCE g_GotoURL(LPCTSTR url, int showcmd)
{
	TCHAR key[MAX_PATH + MAX_PATH];

	// First try ShellExecute()
	HINSTANCE result = ShellExecute(NULL, _T("open"), url, NULL,NULL, showcmd);

	// If it failed, get the .htm regkey and lookup the program
	if ((UINT)result <= HINSTANCE_ERROR) {

		if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS) {
			lstrcat(key, _T("\\shell\\open\\command"));

			if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS) {
				TCHAR *pos;
				pos = _tcsstr(key, _T("\"%1\""));
				if (pos == NULL) {			   // No quotes found
					pos = strstr(key, _T("%1"));	   // Check for %1, without quotes
					if (pos == NULL)			   // No parameter at all...
						pos = key+lstrlen(key)-1;
					else
						*pos = '\0';			 // Remove the parameter
				}
				else
					*pos = '\0';			  // Remove the parameter

				lstrcat(pos, _T(" "));
				lstrcat(pos, url);
				result = (HINSTANCE) WinExec(key,showcmd);
			}
		}
	}

	return result;
}

void CTLiteDoc::OnToolsViewsimulationsummary()
{
	CDlg_AgentClassification* m_pDlg = new CDlg_AgentClassification; 

	g_SummaryDialogVector.push_back (m_pDlg);  // collect memory block
	m_pDlg->m_pDoc = this;
	m_pDlg->SetModelessFlag(true); // voila! this is all it takes to make your dlg modeless!
	m_pDlg->Create(IDD_DIALOG_Summary); 
	m_pDlg->ShowWindow(SW_SHOW); 

	m_bSummaryDialog = true;
}


void CTLiteDoc::OnToolsViewassignmentsummarylog()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"log.csv");
}

void CTLiteDoc::OnHelpVisitdevelopmentwebsite()
{
	g_OpenDocument("https://github.com/zephyr-data-specs/GMNS/", SW_SHOW);
}

bool CTLiteDoc::CheckControlData()
{
	return true;
	std::vector <int> NodeVector;
	std::list<DTANode*>::iterator iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{

		if((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal)
		{

	
		}
	}

	if(NodeVector.size()>0 && m_signal_reresentation_model >=1)
	{

		CString message;
		message.Format ("There are %d signalized nodes with zero cycle length (e.g. %d).\n Do you want to continue?\nDTALite will use a continuous flow model with link capacity constraints for those nodes.", NodeVector.size(),NodeVector[0]);

		if(AfxMessageBox(message,MB_YESNO|MB_ICONINFORMATION)==IDYES)
		{

			return true;
		}


		return false;
	}

	return true;

}




void CTLiteDoc::ResetBackgroundImageCoordinate()
{
	m_LongLatFlag = true;

	float m_XScale = 1;
	float m_YScale = 1;

	if(fabs(m_PointB_x- m_PointA_x)>0.000001)
	{
		m_XScale = (m_PointB_long - m_PointA_long)/(m_PointB_x- m_PointA_x);
	}else
	{

		AfxMessageBox("Horizontal distance between two points is too short. Please redefine two points.");
		return;
	}

	if(fabs(m_PointB_y- m_PointA_y)>0.000001)
	{
		m_YScale = (m_PointB_lat - m_PointA_lat)/(m_PointB_y- m_PointA_y);
	}else
	{

		AfxMessageBox("Vertical distance between two points is too short. Please redefine two points.");
		return;
	}

	float m_XOrigin = m_PointA_x - m_PointA_long /m_XScale;

	float m_YOrigin = m_PointA_y- m_PointA_lat /m_YScale;


	/////////////////////////////////adjustment


	// adjust node coordinates
	std::list<DTANode*>::iterator iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{

		(*iNode)->pt .x  = ((*iNode)->pt .x - m_XOrigin)*m_XScale;
		(*iNode)->pt .y  = ((*iNode)->pt .y - m_YOrigin)*m_YScale;
	}
	//adjust link cooridnates

	std::list<DTALink*>::iterator iLink;

	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		(*iLink)->m_FromPoint.x = ((*iLink)->m_FromPoint.x -m_XOrigin)*m_XScale;
		(*iLink)->m_FromPoint.y = ((*iLink)->m_FromPoint.y -m_YOrigin)*m_YScale;

		(*iLink)->m_ToPoint.x = ((*iLink)->m_ToPoint.x -m_XOrigin)*m_XScale;
		(*iLink)->m_ToPoint.y = ((*iLink)->m_ToPoint.y -m_YOrigin)*m_YScale;


		for(unsigned int si = 0; si< (*iLink)->m_Original_ShapePoints.size(); si++)
		{

			(*iLink)->m_Original_ShapePoints[si].x = ((*iLink)->m_Original_ShapePoints[si].x - m_XOrigin)*m_XScale;
			(*iLink)->m_Original_ShapePoints[si].y = ((*iLink)->m_Original_ShapePoints[si].y - m_YOrigin)*m_YScale;

		}

		for(unsigned int si = 0; si< (*iLink)->m_ShapePoints.size(); si++)
		{

			(*iLink)->m_ShapePoints[si].x = ((*iLink)->m_ShapePoints[si].x - m_XOrigin)*m_XScale;
			(*iLink)->m_ShapePoints[si].y = ((*iLink)->m_ShapePoints[si].y - m_YOrigin)*m_YScale;

		}

	}

	// zone layer
	std::map<int, DTAZone>	:: iterator itr;

	for(itr = m_ZoneMap.begin(); itr != m_ZoneMap.end(); ++itr)
	{
		for(unsigned int si = 0; si< itr->second.m_ShapePoints.size(); si++)
		{
			itr->second.m_ShapePoints[si].x = (itr->second.m_ShapePoints[si].x - m_XOrigin)*m_XScale;
			itr->second.m_ShapePoints[si].y = (itr->second.m_ShapePoints[si].y - m_YOrigin)*m_YScale;

		}

	}

	// image layer

	m_ImageX1 = (m_ImageX1- m_XOrigin)*m_XScale;
	m_ImageY1 = (m_ImageY1- m_YOrigin)*m_YScale;
	m_ImageX2 = (m_ImageX2- m_XOrigin)*m_XScale;
	m_ImageY2 =  (m_ImageY2- m_YOrigin)*m_YScale;

	m_ImageWidth = fabs(m_ImageX2 - m_ImageX1);
	m_ImageHeight = fabs(m_ImageY2 - m_ImageY1);

	GDPoint pt1, pt2;
	pt1.x = m_ImageX1;
	pt1.y = m_ImageY1;

	pt2.x = m_ImageX2;
	pt2.y = m_ImageY2;

	// update image width in miles
	m_ImageWidthInMile = max(m_ImageWidthInMile,g_CalculateP2PDistanceInMileFromLatitudeLongitude(pt1,pt2));

	m_ImageMoveSize = m_ImageWidth/2000.0f;

	CString str_result;
	str_result.Format ("The coordinates has been adjusted to long/lat format.\nNEXTA will reload this project to reflect changed coordinates.");
	if(AfxMessageBox(str_result, MB_ICONINFORMATION)==IDOK)
	{
		SaveProject(m_ProjectFile);  // save time-dependent MOE to input_link MOE file

		OnOpenDocument(m_ProjectFile);
	}

	m_bFitNetworkInitialized = false;
	CalculateDrawingRectangle(false);
	UpdateAllViews(0);
}

void CTLiteDoc::OpenWarningLogFile(CString directory)
{
	m_NEXTALOGFile.open (directory+"NeXTA.log", ios::out);
	if (m_NEXTALOGFile.is_open())
	{
		m_NEXTALOGFile.width(12);
		m_NEXTALOGFile.precision(3) ;
		m_NEXTALOGFile.setf(ios::fixed);
	}else
	{
		AfxMessageBox("File NeXTA.log cannot be opened, and it might be locked by another program!");
	}
}


void CTLiteDoc::OnMoeViewmoes()
{
	// TODO: Add your command handler code here

	CDlgMOETabView dlg;
	dlg.m_pDoc = this;
	dlg.DoModal();
}

void CTLiteDoc::OnImportdataImportExcelFile()
{

}

void CTLiteDoc::OnImportdataImport()
{

}


void CTLiteDoc::AdjustCoordinateUnitToMile()
{

	m_AdjLinkSize +=2;  // add two more elements to be safe

	std::list<DTANode*>::iterator iNode;
	//adjust XY coordinates if the corrdinate system is not consistenty
	if(fabs(m_UnitDistance-1.00)>0.10)  // ask users if we need to adjust the XY coordinates
	{
		if(AfxMessageBox("The link length information in link.csv is not consistent with the X/Y coordinates in node.csv.\n Do you want to adjust the the X/Y coordinate unit to mile in node.csv?", MB_YESNO) == IDYES)

			for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
			{
				(*iNode)->pt.x = ((*iNode)->pt.x - min(m_NetworkRect.left,m_NetworkRect.right))*m_UnitDistance;
				(*iNode)->pt.y = ((*iNode)->pt.y - min(m_NetworkRect.top, m_NetworkRect.bottom))*m_UnitDistance;
			}

			std::list<DTALink*>::iterator iLink;
			for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
			{
				(*iLink)->m_FromPoint = m_NodeNoMap[(*iLink)->m_FromNodeNo]->pt;
				(*iLink)->m_ToPoint = m_NodeNoMap[(*iLink)->m_ToNodeNo]->pt;
			}

			//if(m_LongLatCoordinateFlag)
			//	m_UnitDistance = m_UnitDistance/62/1000.0f;  // 62 is 1 long = 62 miles

			CalculateDrawingRectangle();
			UpdateAllViews(0);

	}
}



void CTLiteDoc::OnMoeAgentpathanalaysis()
{
	if(g_bValidDocumentChanged && g_pAgentPathDlg!=NULL)  // either a new document is added or an old document is delete. 
	{
		//	delete g_pAgentPathDlg; potential memory leak, but operator delete is problematic for gridlist
		g_pAgentPathDlg= NULL;
		g_bValidDocumentChanged = false;
	}

	g_bShowAgentPathDialog = !g_bShowAgentPathDialog;

	if(g_bShowAgentPathDialog)
	{
		if(g_pAgentPathDlg==NULL)
		{
			g_pAgentPathDlg = new CDlg_VehPathAnalysis();
			g_pAgentPathDlg->m_pDoc = this;
			g_pAgentPathDlg->Create(IDD_DIALOG_Agent_PATH);
		}

		// update using pointer to the active document; 

		if(g_pAgentPathDlg->GetSafeHwnd())
		{
			// we udpate the pointer list for document every time we open this link list window
			g_pAgentPathDlg->m_pDoc = this;
			g_pAgentPathDlg->ShowWindow(SW_HIDE);
			g_pAgentPathDlg->ShowWindow(SW_SHOW);
			g_pAgentPathDlg->FilterOriginDestinationPairs();
		}
	}else
	{
		if(g_pAgentPathDlg!=NULL && g_pAgentPathDlg->GetSafeHwnd())
		{
			g_pAgentPathDlg->ShowWindow(SW_HIDE);
		}
	}

}

void CTLiteDoc::HighlightPath(	std::vector<int>	m_LinkVector, int DisplayID = 1)
{
	for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		(*iLink)->m_DisplayLinkID = -1;
	}

	for (unsigned int l = 0; l < m_LinkVector.size(); l++)
	{
		DTALink* pLink = FindLinkWithLinkNo (m_LinkVector[l]);
		if(pLink!=NULL)
		{
			pLink->m_DisplayLinkID = DisplayID;
		}

	}
	UpdateAllViews(0);
}



void CTLiteDoc::HighlightSelectedAgents(bool bSelectionFlag)
{
	CWaitCursor wc;

	for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		(*iLink)->m_DisplayLinkID = -1;
		(*iLink)->m_NumberOfMarkedAgents++;
	}

	if(bSelectionFlag) // selection
	{
		std::list<DTAAgent*>::iterator iAgent;

		bool bTraceFlag = true;

		for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++)
		{
			DTAAgent* pAgent = (*iAgent);
			if(pAgent->m_bMarked)
			{
				for(int link= 1; link<pAgent->m_NodeSize; link++)
				{
					if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end())
					{
						m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo ]->m_DisplayLinkID = 1;
						m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo ]->m_NumberOfMarkedAgents++;
					}
				}
			}		
		}
	} // de-selection: do nothing
	UpdateAllViews(0);

}








void CTLiteDoc::OnImportAgentFile()
{
	// TODO: Add your command handler code here
}

void CTLiteDoc::OnImportNgsimFile()
{
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	pMainFrame->OnShowTimetable();

}

void CTLiteDoc::OpenCSVFileInExcel(CString filename)
{

	HINSTANCE result = ShellExecute(NULL, _T("open"), filename, NULL, NULL, SW_SHOW);

}

bool CTLiteDoc::WriteSubareaFiles()
{
	
	return true;
}






int CTLiteDoc::ReadWorkZoneScenarioData(int RemoveLinkFromNodeID, int RemoveLinkToNodeID)
{

	int i = 0;

	CString workzone_file = m_ProjectDirectory+"Scenario_Work_Zone.csv";
	int error_count  = 0; 
	FILE* st =NULL;
	fopen_s(&st,workzone_file,"r");
	if(st!=NULL)
	{
		while(true)
		{
			int usn  = g_read_integer(st);
			if(usn == -1)
				break;

			int dsn =  g_read_integer(st);

			DTALink* plink = FindLinkWithNodeIDs(usn,dsn,workzone_file );

			if(plink!=NULL)
			{
				CapacityReduction cs;
				cs.bWorkzone  = true; 

				cs.ScenarioNo =  g_read_integer(st); 
				cs.StartDayNo  = g_read_integer(st);
				cs.EndDayNo	   = g_read_integer(st);
				cs.StartTime = g_read_float(st);
				cs.EndTime = g_read_float(st);
				cs.LaneClosurePercentage= g_read_float(st);
				cs.SpeedLimit = g_read_float(st);

				if(RemoveLinkFromNodeID==usn && RemoveLinkToNodeID== dsn)
				{
					//skip
				}else
				{
					plink->CapacityReductionVector.push_back(cs);
					i++;
				}
			}else
			{
				error_count ++;
				if(error_count >= 3) break; // no more than 3 warnings

			}
		}
		fclose(st);
	}

	return i;
}


void CTLiteDoc::OnEditOffsetlinks()
{
	m_bLinkToBeShifted = true;
	OffsetLink();
	UpdateAllViews(0);
}



void CTLiteDoc::ChangeNetworkCoordinates(int LayerNo, float XScale, float YScale, float delta_x, float delta_y)
{
	GDRect NetworkRect;

	bool bRectInitialized = false;


		for (std::list<DTANode*>::iterator iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{
			if ((*iNode)->m_Connections > 0)   // for selected layer only
			{
				if (!bRectInitialized)
				{
					NetworkRect.left = (*iNode)->pt.x;
					NetworkRect.right = (*iNode)->pt.x;
					NetworkRect.top = (*iNode)->pt.y;
					NetworkRect.bottom = (*iNode)->pt.y;
					bRectInitialized = true;
				}

				NetworkRect.Expand((*iNode)->pt);
			}

		}

		float m_XOrigin = NetworkRect.Center().x;

		float m_YOrigin = NetworkRect.Center().y;

		// adjust node coordinates
		std::list<DTANode*>::iterator iNode;
		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			if ((*iNode)->m_LayerNo == LayerNo)  // for selected layer only
			{
				(*iNode)->pt.x = ((*iNode)->pt.x - m_XOrigin)*XScale + m_XOrigin + delta_x;
				(*iNode)->pt.y = ((*iNode)->pt.y - m_YOrigin)*YScale + m_YOrigin + delta_y;
			}
		}

		//adjust link cooridnates

		std::list<DTALink*>::iterator iLink;

		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			if ((*iLink)->m_LayerNo == LayerNo)   // for selected layer only
			{

				(*iLink)->m_FromPoint.x = ((*iLink)->m_FromPoint.x - m_XOrigin)*XScale + m_XOrigin + delta_x;
				(*iLink)->m_FromPoint.y = ((*iLink)->m_FromPoint.y - m_YOrigin)*YScale + m_YOrigin + delta_y;

				(*iLink)->m_ToPoint.x = ((*iLink)->m_ToPoint.x - m_XOrigin)*XScale + m_XOrigin + delta_x;
				(*iLink)->m_ToPoint.y = ((*iLink)->m_ToPoint.y - m_YOrigin)*YScale + m_YOrigin + delta_y;

				for (unsigned int si = 0; si < (*iLink)->m_ShapePoints.size(); si++)
				{

					(*iLink)->m_ShapePoints[si].x = ((*iLink)->m_ShapePoints[si].x - m_XOrigin)*XScale + m_XOrigin + delta_x;
					(*iLink)->m_ShapePoints[si].y = ((*iLink)->m_ShapePoints[si].y - m_YOrigin)*YScale + m_YOrigin + delta_y;

				}

				for (unsigned int si = 0; si < (*iLink)->m_BandLeftShapePoints.size(); si++)
				{

					(*iLink)->m_BandLeftShapePoints[si].x = ((*iLink)->m_BandLeftShapePoints[si].x - m_XOrigin)*XScale + m_XOrigin + delta_x;
					(*iLink)->m_BandLeftShapePoints[si].y = ((*iLink)->m_BandLeftShapePoints[si].y - m_YOrigin)*YScale + m_YOrigin + delta_y;

				}

				for (unsigned int si = 0; si < (*iLink)->m_BandRightShapePoints.size(); si++)
				{

					(*iLink)->m_BandRightShapePoints[si].x = ((*iLink)->m_BandRightShapePoints[si].x - m_XOrigin)*XScale + m_XOrigin + delta_x;
					(*iLink)->m_BandRightShapePoints[si].y = ((*iLink)->m_BandRightShapePoints[si].y - m_YOrigin)*YScale + m_YOrigin + delta_y;

				}


			}


		}
	if (LayerNo == 1) //reference layer
	{
		std::list<DTALine*>::iterator iLine;
		for (iLine = m_DTALineSet.begin(); iLine != m_DTALineSet.end(); iLine++)
		{

			for (unsigned int si = 0; si < (*iLine)->m_ShapePoints.size(); si++)
			{

				(*iLine)->m_ShapePoints[si].x = ((*iLine)->m_ShapePoints[si].x - m_XOrigin)*XScale + m_XOrigin + delta_x;
				(*iLine)->m_ShapePoints[si].y = ((*iLine)->m_ShapePoints[si].y - m_YOrigin)*YScale + m_YOrigin + delta_y;

			}


		}

	}

		
}
void CTLiteDoc::OnFileOpenNetworkOnly()
{
	static char BASED_CODE szFilter[] = "NEXTA Data Files (*.dws;*.tnp)|*.dws; *.tnp|Transportation Network Projects (*.tnp)|*.tnp|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

	if(dlg.DoModal() == IDOK)
	{
		CWaitCursor wait;

		CString ProjectFileName = dlg.GetPathName ();

		if(ProjectFileName.Find("tnp")>=0)  //Transportation network project format
		{
			OnOpenTrafficNetworkDocument(ProjectFileName,true);
		}
		CDlgFileLoading dlg;
		dlg.m_pDoc = this;
		dlg.DoModal ();

		UpdateAllViews(0);

	}

}

void CTLiteDoc::OnLinkAddlink()
{
	if(m_SelectedLinkNo==-1)
	{
		AfxMessageBox("Please select a link first.");
		return;
	}

	DTALink* pLink= m_LinkNoMap [m_SelectedLinkNo];
	if(pLink!=NULL)
	{

		UpdateAllViews(0);
	}
}


CString CTLiteDoc::GetTimeStampStrFromIntervalNo(int time_interval, bool with_single_quote)
{
	CString str;
	int hour = time_interval/4;
	int min = (time_interval - hour*4)*15;

	if(with_single_quote)
	{
		if(hour<10)
			str.Format ("'0%d:%02d",hour,min);
		else
			str.Format ("'%2d:%02d",hour,min);
	}else  //without typewriter single quotes
	{
		if(hour<10)
			str.Format ("0%d:%02d",hour,min);
		else
			str.Format ("%2d:%02d",hour,min);
	}


	return str;

}


CString CTLiteDoc::GetTimeStampFloatingPointStrFromIntervalNo(int time_interval)
{
	CString str;
	int hour = time_interval/4;
	int min = (time_interval - hour*4)*15;

	str.Format ("%dh%02d",hour,min);

	return str;

}

CString CTLiteDoc::GetTimeStampString(int time_stamp_in_min)
{
	CString str;
	int hour = time_stamp_in_min/60;
	int min = time_stamp_in_min - hour*60;

	if(hour<12)
		str.Format ("%02d:%02d AM",hour,min);
	else if(hour==12)
		str.Format ("%02d:%02d PM",12,min);
	else
		str.Format ("%02d:%02d PM",hour-12,min);

	return str;

}


int CTLiteDoc::GetTimeStampfromString(CString str)
{

	int time_stamp_in_min = 0;
	//char m_Text[100];

	//sscanf_s(m_Text, "%d,%s", &LinkType, LinkTypeString, sizeof(LinkTypeString));
	return time_stamp_in_min;

}

CString CTLiteDoc::GetTimeStampString24HourFormat(int time_stamp_in_min)
{
	CString str;
	int hour = time_stamp_in_min/60;
	int min = time_stamp_in_min - hour*60;

	str.Format ("%2d:%02d",hour,min);

	return str;

}
void CTLiteDoc::OnProjectEdittime()
{
}

int CTLiteDoc::FindClassificationNo(DTAAgent* pAgent, Agent_X_CLASSIFICATION x_classfication)
{

	int index = -1;  // no classification

	switch(x_classfication)
	{
	case CLS_all_Agents: index =0 ; break;
	case CLS_agent_type: index = pAgent->m_AgentTypeNo; break;

	case CLS_time_interval_5_min: index = pAgent->m_DepartureTime / 5;

		if (m_AgentSelectionMode == CLS_path_partial_trip)
			index = pAgent->m_path_start_node_departure_time / 5;

		break;
	case CLS_time_interval_15_min: index = pAgent->m_DepartureTime /15; 
		
		if (m_AgentSelectionMode == CLS_path_partial_trip)
			index = pAgent->m_path_start_node_departure_time / 15;

		break;
	case CLS_time_interval_30_min: index = pAgent->m_DepartureTime /30; 
		if (m_AgentSelectionMode == CLS_path_partial_trip)
			index = pAgent->m_path_start_node_departure_time / 30;
		break;
	case CLS_time_interval_60_min: index = pAgent->m_DepartureTime /60; 
		if (m_AgentSelectionMode == CLS_path_partial_trip)
			index = pAgent->m_path_start_node_departure_time / 60;
		break;
	case CLS_time_interval_2_hour: index = pAgent->m_DepartureTime /120;
		if (m_AgentSelectionMode == CLS_path_partial_trip)
			index = pAgent->m_path_start_node_departure_time / 120;
		break;
	case CLS_time_interval_4_hour: index = pAgent->m_DepartureTime /240; 
		if (m_AgentSelectionMode == CLS_path_partial_trip)
			index = pAgent->m_path_start_node_departure_time / 240;
		break;

	case CLS_distance_bin_0_2: index = pAgent->m_Distance /0.2; break;
	case CLS_distance_bin_1: index = pAgent->m_Distance /1; break;

	case CLS_distance_bin_2: index = pAgent->m_Distance /2; break;
	case CLS_distance_bin_5: index = pAgent->m_Distance /5; break;
	case CLS_distance_bin_10: index = pAgent->m_Distance /10; break;
	case CLS_travel_time_bin_2: index = pAgent->m_TripTime /2; break;
	case CLS_travel_time_bin_5: index = pAgent->m_TripTime /5; break;
	case CLS_travel_time_bin_10: index = pAgent->m_TripTime /10; break;
	case CLS_travel_time_bin_30: index = pAgent->m_TripTime /30; break;
	default: 
		TRACE("ERROR. No classification available!");
	};

	return index;
}


CString CTLiteDoc::FindClassificationLabel(Agent_X_CLASSIFICATION x_classfication, int index)
{

	CString label;

	switch(x_classfication)
	{
	case CLS_agent_type: 
		label = GetAgentTypeStr(index);
		break;

	case CLS_time_interval_5_min:
		label.Format("%s-%s", GetTimeStampStrFromIntervalNo(index, false), GetTimeStampStrFromIntervalNo((index + 1), false));
		break;
	case CLS_time_interval_15_min: 
		label.Format ("%s-%s",GetTimeStampStrFromIntervalNo(index,false),GetTimeStampStrFromIntervalNo((index+1),false));
		break;

	case CLS_time_interval_30_min: 
		label.Format ("%s-%s",GetTimeStampStrFromIntervalNo(index*2,false),GetTimeStampStrFromIntervalNo((index+1)*2,false));
		break;

	case CLS_time_interval_60_min: 
		label.Format ("%s-%s",GetTimeStampStrFromIntervalNo(index*4,false),GetTimeStampStrFromIntervalNo((index+1)*4,false));
		break;

	case CLS_time_interval_2_hour: 
		label.Format ("%s-%s",GetTimeStampStrFromIntervalNo(index*8,false),GetTimeStampStrFromIntervalNo((index+1)*8,false));
		break;

	case CLS_time_interval_4_hour: 
		label.Format ("%s-%s",GetTimeStampStrFromIntervalNo(index*16,false),GetTimeStampStrFromIntervalNo((index+1)*16,false));
		break;

	case CLS_distance_bin_0_2: 
		label.Format ("%.1f-%.1f",index*0.2,(index+1)*0.2);
		break;
	case CLS_distance_bin_1: 
		label.Format ("%d-%d",index*1,(index+1)*1);
		break;
	case CLS_distance_bin_2: 
		label.Format ("%d-%d",index*2,(index+1)*2);
		break;

	case CLS_distance_bin_5: 
		label.Format ("%d-%d",index*5,(index+1)*5);
		break;
	case CLS_distance_bin_10: 
		label.Format ("%d-%d",index*10,(index+1)*10);
		break;
	case CLS_travel_time_bin_2: 
		label.Format ("%d-%d",index*2,(index+1)*2);
		break;
	case CLS_travel_time_bin_5: 
		label.Format ("%d-%d",index*5,(index+1)*5);
		break;
	case CLS_travel_time_bin_10: 
		label.Format ("%d-%d",index*10,(index+1)*10);
		break;
	case CLS_travel_time_bin_30: 
		label.Format ("%d-%d",index*30,(index+1)*30);
		break;

	default: 
		TRACE("ERROR. No classification available!");
	};

	return label;
}

bool CTLiteDoc::SelectAgentForAnalysis(DTAAgent* pAgent, Agent_CLASSIFICATION_SELECTION Agent_selection)
{
	if(!pAgent->m_bComplete)
		return false;

	if(Agent_selection == CLS_network)
		return  true;  // all the Agents

	if(Agent_selection == CLS_OD)
		return  pAgent->m_bODMarked;  // marked by Agent path dialog


	if(Agent_selection == CLS_link_set)
	{
		for(int link= 1; link<pAgent->m_NodeSize; link++)
		{

			if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_DisplayLinkID>=0)  // in one of selected links
			{
				return true;		
			}
		}
		return false;
	}

	if(Agent_selection == CLS_path_trip)
	{
		if(m_PathDisplayList.size() == 0)
			return false;

		if(m_SelectPathNo >= m_PathDisplayList.size())
			return false;

		int count_of_links_in_selected_path = 0;

		for(int link= 1; link<pAgent->m_NodeSize; link++)
		{
			if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end() &&  m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_bIncludedBySelectedPath )
			{
				count_of_links_in_selected_path++;	
			}
		}

		if(count_of_links_in_selected_path == m_PathDisplayList[m_SelectPathNo].m_LinkVector.size() && m_PathDisplayList[m_SelectPathNo].m_LinkVector.size() >0)
			return true;
		else
			return false;
	}

	if(Agent_selection == CLS_path_partial_trip)
	{
		if(m_PathDisplayList.size() == 0)
			return false;

		if(m_SelectPathNo >= m_PathDisplayList.size())
			return false;

		int count_of_links_in_selected_path = 0;

		for(int link= 1; link<pAgent->m_NodeSize; link++)
		{
			if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end() &&  m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_bIncludedBySelectedPath )
			{
				count_of_links_in_selected_path++;	
			}
		}

		if(count_of_links_in_selected_path == m_PathDisplayList[m_SelectPathNo].m_LinkVector.size() && m_PathDisplayList[m_SelectPathNo].m_LinkVector.size() >0)
		{
			//update subtrip travel time

			float subtrip_distance = 0;
			float subtrip_free_flow_travel_time = 0;
			bool b_distance_counting_flag = false;
			for(int link= 1; link<pAgent->m_NodeSize; link++)
			{

				if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end() &&  m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_bFirstPathLink  )
				{
					pAgent->m_path_start_node_departure_time = pAgent->m_NodeAry[link-1].ArrivalTimeOnDSN;
					b_distance_counting_flag = true;
				}

				if(b_distance_counting_flag)  // count from the first link to the last link
				{
					subtrip_distance += m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo] ->m_Length ;
					subtrip_free_flow_travel_time += m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo] ->m_FreeFlowTravelTime  ;
				}


				if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end() &&  m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_bLastPathLink  )
				{
					pAgent->m_path_end_node_arrival_time  = pAgent->m_NodeAry[link].ArrivalTimeOnDSN;

					pAgent->m_path_travel_time = pAgent->m_path_end_node_arrival_time - pAgent->m_path_start_node_departure_time;

					b_distance_counting_flag = false;

					break;

				}


			}

			pAgent->m_path_distance  = subtrip_distance;
			pAgent->m_path_free_flow_travel_time = subtrip_free_flow_travel_time;

			return true;
		}
		else
			return false;
	}

	if(Agent_selection == CLS_subarea_generated)
	{
		// if the first link of a Agent is marked, then return true

		if(pAgent->m_NodeSize>2)
		{

			for(int link= 1; link< pAgent->m_NodeSize; link++)
			{

				if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end())  // first  valide link
				{
					if(m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_bIncludedinSubarea == true)
						return true;
					else
						return false;
				}

			}

		}
		return false;
	}

	if(Agent_selection == CLS_subarea_traversing_through)
	{

		if(pAgent->m_NodeSize<3)
			return false;
		// now pAgent->m_NodeSize>=3;

		// condition 1: if the first link of a Agent is marked (in the subarea), then return false
		if(m_LinkNoMap.find(pAgent->m_NodeAry[1].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap.find(pAgent->m_NodeAry[1].LinkNo) != m_LinkNoMap.end())
		{
			if( m_LinkNoMap[pAgent->m_NodeAry[1].LinkNo]->m_bIncludedinSubarea == true)
				return false;
		}

		// now the first link is not in subarea
		// condition 2: a link is in subarea
		bool bPassingSubareaFlag = false;
		for(int link= 2; link<pAgent->m_NodeSize-1; link++)
		{

			if( m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_bIncludedinSubarea )
			{
				bPassingSubareaFlag = true;	
				break;
			}
		}

		if(bPassingSubareaFlag == false)
			return false;
		// now there is a link is in subarea
		// condition 3: test if the last link is out of subarea

		int last_link  = pAgent->m_NodeSize-1;
		if(m_LinkNoMap.find(pAgent->m_NodeAry[last_link].LinkNo) != m_LinkNoMap.end() &&  m_LinkNoMap[pAgent->m_NodeAry[last_link].LinkNo]->m_bIncludedinSubarea == false)
			return true;  // last link is outside of the subarea area, yes!
		else
			return false;

	}

	if(Agent_selection == CLS_subarea_internal_to_external)
	{
		if(pAgent->m_NodeSize<2)
			return false;
		// now pAgent->m_NodeSize>=2;

		// condition 1: if the first link of a Agent is not marked (not in the subarea), then return false
		if( m_LinkNoMap.find(pAgent->m_NodeAry[1].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap[pAgent->m_NodeAry[1].LinkNo]->m_bIncludedinSubarea == false)
			return false;

		// now the first link is in subarea
		// condition 2: test if the last link is out of subarea

		int last_link  = pAgent->m_NodeSize-1;
		if(m_LinkNoMap.find(pAgent->m_NodeAry[last_link].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap[pAgent->m_NodeAry[last_link].LinkNo]->m_bIncludedinSubarea  == false)
			return true;  // outside, yes,
		else
			return false;
	}

	if(Agent_selection == CLS_subarea_external_to_internal)
	{

		if(pAgent->m_NodeSize<2)
			return false;
		// now pAgent->m_NodeSize>=2;

		// condition 1: if the first link of a Agent is marked (in the subarea), then return false
		if(m_LinkNoMap.find(pAgent->m_NodeAry[1].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap[pAgent->m_NodeAry[1].LinkNo]->m_bIncludedinSubarea == true)
			return false;

		// now the first link is not in the subarea
		// condition 2: test if the last link is in subarea

		int last_link  = pAgent->m_NodeSize-1;
		if( m_LinkNoMap.find(pAgent->m_NodeAry[last_link].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap[pAgent->m_NodeAry[last_link].LinkNo]->m_bIncludedinSubarea  == true)
			return true;  // inside, yes!
		else
			return false;
	}
	if(Agent_selection == CLS_subarea_internal_to_internal_trip)
	{

		if(pAgent->m_NodeSize<2)  
			return false;
		// now pAgent->m_NodeSize>=2;

		for(int link= 1; link<pAgent->m_NodeSize; link++)
		{
			if( m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end() && m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_bIncludedinSubarea == false )
				return false; // this is a link outside of the subarea
		}

		// pass all the tests
		return true;
	}


	if(Agent_selection == CLS_subarea_internal_to_internal_subtrip)
	{

		if(pAgent->m_NodeSize<2)  
			return false;
		// now pAgent->m_NodeSize>=2;

		float subtrip_distance = 0;
		float subtrip_free_flow_travel_time = 0;
		int distance_counting_flag = 0;
		for(int link= 1; link<pAgent->m_NodeSize; link++)
		{
			if(m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end())
			{
				DTALink *pLink = m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo];
				if(pLink->m_bIncludedinSubarea == true )
				{
					if(distance_counting_flag==0)
					{
						distance_counting_flag = 1;
						pAgent->m_subarea_start_node_departure_time = pAgent->m_NodeAry [link-1].ArrivalTimeOnDSN ;

					}

					if(distance_counting_flag==1)
					{
						subtrip_distance+= m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_Length ;
						subtrip_free_flow_travel_time += m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_FreeFlowTravelTime ;


						// keep the last record
						pAgent->m_subarea_distance = subtrip_distance; // for inside path
						pAgent->m_subarea_free_flow_travel_time = subtrip_free_flow_travel_time;
						pAgent->m_subarea_end_node_arrival_time  = pAgent->m_NodeAry [link].ArrivalTimeOnDSN ;
						pAgent->m_subarea_travel_time = pAgent->m_subarea_end_node_arrival_time  - pAgent->m_subarea_start_node_departure_time;

					}


				}else if(distance_counting_flag == 1) 
				{  //leaving subarea

					distance_counting_flag = 2;

					break;
				}

			}
		}

		// pass all the tests
		if(distance_counting_flag>=1)
			return true;
	}
	if(Agent_selection == CLS_subarea_boundary_to_bounary_subtrip)
	{

		if(pAgent->m_NodeSize<2)  
			return false;
		// now pAgent->m_NodeSize>=2;

		float subtrip_distance = 0;
		float subtrip_free_flow_travel_time = 0;
		int distance_counting_flag = 0;
		for(int link= 1; link<pAgent->m_NodeSize; link++)
		{
			if( m_LinkNoMap.find(pAgent->m_NodeAry[link].LinkNo) != m_LinkNoMap.end())
			{
				DTALink* pLink = m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo];
				if(pLink->m_bIncludedinSubarea == true )
				{
					if(distance_counting_flag==0)
					{
						distance_counting_flag = 1;
						//record the boundary arrival time
						pAgent->m_subarea_start_node_departure_time = pAgent->m_NodeAry [link-1].ArrivalTimeOnDSN ;

					}

					if(distance_counting_flag==1) // when distance_counting_flag ==0, we will go to this condition right away
					{
						//start counting we  have boundary start time
						subtrip_distance+= m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_Length ;
						subtrip_free_flow_travel_time += m_LinkNoMap[pAgent->m_NodeAry[link].LinkNo]->m_FreeFlowTravelTime ;

						// keep the last record
						pAgent->m_subarea_distance = subtrip_distance; // for inside path
						pAgent->m_subarea_free_flow_travel_time = subtrip_free_flow_travel_time;
						pAgent->m_subarea_end_node_arrival_time  = pAgent->m_NodeAry [link].ArrivalTimeOnDSN ;
						pAgent->m_subarea_travel_time = pAgent->m_subarea_end_node_arrival_time  - pAgent->m_subarea_start_node_departure_time;

						ASSERT(pAgent->m_subarea_travel_time<900);

					}

				}else if(distance_counting_flag == 1) 
				{  //leaving subarea

					distance_counting_flag = 2;
					break;
				}else
				{

					TRACE("\nAgent %d: Link: %d->%d ", pAgent->m_AgentID , pLink->m_FromNodeID , pLink->m_ToNodeID );

				}

			}

		}

		// pass all the tests
		if(distance_counting_flag==2)
			return true;
	}
	return false;
}
void CTLiteDoc::MarkLinksInSubarea()
{
	std::list<DTALink*>::iterator iLink;

	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		(*iLink)->m_bIncludedinSubarea = false;  // reset all the links are not selected by the path
	}

	for (iLink = m_SubareaLinkSet.begin(); iLink != m_SubareaLinkSet.end(); iLink++)
	{
		(*iLink)->m_bIncludedinSubarea = true;
	}
}

void CTLiteDoc::GenerateAgentClassificationData(Agent_CLASSIFICATION_SELECTION Agent_selection, Agent_X_CLASSIFICATION x_classfication)
{
	m_ClassificationTable.clear ();  //reset

	std::list<DTAAgent*>::iterator iAgent;

	//calculate free-flow travel time
	for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++)
	{
		DTAAgent* pAgent = (*iAgent);

		if(pAgent->m_FreeflowTripTime <0.00001f)  // freeflow travel time has not been defined yet
		{

			DTAAgent* pAgent = (*iAgent);

			for(int i=1; i< pAgent->m_NodeSize ; i++)
			{
				if(m_LinkNoMap.find(pAgent->m_NodeAry[i].LinkNo)!= m_LinkNoMap.end()) 
				{
					DTALink* pLinkCurrent = m_LinkNoMap[ pAgent->m_NodeAry[i].LinkNo];
					pAgent->m_FreeflowTripTime+= pLinkCurrent->m_FreeFlowTravelTime ;
				}
			}

		}
	}

	bool bTraceFlag = true;

	for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++)
	{
		DTAAgent* pAgent = (*iAgent);

		pAgent->m_bMarked = false;
		if(SelectAgentForAnalysis(pAgent, m_AgentSelectionMode) == true)  // belong this classification 
		{
			pAgent->m_bMarked = true;

			int index = FindClassificationNo(pAgent,x_classfication);

			if(index>=0)  // -1 will not be consideded
			{
				m_ClassificationTable[index].TotalAgentSize+=1;


				if(Agent_selection== CLS_path_partial_trip)  // subpath
				{
					m_ClassificationTable[index].TotalTravelTime  +=  pAgent->m_path_travel_time;
					m_ClassificationTable[index].TotalFreeflowTravelTime  +=  pAgent->m_path_free_flow_travel_time;

					m_ClassificationTable[index].TotalDistance   += pAgent->m_path_distance;				
					m_ClassificationTable[index].TotalTravelTimePerMile += ( pAgent->m_path_travel_time /max(0.01,pAgent->m_path_distance));

					m_ClassificationTable[index].m_data_vector_travel_time .AddData (pAgent->m_path_travel_time);
					m_ClassificationTable[index].m_data_vector_travel_time_per_mile .AddData ( pAgent->m_path_travel_time /max(0.01,pAgent->m_path_distance));


				}else if (Agent_selection == CLS_subarea_internal_to_internal_subtrip || Agent_selection == CLS_subarea_boundary_to_bounary_subtrip)
				{
					m_ClassificationTable[index].TotalTravelTime  += pAgent->m_subarea_travel_time ;
					m_ClassificationTable[index].TotalFreeflowTravelTime  += pAgent->m_subarea_free_flow_travel_time ;
					m_ClassificationTable[index].TotalDistance   += pAgent->m_subarea_distance;				
					m_ClassificationTable[index].TotalTravelTimePerMile += (pAgent->m_subarea_travel_time)/max(0.01,pAgent->m_subarea_distance);

					m_ClassificationTable[index].m_data_vector_travel_time .AddData (pAgent->m_subarea_travel_time);
					m_ClassificationTable[index].m_data_vector_travel_time_per_mile .AddData (( pAgent->m_subarea_travel_time) /max(0.01,pAgent->m_subarea_distance));
				}
				else  //
				{
					m_ClassificationTable[index].TotalTravelTime  += (pAgent->m_ArrivalTime-pAgent->m_DepartureTime);
					m_ClassificationTable[index].TotalFreeflowTravelTime  += (pAgent->m_FreeflowTripTime);

					m_ClassificationTable[index].TotalDistance   += pAgent->m_Distance;				
					m_ClassificationTable[index].TotalTravelTimePerMile += ( (pAgent->m_ArrivalTime-pAgent->m_DepartureTime)/pAgent->m_Distance);

					m_ClassificationTable[index].m_data_vector_travel_time .AddData (pAgent->m_ArrivalTime-pAgent->m_DepartureTime);
					m_ClassificationTable[index].m_data_vector_travel_time_per_mile .AddData (
						( pAgent->m_ArrivalTime-pAgent->m_DepartureTime) /max(0.01,pAgent->m_Distance));
			}

			}

		}

	}

	//variability
	for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++)
	{
		DTAAgent* pAgent = (*iAgent);


		if(SelectAgentForAnalysis(pAgent, m_AgentSelectionMode) == true) 
		{
			int index = FindClassificationNo(pAgent,x_classfication);

			if(index>=0)  // -1 will not be consideded
			{
				float deviation = 0;
				float travel_time_per_mile_deviation = 0;


				if(Agent_selection== CLS_path_partial_trip)  //  subpath
				{
					deviation = (pAgent->m_path_travel_time)- 
						(m_ClassificationTable[index].TotalTravelTime/max(1,m_ClassificationTable[index].TotalAgentSize));


					travel_time_per_mile_deviation = 
						(pAgent->m_path_travel_time )/max(0.001,pAgent->m_path_distance  ) -  
						(m_ClassificationTable[index].TotalTravelTimePerMile/max(1,m_ClassificationTable[index].TotalAgentSize));
				}else if(Agent_selection== CLS_subarea_internal_to_internal_subtrip || Agent_selection == CLS_subarea_boundary_to_bounary_subtrip)  // subarea partial path
				{
					deviation = (pAgent->m_subarea_travel_time )- 
						(m_ClassificationTable[index].TotalTravelTime/max(1,m_ClassificationTable[index].TotalAgentSize));


					travel_time_per_mile_deviation = 
						(pAgent->m_subarea_travel_time )/max(0.001,pAgent->m_subarea_distance  ) -  
						(m_ClassificationTable[index].TotalTravelTimePerMile/max(1,m_ClassificationTable[index].TotalAgentSize));

				}else
				{
					deviation = (pAgent->m_ArrivalTime-pAgent->m_DepartureTime)- 
						(m_ClassificationTable[index].TotalTravelTime/max(1,m_ClassificationTable[index].TotalAgentSize));

					travel_time_per_mile_deviation = 
						(pAgent->m_ArrivalTime-pAgent->m_DepartureTime)/max(0.001,pAgent->m_Distance ) -  
						(m_ClassificationTable[index].TotalTravelTimePerMile/max(1,m_ClassificationTable[index].TotalAgentSize));
				}

				m_ClassificationTable[index].TotalTravelTimeVariance   +=  deviation*deviation;
				m_ClassificationTable[index].TotalTravelTimePerMileVariance   +=  travel_time_per_mile_deviation*travel_time_per_mile_deviation;

			}

		}

	}
}


void CTLiteDoc::GenerateClassificationForDisplay(Agent_X_CLASSIFICATION x_classfication, Agent_Y_CLASSIFICATION y_classfication)
{
	// input: 	m_ClassificationTable
	// output: label and display value

	std::map< int, AgentStatistics >::iterator iter;

	for ( iter = m_ClassificationTable.begin(); iter != m_ClassificationTable.end(); iter++ )
	{
		int index = iter->first;
		float value= 0;

		m_ClassificationTable[index].Label = FindClassificationLabel(x_classfication,index);

		// enum Agent_Y_CLASSIFICATION {CLS_Agent_count,CLS_total_travel_time,CLS_avg_travel_time,CLS_total_toll_cost,CLS_avg_toll_cost,CLS_total_generalized_cost,CLS_avg_generalized_cost,CLS_total_travel_distance, CLS_avg_travel_distance,CLS_total_CO2,CLS_avg_CO2};


		switch(y_classfication)
		{
		case CLS_Agent_count: 
			value = m_ClassificationTable[index].TotalAgentSize;
			break;
		case CLS_cumulative_Agent_count: 

			value  = 0;
			for(int ii = 0; ii <= index; ii++)
				if(m_ClassificationTable.find(ii) != m_ClassificationTable.end())
				{
					value += m_ClassificationTable[ii].TotalAgentSize;
				}
				break;

		case CLS_total_travel_time: 
			value = m_ClassificationTable[index].TotalTravelTime ;
			break;
		case CLS_avg_travel_time: 
			value = m_ClassificationTable[index].TotalTravelTime/max(1,m_ClassificationTable[index].TotalAgentSize);
			break;

		case CLS_travel_time_95_percentile: 
			value = m_ClassificationTable[index].m_data_vector_travel_time .GetDataVectorPercentage (95);
			break;

		case CLS_travel_time_90_percentile: 
			value = m_ClassificationTable[index].m_data_vector_travel_time .GetDataVectorPercentage (90);
			break;

		case CLS_travel_time_80_percentile: 
			value = m_ClassificationTable[index].m_data_vector_travel_time .GetDataVectorPercentage (80);
			break;

		case CLS_travel_time_per_mile_95_percentile: 
			value = m_ClassificationTable[index].m_data_vector_travel_time_per_mile .GetDataVectorPercentage (95);
			break;

		case CLS_travel_time_per_mile_90_percentile: 
			value = m_ClassificationTable[index].m_data_vector_travel_time_per_mile .GetDataVectorPercentage (90);
			break;

		case CLS_travel_time_per_mile_80_percentile: 
			value = m_ClassificationTable[index].m_data_vector_travel_time_per_mile .GetDataVectorPercentage (80);
			break;
			//Buffer Index = (95th percentile travel time ?mean travel time) / mean travel time
		case CLS_travel_time_Buffer_Index: 
			value = (m_ClassificationTable[index].m_data_vector_travel_time .GetDataVectorPercentage (95)- m_ClassificationTable[index].m_data_vector_travel_time.GetDataVectorMean ())/max(0.01,m_ClassificationTable[index].m_data_vector_travel_time.GetDataVectorMean ());
			break;
			//Skew Index = (90th percentile travel time ?median travel time) / (median travel time ?10th percentile travel time)
		case CLS_travel_time_Skew_Index: 
			value = (m_ClassificationTable[index].m_data_vector_travel_time .GetDataVectorPercentage (90)- m_ClassificationTable[index].m_data_vector_travel_time.GetDataVectorPercentage(50))/
				max(0.01,m_ClassificationTable[index].m_data_vector_travel_time.GetDataVectorPercentage (50) -m_ClassificationTable[index].m_data_vector_travel_time.GetDataVectorPercentage (10) );
			break;
		case CLS_travel_time_Travel_Time_Index:  // mean of travel time distribution/free flow travel time for a given path
			value = m_ClassificationTable[index].TotalTravelTime/max(0.01,m_ClassificationTable[index].TotalFreeflowTravelTime);
			break;
		case CLS_travel_time_Planning_Time_Index: //95th percentile of travel time distribution/free flow travel time for a given path
			value = m_ClassificationTable[index].m_data_vector_travel_time .GetDataVectorPercentage (95)/max(0.01,m_ClassificationTable[index].TotalFreeflowTravelTime/m_ClassificationTable[index].TotalAgentSize);
			break;

		case CLS_total_travel_distance: 
			value = m_ClassificationTable[index].TotalDistance   ;
			break;
		case CLS_avg_travel_distance: 
			value = m_ClassificationTable[index].TotalDistance /max(1,m_ClassificationTable[index].TotalAgentSize);
			break;
		case CLS_avg_speed: 
			value = m_ClassificationTable[index].TotalDistance /max(0.1,m_ClassificationTable[index].TotalTravelTime/60.0f);
			break;
		case CLS_travel_time_STD: 
			value = sqrt(m_ClassificationTable[index].TotalTravelTimeVariance /max(1,m_ClassificationTable[index].TotalAgentSize));
			break;
		case CLS_travel_time_per_mile_STD: 
			value = sqrt(m_ClassificationTable[index].TotalTravelTimePerMileVariance /max(1,m_ClassificationTable[index].TotalAgentSize));
			break;
		case CLS_avg_travel_time_per_mile: 
			value = m_ClassificationTable[index].TotalTravelTimePerMile /max(1,m_ClassificationTable[index].TotalAgentSize);
			break;


	


		default: 
			value = 0;
			TRACE("ERROR. No classification available!");
		};

		m_ClassificationTable[index].DisplayValue = value;

	}
}


void CTLiteDoc::OnLinkAgentstatisticsanalaysis()
{
	if(m_SelectedLinkNo==-1)
	{
		AfxMessageBox("Please select a link first.");
		return;
	}

	m_AgentSelectionMode = CLS_link_set;  // select link analysis

	CDlg_AgentClassification dlg;

	dlg.m_AgentSelectionNo = CLS_link_set;
	dlg.m_pDoc = this;
	dlg.DoModal ();

}



void CTLiteDoc::OnSubareaDeletesubarea()
{
	m_SubareaShapePoints.clear ();
	UpdateAllViews(0);
}

void CTLiteDoc::OnSubareaViewAgentstatisticsassociatedwithsubarea()
{
	// TODO: Add your command handler code here
}

void CTLiteDoc::OnLinkLinkbar()
{


}




void CTLiteDoc::OnLinkIncreaseoffsetfortwo()
{
	std::list<DTALink*>::iterator iLink;

	double minimum_link_length = 999999;
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		if((*iLink) -> m_bToBeShifted)
		{
			if((*iLink)->m_Length <  minimum_link_length)
				minimum_link_length = (*iLink)->m_Length ;
		}
	}

	double min_offset_value = 0.001;
	
	if(minimum_link_length < 9999)  // with data
		min_offset_value = max(minimum_link_length * 0.01, 0.001);

	m_OffsetInDistance += min_offset_value;
	m_bLinkToBeShifted  = true;
	OffsetLink();  // offset shape points
	GenerateOffsetLinkBand();  // from shape points to reference points
	UpdateAllViews(0);
}

void CTLiteDoc::OnLinkDecreaseoffsetfortwo()
{
	std::list<DTALink*>::iterator iLink;

	double minimum_link_length = 999999;
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		if((*iLink) -> m_bToBeShifted)
		{
			if((*iLink)->m_Length <  minimum_link_length)
				minimum_link_length = (*iLink)->m_Length ;
		}
	}

	double min_offset_value = max(minimum_link_length*0.01, 0.001);
	m_OffsetInDistance -= min_offset_value;


	m_bLinkToBeShifted  = true;
	OffsetLink();
	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}

void CTLiteDoc::OnLinkNooffsetandnobandwidth()
{
	std::list<DTALink*>::iterator iLink;

	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		(*iLink)->m_ShapePoints.clear();

		(*iLink)->m_ShapePoints .push_back ((*iLink)->m_FromPoint);
		(*iLink)->m_ShapePoints .push_back ((*iLink)->m_ToPoint);
	}

	m_OffsetInDistance=0.1;
	m_bLinkToBeShifted  = true;
	m_LaneWidthInKM = 0.004;
	OffsetLink();
	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}

void CTLiteDoc::OnViewShowhideLegend()
{
	m_bShowLegend = !m_bShowLegend;

	ShowLegend(m_bShowLegend);
}

void CTLiteDoc::ShowLegend(bool ShowLegendStatus)
{
	m_bShowLegend = ShowLegendStatus;
	if(m_bShowLegend)
	{
		if(g_pLegendDlg==NULL)
		{
			g_pLegendDlg = new CDlg_Legend();
			g_pLegendDlg->m_pDoc = this;
			g_pLegendDlg->Create(IDD_DIALOG_Legend);
		}

		// update using pointer to the active document; 

		if(g_pLegendDlg->GetSafeHwnd())
		{
			g_pLegendDlg->m_pDoc = this;
			g_pLegendDlg->ShowWindow(SW_HIDE);
			g_pLegendDlg->ShowWindow(SW_SHOW);
		}
	}else
	{
		if(g_pLegendDlg!=NULL && g_pLegendDlg->GetSafeHwnd())
		{
			g_pLegendDlg->ShowWindow(SW_HIDE);
		}
	}
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	pMainFrame->UpdateLegendView();

}
void CTLiteDoc::OnMoeViewlinkmoesummaryfile()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_LinkMOE.csv");
}

void CTLiteDoc::OnViewCalibrationview()
{
	m_bShowCalibrationResults = !m_bShowCalibrationResults;

	if(m_bShowCalibrationResults == true)
	{
		m_LinkMOEMode = MOE_volume;
		OnMoeViewoddemandestimationsummaryplot();
		UpdateAllViews(0);
	}
}

void CTLiteDoc::OnUpdateViewCalibrationview(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowCalibrationResults);
}

void CTLiteDoc::OnMoeViewtrafficassignmentsummaryplot()
{
}

void CTLiteDoc::OnMoeViewoddemandestimationsummaryplot()
{

}

void CTLiteDoc::OnMoeViewoddemandestimationsummaryplotLanedensity()
{
}

void CTLiteDoc::OnProjectEditpricingscenariodata()
{

}

void CTLiteDoc::OnLinkViewlink()
{

}

void CTLiteDoc::OnDeleteSelectedLink()
{
	if(m_SelectedLinkNo == -1)
	{
		AfxMessageBox("Please select a link first.");
		return;
	}
	DeleteLink(m_SelectedLinkNo);
	m_SelectedLinkNo = -1;
	UpdateAllViews(0);

}


void CTLiteDoc::OnImportRegionalplanninganddtamodels()
{
}

void CTLiteDoc::OnExportGenerateTravelTimeMatrix()
{


}

void CTLiteDoc::OnExportGenerateshapefiles()
{

}

void CTLiteDoc::OnLinkmoedisplayQueueLengthRatio()
{
	m_LinkMOEMode = MOE_queue_length;
	ShowLegend(false);
	GenerateOffsetLinkBand();
	UpdateAllViews(0);

}

void CTLiteDoc::OnUpdateLinkmoedisplayQueueLengthRatio(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_queue_length);
}





void CTLiteDoc::ZoomToSelectedNode(int SelectedNodeID)
{
	if(m_NodeIDtoNodeNoMap.find(SelectedNodeID) != m_NodeIDtoNodeNoMap.end())
	{
		DTANode* pNode= m_NodeIDMap[SelectedNodeID];
		m_Origin = pNode->pt;
		m_SelectedLinkNo = -1;
		m_SelectedNodeNo = pNode->m_NodeNo ;

		CTLiteView* pView = 0;
		POSITION pos = GetFirstViewPosition();
		if(pos != NULL)
		{
			pView = (CTLiteView*) GetNextView(pos);
			if(pView!=NULL)
			{
				pView->m_Origin = m_Origin;
				pView->Invalidate ();
			}
		}

	}
}

void CTLiteDoc::ZoomToSelectedLink(int SelectedLinkNo)
{
	if(m_LinkNoMap.find(SelectedLinkNo) != m_LinkNoMap.end())
	{
		DTALink* pLink= m_LinkNoMap [SelectedLinkNo];
		GDPoint pt;
		pt.x = (pLink->m_FromPoint.x + pLink->m_ToPoint.x)/2;
		pt.y = (pLink->m_FromPoint.y + pLink->m_ToPoint.y)/2;
		m_Origin = pt;

		CTLiteView* pView = 0;
		POSITION pos = GetFirstViewPosition();
		if(pos != NULL)
		{
			pView = (CTLiteView*) GetNextView(pos);
			if(pView!=NULL)
			{
				pView->m_Origin = m_Origin;
				pView->Invalidate ();
			}
		}

	}
}

void CTLiteDoc::ShowPathListDlg(bool bShowFlag)
{
	if(bShowFlag)
	{
		if(g_pPathListDlg==NULL)
		{
			g_pPathListDlg = new CDlgPathList();
			g_pPathListDlg->m_pDoc = this;
			g_pPathListDlg->Create(IDD_DIALOG_PATH_LIST);
		}

		// update using pointer to the active document; 

		if(g_pPathListDlg->GetSafeHwnd())
		{
			g_pPathListDlg->m_pDoc = this;
			g_pPathListDlg->ReloadData ();
			g_pPathListDlg->ShowWindow(SW_HIDE);
			g_pPathListDlg->ShowWindow(SW_SHOW);
		}
	}else
	{
		if(g_pPathListDlg!=NULL && g_pPathListDlg->GetSafeHwnd())
		{
			g_pPathListDlg->ShowWindow(SW_HIDE);
		}
	}
}

void CTLiteDoc::OnMoePathlist()
{
	//	m_bShowPathList = !m_bShowPathList;
	ShowPathListDlg(m_bShowPathList);
}


bool g_TestValidDocument(CTLiteDoc* pDoc)
{
	std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
	while (iDoc != g_DocumentList.end())
	{
		if(pDoc == (*iDoc))
			return true;

		iDoc++;
	}

	return false;
}

void CTLiteDoc::OnViewShowmoe()
{
	g_LinkMOEDlgShowFlag = !g_LinkMOEDlgShowFlag;
	if(g_LinkMOEDlgShowFlag)
	{
		if(g_LinkMOEDlg==NULL)
		{
			g_LinkMOEDlg = new CDlgMOE();

			g_LinkMOEDlg->m_TimeLeft = 0 ;
			g_LinkMOEDlg->m_TimeRight = g_Simulation_Time_Horizon ;
			g_LinkMOEDlg->m_pDoc = this;
			g_LinkMOEDlg->SetModelessFlag(TRUE);
			g_LinkMOEDlg->Create(IDD_DIALOG_MOE);

		}

		if(g_LinkMOEDlg->GetSafeHwnd())
		{
			// assignemnt document pointers
			std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
			while (iDoc != g_DocumentList.end())
			{
				if ((*iDoc)->m_NodeSet.size()>0)
				{
					if(g_LinkMOEDlg->m_pDoc ==NULL)
						g_LinkMOEDlg->m_pDoc = (*iDoc);
					else if((*iDoc)!= g_LinkMOEDlg->m_pDoc)
					{
						g_LinkMOEDlg->m_pDoc2 = (*iDoc);
						g_LinkMOEDlg->m_bDoc2Ready = true;
					}
				}
				iDoc++;
			}
			g_LinkMOEDlg->ShowWindow(SW_HIDE);
			g_LinkMOEDlg->ShowWindow(SW_SHOW);
		}
	}else
	{
		if(g_LinkMOEDlg!=NULL && g_LinkMOEDlg->GetSafeHwnd())
		{
			g_LinkMOEDlg->ShowWindow(SW_HIDE);
		}
	}

}

void CTLiteDoc::OnUpdateViewShowmoe(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(g_LinkMOEDlgShowFlag);
}

void CTLiteDoc::OnFileUploadlinkdatatogooglefusiontable()
{
	AfxMessageBox("Please first save the data set to a folder, and they you can uploadlink.csv to Goolge Fusion Tables.");
}

void CTLiteDoc::On3Viewdatainexcel()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_ODMOE.csv");
}

void CTLiteDoc::On5Viewdatainexcel()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_LinkTDMOE.csv");
}

void CTLiteDoc::OnMoeViewnetworktimedependentmoe()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_NetworkTDMOE.csv");
}

void CTLiteDoc::On2Viewdatainexcel33398()
{
	// TODO: Add your command handler code here
}

void CTLiteDoc::On2Viewnetworkdata()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_NetworkTDMOE.csv");
}

void CTLiteDoc::On3Viewoddatainexcel()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_ODMOE.csv");
}

void CTLiteDoc::OnMoeOpenallmoetables()
{


	OpenCSVFileInExcel(m_ProjectDirectory+"output_summary.csv");
	OpenCSVFileInExcel(m_ProjectDirectory+"output_NetworkTDMOE.csv");
	OpenCSVFileInExcel(m_ProjectDirectory+"output_ODMOE.csv");
	OpenCSVFileInExcel(m_ProjectDirectory+"output_LinkMOE.csv");



	//	OpenCSVFileInExcel(m_ProjectDirectory+"output_NetworkTDMOE.csv");
	//	OpenCSVFileInExcel(m_ProjectDirectory+"output_LinkTDMOE.csv");
}



void CTLiteDoc::OnBnClickedButtonDatabase()
{

}

void CTLiteDoc::OnToolsUnittesting()
{


	// update using pointer to the active document; 


}





void CTLiteDoc::OnToolsGeneratephysicalzonecentroidsonroadnetwork()
{
	CWaitCursor cursor;

	// given conditions in node.csv, zone_id, super_zone_id
	// step 1: mark old centroids, map them to the new super zone id;
		
	// step 2: create new centriods for each super zone
	
	//step 3: for each original centroid, creat  a pair of incoming and outgoing links to super centroid, if exist

	std::list<DTALink*>::iterator iLink;
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		DTALink * pLink = (*iLink);

		if(pLink==NULL)
			break;
		DTALink * pRevLink = NULL; //reversed link
		unsigned long ReversedLinkKey = GetLinkKey(pLink->m_ToNodeNo, pLink->m_FromNodeNo);

		int reversed_link_id = 0;
		if ( m_NodeIDtoLinkMap.find ( ReversedLinkKey) != m_NodeIDtoLinkMap.end())
		{
			pRevLink = m_NodeIDtoLinkMap[ReversedLinkKey];
		}

		DTANode* pFromNode = m_NodeNoMap[pLink->m_FromNodeNo ];
		DTANode* pToNode = m_NodeNoMap[pLink->m_ToNodeNo ];


		if(pFromNode->m_CentroidUpdateFlag >=1 && pRevLink != NULL && m_LinkTypeMap[pLink->m_link_type ].IsConnector()) // old centroid and no-external origin node // if it is an external origin node, then we do not need to add nodes
		{
			int zone_number = pFromNode->m_ZoneID ;
			int new_node_number;
			// construct new node number based on from and to node ids
			if(pFromNode->m_NodeID < pToNode->m_NodeID)
				new_node_number = pFromNode->m_NodeID * 10000 + pToNode->m_NodeID ;  // fromID*10000+ToID
			else
				new_node_number = pToNode->m_NodeID * 10000 +  pFromNode->m_NodeID ;  // ToID*10000+fromID

			// add new node and update from_node
			if(m_NodeIDtoNodeNoMap.find(new_node_number) == m_NodeIDtoNodeNoMap.end() )
			{
				GDPoint	pt;
				pt.x = (pFromNode->pt.x + pToNode->pt.x) /2;
				pt.y = (pFromNode->pt.y + pToNode->pt.y) /2;

				AddNewNode(pt, new_node_number,false);


				int new_node_no = m_NodeIDtoNodeNoMap[new_node_number];

				DTANode* pNewNode = m_NodeNoMap [new_node_no] ;
				m_NodeIDtoZoneNameMap[new_node_number] = zone_number;

				pNewNode -> m_ZoneID = zone_number;
				pNewNode ->m_Name == "New_Node";


				//update from node of this link
				pLink->m_FromNodeID = new_node_number;
				pLink->m_FromNodeNo  = new_node_no;
				pLink->m_FromPoint = pt;
				pLink->m_CentroidUpdateFlag = 1;

				// do not look at the downstream node as centroid again!
				pToNode->m_CentroidUpdateFlag  = 0;

				if( pToNode->m_NodeID == 54317)
					TRACE("");

				pLink->m_ShapePoints .clear();
				pLink->m_ShapePointRatios  .clear();
				pLink->m_ShapePoints.push_back (pLink->m_FromPoint);
				pLink->m_ShapePoints.push_back (pLink->m_ToPoint);

				//update downstream node of the reversed link
				pRevLink->m_CentroidUpdateFlag = 1;
				pRevLink->m_ToNodeID = new_node_number;
				pRevLink->m_ToNodeNo  = new_node_no;
				pRevLink->m_ToPoint = pt;

				pRevLink->m_ShapePoints .clear();
				pRevLink->m_ShapePointRatios  .clear();
				pRevLink->m_ShapePoints.push_back (pLink->m_ToPoint);
				pRevLink->m_ShapePoints.push_back (pLink->m_FromPoint);

			}  //two-way link  from node

			pFromNode->m_CentroidUpdateFlag+=1; // +1 as to be removed, we "touch" this node

		}  // centriod 

	}  // for each link


	// step 2.1: do not delete adjacent nodes of physical links

	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{

		DTALink * pLink = (*iLink);
		if(m_LinkTypeMap[pLink->m_link_type ].IsConnector()== false)  // not a connector, so the ajacent node number will still appear in link.csv file after generating physical links 
		{

			m_NodeNoMap [pLink->m_FromNodeNo ] -> m_CentroidUpdateFlag = 0; // no change
			m_NodeNoMap [pLink->m_ToNodeNo ] -> m_CentroidUpdateFlag = 0; // no change


		}

	}
	// step 3: remove old centroids
	std::list<DTANode*>::iterator iNode;
	iNode = m_NodeSet.begin();

	while(iNode != m_NodeSet.end())
	{
		if((*iNode)->m_CentroidUpdateFlag >= 2)
		{
			int ZoneID = (*iNode)->m_ZoneID;

			m_NodeNoMap[(*iNode)->m_NodeNo ] = NULL;
			m_NodeIDtoNodeNoMap[(*iNode)->m_NodeID  ] = -1;
			iNode = m_NodeSet.erase  (iNode); //  correctly update the iterator to point to the location after the iterator we removed.


		}else
		{
			++iNode;
		}
	}

	// update m_path map 
	m_Movement3NodeMap.clear();

	std::map<CString, PathStatistics> ::iterator itr_path;
	for (itr_path = m_PathMap.begin(); itr_path != m_PathMap.end(); itr_path++)
	{

		// update node numbers
		(*itr_path).second.m_NodeVector.clear();

		int i;
		for(i = 0; i < (*itr_path).second.m_LinkPointerVector.size(); i++)
		{

			DTALink* pLink = (*itr_path).second.m_LinkPointerVector[i];

			if(pLink!=NULL)
			{
				int from_node_number = pLink->m_FromNodeID   ;
				int to_node_number = pLink->m_ToNodeID ;

				if(i==0) 
				{
					(*itr_path).second.m_NodeVector.push_back(from_node_number);
				}

				(*itr_path).second.m_NodeVector.push_back(to_node_number);
			}


		}

	}

	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}



void CTLiteDoc::OnNodeIncreasenodetextsize()
{

	m_NodeTextDisplayRatio *=1.1;

	UpdateAllViews(0);

}

void CTLiteDoc::OnNodeDecreasenodetextsize()
{
	m_NodeTextDisplayRatio /=1.1;

	if(m_NodeTextDisplayRatio <2)  
		m_NodeTextDisplayRatio = 2;

	UpdateAllViews(0);
}



void CTLiteDoc::OnImportSynchroutdfcsvfiles()
{
	ReadSynchroUniversalDataFiles();
	bSynchroImportReadyToSaveFlag = true;

	AfxMessageBox("Please first save and reload the data set to check movement attributes", MB_ICONINFORMATION);

}

void CTLiteDoc::OnProjectEditmoesettings()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"input_MOE_settings.csv");
}

void CTLiteDoc::OnProjectMultiScenarioResults()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_multi_scenario_results.csv");
}

void CTLiteDoc::OnProject12()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_Path.csv");
}

void CTLiteDoc::OnViewMovementMoe()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"output_MovementMOE.csv");
}

void CTLiteDoc::OnProjectTimeDependentLinkMoe()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"link_performance.csv");
}

void CTLiteDoc::OnViewOdmeResult()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"agent.csv");
}


void CTLiteDoc::OnProjectOdmatrixestimationinput()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"ODME_Settings.txt");
}

void CTLiteDoc::OnProjectInputsensordataforodme()
{
	OpenCSVFileInExcel(m_ProjectDirectory+"sensor_count.csv");
}




void CTLiteDoc::ShowTextLabel()
{
}
void CTLiteDoc::OnLinkattributedisplayLinkname()
{
	m_LinkMOEMode = MOE_none;
	ShowTextLabel();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateLinkattributedisplayLinkname(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_none);
}


void CTLiteDoc::GenerateMovementCountFromAgentFile(float PeakHourFactor)
{

	CWaitCursor wait;
	m_PeakHourFactor = PeakHourFactor;


	if(m_Movement3NodeMap.size()>=1)
		return;

	if(m_AgentSet.size()>=50000)
	{
		AfxMessageBox("Generating movement count from Agent/agent files might take a while...", MB_ICONINFORMATION);

	}

	std::list<DTAAgent*>::iterator iAgent;

	for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++)
	{
		DTAAgent* pAgent = (*iAgent);

		for(int i= 2; i<pAgent->m_NodeSize; i++)
		{

			if(m_LinkNoMap.find(pAgent->m_NodeAry[i-1].LinkNo) != m_LinkNoMap.end()
				&&  m_LinkNoMap.find(pAgent->m_NodeAry[i].LinkNo) != m_LinkNoMap.end())
			{
				DTALink* pLink0 = m_LinkNoMap[pAgent->m_NodeAry[i-1].LinkNo]; // i=0, LinkNo = -1;
				DTALink* pLink1 = m_LinkNoMap[pAgent->m_NodeAry[i].LinkNo];

				CString movement_label;
				movement_label.Format ("%d;%d;%d", pLink0->m_FromNodeID  ,  pLink0->m_ToNodeID , pLink1->m_ToNodeID);
				m_Movement3NodeMap[movement_label].TotalAgentSize ++;

			}
		}
	}

	FILE* st;

	CString directory;
	directory = m_ProjectFile.Left(m_ProjectFile.ReverseFind('\\') + 1);

	fopen_s(&st,directory+"input_movement_3_node_format.csv","w");
	if(st!=NULL)
	{
		fprintf(st,"movement_index,three-node key,count\n");

		int movement_index = 1;
		std::map<CString, Movement3Node> ::const_iterator itr;
		for (itr = m_Movement3NodeMap.begin(); itr != m_Movement3NodeMap.end(); itr++)
		{
			fprintf(st, "%d,%s,%d\n", 
				movement_index++,
				(*itr).first, 
				(*itr).second.TotalAgentSize );

		}
		fclose(st);
	}else
	{
		AfxMessageBox("File movement.csv cannot be opened.");

	}


}

void CTLiteDoc::OnAssignmentSimulatinSettinsClicked()
{
	if(m_ProjectDirectory.GetLength()==0)
	{
		AfxMessageBox("The project has not been loaded.");
		return;
	}

	
	

	CAssignmentSimulationSettingDlg dlg;
	dlg.m_pDoc = this;
	dlg.DoModal();
}

void CTLiteDoc::OnProjectNetworkData()
{
	if(m_ProjectDirectory.GetLength()==0)
	{
		AfxMessageBox("The project has not been loaded.");
		return;
	}
	CNetworkDataSettingDlg dlg;
	dlg.m_pDoc = this;
	dlg.DoModal();

}

void CTLiteDoc::OnLinkAddsensor()
{

	//if(m_SelectedLinkNo==-1)
	//{
	//	AfxMessageBox("Please select a link first.");
	//	return;
	//}

	//DTALink* pLink= m_LinkNoMap [m_SelectedLinkNo];
	//if(pLink!=NULL)
	//{

	//	CNetworkDataSettingDlg dlg;
	//	dlg.m_pDoc = this;
	//	dlg.m_SelectTab = 4; // _SENSOR_DATA
	//	dlg.m_SelectedFromNodeName = pLink->m_FromNodeID;
	//	dlg.m_SelectedToNodeName = pLink->m_ToNodeID;

	//	if(dlg.DoModal()==IDOK)
	//	{
	//		ReadSensorCountData();
	//		UpdateAllViews(0);
	//	}

	//}
}



void CTLiteDoc::OnMoeTableDialog()
{
	CDlgMOETabView dlg;
	dlg.m_pDoc = this;
	dlg.DoModal();

}

DTALink* CTLiteDoc::FindLinkFromCoordinateLocation(float x1, float y1, float x2, float y2, float min_distance_in_mile)
{
	DTALink* pSelectedLink = NULL;
	float min_distance = 999999;

	int x_key = (x1 - m_GridRect.left) / m_GridXStep;
	int y_key = (y1 - m_GridRect.bottom) / m_GridYStep;

	//feasible region
	x_key = max(0, x_key);
	x_key = min(_MAX_GRID_SIZE - 1, x_key);

	y_key = max(0, y_key);
	y_key = min(_MAX_GRID_SIZE - 1, y_key);

	for (int i = 0; i < m_GridMatrix[x_key][y_key].m_LinkNoVector.size(); i++)
	{
		int linkno = m_GridMatrix[x_key][y_key].m_LinkNoVector[i];
		DTALink* pLink = m_LinkNoMap[linkno];
		float distance = sqrt(pow(x1 - pLink->m_FromPoint.x, 2) + pow(y1 - pLink->m_FromPoint.y, 2) + pow(x2 - pLink->m_ToPoint.x, 2) + pow(y2 - pLink->m_ToPoint.y, 2));
		if (distance < min_distance)
		{
			min_distance = distance;
			pSelectedLink = pLink;

		}

	}

	int from_x_key = x_key;
	int from_y_key = y_key;


	x_key = (x2 - m_GridRect.left) / m_GridXStep;
	y_key = (y2 - m_GridRect.bottom) / m_GridYStep;

	//feasible region
	x_key = max(0, x_key);
	x_key = min(_MAX_GRID_SIZE - 1, x_key);

	y_key = max(0, y_key);
	y_key = min(_MAX_GRID_SIZE - 1, y_key);

	if (from_x_key != x_key || from_y_key != y_key)
	{
		for (int i = 0; i < m_GridMatrix[x_key][y_key].m_LinkNoVector.size(); i++)
		{
			int linkno = m_GridMatrix[x_key][y_key].m_LinkNoVector[i];
			DTALink* pLink = m_LinkNoMap[linkno];
			float distance = sqrt(pow(x1 - pLink->m_FromPoint.x, 2) + pow(y1 - pLink->m_FromPoint.y, 2) + pow(x2 - pLink->m_ToPoint.x, 2) + pow(y2 - pLink->m_ToPoint.y, 2));
			if (distance < min_distance)
			{
				min_distance = distance;
				pSelectedLink = pLink;

			}

		}
	}


	float distance_in_mile = min_distance / max(0.0000001, m_UnitDistance);
	if (distance_in_mile < min_distance_in_mile)
		return pSelectedLink;
	else
		return NULL;
}

bool CTLiteDoc::ReadSensorTrajectoryData(LPCTSTR lpszFileName)
{
	CCSVParser parser;
	int i= 0;

	bool bRectInitialized = false;

	if (parser.OpenCSVFile(CString2StdString(lpszFileName)))
	{

		while(parser.ReadRecord())
		{

			AgentLocationRecord element;
			
			if(parser.GetValueByFieldName("time_stamp_in_second",element.time_stamp_in_second) == false)
				break;

			if(parser.GetValueByFieldName("trace_id",element.agent_id) == false)
				break;


			if(parser.GetValueByFieldName("x",element.x) == false)
				break;

			if(parser.GetValueByFieldName("y",element.y) == false)
				break;


			AddLocationRecord(element);
			
			if(!bRectInitialized)
			{
				m_NetworkRect.left = element.x ;
				m_NetworkRect.right = element.x;
				m_NetworkRect.top = element.y;
				m_NetworkRect.bottom = element.y;
				bRectInitialized = true;
			}

			GDPoint point;
			point.x = element.x;
			point.y = element.y;

			m_NetworkRect.Expand(point);
		
			
			i ++;

		}
	
		m_AgentLocationLoadingStatus.Format("%d trace location records are loaded from file %s.",i,lpszFileName);	
	}

		return true;
}

void CTLiteDoc::ResetZoneIDVector()
{

	//update m_ODSize
	std::map<int, DTAZone>	:: const_iterator itr;

	for(itr = m_ZoneMap.begin(); itr != m_ZoneMap.end(); itr++)
	{
		if( itr->first > m_ODSize)
		{

			m_ODSize = itr->first;
		}
	}

	

}

void CTLiteDoc::ResetODMOEMatrix()
{

	if(m_ODSize== m_PreviousODSize)
		return; 

	ResetZoneIDVector();

	if(m_AgentSet.size() ==0)  // no vheicle data, no need to process OD matrix
		return;

	m_ZoneNoSize = this->m_ZoneID2ZoneNoMap.size();
	if(m_ODMOEMatrix == NULL  )
	{
		m_ODMOEMatrix = Allocate3DDynamicArray<AgentStatistics>(m_AgentTypeSize,m_ZoneNoSize,m_ZoneNoSize);
		m_PreviousAgentTypeSize = m_AgentTypeSize;
		m_PreviousZoneNoSize = m_ZoneNoSize ;
	}
	else
	{
		Deallocate3DDynamicArray<AgentStatistics>(m_ODMOEMatrix,m_PreviousAgentTypeSize, m_PreviousZoneNoSize);
		m_ODMOEMatrix = Allocate3DDynamicArray<AgentStatistics>(m_AgentTypeSize,m_ZoneNoSize,m_ZoneNoSize);
		m_PreviousAgentTypeSize = m_AgentTypeSize;
		m_PreviousZoneNoSize = m_ZoneNoSize ;
	}

	int p, i, j;

	for(p=0;p<m_AgentTypeSize; p++)
	{
		for(i= 0; i < m_ZoneNoSize ; i++)
		{
			for(j= 0; j< m_ZoneNoSize ; j++)
			{
				m_ODMOEMatrix[p][i][j].Reset ();
			}

		}

	}



	int count  = 0;
	std::list<DTAAgent*>::iterator iAgent;

	int error_count = 0;

	for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++, count++)
	{
		DTAAgent* pAgent = (*iAgent);
		if(/*pAgent->m_NodeSize >= 2 && */pAgent->m_bComplete )
		{
			int p = pAgent->m_AgentTypeNo ;
			p = 0;

			//if(m_ZoneIDToNodeNoMap.find(pAgent->m_o_ZoneID)== m_ZoneIDToNodeNoMap.end())
			//	continue;

			//if (m_ZoneIDToNodeNoMap.find(pAgent->m_d_ZoneID) == m_ZoneIDToNodeNoMap.end())
			//	continue;

			int OrgNo = m_ZoneID2ZoneNoMap[pAgent->m_o_ZoneID];
			int DesNo = m_ZoneID2ZoneNoMap[pAgent->m_d_ZoneID];

			m_ODMOEMatrix[p][OrgNo][DesNo].TotalAgentSize+= pAgent->m_Volume;
			m_ODMOEMatrix[p][OrgNo][DesNo].TotalTravelTime += (pAgent->m_ArrivalTime-pAgent->m_DepartureTime);
			m_ODMOEMatrix[p][OrgNo][DesNo].TotalDistance += pAgent->m_Distance;



		}
	}

	m_PreviousODSize = m_ODSize;

}


void CTLiteDoc::OnSensortoolsConverttoHourlyVolume()
{
	SensortoolsConverttoHourlyVolume();
	OpenCSVFileInExcel( m_ProjectDirectory +"export_validation_results.csv");
}
void CTLiteDoc::SensortoolsConverttoHourlyVolume()
{
}

int CTLiteDoc::SelectLink(GDPoint point, double& final_matching_distance)
{
	double Min_distance  = 99; // at least interset with a link

	int SelectedLinkNo = -1;

	for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink !=m_LinkSet.end(); iLink++)
	{

		for(int si = 0; si < (*iLink) ->m_ShapePoints .size()-1; si++)
		{

			GDPoint p0 = point;
			GDPoint pfrom =  (*iLink)->m_ShapePoints[si];
			GDPoint pto = (*iLink)->m_ShapePoints[si+1];;

			float distance = g_GetPoint2LineDistance(p0, pfrom, pto, m_UnitDistance);

			if(distance >0 && distance < Min_distance && (*iLink)->m_DisplayLinkID <0)  // not selected
			{

				m_SelectedLinkNo = (*iLink)->m_LinkNo ;
				Min_distance = distance;
				SelectedLinkNo = (*iLink)->m_LinkNo ;
			}
		}
	}

	final_matching_distance = Min_distance;

	return SelectedLinkNo;
}



void CTLiteDoc::OnTrafficcontroltoolsTransfermovementdatafromreferencenetworktocurrentnetwork()
{

	MapSignalDataAcrossProjects();
}

void CTLiteDoc::OnDemandtoolsGenerateinput()
{

}

bool CTLiteDoc::ReadAgentTrajectory(LPCTSTR lpszFileName)
{
	CCSVParser parser;
	int i= 0;

	bool bRectInitialized = false;

	if (parser.OpenCSVFile(CString2StdString(lpszFileName)))
	{

			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->m_bShowLayerMap[layer_Trajectory] = false;

			string agent_id;
			string timestamp;

			std::map<string, int> agent_id_str_map;
			std::map<string, int> timestamp_str_map;


			while (parser.ReadRecord())
			{

				AgentLocationRecord element;


				if (parser.GetValueByFieldName("agent_id", agent_id) == false)
					continue;


					element.agent_id = agent_id;
				 

				DTAAgent* pAgent = 0;

				if (m_AgentIDMap.find(element.agent_id) != m_AgentIDMap.end())
				{
					pAgent = m_AgentIDMap[element.agent_id];
					pAgent->m_bComplete = true;
				}
				else
				{
					pAgent = new DTAAgent();
					m_AgentIDMap[element.agent_id] = pAgent;
				}

				pAgent->m_AgentID = element.agent_id;

				parser.GetValueByFieldName("o_zone_id", pAgent->m_o_ZoneID);
				parser.GetValueByFieldName("d_zone_id", pAgent->m_d_ZoneID);

				m_ZoneMap[pAgent->m_o_ZoneID].m_OriginAgentSize += 1;
			m_ZoneMap[pAgent->m_d_ZoneID].m_OriginAgentSize += 1;

			if (m_ZoneID2ZoneNoMap.find(pAgent->m_o_ZoneID) == m_ZoneID2ZoneNoMap.end())  // not defined
			{
				int m_ZoneNoSize = m_ZoneID2ZoneNoMap.size();
				m_ZoneID2ZoneNoMap[pAgent->m_o_ZoneID] = m_ZoneNoSize;
				m_ZoneNo2ZoneIDMap[m_ZoneNoSize] = pAgent->m_o_ZoneID;
			}

			if (m_ZoneID2ZoneNoMap.find(pAgent->m_d_ZoneID) == m_ZoneID2ZoneNoMap.end())  // not defined
			{
				int m_ZoneNoSize = m_ZoneID2ZoneNoMap.size();
				m_ZoneID2ZoneNoMap[pAgent->m_d_ZoneID] = m_ZoneNoSize;
				m_ZoneNo2ZoneIDMap[m_ZoneNoSize] = pAgent->m_d_ZoneID;

			}



//				element.time_stamp_in_second = hh * 3600 + mm * 60 + ss;

				string geo_string;
				std::vector<CCoordinate> CoordinateVector;
				if (parser.GetValueByFieldName("geometry", geo_string))
				{
					// overwrite when the field "geometry" exists
					CGeometry geometry(geo_string);
					CoordinateVector = geometry.GetCoordinateList();
					if (CoordinateVector.size() >= 2)
					{
						for (int j = 0; j < CoordinateVector.size(); j++)
						{
							GDPoint	pt;
							pt.x = CoordinateVector[j].X;
							pt.y = CoordinateVector[j].Y;

							AgentLocationRecord element_point;
							//element.time_stamp_in_second = time_sequence[j] * 60;  // min to second

							element_point.agent_id = agent_id;
							element_point.x = pt.x;
							element_point.y = pt.y;
							AddLocationRecord(element_point);
							m_NetworkRect.Expand(pt);
						}
					}
				}

				i++;

			}
				std::map<string, AgentLocationTimeIndexedMap>::iterator itr2;

				for (itr2 = m_AgentWithLocationVectorMap.begin();
					itr2 != m_AgentWithLocationVectorMap.end(); itr2++)
				{		//scan all Agent records at this timestamp

					DTAAgent* pAgent = 0;

					if (m_AgentIDMap.find(itr2->first) != m_AgentIDMap.end())
					{
						pAgent = m_AgentIDMap[itr2->first];

						pAgent->m_Distance = 10; // km to miles


						pAgent->m_DepartureTime = itr2->second.AgentLocationRecordVector[0].time_stamp_in_second / 60;  // 60 seconds per min
						pAgent->m_ArrivalTime = itr2->second.AgentLocationRecordVector[itr2->second.AgentLocationRecordVector.size() - 1].time_stamp_in_second / 60;


						pAgent->m_TripTime = pAgent->m_ArrivalTime - pAgent->m_DepartureTime;

						if (g_Simulation_Time_Horizon < pAgent->m_ArrivalTime)
							g_Simulation_Time_Horizon = pAgent->m_ArrivalTime;

						pAgent->m_bComplete = true;
						pAgent->m_AgentType = 1;


					}

			
				}
	
		m_AgentLocationLoadingStatus.Format("%d agent location records for %d agents are loaded from file %s.",i,m_AgentWithLocationVectorMap.size(),lpszFileName);	
	}
return true;
}





void CTLiteDoc::OnSubareaCreatezonefromsubarea()
{
	int zone_number = -1;  // starting default number 


//	PushBackNetworkState();


	Modify ();
	if(m_SubareaNodeSet.size()>0 )
	{
		m_PreviousODSize  = -1; // require to regenerate od zone vector

		DTANode* pNode = m_SubareaNodeSet[0];



		if(m_ZoneMap.find(pNode->m_NodeID)==m_ZoneMap.end())
		{
			zone_number = pNode->m_NodeID ;

			m_ZoneMap [zone_number].m_ZoneID = zone_number;

		}else
		{  // Zone has been defined. we can just update zone boundary

			// this node has a zone Id already, we just update their zone boundary 
			if(m_ZoneMap.find(pNode->m_ZoneID)!=m_ZoneMap.end())
			{

				zone_number = pNode->m_ZoneID;
				m_ZoneMap [zone_number].m_ShapePoints.clear ();

				for (unsigned int sub_i= 0; sub_i < m_SubareaShapePoints.size(); sub_i++)
				{
					m_ZoneMap [zone_number].m_ShapePoints .push_back (m_SubareaShapePoints[sub_i]);

				}



			}


			return;
		}

	}
	if(zone_number==-1)
	{ // find largest zone number for the new zone


		std::map<int, DTAZone>	:: const_iterator itr;

		for(itr = m_ZoneMap.begin(); itr != m_ZoneMap.end(); itr++)
		{
			if( zone_number <= itr->first)
				zone_number = itr->first +1;

		}

		if(zone_number==-1)  // no zone has been defined yet
			zone_number = 1; 

		m_ZoneMap [zone_number].m_ZoneID = max(1,zone_number);

	}


	for (unsigned int sub_i= 0; sub_i < m_SubareaShapePoints.size(); sub_i++)
	{
		m_ZoneMap [zone_number].m_ShapePoints .push_back (m_SubareaShapePoints[sub_i]);

	}


	// add activity locations if there is no activity location being assigned yet
	m_SubareaShapePoints.clear();
	UpdateAllViews(0);
}


bool CTLiteDoc::FindObject(eSEARCHMODE SearchMode, int value1, int value2)
{

	if(SearchMode == efind_link)
	{

		DTALink* pLink = FindLinkWithNodeIDs(value1 ,value2 );

		if(pLink !=NULL)
		{
			m_SelectedLinkNo = pLink->m_LinkNo ;
			m_SelectedNodeNo = -1;

			ZoomToSelectedLink(pLink->m_LinkNo);


		}
	}

	if(SearchMode == efind_node)
	{
		DTANode* pNode = FindNodeWithNodeID (value1);
		if(pNode !=NULL)
		{
			m_SelectedLinkNo = -1;
			m_SelectedNodeNo = pNode->m_NodeNo ;

			ZoomToSelectedNode(value1);

		} return false;
	}

	if(SearchMode == efind_path)
	{

		DTANode* pFromNode = FindNodeWithNodeID (value1);
		if(pFromNode ==NULL)
		{
			CString str;
			str.Format ("From Node %d cannot be found.",value1);
			AfxMessageBox(str);
			return false; 
		}else
		{
			m_ONodeNo = pFromNode->m_NodeNo;

		}
		DTANode* pToNode = FindNodeWithNodeID (value2);
		if(pToNode ==NULL)
		{
			CString str;
			str.Format ("To Node %d cannot be found.",value2);
			AfxMessageBox(str);
			return false;
		}else
		{
			m_DNodeNo = pToNode->m_NodeNo;
		}

		Routing(false);

	}

	if(SearchMode == efind_Agent)
	{
		//string SelectedAgentID = value1; // internal Agent index starts from zero
		//std::vector<int> LinkVector;

		//if(m_AgentIDMap.find(SelectedAgentID) == m_AgentIDMap.end())
		//{
		//	if(SelectedAgentID>=0)
		//	{
		//		CString str_message;
		//		str_message.Format ("Agent Id %d cannot be found.", SelectedAgentID+1);
		//		AfxMessageBox(str_message);
		//	}

		//	m_SelectedAgentID = -1;
		//	HighlightPath(LinkVector,1);

		//	return true;
		//}

		//m_SelectedAgentID = SelectedAgentID;

		//DTAAgent* pAgent = m_AgentIDMap[SelectedAgentID];

		//CPlayerSeekBar m_wndPlayerSeekBar;


		//// set departure time to the current time of display
		//g_Simulation_Time_Stamp = pAgent->m_DepartureTime +1;

		//CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();

		//pMainFrame->m_wndPlayerSeekBar.SetPos(g_Simulation_Time_Stamp);

		//m_LinkMOEMode = MOE_Agent;

		//for(int link= 1; link<pAgent->m_NodeSize; link++)
		//{
		//	LinkVector.push_back (pAgent->m_NodeAry[link].LinkNo);
		//}

		//HighlightPath(LinkVector,1);

		//m_HighlightGDPointVector.clear();

		//m_HighlightGDPointVector = pAgent->m_GPSLocationVector ; // assign the GPS points to be highlighted

	}
	UpdateAllViews(0);

	return true;
}


void CTLiteDoc::OnTrafficcontroltoolsTransfersignaldatafromreferencenetworktocurrentnetwork()
{
	// step 0: make sure two networks have been opened

	CTLiteDoc* pReferenceDoc = NULL;
	std::list<CTLiteDoc*>::iterator iDoc = g_DocumentList.begin ();
	int project_index = 0 ;
	while (iDoc != g_DocumentList.end())
	{
		if((*iDoc)->m_NodeSet.size() >0 && (*iDoc)!=this)  
		{

			pReferenceDoc = (*iDoc);

			project_index++;
		}
		iDoc++;
	}

	if(pReferenceDoc == NULL)
	{

		AfxMessageBox("Please open the reference network data set in anoter window.");
		return;
	}

	int count_node = 0 ;
	int count_link = 0 ;
	std::list<DTANode*>::iterator  iNode;


	FILE* st = NULL;

	CString str_log_file;
	str_log_file.Format ("%s\\signal_data_reading_log.csv",m_ProjectDirectory);

	fopen_s(&st,str_log_file,"w");


	if( st !=NULL)
	{


		//	fprintf(st,"intersection_name1,intersection_name2, baseline_node_id,reference_node_id,base_line_cycle_length_in_second,reference_cycle_length_in_second\n");


		fprintf(st,"------\n");
		fprintf(st,"Step 2: Read Cycle length and offset,\n");

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			int baseline_node_id = (*iNode)->m_NodeID  ;
			if((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal)
			{

				DTANode* pRefNode = NULL;
				int reference_node_number =  (*iNode)->m_NodeID;



			}

		}

		// step 2: fetch data for all links
		for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			DTALink* pThisLink = (*iLink);
			DTALink* pReferenceLink =  pReferenceDoc->FindLinkWithNodeIDs (pThisLink->m_FromNodeID ,pThisLink->m_ToNodeID );

			DTANode* pDownstreamNode = m_NodeIDMap[pThisLink->m_ToNodeID ];

			if(pReferenceLink!=NULL && 
				(pDownstreamNode->m_ControlType == m_ControlType_PretimedSignal || pDownstreamNode->m_ControlType == m_ControlType_ActuatedSignal))
			{
				DTANode* pRefNode = pReferenceDoc->m_NodeIDMap[pThisLink->m_ToNodeID ];

				if(pRefNode->m_ControlType == pReferenceDoc->m_ControlType_PretimedSignal || pRefNode->m_ControlType == pReferenceDoc->m_ControlType_ActuatedSignal)
				{

					fprintf(st,"Link,%d->%d,  Old # of left turn lanes,%d, Updated of left turn lanes,%d,Old # of right turn lanes,%d, Updated of right turn lanes,%d\n", 
						pThisLink->m_FromNodeID,pThisLink->m_ToNodeID,
						pThisLink->m_NumberOfLeftTurnLanes,
						pReferenceLink->m_NumberOfLeftTurnLanes,
						pThisLink->m_NumberOfRightTurnLanes ,
						pReferenceLink->m_NumberOfRightTurnLanes
						);

					pThisLink->m_NumberOfLeftTurnLanes   = pReferenceLink->m_NumberOfLeftTurnLanes;
					pThisLink->m_NumberOfRightTurnLanes    = pReferenceLink->m_NumberOfRightTurnLanes;


					count_link++;
				}
			}
		}


		fclose(st);
	}else
	{
		CString str;
		str.Format("File %s cannot be opened.", str_log_file);
		AfxMessageBox(str);
	}


	OpenCSVFileInExcel(str_log_file);

	CString str;
	str.Format("%d nodes and %d links have obtained cycle length and signal timing from the reference network. To accept the changes, please save the network.",count_node, count_link);
	AfxMessageBox(str, MB_ICONINFORMATION);

}

void CTLiteDoc::OnImportBackgroundimage()
{

}

void CTLiteDoc::OnZoneDeletezone()
{
	Modify ();

	m_ZoneMap.erase (this->m_SelectedZoneID );
	this->m_SelectedZoneID = -1;
	UpdateAllViews(0);


}





void CTLiteDoc::OnNodeViewnodedata()
{
	if(m_ProjectDirectory.GetLength()==0)
	{
		AfxMessageBox("The project has not been loaded.");
		return;
	}
	CNetworkDataSettingDlg dlg;
	dlg.m_SelectTab = _NODE_DATA; 
	dlg.m_pDoc = this;

	if(m_SelectedNodeNo>= 0 )
	{
		dlg.m_SelectRowByUser  = m_SelectedNodeNo;

	}
	dlg.DoModal();
}

void CTLiteDoc::OnLinkViewlinkdata()
{
	if(m_ProjectDirectory.GetLength()==0)
	{
		AfxMessageBox("The project has not been loaded.");
		return;
	}
	CNetworkDataSettingDlg dlg;
	dlg.m_SelectTab = _LINK_DATA; 
	dlg.m_pDoc = this;

	dlg.m_SelectRowByUser =  m_SelectedLinkNo;
	dlg.DoModal();


}

void CTLiteDoc::OnMovementViewmovementdatatable()
{
	if(m_ProjectDirectory.GetLength()==0)
	{
		AfxMessageBox("The project has not been loaded.");
		return;
	}
	CNetworkDataSettingDlg dlg;
	dlg.m_SelectTab = _MOVEMENT_DATA; 
	dlg.m_pDoc = this;
	dlg.DoModal();
}

void CTLiteDoc::OnOdmatrixOddemandmatrix()
{
	// TODO: Add your command handler code here
}

void CTLiteDoc::OnWorkzoneViewworkzonedata()
{

}

void CTLiteDoc::OnVmsViewvmsdatatable()
{

}

void CTLiteDoc::OnTollViewtolldatatable()
{

}

void CTLiteDoc::OnDetectorViewsensordatatable()
{
}



void CTLiteDoc::OnLinkmoeExportlinkmoedatatoshapefile()
{

}


void CTLiteDoc::OnNodeExportnodelayertogisshapefile()
{

}

void CTLiteDoc::OnZoneExportzonelayertogisshapefile()
{
}

void CTLiteDoc::OnGridUsemileasunitoflength()
{
	m_bUseMileVsKMFlag = true;
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateGridUsemileasunitoflength(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bUseMileVsKMFlag == true);
}

void CTLiteDoc::OnGridUsekmasunitoflength()
{
	m_bUseMileVsKMFlag = false;
	UpdateAllViews(0);

}

void CTLiteDoc::OnUpdateGridUsekmasunitoflength(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bUseMileVsKMFlag == false);
}

void CTLiteDoc::OnGridUselong()
{
	m_LongLatFlag = !m_LongLatFlag;
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateGridUselong(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LongLatFlag);
}








void CTLiteDoc::OnShowMoePathlist()
{
	m_bShowPathList = true;
	ShowPathListDlg(m_bShowPathList);}


void CTLiteDoc::OnHelpReportbug()
{
	g_OpenDocument("http://code.google.com/p/nexta/issues/list", SW_SHOW);
}







int CTLiteDoc:: FindNonCentroidNodeIDWithCoordinate(double x, double y, int this_node_name)
{

	DTANode* pNode= NULL;

	double min_distance = 999999;
	int NodeNo = -1;
	for (std::list<DTANode*>::iterator  iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{

		if((*iNode)->m_NodeID  != this_node_name && (*iNode)->m_ZoneID <=0)  // non centroid node
		{
			double distance = sqrt( ((*iNode)->pt.x - x)*((*iNode)->pt.x - x) + ((*iNode)->pt.y - y)*((*iNode)->pt.y - y));
			if( distance <  min_distance)
			{
				min_distance= distance;
				pNode = (*iNode);
			}
		}
	}
	if(pNode != NULL)
		return pNode->m_NodeID;
	else
		return NULL;
}





void CTLiteDoc::OnMovementHidenon()
{
	m_hide_non_specified_movement_on_freeway_and_ramp = !m_hide_non_specified_movement_on_freeway_and_ramp;
}

void CTLiteDoc::OnUpdateMovementHidenon(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_hide_non_specified_movement_on_freeway_and_ramp);
}


void CTLiteDoc::OnUpdateZoneChangezonenumber(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (m_SelectedZoneID>=1);

}

void CTLiteDoc::OnChangelinktypecolorFreeway()
{

	CColorDialog dlg(theApp.m_FreewayColor, CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_FreewayColor= dlg.GetColor();
		UpdateAllViews(0);

		FileSaveColorScheme();
	}


}

void CTLiteDoc::OnChangelinktypecolorRamp()
{
	CColorDialog dlg(theApp.m_RampColor, CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_RampColor= dlg.GetColor();
		UpdateAllViews(0);
		FileSaveColorScheme();

	}
}

void CTLiteDoc::OnChangelinktypecolorArterial()
{
	CColorDialog dlg(theApp.m_ArterialColor, CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_ArterialColor= dlg.GetColor();
		UpdateAllViews(0);
		FileSaveColorScheme();

	}
}

void CTLiteDoc::OnChangelinktypecolorConnector()
{
	CColorDialog dlg(theApp.m_ConnectorColor, CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_ConnectorColor= dlg.GetColor();
		UpdateAllViews(0);
		FileSaveColorScheme();

	}
}

void CTLiteDoc::OnChangelinktypecolorTransit()
{
	CColorDialog dlg(theApp.m_TransitColor, CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_TransitColor= dlg.GetColor();
		UpdateAllViews(0);
		FileSaveColorScheme();

	}
}

void CTLiteDoc::OnChangelinktypecolorWalkingmode()
{
	CColorDialog dlg(theApp.m_WalkingColor, CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_WalkingColor= dlg.GetColor();
		UpdateAllViews(0);
		FileSaveColorScheme();

	}
}

void CTLiteDoc::OnChangelinktypecolorResettodefaultcolorschema()
{

	theApp.m_FreewayColor = RGB(030,144,255);
	theApp.m_RampColor = RGB(160,032,240); 
	theApp.m_ArterialColor = RGB(034,139,034);
	theApp.m_ConnectorColor = RGB(255,165,000);
	theApp.m_TransitColor = RGB(255,0,255);
	theApp.m_WalkingColor = RGB(127,255,0);
	theApp.m_ParkingandRideColor = RGB(0, 255, 0);
}

void CTLiteDoc::OnNodeChangenodecolor()
{
	CColorDialog dlg(theApp.m_NodeColor , CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_NodeColor= dlg.GetColor();
		UpdateAllViews(0);
		FileSaveColorScheme();

	}

}

void CTLiteDoc::OnNodeChangenodebackgroundcolor()
{
	CColorDialog dlg(theApp.m_NodeBrushColor , CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_NodeBrushColor= dlg.GetColor();
		UpdateAllViews(0);
		FileSaveColorScheme();

	}
}

void CTLiteDoc::OnZoneChangezonecolor()
{
	CColorDialog dlg(theApp.m_ZoneColor , CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_ZoneColor= dlg.GetColor();
		UpdateAllViews(0);
		FileSaveColorScheme();

	}

}

void CTLiteDoc::OnEditUndo33707()
{
	Undo();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateEditUndo33707(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_NetworkState.size()>0);
}

void CTLiteDoc::OnEditRedo33709()
{
	Redo();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateEditRedo33709(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_RedoNetworkState.size()>0);
}
void CTLiteDoc::PushBackNetworkState()
{
	CWaitCursor wait;

	m_RedoNetworkState.clear();

	NetworkState element;

	element.l_NodeNoMap = m_NodeNoMap;
	element.l_NodeIDMap = m_NodeNoMap;
	element.l_LinkNoMap = m_LinkNoMap;
	element.l_NodeIDtoLinkMap = m_NodeIDtoLinkMap;
	

	element.l_NodeSet = m_NodeSet;
	element.l_LinkSet = m_LinkSet;

	element.l_SubareaNodeSet = m_SubareaNodeSet;
	element.l_SubareaLinkSet = m_SubareaLinkSet;
	element.l_ZoneMap =  m_ZoneMap;

	element.l_NodeIDtoNodeNoMap = m_NodeIDtoNodeNoMap;
	element.l_NodeNotoZoneNameMap = m_NodeIDtoZoneNameMap ;
	element.l_SubareaShapePoints = m_SubareaShapePoints;


	m_NetworkState.push_back(element);

	if(m_NetworkState.size()>=20)  // no more than 20 steps in undo
	{
		m_NetworkState.erase (m_NetworkState.begin ());

	}



}
void CTLiteDoc::Undo()
{

	NetworkState current_element;

	current_element.l_NodeSet.clear ();

	current_element.l_NodeNoMap = m_NodeNoMap;
	current_element.l_NodeIDMap = m_NodeNoMap;
	current_element.l_LinkNoMap = m_LinkNoMap;
	current_element.l_NodeIDtoLinkMap = m_NodeIDtoLinkMap;
	

	current_element.l_NodeSet = m_NodeSet;
	current_element.l_LinkSet = m_LinkSet;

	current_element.l_SubareaNodeSet = m_SubareaNodeSet;
	current_element.l_SubareaLinkSet = m_SubareaLinkSet;
	current_element.l_ZoneMap =  m_ZoneMap;

	current_element.l_NodeIDtoNodeNoMap = m_NodeIDtoNodeNoMap;
	current_element.l_NodeNotoZoneNameMap = m_NodeIDtoZoneNameMap ;
	current_element.l_SubareaShapePoints = m_SubareaShapePoints;
	TRACE("\n%d links", current_element.l_LinkSet.size());

	m_RedoNetworkState.push_back(current_element);		

	if(m_NetworkState.size() >0)
	{
		CWaitCursor wait;
		NetworkState element =  m_NetworkState[m_NetworkState.size ()-1];


		m_NodeSet = element.l_NodeSet;
		m_LinkSet = element.l_LinkSet;

		m_SubareaNodeSet = element.l_SubareaNodeSet;
		m_SubareaLinkSet = element.l_SubareaLinkSet;

		m_ZoneMap = element.l_ZoneMap ;

		m_NodeIDtoNodeNoMap = element.l_NodeIDtoNodeNoMap;
		m_NodeIDtoZoneNameMap = element.l_NodeNotoZoneNameMap;

		m_NodeNoMap = element.l_NodeNoMap;
		m_NodeNoMap = element.l_NodeIDMap;
		m_LinkNoMap = element.l_LinkNoMap;
		m_NodeIDtoLinkMap = element.l_NodeIDtoLinkMap;
		m_LinkIDtoLinkMap = element.l_LinkIDtoLinkMap;
		m_SubareaShapePoints = element.l_SubareaShapePoints;

		m_NetworkState.pop_back ();

		TRACE("\nRedo: %d links", element.l_LinkSet.size());

		//	m_RedoNetworkState.push_back (element);




		UpdateAllViews(0);
	}


}

void CTLiteDoc::Redo()
{	

	if(m_RedoNetworkState.size() >0)
	{
		CWaitCursor wait;

		NetworkState current_element;

		current_element.l_NodeSet.clear ();

		current_element.l_NodeNoMap = m_NodeNoMap;
		current_element.l_NodeIDMap = m_NodeNoMap;
		current_element.l_LinkNoMap = m_LinkNoMap;
		current_element.l_NodeIDtoLinkMap = m_NodeIDtoLinkMap;
		current_element.l_LinkIDtoLinkMap = m_LinkIDtoLinkMap;


		current_element.l_NodeSet = m_NodeSet;
		current_element.l_LinkSet = m_LinkSet;

		current_element.l_SubareaNodeSet = m_SubareaNodeSet;
		current_element.l_SubareaLinkSet = m_SubareaLinkSet;
		current_element.l_ZoneMap =  m_ZoneMap;

		current_element.l_NodeIDtoNodeNoMap = m_NodeIDtoNodeNoMap;
		current_element.l_NodeNotoZoneNameMap = m_NodeIDtoZoneNameMap ;
		current_element.l_SubareaShapePoints = m_SubareaShapePoints;

		m_NetworkState.push_back (current_element);

		NetworkState element =  m_RedoNetworkState[m_RedoNetworkState.size()-1];

		TRACE("\nRedo to %d links", element.l_LinkSet.size());

		m_NodeSet = element.l_NodeSet;
		m_LinkSet = element.l_LinkSet;

		m_SubareaNodeSet = element.l_SubareaNodeSet;
		m_SubareaLinkSet = element.l_SubareaLinkSet;

		m_ZoneMap = element.l_ZoneMap ;

		m_NodeIDtoNodeNoMap = element.l_NodeIDtoNodeNoMap;
		m_NodeIDtoZoneNameMap = element.l_NodeNotoZoneNameMap;

		m_NodeNoMap = element.l_NodeNoMap;
		m_NodeNoMap = element.l_NodeIDMap;
		m_LinkNoMap = element.l_LinkNoMap;
		m_NodeIDtoLinkMap = element.l_NodeIDtoLinkMap;
		m_LinkIDtoLinkMap = element.l_LinkIDtoLinkMap;
		m_SubareaShapePoints = element.l_SubareaShapePoints;



		m_RedoNetworkState.pop_back ();

		TRACE("\n%d links", element.l_LinkSet.size());


		UpdateAllViews(0);
	}


}


void CTLiteDoc::GenerateMovementShapePoints()
{

	std::list<DTANode*>::iterator iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		DTANode* pNode = (*iNode);

		double size_ratio= m_MovementTextBoxSizeInDistance/250;

		double node_set_back = size_ratio*150 * m_UnitDistance;

		double link_length = size_ratio*250 * m_UnitDistance;
		double movement_length = size_ratio*100 * m_UnitDistance;
		double lane_width = size_ratio*10 * m_UnitDistance;


		double text_length =  m_MovementTextBoxSizeInDistance * 0.15 *  m_UnitDistance ;

		std::map<CString, double> Turn_Degree_map;

		for (unsigned int i=0;i< pNode->m_MovementVector .size();i++)
		{
			DTANodeMovement movement = pNode->m_MovementVector[i];

			pNode->m_MovementVector[i].m_ShapePoints .clear ();


			if(pNode->m_ControlType != m_ControlType_PretimedSignal && 
				pNode->m_ControlType != m_ControlType_ActuatedSignal)
				continue;


			if(m_LinkNoMap.find(movement.IncomingLinkNo) ==  m_LinkNoMap.end())
				return;

			if(m_LinkNoMap.find(movement.OutgoingLinkNo) ==  m_LinkNoMap.end())
				return;

			DTALink* pInLink  = m_LinkNoMap [movement.IncomingLinkNo];
			DTALink* pOutLink  = m_LinkNoMap [movement.OutgoingLinkNo ];

			int destination_node  = pOutLink->m_ToNodeID; 


			GDPoint p1, p2, p3, p_text;
			// 1: fetch all data
			p1  = m_NodeIDMap[movement.in_link_from_node_id ]->pt;
			p2  = m_NodeIDMap[movement.in_link_to_node_id ]->pt;
			p3  = m_NodeIDMap[movement.out_link_to_node_id]->pt;

			// reserved direction
			double DeltaX = p2.x - p1.x ;
			double DeltaY = p2.y - p1.y ;
			double theta = atan2(DeltaY, DeltaX);

			double movement_approach_turnection_theta = atan2(p3.y - p1.y, p3.x - p1.x);


			GDPoint p1_new, p2_new, p3_new;
			GDPoint p1_text, p3_text;
			GDPoint pt_movement[3];


			// 2. set new origin
			p2_new.x = pNode->pt.x +(-1)*node_set_back*cos(theta);  
			p2_new.y = pNode->pt.y + (-1)*node_set_back*sin(theta);

			p1_new.x = pNode->pt.x +(-1)*link_length*cos(theta);
			p1_new.y = pNode->pt.y + (-1)*link_length*sin(theta);

			p_text.x = p1_new.x - text_length*cos(theta);
			p_text.y = p1_new.y - text_length*sin(theta);


			// 3 determine the control point for  PolyBezier
			double lane_width =  size_ratio*3*m_LaneWidthInKM * m_UnitDistance;
			double movement_offset = 0;
			double text_offset = 0.7*lane_width;
			int mid_lane_number = 4+ pInLink->m_NumberOfLanes / 2;
			float control_point_ratio = 0;

			float MovementTextFontSize = 8;

			CString movement_approach_turnection_label;
			movement_approach_turnection_label.Format ("%d,%s", movement.in_link_from_node_id , GetTurnString(movement.movement_turn));

			if(movement.movement_turn == DTA_Through ) 
			{

				if(Turn_Degree_map.find (movement_approach_turnection_label ) != Turn_Degree_map.end())
				{  // this direction has been used/defined.

					if( Turn_Degree_map[movement_approach_turnection_label ] <  movement_approach_turnection_theta)
						movement_offset = lane_width * (mid_lane_number - 0.7);
					else
						movement_offset = lane_width * (mid_lane_number + 0.7);

				}else
				{
					movement_offset = lane_width * mid_lane_number;

				}

			}else if(movement.movement_turn == DTA_LeftTurn || movement.movement_turn == DTA_LeftTurn2 ) 
			{
				movement_offset = lane_width * (mid_lane_number-1);
			}else
			{
				movement_offset = lane_width * (mid_lane_number+1);
			}

			// keep a record
			Turn_Degree_map[movement_approach_turnection_label] = movement_approach_turnection_theta;


			GDPoint pt_from, pt_to, pt_text;
			pt_from.x = p1_new.x + movement_offset* cos(theta-PI/2.0f);
			pt_from.y = p1_new.y + movement_offset* sin(theta-PI/2.0f);


			double alpha  = 0.3;

			pt_to.x  = p2_new.x + movement_offset* cos(theta-PI/2.0f);
			pt_to.y  =  p2_new.y + movement_offset* sin(theta-PI/2.0f);

			// direction

			DeltaX = p2.x - p3.x ;
			DeltaY = p2.y - p3.y ;
			theta = atan2(DeltaY, DeltaX);


			if(movement.movement_turn == DTA_Through ) 
			{
				p3_new.x = pt_to.x - movement_length * cos(theta);
				p3_new.y = pt_to.y - movement_length * sin(theta);

			}else if(movement.movement_turn == DTA_LeftTurn ) 
			{
				p3_new.x = pt_to.x - movement_length * cos(theta -PI/4.0f);
				p3_new.y = pt_to.y - movement_length * sin(theta -PI/4.0f);
			}else
			{
				p3_new.x = pt_to.x - movement_length * cos(theta +PI/4.0f);
				p3_new.y = pt_to.y - movement_length * sin(theta +PI/4.0f);
			}

			// 		GDPoint p1_new, p2_new, p3_new;
			pNode->m_MovementVector[i].m_ShapePoints.push_back(p1_new);
			pNode->m_MovementVector[i].m_ShapePoints.push_back(p2_new);
			pNode->m_MovementVector[i].m_ShapePoints.push_back(p3_new);

		}  // per movement
	} // per node

}
void CTLiteDoc::OnMovementSetupnumberofleftturnlanesforsignalizednodes()
{


}

void CTLiteDoc::OnMovementOptimizephasingandtimingdataforalltrafficsignalsthroughqem()
{
}


void CTLiteDoc::OnLinkmoedisplayBottleneck()
{
	m_LinkMOEMode = MOE_bottleneck;
	ShowLegend(true);
	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}

void CTLiteDoc::OnUpdateLinkmoedisplayBottleneck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_LinkMOEMode == MOE_bottleneck);
}
float CTLiteDoc::GetNodeTotalDelay(int ToNodeID, int time, int& LOS)
{
	float total_delay = 0;
	DTANode* pToNode = m_NodeIDMap[ToNodeID];

	int RelativeStartTime = 9999;
	for(unsigned int i = 0; i< pToNode->m_IncomingLinkVector.size(); i++)
	{

		DTALink* pLink = m_LinkNoMap[pToNode->m_IncomingLinkVector[i]];
		total_delay+= pLink->GetDynamicNodeDelay(time );

		RelativeStartTime = min(RelativeStartTime, pLink->GetImpactRelativeStartTime(g_Simulation_Time_Stamp ));

	}
	float power = RelativeStartTime*1.0/g_ImpactStudyPeriodInMin*100;
	if(power>100)
		power = 100;

	LOS = GetLOSCode(power);

	return total_delay/max(1,pToNode->m_IncomingLinkVector.size());

}



void CTLiteDoc::OnGisplanningdatasetConfigureimportingsettingfile()
{

	//CDlg_DataImportWizard dlgWizard;
	//dlgWizard.DoModal();

	AfxMessageBox("NEXTA 64-bit version does not support shape file importing function. Please use NEXTA_32.exe. ");
	return;



	// TODO: Add your command handler code here
}


void CTLiteDoc::OnSubareaExporttotalnumberofAgentsinsubarea()
{

	CString file_name  = m_ProjectDirectory+"Export_total_number_of_Agents_in_subarea.csv";
	FILE* pFile;
	fopen_s(&pFile,file_name,"w");
	if(pFile!=NULL)
	{
		fprintf(pFile, "time_in_min,time_clock,total_number_of_Agents_in_subarea,avg_speed_in_mile,avg_speed_in_km\n");
		for(int t = 0; t< 1440; t++)
		{

			float total_number_of_Agents = 0;

			float total_speed_value = 0;

				for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
			{
				if((*iLink)->m_bIncludedinSubarea && m_SubareaShapePoints.size()>=3)
				{

					float number_of_Agents = (*iLink)->GetSimulatedDensity (t)*(*iLink)->m_Length * (*iLink)->m_NumberOfLanes;
				
					total_number_of_Agents += number_of_Agents;
					total_speed_value+= (*iLink)->GetTDSpeed (t) * number_of_Agents;
				
				}


			}

				float avg_speed = total_speed_value/max(1,total_number_of_Agents);
				float avg_speed_in_km = avg_speed*1.609344;


				fprintf(pFile,"%d,%s,%.0f,%.2f,%.2f\n",t,GetTimeStampString24HourFormat (t),total_number_of_Agents,avg_speed,avg_speed_in_km);

		}

		fclose(pFile);

	this->OpenCSVFileInExcel (file_name);

	}

}



void CTLiteDoc::OnToolsConfiguration()
{

}








void CTLiteDoc::OnPathClearallpathdisplay()
{
	for (unsigned int p = 0; p < m_PathDisplayList.size(); p++) // for each path
	{
		m_PathDisplayList[p].m_LinkVector.clear();
	}
	m_PathDisplayList.clear();

	m_ONodeNo = -1;
	m_DNodeNo = -1;

	UpdateAllViews(0);
}



void CTLiteDoc::SaveTimingData()
{
	CString directory;
	directory = m_ProjectFile.Left(m_ProjectFile.ReverseFind('\\') + 1);

	CString AMS_File = directory + "input_timing.csv";
	FILE* st = NULL;

	fopen_s(&st, AMS_File, "w");
	if (st != NULL)
	{
		fprintf(st, "int_id,timing_plan_no,phase_id,next_phase_id,green_duration,movement_str,movement_dir_str,movement_sim_hourly_volume_delay_in_sec,\n");

		// print out matrix 
		for (std::list<DTANode*>::iterator iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{
			if (((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal))
			{


				CString timing_plan_name;

				int p;
				for (int timeplan = 1; timeplan < 20; timeplan++)
				{

					timing_plan_name.Format("%d", timeplan);

					for (p = 1; p < 20; p++)
					{

						//refetch data 
						DTA_Phasing_Data_Matrix element = GetPhaseData((*iNode)->m_NodeID);

						if (element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + p), PHASE_GreenDuration).GetLength() > 0 ||
							element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + p), PHASE_MOVEMENT_VECTOR).GetLength() > 0
							)
						{
							fprintf(st, "%d,%d,%d,%s,%s,%s,%s,",
								(*iNode)->m_NodeID,
								timeplan, p,
								element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + p), PHASE_NextPhaseNo).GetString(),
								element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + p), PHASE_GreenDuration).GetString(),
								element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + p), PHASE_MOVEMENT_VECTOR).GetString(),
								element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + p), PHASE_MOVEMENT_DIR_VECTOR).GetString());

							string movement_str = element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + p), PHASE_MOVEMENT_VECTOR).GetString();

							CString movement_MOE_str;
							if (movement_str.size() > 0)
							{
								for (unsigned int m = 0; m < (*iNode)->m_MovementVector.size(); m++)
								{
									bool bMovementIncluded = IfMovementDirIncludedInPhase((*iNode)->m_NodeID,
										 p, m);

									if (bMovementIncluded)
									{

										CString sub_movement_str;

										float number_of_hours = max(0.01, (m_DemandLoadingEndTimeInMin - m_DemandLoadingStartTimeInMin) / 60.0);

										float sim_turn_hourly_count = (*iNode)->m_MovementVector[m].sim_turn_count / number_of_hours;

										if(sim_turn_hourly_count>1)
										{ 
										sub_movement_str.Format(";%d_%d_%s:%.1f:%.1f", m_NodeIDMap[(*iNode)->m_MovementVector[m].in_link_from_node_id]->m_NodeID,
											m_NodeIDMap[(*iNode)->m_MovementVector[m].out_link_to_node_id]->m_NodeID,
											GetTurnShortString((*iNode)->m_MovementVector[m].movement_turn),
											sim_turn_hourly_count,
											(*iNode)->m_MovementVector[m].sim_turn_delay*60);

										movement_MOE_str += sub_movement_str;
										}


									}

								}


								//
							}

							fprintf(st, "%s,", movement_MOE_str);
							fprintf(st, "\n");
						}


					} //p
				}

			}
		}

			fclose(st);
		}
	else
	{
		AfxMessageBox("File input_timing_backup.csv cannot be opened.");
	}

	}



void CTLiteDoc::OnToolsImportsynchroncombinedfile()
{
	// TODO: Add your command handler code here
}


void CTLiteDoc::OnOdmatrixRapid()
{
	CWaitCursor ws;
	m_TrafficFlowModelFlag = 0;
	m_DemandAlpha += 0.1;
	
	GenerateOffsetLinkBand();
	UpdateAllViews(0);
}


void CTLiteDoc::OnOdmatrixRapidDecrease()
{

	CWaitCursor ws;
	m_TrafficFlowModelFlag = 0;
	m_DemandAlpha -= 0.1;

	GenerateOffsetLinkBand();

	UpdateAllViews(0);
}


void CTLiteDoc::OnButtonAbm()
{

	CWaitCursor curs;
	if (!SetCurrentDirectory(m_ProjectDirectory))
	{
		if (m_ProjectDirectory.GetLength() == 0)
		{
			AfxMessageBox("The project directory has not been specified.");
			return;
		}
		CString msg;
		msg.Format("The specified project directory %s does not exist.", m_ProjectDirectory);
		AfxMessageBox(msg);
		return;
	}
	CString sCommand;
	CString strParam;
	CTime ExeStartTime = CTime::GetCurrentTime();

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	sCommand.Format("%s\\%s", pMainFrame->m_CurrentDirectory, "AgBM.exe");

	ProcessExecute(sCommand, strParam, m_ProjectDirectory, true);

}


void CTLiteDoc::OnHelpVisitdevelopmentwebsiteDtalite()
{
	g_OpenDocument("https://github.com/xzhou99/dtalite_software_release", SW_SHOW);
}





void CTLiteDoc::OnToolsSimulationsettings()
{
	OpenCSVFileInExcel(m_ProjectDirectory + "settings.csv");
}


void CTLiteDoc::OnToolsImportgtfsdata()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	CFileDialog fdlg(TRUE, "routes.txt", "routes.txt", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING,
		"GTFS (routes.txt)|*.txt|", pMainFrame, 0, true);


	if (fdlg.DoModal() == IDOK)
	{
		CString path = fdlg.GetPathName();
		CWaitCursor wait;

		CString directory;
		directory = path.Left(path.ReverseFind('\\') + 1);

//		ReadTransitFiles(directory);
		ReadTransitFiles_Leg(directory);

			CString msg;
			if (m_NodeSet.size() > 0)
			{
				msg.Format("Files node.csv and link.csv  have been successfully created with %d nodes and %d links.",
					m_NodeSet.size(),
					m_LinkSet.size());
				AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION);
			}


	}
	


}

void CTLiteDoc::OnAgentChangeagentcolor1()
{
	CColorDialog dlg(theApp.m_AgentColor[1], CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_AgentColor[1] = dlg.GetColor();
		UpdateAllViews(0);

		FileSaveColorScheme();
	}
}

void CTLiteDoc::OnAgentChangeagentcolor2()
{
	CColorDialog dlg(theApp.m_AgentColor[2], CC_FULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_AgentColor[2] = dlg.GetColor();
		UpdateAllViews(0);
		FileSaveColorScheme();
	}
}


void CTLiteDoc::OnToolsGeneratezonegrid()
{
	m_ZoneMap.clear();
	// step 1. find the map rectangle
	float grid_resolution = 0.01;
	bool bRectInitialized = false;
	m_AdjLinkSize = 0;

	for (std::list<DTANode*>::iterator iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
			if (!bRectInitialized)
			{
				m_GridRect.left = (*iNode)->pt.x;
				m_GridRect.right = (*iNode)->pt.x;
				m_GridRect.top = (*iNode)->pt.y;
				m_GridRect.bottom = (*iNode)->pt.y;
				bRectInitialized = true;
			}

			m_GridRect.Expand((*iNode)->pt);
	}

	// step 2: user input for scale
	float value = m_Doc_Resolution;

	//min scale
	float temp_resolution = max(m_GridRect.Width() / (_MAX_GRID_SIZE +2), m_GridRect.Height() / (_MAX_GRID_SIZE+2));

	// use user scale
	if(value > temp_resolution)
	temp_resolution = value;

	vector<double> ResolutionVector;

	ResolutionVector.push_back(0.001);
	ResolutionVector.push_back(0.002);
	ResolutionVector.push_back(0.005);
	ResolutionVector.push_back(0.01);
	ResolutionVector.push_back(0.02);
	ResolutionVector.push_back(0.05);
	ResolutionVector.push_back(0.1);
	ResolutionVector.push_back(0.2);
	ResolutionVector.push_back(0.5);
	ResolutionVector.push_back(1);
	ResolutionVector.push_back(2);
	ResolutionVector.push_back(5);
	ResolutionVector.push_back(10);
	ResolutionVector.push_back(20);
	ResolutionVector.push_back(50);
	ResolutionVector.push_back(100);
	ResolutionVector.push_back(200);
	ResolutionVector.push_back(500);
	ResolutionVector.push_back(1000);
	ResolutionVector.push_back(2000);
	ResolutionVector.push_back(5000);
	ResolutionVector.push_back(10000);
	ResolutionVector.push_back(20000);
	ResolutionVector.push_back(50000);
	ResolutionVector.push_back(100000);
	ResolutionVector.push_back(200000);
	ResolutionVector.push_back(500000);
	ResolutionVector.push_back(1000000);


	double ClosestResolution = 1;

	if (temp_resolution < ResolutionVector[0])
		temp_resolution = ResolutionVector[0];

	for (unsigned int i = 0; i < ResolutionVector.size()-1; i++)
	{
		if ((temp_resolution > ResolutionVector[i]+0.00001) && temp_resolution < ResolutionVector[i+1])
		{
			temp_resolution = ResolutionVector[i + 1]; // round up
			break;

		}
	}


	m_GridXStep = temp_resolution;
	m_GridYStep = temp_resolution;


	//round the rectangle

if(m_GridRect.left >=0 )
{ 
	m_GridRect.left = int(m_GridRect.left/m_GridXStep-0.99)* m_GridXStep;
	m_GridRect.right = int(m_GridRect.right / m_GridXStep+0.99) * m_GridXStep;
}
else // m_GridRect.left <=0
{
	m_GridRect.left = int(m_GridRect.left / m_GridXStep-0.99) * m_GridXStep;
	m_GridRect.right = int(m_GridRect.right / m_GridXStep) * m_GridXStep;

}

if(m_GridRect.top>=0)
{ 
	m_GridRect.top = int(m_GridRect.top / m_GridXStep +0.99) * m_GridYStep;
	m_GridRect.bottom = int(m_GridRect.bottom / m_GridXStep) * m_GridYStep;
}
else
{
	m_GridRect.top = int(m_GridRect.top / m_GridXStep +0.99) * m_GridYStep;
	m_GridRect.bottom = int(m_GridRect.bottom / m_GridXStep -0.99) * m_GridYStep;
}

int grid_Xsize = m_GridRect.Width() / temp_resolution;
int grid_Ysize = m_GridRect.Height() / temp_resolution;

ASSERT(grid_Xsize <= _MAX_GRID_SIZE-2);
 ASSERT(grid_Xsize <= _MAX_GRID_SIZE - 2);

for (int x_i = 0; x_i <= grid_Xsize; x_i++)
	for (int y_i = 0; y_i <= grid_Ysize; y_i++)
	{

		m_GridMatrix[x_i][y_i].m_NodeVector.clear();
	}


	int time_interval_no = 0;
	for (std::list<DTANode*>::iterator iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
			int x_key = ((*iNode)->pt.x - m_GridRect.left) / m_GridXStep;
			int y_key = grid_Ysize - ((*iNode)->pt.y - m_GridRect.bottom) / m_GridYStep;  // we use grid_Ysize - as the origin of the canvas is on the top

			//feasible region
			x_key = max(0, x_key);
			x_key = min(grid_Xsize, x_key);

			y_key = max(0, y_key);
			y_key = min(grid_Ysize, y_key);


			m_GridMatrix[x_key][y_key].m_NodeVector.push_back((*iNode)->m_NodeID);
	}


	//scan x and y index in the grid
	for (int x_i =0; x_i <= grid_Xsize; x_i++)
		for (int y_i = 0; y_i <= grid_Ysize; y_i++)
		{

			if (m_GridMatrix[x_i][y_i].m_NodeVector .size() > 0)
			{


				int zone_id = (x_i+1) * (1000) + (y_i+1);

				float left, right, top, bottom;
				if(m_GridRect.left>=0)
				{
					left = m_GridRect.left + m_GridXStep * x_i;
					right = m_GridRect.left + m_GridXStep * (x_i + 1);
				}
				else
				{
					left = m_GridRect.left + m_GridXStep * x_i;
					right = m_GridRect.left + m_GridXStep * (x_i + 1);
				}

				if (m_GridRect.bottom >= 0)
				{
					top = m_GridRect.top - m_GridYStep * (y_i);
					bottom = m_GridRect.top - m_GridYStep * (y_i+1);
				}
				else
				{
					top = m_GridRect.top + m_GridYStep * (y_i);
					bottom = m_GridRect.top + m_GridYStep * (y_i-1);

				}

				int si;

					std::string s("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
					std::string str_letter;

					int digit = (int)(x_i / 26);
					if(digit >=1)
						str_letter = s.at(digit % s.size());

					int reminder = x_i - digit * 26;
					str_letter += s.at(reminder % s.size());
					
					m_ZoneMap[zone_id].m_Production = 0;
					m_ZoneMap[zone_id].m_Attraction = 0;
					// tally production and attraction for each zone from all activity nodes in zone

					
					for (int ni = 0; ni < m_GridMatrix[x_i][y_i].m_NodeVector.size(); ni++)
					{

						if (m_NodeIDMap[m_GridMatrix[x_i][y_i].m_NodeVector[ni]]->m_NodeProduction > 0.00001 || m_NodeIDMap[m_GridMatrix[x_i][y_i].m_NodeVector[ni]]->m_NodeAttraction > 0.00001)
						{
							if (m_NodeIDMap[m_GridMatrix[x_i][y_i].m_NodeVector[ni]]->m_NodeProduction > 0.00001)
							{
								m_ZoneMap[zone_id].m_ProductionActivityNodeVector.push_back(m_GridMatrix[x_i][y_i].m_NodeVector[ni]);
								m_ZoneMap[zone_id].m_Production += m_NodeIDMap[m_GridMatrix[x_i][y_i].m_NodeVector[ni]]->m_NodeProduction;
							}

							if (m_NodeIDMap[m_GridMatrix[x_i][y_i].m_NodeVector[ni]]->m_NodeAttraction > 0.00001)
							{ 
								m_ZoneMap[zone_id].m_AttractionActivityNodeVector.push_back(m_GridMatrix[x_i][y_i].m_NodeVector[ni]);
								m_ZoneMap[zone_id].m_Attraction += m_NodeIDMap[m_GridMatrix[x_i][y_i].m_NodeVector[ni]]->m_NodeAttraction;
							}

							m_NodeIDMap[m_GridMatrix[x_i][y_i].m_NodeVector[ni]]->m_ZoneID = zone_id;  // assign zone id for all activity nodes

						}


					}

					// integer alphabet with numeric index
					CString zone_name;
					zone_name.Format("%s%d", str_letter.c_str(), (y_i) + 1);

				m_ZoneMap[zone_id].name = CString2StdString(zone_name);

				GDPoint	pt0, pt1, pt2, pt3, pt4;
				GDPoint center;
				center.x = (left + right) / 2.0;
				center.y = (top + bottom) / 2.0;

				pt0.x = left; 	pt0.y = top;
				pt1.x = right; 	pt1.y = top;
				pt2.x = right; 	pt2.y = bottom;
				pt3.x = left; 	pt3.y = bottom;
				pt4.x = left; 	pt4.y = top;

				m_ZoneMap[zone_id].m_ZoneID = zone_id;
				m_ZoneMap[zone_id].m_ShapePoints.push_back(pt0);
				m_ZoneMap[zone_id].m_ShapePoints.push_back(pt1);
				m_ZoneMap[zone_id].m_ShapePoints.push_back(pt2);
				m_ZoneMap[zone_id].m_ShapePoints.push_back(pt3);
				m_ZoneMap[zone_id].m_ShapePoints.push_back(pt4);
				m_ZoneMap[zone_id].m_Center = center;
			}
	

		}	// per node in a grid cell

// determine accessibility
	m_ODMatrixMap.clear();
	std::map<int, DTAZone>	::iterator itr_o;
	std::map<int, DTAZone>	::iterator itr_d;

	CWaitCursor wc;

	m_ONodeNo = -1;
	m_DNodeNo = -1;
	m_RandomRoutingCoefficient = 0;

	for (itr_o = m_ZoneMap.begin(); itr_o != m_ZoneMap.end(); itr_o++)  // for each origin
	{
		float total_relative_attraction = 0;
		int destination_zone;

		if (itr_o->second.m_ProductionActivityNodeVector.size() == 0)
			continue;

		int m_ONodeNo = m_NodeIDMap[itr_o->second.m_ProductionActivityNodeVector[0]]->m_NodeNo ;
		int m_DNodeNo = -1;

		for (itr_d = m_ZoneMap.begin(); itr_d != m_ZoneMap.end(); itr_d++)
		{
			if (itr_o->second.m_AttractionActivityNodeVector.size() == 0)
				continue;

			m_DNodeNo = m_NodeIDMap[itr_d->second.m_AttractionActivityNodeVector[0]]->m_NodeNo;
			
			//Travel Estimation Techniques for Urban Planning(NCHRP365, 1998) suggests that the gamma function be used with the following parameters :

			//Travel Estimation Techniques for Urban Planning Gamma Function Parameters
			//	Trip Purpose	a	b	c
			//	HBW	28507 - 0.020 - 0.123
			//	HBO	139173 - 1.285 - 0.094
			//	NHB	219113 - 1.332 - 0.010

			float coefficeint = -0.1;
			
			float travel_distance = g_CalculateP2PDistanceInMileFromLatitudeLongitude(itr_o->second.m_Center, itr_d->second.m_Center); 

			float accessibility_factor = 1/max(0.01, travel_distance);

			CString label;
			label.Format("%d,%d", itr_o->first, itr_d->first);

			m_ODMatrixMap[label].Origin = itr_o->first;
			m_ODMatrixMap[label].Destination = itr_d->first;
			m_ODMatrixMap[label].TotalCost = travel_distance;
			m_ODMatrixMap[label].Accessibility_Factor = accessibility_factor;
		}
	}


	float total_demand = 0;
	long line_no = 0;

	CString  directory = m_ProjectDirectory;
	FILE* st = NULL;
	fopen_s(&st, directory + "demand.csv", "w");

	FILE* st_g = NULL;
	fopen_s(&st_g, directory + "demand_geometry.csv", "w");

	if(st_g == NULL)
	{
	AfxMessageBox("Error: File demand_geometry.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
	return;
	}


	if (st != NULL)
	{
		fprintf(st, "o_zone_id,d_zone_id,volume\n");
		fprintf(st_g, "o_zone_id,d_zone_id,o_zname,d_zname,volume,cost,accessibility,geometry\n");

		for (itr_o = m_ZoneMap.begin(); itr_o != m_ZoneMap.end(); itr_o++)  // for each origin
		{
			float total_relative_attraction = 0;

			int destination_zone;

			for (itr_d = m_ZoneMap.begin(); itr_d != m_ZoneMap.end(); itr_d++)
			{
				destination_zone = itr_d->first;
				float accessibility_factor = 1;
				CString label;
				label.Format("%d,%d", itr_o->first, itr_d->first);

				if (m_ODMatrixMap.find(label) != m_ODMatrixMap.end())
				{
					accessibility_factor = m_ODMatrixMap[label].Accessibility_Factor;
				}

				total_relative_attraction += (itr_d)->second.m_Attraction * accessibility_factor;


			}



			float sub_total = 0;
			for (itr_d = m_ZoneMap.begin(); itr_d != m_ZoneMap.end(); itr_d++)  // for each destination
			{
				int origin_zone = itr_o->first;
				int destination_zone = itr_d->first;

				float accessibility_factor = 1;
				CString label;
				label.Format("%d,%d", itr_o->first, itr_d->first);

				if (m_ODMatrixMap.find(label) != m_ODMatrixMap.end())
				{
					accessibility_factor = m_ODMatrixMap[label].Accessibility_Factor;
				


				m_ODMatrixMap[label].Origin = origin_zone;
				m_ODMatrixMap[label].Destination = destination_zone;
         		 float demand_volume = itr_o->second.m_Production * (itr_d)->second.m_Attraction * accessibility_factor / max(0.00001, total_relative_attraction);
				m_ODMatrixMap[label].demand_volume = demand_volume;
				
				if(demand_volume >0.0001)
				{ 
				fprintf(st, "%d,%d,%f\n", origin_zone, destination_zone, demand_volume);

				fprintf(st_g, "%d,%d,%s,%s,%f,%f,%f,", itr_o->first, itr_d->first,
					itr_o->second.name.c_str(), itr_d->second.name.c_str(),
					demand_volume,
					m_ODMatrixMap[label].TotalCost,
					m_ODMatrixMap[label].Accessibility_Factor);
				fprintf(st_g, "\"LINESTRING (");
				fprintf(st_g, "%f %f,%f %f", itr_o->second.m_Center.x, itr_o->second.m_Center.y, itr_d->second.m_Center.x, itr_d->second.m_Center.y);
				fprintf(st_g, ")\"");
				fprintf(st_g, "\n");
				}

				}


			} // for each origin
		}
		
		fclose(st);
		fclose(st_g);
	}
	else
	{
		AfxMessageBox("Error: File demand.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");

	}

	m_ONodeNo = -1;
	m_DNodeNo = -1;

	SaveZoneFile();
	SaveNodeFile();
//	SaveLinkData();
//	AfxMessageBox("3 files zone.csv, demand.csv and demand_geometry.cvs have been generated in the project folder. Please exit from NeXTA and reload the network in NeXTA again.", MB_ICONINFORMATION);
}


void CTLiteDoc::OnBackgroundimageReloadbackgroundimage()
{
	ReadBackgroundImageFile();
	UpdateAllViews(0);

}

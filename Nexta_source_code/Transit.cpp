// Transit.cpp : Implementation file
//
//  Portions Copyright 2012 Shuguang Li (xalxlsg@gmail.com), Xuesong Zhou (zhou99@gmail.com)

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
#pragma once

#include "stdafx.h"
#include "TLite.h"
#include "Network.h"
#include "Geometry.h"
#include "CSVParser.h"
#include <vector>
#include <algorithm>
#include "TLiteDoc.h"
#include "TLiteView.h"
#include "MainFrm.h"
extern string g_time_coding(float time_stamp);
bool PT_Network::ReadGTFFiles(GDRect network_rect)  // Google Transit files
{

	//	// step 1: read  route files
	string str0 = m_ProjectDirectory +"routes.txt";
	//string str =  "h:/routes.csv";

	CT2CA pszConvertedAnsiString (str0.c_str());
	// construct a std::string using the LPCSTR input
	std::string  strStd (pszConvertedAnsiString);
	CCSVParser parser;

	if (parser.OpenCSVFile(strStd))
	{

		//	AfxMessageBox("Start reading Google Transit Feed files...", MB_ICONINFORMATION);

		int count =0;
		PT_Route route;
		while(parser.ReadRecord())
		{


			if(parser.GetValueByFieldName("route_id",route.route_id ) == false)
				break;
			if(parser.GetValueByFieldName("route_long_name",route.route_long_name) == false)
				route.route_long_name="";
			if(parser.GetValueByFieldName("route_short_name",route.route_short_name) == false)
				route.route_short_name="";			
			if(parser.GetValueByFieldName("route_url",route.route_url) == false)
				route.route_url="";			
			if(parser.GetValueByFieldName("route_type",route.route_type) == false)
				route.route_type="";		

			// make sure there is not duplicated key
			// If the requested key is not found, find() returns the end iterator for
			//the container, so:

			if(m_PT_RouteMap.find(route.route_id)  == m_PT_RouteMap.end() )
			{ 
				m_PT_RouteMap[route.route_id] = route;  
			}
			else
			{
				AfxMessageBox("Duplicated Route ID!");
			}


			count++;

		}
		parser.CloseCSVFile ();

		CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();

		pMainFrame->m_bShowLayerMap[layer_transit] = true;

	}else
	{
		return false;
	}

	CString missing_stops_message;
	// step 1: read stop information
	string str2 = m_ProjectDirectory +"stops.txt";


	CT2CA pszConvertedAnsiString2 (str2.c_str());
	// construct a std::string using the LPCSTR input
	std::string  strStd2 (pszConvertedAnsiString2);
	if (parser.OpenCSVFile(strStd2))
	{
		int count =0;
		while(parser.ReadRecord())
		{
			PT_Stop stop;

			if(parser.GetValueByFieldName("stop_id",stop.stop_id ) == false)
				break;


			bool NonnegativeFlag = false;
			if(parser.GetValueByFieldName("stop_lat",stop.m_ShapePoint.y, NonnegativeFlag) == false)
				break;

			if(parser.GetValueByFieldName("stop_lon",stop.m_ShapePoint.x , NonnegativeFlag) == false)
				break;

			if(parser.GetValueByFieldName("direction",stop.direction  ) == false)
				stop.direction="";

			if(parser.GetValueByFieldName("location_type",stop.location_type) == false)
				stop.location_type=0;

			if(parser.GetValueByFieldName("position",stop.position  ) == false)
				stop.position="";

			if(parser.GetValueByFieldName("stop_code",stop.stop_code ) == false)
				stop.stop_code=0;

			parser.GetValueByFieldName("stop_desc",stop.stop_desc);


			if(parser.GetValueByFieldName("stop_name",stop.stop_name) == false)
				stop.stop_name="";

			if(parser.GetValueByFieldName("zone_id",stop.zone_id) == false)
				stop.zone_id=0;


			if(m_PT_StopMap.find(stop.stop_id) == m_PT_StopMap.end())
			{
				m_PT_StopMap[stop.stop_id] = stop;

			}else
			{   CString msg;
			msg.Format("Duplicated Stop ID %d", stop.stop_id);

			AfxMessageBox(msg);
			}

			count++;

		}
		parser.CloseCSVFile ();




	}


	//read trip file
	string str3 = m_ProjectDirectory +"trips.txt";
	CT2CA pszConvertedAnsiString3 (str3.c_str());
	// construct a std::string using the LPCSTR input
	std::string  strStd3 (pszConvertedAnsiString3);
	if (parser.OpenCSVFile(strStd3))
	{
		int count =0;
		while(parser.ReadRecord())
		{
			PT_Trip trip;

			if(parser.GetValueByFieldName("trip_id",trip.trip_id) == false)
				break;

			if(parser.GetValueByFieldName("route_id",trip.route_id) == false)
				break;			

			if(parser.GetValueByFieldName("service_id",trip.service_id) == false)
				break;			

			if(parser.GetValueByFieldName("block_id",trip.block_id) == false)
				trip.block_id=0;			

			if(parser.GetValueByFieldName("direction_id",trip.direction_id) == false)
				trip.direction_id=0;

			if(parser.GetValueByFieldName("shape_id",trip.shape_id) == false)
				trip.shape_id=0;

			if(parser.GetValueByFieldName("trip_type",trip.trip_type) == false)
				trip.trip_type=0;	 	

			if(m_PT_TripMap.find(trip.trip_id) == m_PT_TripMap.end())
			{
				m_PT_TripMap[trip.trip_id] = trip;
			}else
			{
				AfxMessageBox("Duplicated Trip ID!");
			}
			count++;
		}
		parser.CloseCSVFile ();
	}



	// read stop_times.txt
	int stop_times_count =0;
	int max_stop_times_record  = 1000;

	string str4 = m_ProjectDirectory +"stop_times.txt";
	CT2CA pszConvertedAnsiString4 (str4.c_str());
	// construct a std::string using the LPCSTR input
	std::string  strStd4 (pszConvertedAnsiString4);

	CCSVParser parser_st;
	if (parser_st.OpenCSVFile(strStd4))
	{

		while(parser_st.ReadRecord())
		{
			PT_StopTime TransitStopTime;

			if(parser_st.GetValueByFieldName("stop_id",TransitStopTime.stop_id) == false)
				break;	

//			TransitStopTime.stop_id = 

			string time_string;
			char char_array[20];
			if(parser_st.GetValueByFieldName("arrival_time",time_string) == false)
				break;

			int hour,min,second;
			sprintf(char_array,"%s",time_string.c_str ());
			sscanf(char_array,"%d:%d:%d", &hour,&min,&second);

			TransitStopTime.arrival_time = hour*60+min;

			if(parser_st.GetValueByFieldName("departure_time",time_string) == false)
				break;	

			sprintf(char_array,"%s",time_string.c_str ());
			sscanf(char_array,"%d:%d:%d", &hour,&min,&second);
			TransitStopTime.departure_time =  hour*60+min;

			if(parser_st.GetValueByFieldName("trip_id",TransitStopTime.trip_id) == false)
				break; 


			if(parser_st.GetValueByFieldName("stop_sequence",TransitStopTime.stop_sequence) == false)
				break; 			

			/*
			if(parser_st.GetValueByFieldName("drop_off_type",TransitStopTime.drop_off_type) == false)
			TransitStopTime.drop_off_type=0; 

			if(parser_st.GetValueByFieldName("pickup_type",TransitStopTime.pickup_type) == false)
			TransitStopTime.pickup_type=0;

			if(parser_st.GetValueByFieldName("shape_dist_traveled",TransitStopTime.shape_dist_traveled) == false)
			TransitStopTime.shape_dist_traveled=0;	

			if(parser_st.GetValueByFieldName("stop_headsign",TransitStopTime.stop_headsign) == false)
			TransitStopTime.stop_headsign=0;	

			if(parser_st.GetValueByFieldName("timepoint",TransitStopTime.timepoint) == false)
			TransitStopTime.timepoint=0;*/	

			if(m_PT_StopMap.find(TransitStopTime.stop_id)!= m_PT_StopMap.end())
			{
				TransitStopTime.pt.x = m_PT_StopMap[TransitStopTime.stop_id].m_ShapePoint .x ;
				TransitStopTime.pt.y = m_PT_StopMap[TransitStopTime.stop_id].m_ShapePoint .y ;

			}else
			{
				if(missing_stops_message.GetLength ()<1000)
				{
					CString msg;
					msg.Format ("Mising stop %d\n",TransitStopTime.stop_id);
					missing_stops_message+=msg;

				}

			}

		//	m_PT_StopTimeVector.push_back(TransitStopTime) ;
			stop_times_count++;

			//if(stop_times_count >=max_stop_times_record)  // for testing purposes
			//	break;

			if(m_PT_TripMap.find(TransitStopTime.trip_id) != m_PT_TripMap.end())
			{
				m_PT_TripMap[TransitStopTime.trip_id].m_PT_StopTimeVector .push_back(TransitStopTime);
			}

		}
		parser_st.CloseCSVFile ();

	}

	CString message;

	int trip_size = m_PT_TripMap.size();
	int stop_size = m_PT_StopMap.size();
	int stop_time_size = stop_times_count;
	message.Format("%d transit trips, %d stops and %d stop time records are loaded.", trip_size, stop_size, stop_time_size);

	AfxMessageBox(message, MB_ICONINFORMATION);

	if(missing_stops_message.GetLength ()>0)
	{
		AfxMessageBox(missing_stops_message);	
	}

	/*

	//read transfer file
	string str6 = m_ProjectDirectory +"transfers.txt";
	CT2CA pszConvertedAnsiString6 (str6.c_str());
	// construct a std::string using the LPCSTR input
	std::string  strStd6 (pszConvertedAnsiString6);
	if (parser.OpenCSVFile(strStd6))
	{
	int count =0;
	while(parser.ReadRecord())
	{
	PT_transfers transfers;

	if(parser.GetValueByFieldName("from_stop_id",transfers.from_stop_id) == false)
	break;	

	if(parser.GetValueByFieldName("to_stop_id",transfers.to_stop_id) == false)
	break; 			

	if(parser.GetValueByFieldName("transfer_type",transfers.transfer_type) == false)
	break;

	m_PT_transfers.push_back(transfers) ;
	count++;
	}
	parser.CloseCSVFile ();
	}

	*/

	// map matching


	return true;
}

void CTLiteDoc::ReadTransitFiles(CString TransitDataProjectFolder)
{

	m_PT_network.m_ProjectDirectory = TransitDataProjectFolder;
	m_PT_network.ReadGTFFiles(m_NetworkRect);
	
	std::map<string, int> stop_id_to_no_map;

	// create node
		std::map<string, PT_Stop>::iterator iPT_StopMap;
		for (iPT_StopMap = m_PT_network.m_PT_StopMap.begin(); iPT_StopMap != m_PT_network.m_PT_StopMap.end(); iPT_StopMap++)
		{
			if(fabs((*iPT_StopMap).second.m_ShapePoint.x )>0.0001 && fabs((*iPT_StopMap).second.m_ShapePoint.y) > 0.0001)
			{ 
			DTANode* pNode = AddNewNode((*iPT_StopMap).second.m_ShapePoint, stop_id_to_no_map.size()+1);
			pNode->m_Name = (*iPT_StopMap).second.stop_id;
			stop_id_to_no_map[(*iPT_StopMap).second.stop_id] = stop_id_to_no_map.size()+1;
			TRACE("%d: %s\n", stop_id_to_no_map.size() + 1, (*iPT_StopMap).second.stop_id.c_str());
			}
		}

//mark zone id
		std::map<int, PT_Trip>::iterator iPT_TripMap;
		for (iPT_TripMap = m_PT_network.m_PT_TripMap.begin(); iPT_TripMap != m_PT_network.m_PT_TripMap.end(); iPT_TripMap++)
		{

			if ((*iPT_TripMap).second.m_PT_StopTimeVector.size() >= 2)
			{
				int i = 0;

				if (stop_id_to_no_map.find((*iPT_TripMap).second.m_PT_StopTimeVector[i].stop_id) != stop_id_to_no_map.end() )
				{
					int node_id = stop_id_to_no_map[(*iPT_TripMap).second.m_PT_StopTimeVector[i].stop_id];
					DTANode* pNode = FindNodeWithNodeID(node_id);
					if (pNode != NULL)
					{
						pNode->m_ZoneID = node_id;
					}
				}

				i = (*iPT_TripMap).second.m_PT_StopTimeVector.size() - 1;
				if (stop_id_to_no_map.find((*iPT_TripMap).second.m_PT_StopTimeVector[i].stop_id) != stop_id_to_no_map.end())
				{
					int node_id = stop_id_to_no_map[(*iPT_TripMap).second.m_PT_StopTimeVector[i].stop_id];
					DTANode* pNode = FindNodeWithNodeID(node_id);
					if (pNode != NULL)
					{
						pNode->m_ZoneID = node_id;
					}

				}
			}
		}

		

		int count = 0;
		std::map<int, int> RouteMap;

		std::map<CString, int> TransitLinkMap;
		std::map<CString, int> TransitLegMap;

		// step 1: for each trip
		for (iPT_TripMap = m_PT_network.m_PT_TripMap.begin(); iPT_TripMap != m_PT_network.m_PT_TripMap.end(); iPT_TripMap++)
		{

			// step 2: for each stop time pair
			if ((*iPT_TripMap).second.m_PT_StopTimeVector.size() >= 2)
			{

				for (int i = 0; i < (*iPT_TripMap).second.m_PT_StopTimeVector.size() - 1; i++)
				{
					string stopid_1 = (*iPT_TripMap).second.m_PT_StopTimeVector[i].stop_id;
					string stopid_2 = (*iPT_TripMap).second.m_PT_StopTimeVector[i + 1].stop_id;


					if (stop_id_to_no_map.find(stopid_1) != stop_id_to_no_map.end() && stop_id_to_no_map.find(stopid_2) != stop_id_to_no_map.end())
					{

						CString transit_link_key;
						transit_link_key.Format("%s-%s", stopid_1.c_str(), stopid_2.c_str());

						if (TransitLinkMap.find(transit_link_key) == TransitLinkMap.end())  //find nothing
						{ //not defined yet
							// add a new link
							//	fprintf(st, "name,from_node_id,to_node_id,direction,length,number_of_lanes,speed_limit,lane_cap,link_type,demand_type_code");

							float link_length = 1;
							
							//g_CalculateP2PDistanceInMileFromLatitudeLongitude((*iPT_TripMap).second.m_PT_StopTimeVector[i].pt,
							//	(*iPT_TripMap).second.m_PT_StopTimeVector[i + 1].pt);

							if (m_PT_network.m_PT_RouteMap.find((*iPT_TripMap).second.route_id) == m_PT_network.m_PT_RouteMap.end())
								continue;

							
							int stop_no_1 = stop_id_to_no_map[stopid_1];
							int stop_no_2 = stop_id_to_no_map[stopid_2];

							DTALink* pLink = AddNewLink(stop_no_1, stop_no_2, false, true);
							pLink->m_Name = m_PT_network.m_PT_RouteMap[(*iPT_TripMap).second.route_id].route_short_name;
//								(*iPT_TripMap).second.trip_id,
//								stopid_2,
//								link_length);

							//define this link
							TransitLinkMap[transit_link_key] = TransitLinkMap.size();
						}

						// added for legs
						CString transit_leg_key;
						transit_leg_key.Format("%d-%s-%s", (*iPT_TripMap).second.route_id, stopid_1.c_str(), stopid_2.c_str());

						if (TransitLegMap.find(transit_leg_key) == TransitLegMap.end())  //find nothing
						{ //not defined yet
							// add a new leg link
							//	fprintf(st, "name,from_node_id,to_node_id,direction,length,number_of_lanes,speed_limit,lane_cap,link_type,demand_type_code");

							float link_length = 1;

							//g_CalculateP2PDistanceInMileFromLatitudeLongitude((*iPT_TripMap).second.m_PT_StopTimeVector[i].pt,
							//	(*iPT_TripMap).second.m_PT_StopTimeVector[i + 1].pt);

							if (m_PT_network.m_PT_RouteMap.find((*iPT_TripMap).second.route_id) == m_PT_network.m_PT_RouteMap.end())
								continue;


							int stop_no_1 = stop_id_to_no_map[stopid_1];
							int stop_no_2 = stop_id_to_no_map[stopid_2];

							DTALink* pLink = AddNewLink(stop_no_1, stop_no_2, false, true);
							pLink->m_Name = m_PT_network.m_PT_RouteMap[(*iPT_TripMap).second.route_id].route_short_name;
							//								(*iPT_TripMap).second.trip_id,
							//								stopid_2,
							//								link_length);

														//define this link
							TransitLinkMap[transit_link_key] = TransitLinkMap.size();
						}


					
					}
					else
					{
						TRACE("Missing transit link, stop id not defined!");
					}
				}
			}

		}  // for each trip

	

	FILE* st = NULL;
	fopen_s(&st, m_PT_network.m_ProjectDirectory + "route.csv", "w");
	if (st == NULL)
	{
		AfxMessageBox("Error: File route.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
		return;
	}

	if (st != NULL)
	{

		fprintf(st, "agent_id,agent_type,trip_id,route_id,route_id_short_name,o_zone_id,d_zone_id,travel_time,distance,node_sequence,time_sequence\n");

		int count = 0;
		std::map<int, int> RouteMap;

		int agent_id = 1;

		// step 1: for each trip
		std::map<int, PT_Trip>::iterator iPT_TripMap;
		for (iPT_TripMap = m_PT_network.m_PT_TripMap.begin(); iPT_TripMap != m_PT_network.m_PT_TripMap.end(); iPT_TripMap++)
		{

			int o_zone_id = 1;
			int d_zone_id = 2;

			float travel_time = 0;
			float distance = 0;
			
			if((*iPT_TripMap).second.m_PT_StopTimeVector.size() >=2)
			{
				travel_time  = (*iPT_TripMap).second.m_PT_StopTimeVector[(*iPT_TripMap).second.m_PT_StopTimeVector.size() - 1].arrival_time
					- (*iPT_TripMap).second.m_PT_StopTimeVector[0].arrival_time;

				int i = 0;
				{
					if(stop_id_to_no_map.find((*iPT_TripMap).second.m_PT_StopTimeVector[i].stop_id) != stop_id_to_no_map.end())
					{
					int node_id = stop_id_to_no_map[(*iPT_TripMap).second.m_PT_StopTimeVector[i].stop_id];
					DTANode* pNode = FindNodeWithNodeID(node_id);
					if (pNode != NULL)
					{
						o_zone_id = node_id;
					}
					}
				}

				i = (*iPT_TripMap).second.m_PT_StopTimeVector.size() - 1;
				{
					if (stop_id_to_no_map.find((*iPT_TripMap).second.m_PT_StopTimeVector[i].stop_id) != stop_id_to_no_map.end())
					{ 
					int node_id = stop_id_to_no_map[(*iPT_TripMap).second.m_PT_StopTimeVector[i].stop_id];
					DTANode* pNode = FindNodeWithNodeID(node_id);
					if (pNode != NULL)
					{
						d_zone_id = node_id;
					}
					}

				}

				for (int i = 0; i < (*iPT_TripMap).second.m_PT_StopTimeVector.size()-1; i++)
				{

							GDPoint pt0 = (*iPT_TripMap).second .m_PT_StopTimeVector[i].pt;
							GDPoint pt1 = (*iPT_TripMap).second .m_PT_StopTimeVector[i+1].pt;


			

							distance+= g_CalculateP2PDistanceInMileFromLatitudeLongitude(pt0, pt1);

				} 

			}

			fprintf(st, "%d,transit,%d,%d,%s,%d,%d,%.3f,%.3f,",
				agent_id,
				(*iPT_TripMap).second.trip_id,
				(*iPT_TripMap).second.route_id,
				m_PT_network.m_PT_RouteMap[(*iPT_TripMap).second.route_id].route_long_name.c_str(),
				o_zone_id,
				d_zone_id,
				travel_time,
				distance
				);

			agent_id++;

			// step 2: for each stop-time node
			if ((*iPT_TripMap).second.m_PT_StopTimeVector.size() >= 2)
			{
				for (int i = 0; i < (*iPT_TripMap).second.m_PT_StopTimeVector.size(); i++)
				{
					fprintf(st, "%d;", stop_id_to_no_map[(*iPT_TripMap).second.m_PT_StopTimeVector[i].stop_id]);
				}
				fprintf(st, ",");
			}

			// step 2: for each stop-time time data
			if ((*iPT_TripMap).second.m_PT_StopTimeVector.size() >= 2)
			{
				for (int i = 0; i < (*iPT_TripMap).second.m_PT_StopTimeVector.size(); i++)
				{
					fprintf(st, "%s;",
						g_time_coding((*iPT_TripMap).second.m_PT_StopTimeVector[i].arrival_time).c_str());
				}
				fprintf(st, ",");
			}


			fprintf(st, "\n");
		}

		AfxMessageBox("File route.csv has been created.", MB_ICONINFORMATION);

		fclose(st);
	}

//	m_TransitDataLoadingStatus.Format ("%d transit trips have been loaded.",m_PT_network.m_PT_TripMap.size() );

	/*	if( m_PT_network.m_PT_TripMap.size()>0 && AfxMessageBox("Do you want to generate bus trip data?",MB_YESNO|MB_ICONINFORMATION)==IDYES)
	{
	TransitTripMatching();
	}
	*/

		CalculateDrawingRectangle(true);
		m_bFitNetworkInitialized = false;
}

//
//bool CTLiteDoc::TransitTripMatching()
//{
//
//	FILE* st;
//	fopen_s(&st,m_PT_network.m_ProjectDirectory +"input_transit_trip.csv","w");
//	if(st==NULL)
//	{
//		AfxMessageBox("Error: File input_transit_trip.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
//	}
//
//	if(st!=NULL)
//	{
//
//		fprintf(st,"route_id,trip_id,link_sequence_no,from_node_id,to_node_id,street_name,stop_id,stop_sequence,arrival_time,departure_time\n");
//
//
//		std::list<DTALink*>::iterator iLink;
//
//		// pass 0: initialization 
//		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)  // for each link in this network
//		{
//			(*iLink)-> m_OriginalLength = (*iLink)->m_Length;
//
//		}
//
//		int count = 0;
//		std::map<int, int> RouteMap;
//
//		// step 2: assemble data
//		std::map<int, PT_Trip>::iterator iPT_TripMap;
//		for ( iPT_TripMap= m_PT_network.m_PT_TripMap.begin() ; iPT_TripMap != m_PT_network.m_PT_TripMap.end(); iPT_TripMap++ )
//		{
//			if(m_pNetwork ==NULL)  // we only build the network once
//			{
//				m_pNetwork = new DTANetworkForSP(m_NodeSet.size(), m_LinkSet.size(), 1, 1, m_AdjLinkSize);  //  network instance for single processor in multi-thread environment
//			}
//
//			count++;
//			if(count>=50)
//				break;
//
//			int stop_time_size  = (*iPT_TripMap).second .m_PT_StopTimeVector.size();
//			if(stop_time_size>=2)
//			{
//				//find OD nodes for each vehicle
//				m_OriginNodeID = FindClosestNode((*iPT_TripMap).second .m_PT_StopTimeVector[0].pt);
//				m_DestinationNodeID = FindClosestNode((*iPT_TripMap).second .m_PT_StopTimeVector[stop_time_size-1].pt);
//				// set new length for links
//
//				if(m_OriginNodeID==-1 || m_DestinationNodeID==-1)
//					continue;
//
//				// pass 0: initialization 
//				for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)  // for each link in this network
//				{
//					(*iLink)->m_StopTimeRecord.stop_id = -1;
//					(*iLink)->m_Length = 999999;
//
//				}
//				int max_link_id = 1;
//
//
//				// first pass: link distance
//
//				for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)  // for each link in this network
//				{
//
//					////					if( m_LinkTypeMap[(*iLink)->m_link_type].IsTransit())
//					//					{
//					//
//					//						(*iLink)->m_Length = 999999; // // not using light rail link
//					//
//					//					}else
//					{
//						double min_p_to_link_distance = 999999;
//
//						GDPoint pfrom = (*iLink)->m_FromPoint;
//						GDPoint pto = (*iLink)->m_ToPoint;
//
//						GDPoint mid_point;
//						mid_point.x = (pfrom.x + pto.x)/2;
//						mid_point.y = (pfrom.y + pto.y)/2;
//
//
//						int Intersection_Count  = 0;
//
//
//
//						for(int i = 0; i < stop_time_size; i++ )
//						{
//
//							GDPoint pt0 = (*iPT_TripMap).second .m_PT_StopTimeVector[i].pt;
//							GDPoint pt1 = (*iPT_TripMap).second .m_PT_StopTimeVector[min(i+1,stop_time_size-1)].pt;
//							GDPoint pt2 = (*iPT_TripMap).second .m_PT_StopTimeVector[min(i+2,stop_time_size-1)].pt;
//
//
//							float distance;
//
//							distance = (g_GetPoint2Point_Distance(pfrom, pt0) +  g_GetPoint2Point_Distance(mid_point, pt1) +  g_GetPoint2Point_Distance(pto, pt2))/3;
//							if(distance < min_p_to_link_distance)
//							{
//								min_p_to_link_distance = distance;
//							}
//
//						} 
//						// determine the distance to GPS traces as length
//
//						double dis_in_feet = min_p_to_link_distance /m_UnitFeet;
//
//						(*iLink)->m_Length = dis_in_feet; // keep the original link length
//					}
//			 }
//
//
//				m_RandomRoutingCoefficient  = 0;  // no random cost
//				//build physical network
//				m_pNetwork->BuildPhysicalNetwork(&m_NodeSet, &m_LinkSet, m_RandomRoutingCoefficient, false);
//				int NodeNodeSum = 0;
//				int PathLinkList[MAX_NODE_SIZE_IN_A_PATH];//save link ids in a path(a path means the trajactory of a vehicle obtained from GPS)
//				float TotalCost;
//				bool distance_flag = true;
//				int  NodeSize;
//				//get the total number of the nodes in the network we build,and give the link ids to PathLinkList
//				NodeSize= m_pNetwork->SimplifiedTDLabelCorrecting_DoubleQueue(m_OriginNodeID, 0, m_DestinationNodeID, 1, 10.0f,PathLinkList,TotalCost, distance_flag, false, false,m_RandomRoutingCoefficient);   // Pointer to previous node (node)    
//				//update m_PathDisplayList
//				if(NodeSize <= 1)
//				{
//					TRACE("error");
//				}
//
//				NodeSize = min(NodeSize, MAX_NODE_SIZE_IN_A_PATH);
//
//				std::vector<int> link_vector;// a vector to save link ids
//				for (int i=0 ; i < NodeSize-1; i++)
//				{
//					link_vector.push_back (PathLinkList[i]);// save the link ids
//					DTALink* pLink = m_LinkNotoLinkMap[PathLinkList[i]];
//					if(pLink!=NULL)
//					{ 
//						//if(i==0)
//						//	(*iPT_TripMap).second .m_PathNodeVector.push_back (pLink->m_FromNodeID );
//						//
//						//(*iPT_TripMap).second .m_PathNodeVector.push_back (pLink->m_ToNodeID );
//
//						fprintf(st,"%d,%d,%d,%d,%d,%s,",(*iPT_TripMap).second.route_id, (*iPT_TripMap).second.trip_id ,
//							i,pLink->m_FromNodeNumber , pLink->m_ToNodeNumber, pLink->m_Name.c_str ());
//
//						for(int s_i = 0; s_i < stop_time_size; s_i++ )
//						{
//							GDPoint pfrom = pLink->m_FromPoint;
//							GDPoint pto = pLink->m_ToPoint;
//
//							float distance = g_GetPoint2LineDistance((*iPT_TripMap).second .m_PT_StopTimeVector[s_i].pt, pfrom, pto,m_UnitMile);  // go through each GPS location point
//
//							if(distance < 90)  // with intersection
//							{
//								PT_StopTime element = (*iPT_TripMap).second .m_PT_StopTimeVector[s_i];
//								fprintf(st,"%d,%d,%d,%d,", element.stop_id, element.stop_sequence ,element.arrival_time , element.departure_time);
//								break;
//							}
//
//						}
//						fprintf(st,"\n");
//
//					}
//
//				} // for all links
//
//			}
//		}  // for each transit trip
//
//
//		for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)  // for each link in this network
//		{
//			(*iLink)->m_Length = (*iLink)-> m_OriginalLength;
//		}
//
//		fclose(st);
//	}
//	return true;
//}

bool PT_Network::ReadGTFFiles_Leg(GDRect network_rect)  // Google Transit files
{

	//	// step 1: read  route files
	string str0 = m_ProjectDirectory + "routes.txt";

	CT2CA pszConvertedAnsiString(str0.c_str());
	// construct a std::string using the LPCSTR input
	std::string  strStd(pszConvertedAnsiString);
	CCSVParser parser;

	if (parser.OpenCSVFile(strStd))
	{

		//	AfxMessageBox("Start reading Google Transit Feed files...", MB_ICONINFORMATION);

		int count = 0;
		PT_Route route;
		while (parser.ReadRecord())
		{


			if (parser.GetValueByFieldName("route_id", route.route_id) == false)
				break;
			if (parser.GetValueByFieldName("route_long_name", route.route_long_name) == false)
				route.route_long_name = "";
			if (parser.GetValueByFieldName("route_short_name", route.route_short_name) == false)
				route.route_short_name = "";
			if (parser.GetValueByFieldName("route_url", route.route_url) == false)
				route.route_url = "";
			if (parser.GetValueByFieldName("route_type", route.route_type) == false)
				route.route_type = "";

			// make sure there is not duplicated key
			// If the requested key is not found, find() returns the end iterator for
			//the container, so:

			if (m_PT_RouteMap.find(route.route_id) == m_PT_RouteMap.end())
			{
				m_PT_RouteMap[route.route_id] = route;
			}
			else
			{
				AfxMessageBox("Duplicated Route ID!");
			}


			count++;

		}
		parser.CloseCSVFile();

		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

		pMainFrame->m_bShowLayerMap[layer_transit] = true;

	}
	else
	{
		return false;
	}

	CString missing_stops_message;
	// step 1: read stop information
	string str2 = m_ProjectDirectory + "stops.txt";


	CT2CA pszConvertedAnsiString2(str2.c_str());
	// construct a std::string using the LPCSTR input
	std::string  strStd2(pszConvertedAnsiString2);
	if (parser.OpenCSVFile(strStd2))
	{
		int count = 0;
		while (parser.ReadRecord())
		{
			PT_Stop stop;

			if (parser.GetValueByFieldName("stop_id", stop.stop_id) == false)
				break;


			bool NonnegativeFlag = false;
			if (parser.GetValueByFieldName("stop_lat", stop.m_ShapePoint.y, NonnegativeFlag) == false)
				break;

			if (parser.GetValueByFieldName("stop_lon", stop.m_ShapePoint.x, NonnegativeFlag) == false)
				break;

			if (parser.GetValueByFieldName("direction", stop.direction) == false)
				stop.direction = "";

			if (parser.GetValueByFieldName("location_type", stop.location_type) == false)
				stop.location_type = 0;

			if (parser.GetValueByFieldName("position", stop.position) == false)
				stop.position = "";

			if (parser.GetValueByFieldName("stop_code", stop.stop_code) == false)
				stop.stop_code = 0;

			parser.GetValueByFieldName("stop_desc", stop.stop_desc);


			if (parser.GetValueByFieldName("stop_name", stop.stop_name) == false)
				stop.stop_name = "";

			if (parser.GetValueByFieldName("zone_id", stop.zone_id) == false)
				stop.zone_id = 0;

			// LEG: we do not create stop id here, we will create stop together with route

			if (m_PT_StopMap.find(stop.stop_id) == m_PT_StopMap.end())
			{
				m_PT_StopMap[stop.stop_id] = stop;

			}
			else
			{
				CString msg;
				msg.Format("Duplicated Stop ID %d", stop.stop_id);

				AfxMessageBox(msg);
			}

			count++;

		}
		parser.CloseCSVFile();




	}


	//read trip file
	string str3 = m_ProjectDirectory + "trips.txt";
	CT2CA pszConvertedAnsiString3(str3.c_str());
	// construct a std::string using the LPCSTR input
	std::string  strStd3(pszConvertedAnsiString3);
	if (parser.OpenCSVFile(strStd3))
	{
		int count = 0;
		while (parser.ReadRecord())
		{
			PT_Trip trip;

			if (parser.GetValueByFieldName("trip_id", trip.trip_id) == false)
				break;

			if (parser.GetValueByFieldName("route_id", trip.route_id) == false)
				break;

			if (parser.GetValueByFieldName("service_id", trip.service_id) == false)
				break;

			if (parser.GetValueByFieldName("block_id", trip.block_id) == false)
				trip.block_id = 0;

			if (parser.GetValueByFieldName("direction_id", trip.direction_id) == false)
				trip.direction_id = 0;

			if (parser.GetValueByFieldName("shape_id", trip.shape_id) == false)
				trip.shape_id = 0;

			if (parser.GetValueByFieldName("trip_type", trip.trip_type) == false)
				trip.trip_type = 0;

			if (m_PT_TripMap.find(trip.trip_id) == m_PT_TripMap.end())
			{
				m_PT_TripMap[trip.trip_id] = trip;
			}
			else
			{
				AfxMessageBox("Duplicated Trip ID!");
			}
			count++;
		}
		parser.CloseCSVFile();
	}



	// read stop_times.txt
	int stop_times_count = 0;
	int max_stop_times_record = 1000;

	string str4 = m_ProjectDirectory + "stop_times.txt";
	CT2CA pszConvertedAnsiString4(str4.c_str());
	// construct a std::string using the LPCSTR input
	std::string  strStd4(pszConvertedAnsiString4);

	CCSVParser parser_st;
	if (parser_st.OpenCSVFile(strStd4))
	{

		while (parser_st.ReadRecord())
		{
			PT_StopTime TransitStopTime;

			if (parser_st.GetValueByFieldName("stop_id", TransitStopTime.stop_id) == false)
				break;

			//			TransitStopTime.stop_id = 

			string time_string;
			char char_array[20];
			if (parser_st.GetValueByFieldName("arrival_time", time_string) == false)
				break;

			int hour, min, second;
			sprintf(char_array, "%s", time_string.c_str());
			sscanf(char_array, "%d:%d:%d", &hour, &min, &second);

			TransitStopTime.arrival_time = hour * 60 + min;

			if (parser_st.GetValueByFieldName("departure_time", time_string) == false)
				break;

			sprintf(char_array, "%s", time_string.c_str());
			sscanf(char_array, "%d:%d:%d", &hour, &min, &second);
			TransitStopTime.departure_time = hour * 60 + min;

			if (parser_st.GetValueByFieldName("trip_id", TransitStopTime.trip_id) == false)
				break;


			if (parser_st.GetValueByFieldName("stop_sequence", TransitStopTime.stop_sequence) == false)
				break;

			/*
			if(parser_st.GetValueByFieldName("drop_off_type",TransitStopTime.drop_off_type) == false)
			TransitStopTime.drop_off_type=0;

			if(parser_st.GetValueByFieldName("pickup_type",TransitStopTime.pickup_type) == false)
			TransitStopTime.pickup_type=0;

			if(parser_st.GetValueByFieldName("shape_dist_traveled",TransitStopTime.shape_dist_traveled) == false)
			TransitStopTime.shape_dist_traveled=0;

			if(parser_st.GetValueByFieldName("stop_headsign",TransitStopTime.stop_headsign) == false)
			TransitStopTime.stop_headsign=0;

			if(parser_st.GetValueByFieldName("timepoint",TransitStopTime.timepoint) == false)
			TransitStopTime.timepoint=0;*/

			//LEG: 

			string route_id;
			if (m_PT_TripMap.find(TransitStopTime.trip_id) != m_PT_TripMap.end())
			{
				route_id = m_PT_TripMap[TransitStopTime.trip_id].route_id;
			}

			string route_stop_id = route_id +"_ "+ TransitStopTime.stop_id;  // create route_stop vertex
			TransitStopTime.route_stop_id = route_stop_id;

			
			if (m_PT_RouteStopMap.find(route_stop_id) == m_PT_RouteStopMap.end())  // not exist
			{
				m_PT_RouteStopMap[route_stop_id] = m_PT_StopMap[TransitStopTime.stop_id];  // create link virtual stop id with physical stop id
				m_PT_RouteStopMap[route_stop_id].route_stop_id = route_stop_id;
				

			}
			
			if (m_PT_RouteStopMap.find(route_stop_id) != m_PT_RouteStopMap.end())
			{
				TransitStopTime.pt.x = m_PT_StopMap[TransitStopTime.stop_id].m_ShapePoint.x;
				TransitStopTime.pt.y = m_PT_StopMap[TransitStopTime.stop_id].m_ShapePoint.y;

			}
			else
			{

			}

			stop_times_count++;

			if (m_PT_TripMap.find(TransitStopTime.trip_id) != m_PT_TripMap.end())
			{
				m_PT_TripMap[TransitStopTime.trip_id].m_PT_StopTimeVector.push_back(TransitStopTime);  // add stop time elements into trip's list
			}

		}
		parser_st.CloseCSVFile();

	}

	CString message;

	int trip_size = m_PT_TripMap.size();
	int stop_size = m_PT_StopMap.size();
	int stop_time_size = stop_times_count;
	message.Format("%d transit trips, %d route_stops and %d stop time records are loaded.", trip_size, stop_size, stop_time_size);

	AfxMessageBox(message, MB_ICONINFORMATION);

	if (missing_stops_message.GetLength() > 0)
	{
		AfxMessageBox(missing_stops_message);
	}

	/*

	//read transfer file
	string str6 = m_ProjectDirectory +"transfers.txt";
	CT2CA pszConvertedAnsiString6 (str6.c_str());
	// construct a std::string using the LPCSTR input
	std::string  strStd6 (pszConvertedAnsiString6);
	if (parser.OpenCSVFile(strStd6))
	{
	int count =0;
	while(parser.ReadRecord())
	{
	PT_transfers transfers;

	if(parser.GetValueByFieldName("from_stop_id",transfers.from_stop_id) == false)
	break;

	if(parser.GetValueByFieldName("to_stop_id",transfers.to_stop_id) == false)
	break;

	if(parser.GetValueByFieldName("transfer_type",transfers.transfer_type) == false)
	break;

	m_PT_transfers.push_back(transfers) ;
	count++;
	}
	parser.CloseCSVFile ();
	}

	*/

	// map matching


	return true;
}

void CTLiteDoc::ReadTransitFiles_Leg(CString TransitDataProjectFolder)
{

	m_PT_network.m_ProjectDirectory = TransitDataProjectFolder;
	m_PT_network.ReadGTFFiles_Leg(m_NetworkRect);

	std::map<string, int> route_stop_id_to_no_map;

	std::map<string, int> stop_id_to_no_map;

	// create physical node
	std::map<string, PT_Stop>::iterator iPT_StopMap;
	for (iPT_StopMap = m_PT_network.m_PT_StopMap.begin(); iPT_StopMap != m_PT_network.m_PT_StopMap.end(); iPT_StopMap++)
	{
		if (fabs((*iPT_StopMap).second.m_ShapePoint.x) > 0.0001 && fabs((*iPT_StopMap).second.m_ShapePoint.y) > 0.0001)
		{
			DTANode* pNode = AddNewNode((*iPT_StopMap).second.m_ShapePoint, stop_id_to_no_map.size() + 1);
			pNode->m_Name = (*iPT_StopMap).second.stop_id;
			stop_id_to_no_map[(*iPT_StopMap).second.stop_id] = stop_id_to_no_map.size() + 1;
			TRACE("%d: %s\n", stop_id_to_no_map.size() + 1, (*iPT_StopMap).second.stop_id.c_str());
		}
	}


	// create route stop vetex

	for (iPT_StopMap = m_PT_network.m_PT_RouteStopMap.begin(); iPT_StopMap != m_PT_network.m_PT_RouteStopMap.end(); iPT_StopMap++)
	{
		if (fabs((*iPT_StopMap).second.m_ShapePoint.x) > 0.0001 && fabs((*iPT_StopMap).second.m_ShapePoint.y) > 0.0001)
		{
			DTANode* pNode = AddNewNode((*iPT_StopMap).second.m_ShapePoint, m_PT_network.m_PT_StopMap.size() + route_stop_id_to_no_map.size() + 1);  // super route-stop node;
			pNode->m_Name = (*iPT_StopMap).first;
			route_stop_id_to_no_map[(*iPT_StopMap).first] = m_PT_network.m_PT_StopMap.size()+route_stop_id_to_no_map.size() + 1;
			TRACE("%d: %s\n", route_stop_id_to_no_map.size() + 1, (*iPT_StopMap).first.c_str());
		}
	}

	//mark origin and destination zone id
	std::map<int, PT_Trip>::iterator iPT_TripMap;
	for (iPT_TripMap = m_PT_network.m_PT_TripMap.begin(); iPT_TripMap != m_PT_network.m_PT_TripMap.end(); iPT_TripMap++)
	{

		if ((*iPT_TripMap).second.m_PT_StopTimeVector.size() >= 2)
		{
			int i = 0;

			if (route_stop_id_to_no_map.find((*iPT_TripMap).second.m_PT_StopTimeVector[i].route_stop_id) != route_stop_id_to_no_map.end())
			{
				int node_id = route_stop_id_to_no_map[(*iPT_TripMap).second.m_PT_StopTimeVector[i].route_stop_id];
				DTANode* pNode = FindNodeWithNodeID(node_id);
				if (pNode != NULL)
				{
					pNode->m_ZoneID = node_id;
				}
			}

			i = (*iPT_TripMap).second.m_PT_StopTimeVector.size() - 1;
			if (route_stop_id_to_no_map.find((*iPT_TripMap).second.m_PT_StopTimeVector[i].route_stop_id) != route_stop_id_to_no_map.end())
			{
				int node_id = route_stop_id_to_no_map[(*iPT_TripMap).second.m_PT_StopTimeVector[i].route_stop_id];
				DTANode* pNode = FindNodeWithNodeID(node_id);
				if (pNode != NULL)
				{
					pNode->m_ZoneID = node_id;
				}

			}
		}
	}



	int count = 0;
	std::map<int, int> RouteMap;

	std::map<CString, int> TransitLinkMap;
	std::map<CString, int> TransitLegMap;

	// step 1: for each trip
	for (iPT_TripMap = m_PT_network.m_PT_TripMap.begin(); iPT_TripMap != m_PT_network.m_PT_TripMap.end(); iPT_TripMap++)
	{

		// step 2: for each stop time pair
		if ((*iPT_TripMap).second.m_PT_StopTimeVector.size() >= 2)
		{

			for (int i = 0; i < (*iPT_TripMap).second.m_PT_StopTimeVector.size() - 1; i++)
			{
				string route_stopid_1 = (*iPT_TripMap).second.m_PT_StopTimeVector[i].route_stop_id;
				string route_stopid_2 = (*iPT_TripMap).second.m_PT_StopTimeVector[i + 1].route_stop_id;


				if (route_stop_id_to_no_map.find(route_stopid_1) != route_stop_id_to_no_map.end() && route_stop_id_to_no_map.find(route_stopid_2) != route_stop_id_to_no_map.end())
				{

					CString transit_link_key;
					transit_link_key.Format("%s-%s", route_stopid_1.c_str(), route_stopid_2.c_str());

					if (TransitLinkMap.find(transit_link_key) == TransitLinkMap.end())  //find nothing
					{ //not defined yet
						// add a new link
						//	fprintf(st, "name,from_node_id,to_node_id,direction,length,number_of_lanes,speed_limit,lane_cap,link_type,demand_type_code");

						float link_length = 1;

						//g_CalculateP2PDistanceInMileFromLatitudeLongitude((*iPT_TripMap).second.m_PT_StopTimeVector[i].pt,
						//	(*iPT_TripMap).second.m_PT_StopTimeVector[i + 1].pt);

						if (m_PT_network.m_PT_RouteMap.find((*iPT_TripMap).second.route_id) == m_PT_network.m_PT_RouteMap.end())
							continue;


						int stop_no_1 = route_stop_id_to_no_map[route_stopid_1];
						int stop_no_2 = route_stop_id_to_no_map[route_stopid_2];


						DTALink* pLink = AddNewLink(stop_no_1, stop_no_2, false, true);
						string LinkName = route_stopid_1 + "->" + route_stopid_2;
						pLink->m_Name = LinkName;
						//								(*iPT_TripMap).second.trip_id,
						//								stopid_2,
						//								link_length);

													//define this link
						TransitLinkMap[transit_link_key] = TransitLinkMap.size();
					}

				}
				else
				{
					TRACE("Missing transit link, stop id not defined!");
				}
			}
		}

	}  // for each trip



	FILE* st = NULL;
	fopen_s(&st, m_PT_network.m_ProjectDirectory + "route.csv", "w");
	if (st == NULL)
	{
		AfxMessageBox("Error: File route.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
		return;
	}

	if (st != NULL)
	{

		fprintf(st, "agent_id,agent_type,trip_id,route_id,route_id_short_name,o_zone_id,d_zone_id,travel_time,distance,node_sequence,time_sequence\n");

		int count = 0;
		std::map<int, int> RouteMap;

		int agent_id = 1;

		// step 1: for each trip
		std::map<int, PT_Trip>::iterator iPT_TripMap;
		for (iPT_TripMap = m_PT_network.m_PT_TripMap.begin(); iPT_TripMap != m_PT_network.m_PT_TripMap.end(); iPT_TripMap++)
		{

			int o_zone_id = 1;
			int d_zone_id = 2;

			float travel_time = 0;
			float distance = 0;

			if ((*iPT_TripMap).second.m_PT_StopTimeVector.size() >= 2)
			{
				travel_time = (*iPT_TripMap).second.m_PT_StopTimeVector[(*iPT_TripMap).second.m_PT_StopTimeVector.size() - 1].arrival_time
					- (*iPT_TripMap).second.m_PT_StopTimeVector[0].arrival_time;

				int i = 0;
				{
					if (route_stop_id_to_no_map.find((*iPT_TripMap).second.m_PT_StopTimeVector[i].route_stop_id) != route_stop_id_to_no_map.end())
					{
						int node_id = route_stop_id_to_no_map[(*iPT_TripMap).second.m_PT_StopTimeVector[i].route_stop_id];
						DTANode* pNode = FindNodeWithNodeID(node_id);
						if (pNode != NULL)
						{
							o_zone_id = node_id;
						}
					}
				}

				i = (*iPT_TripMap).second.m_PT_StopTimeVector.size() - 1;
				{
					if (route_stop_id_to_no_map.find((*iPT_TripMap).second.m_PT_StopTimeVector[i].route_stop_id) != route_stop_id_to_no_map.end())
					{
						int node_id = route_stop_id_to_no_map[(*iPT_TripMap).second.m_PT_StopTimeVector[i].route_stop_id];
						DTANode* pNode = FindNodeWithNodeID(node_id);
						if (pNode != NULL)
						{
							d_zone_id = node_id;
						}
					}

				}

				for (int i = 0; i < (*iPT_TripMap).second.m_PT_StopTimeVector.size() - 1; i++)
				{

					GDPoint pt0 = (*iPT_TripMap).second.m_PT_StopTimeVector[i].pt;
					GDPoint pt1 = (*iPT_TripMap).second.m_PT_StopTimeVector[i + 1].pt;

					distance += g_CalculateP2PDistanceInMileFromLatitudeLongitude(pt0, pt1);

				}

			}

			fprintf(st, "%d,transit,%d,%s,%s,%d,%d,%.3f,%.3f,",
				agent_id,
				(*iPT_TripMap).second.trip_id,
				(*iPT_TripMap).second.route_id.c_str(),
				m_PT_network.m_PT_RouteMap[(*iPT_TripMap).second.route_id].route_long_name.c_str(),
				o_zone_id,
				d_zone_id,
				travel_time,
				distance
			);

			agent_id++;

			// step 2: for each stop-time node
			if ((*iPT_TripMap).second.m_PT_StopTimeVector.size() >= 2)
			{
				for (int i = 0; i < (*iPT_TripMap).second.m_PT_StopTimeVector.size(); i++)
				{
					fprintf(st, "%d;", route_stop_id_to_no_map[(*iPT_TripMap).second.m_PT_StopTimeVector[i].route_stop_id]);
				}
				fprintf(st, ",");
			}

			// step 2: for each stop-time time data
			if ((*iPT_TripMap).second.m_PT_StopTimeVector.size() >= 2)
			{
				for (int i = 0; i < (*iPT_TripMap).second.m_PT_StopTimeVector.size(); i++)
				{
					fprintf(st, "%s;",
						g_time_coding((*iPT_TripMap).second.m_PT_StopTimeVector[i].arrival_time).c_str());
				}
				fprintf(st, ",");
			}


			fprintf(st, "\n");
		}

		AfxMessageBox("File route.csv has been created.", MB_ICONINFORMATION);

		fclose(st);
	}

	//	m_TransitDataLoadingStatus.Format ("%d transit trips have been loaded.",m_PT_network.m_PT_TripMap.size() );

		/*	if( m_PT_network.m_PT_TripMap.size()>0 && AfxMessageBox("Do you want to generate bus trip data?",MB_YESNO|MB_ICONINFORMATION)==IDYES)
		{
		TransitTripMatching();
		}
		*/

	CalculateDrawingRectangle(true);
	m_bFitNetworkInitialized = false;
}

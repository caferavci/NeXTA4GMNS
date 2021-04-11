//Portions Copyright 2012 Xuesong Zhou.
//
//   If you help write or modify the code, please also list your names here.


#include "stdafx.h"
#include "..//Geometry.h"
#include "..//TLite.h"
#include "..//Network.h"
#include "..//TLiteDoc.h"
#include "..//Geometry.h"
#include "..//CSVParser.h"
#include "..//MainFrm.h"
#include <iostream>                          // for cout, endl
#include <fstream>                           // for ofstream
#include <sstream>
#include <iostream>     // std::cout, std::endl
#include <iomanip>      // std::setw
using namespace std;

//#include "DYNASMART-interace.h"

// add description for DYNASMART
// Important References:
// Jayakrishnan, R., Mahmassani, H. S., and Hu, T.-Y., 1994a. An evaluation tool for advanced traffic information and management systems in urban networks. Transportation Research Part C, Vol. 2, No. 3, pp. 129-147.
// 
std::map<int, int> NodeIDtoZoneNameMap;

extern int g_read_number_of_numerical_values(char* line_string, int length, std::vector<float>& values);
float g_read_float_from_a_line(FILE *f, int &end_of_line);



bool CTLiteDoc::ReadDYNASMART_ControlFile_ForAMSHub()
{
	FILE* st;

	int number_of_nodes = 0;
	int number_of_signals = 0;

	std::vector<int> DSP_signal_node_vector;
	fopen_s(&st,m_ProjectDirectory+"control.dat","r");

		int num_timing_plan = g_read_integer(st);

		double start_time = g_read_float(st);
		char  str_line[2000]; // input string
		int str_line_size;
		g_read_a_line(st,str_line, str_line_size); //  skip the second line

		// read the first block: Node - Control Type
		int last_good_node_number = 0;
		for ( int i = 0; i< m_NodeSet.size(); i++)
		{
			int node_name = g_read_integer(st);
			if(node_name == -1)
			{
				CString str;
				str.Format("Error in reading the node block of control.dat. Last valid node number = %d ", last_good_node_number);
				AfxMessageBox(str, MB_ICONINFORMATION);
				fclose(st);
				return false;
			}

			last_good_node_number = node_name;

			if(m_NodeIDtoNodeNoMap.find(node_name)!=m_NodeIDtoNodeNoMap.end())
			{
				DTANode*  pNode = m_NodeNoMap[m_NodeIDtoNodeNoMap[node_name]];
				pNode->m_ControlType  = g_read_integer(st);

				if(pNode->m_ControlType == m_ControlType_PretimedSignal || pNode->m_ControlType == m_ControlType_ActuatedSignal)
				{
					DSP_signal_node_vector.push_back (node_name);
				}


				pNode->m_NumberofPhases = g_read_integer(st);
				//pNode->m_CycleLengthInSecond = 
					g_read_integer(st);

				TRACE("\nNode Number = %d, type = %d",node_name,pNode->m_ControlType );
			}else
			{
				TRACE("\nMissing Node Number = %d",node_name);
				g_read_integer(st);
				g_read_integer(st);
				g_read_integer(st);



			}

		}

		// read the second block: Phase time and movement
		// read node by node

		for ( int i = 0; i< DSP_signal_node_vector.size(); i++)
		{

			int node_name = DSP_signal_node_vector[i];
			if(m_NodeIDtoNodeNoMap.find(node_name)!=m_NodeIDtoNodeNoMap.end())
			{
				DTANode*  pNode = m_NodeNoMap[m_NodeIDtoNodeNoMap[node_name]];

					pNode-> m_bSignalData = true;
					number_of_signals++;

					for(int p  = 0; p < pNode->m_NumberofPhases; p++)
					{
						int node_name = g_read_integer(st);

						last_good_node_number = node_name;


						int phase_ID = g_read_integer(st);

						DTANodePhase phase;
						phase.max_green  = g_read_integer(st);
						phase.min_green   = g_read_integer(st);
						phase.amber  = g_read_integer(st);
						int approach = g_read_integer(st);

						// approach node numbers (reserved 4 nodes)
						g_read_integer(st);
						g_read_integer(st);
						g_read_integer(st);
						g_read_integer(st);

						//
						// read all possible approaches
						for(int i=0; i< approach; i++)
						{

							int in_link_from_node_id = g_read_integer(st);
							int in_link_to_node_id = g_read_integer(st);
							int phase_ID2 = g_read_integer(st);     // remember to read redundant phase id

							int movement_size  = g_read_integer(st);

							for(int k=0; k<movement_size; k++)
							{
								int out_link_to_node_id = g_read_integer(st);

								CString movement_str;
								movement_str.Format("%d;%d;%d", in_link_from_node_id, in_link_to_node_id, out_link_to_node_id);
								phase.movement_vector.push_back(movement_str);
							}  // movement
						} // approach 
				

						pNode->m_node_phase_vector.push_back(phase);
					}   // control data
			}
		}  // for each node
		fclose(st);


		m_SignalDataLoadingStatus.Format ("%d signals are loaded.",number_of_signals);
	

	return true;
}


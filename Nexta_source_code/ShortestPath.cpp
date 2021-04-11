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

//shortest path calculation

// note that the current implementation is only suitable for time-dependent minimum time shortest path on FIFO network, rather than time-dependent minimum cost shortest path
// the key reference (1) Shortest Path Algorithms in Transportation Models http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.51.5192
// (2) most efficient time-dependent minimum cost shortest path algorithm for all departure times
// Time-dependent, shortest-path algorithm for real-time intelligent Agent highway system applications&quot;, Transportation Research Record 1408 ?Ziliaskopoulos, Mahmassani - 1993

#include "stdafx.h"
#include "Network.h"
extern long g_Simulation_Time_Horizon;


void DTANetworkForSP::BuildPhysicalNetwork(std::list<DTANode*>*	p_NodeSet, std::list<DTALink*>*		p_LinkSet, float RandomCostCoef,bool bOverlappingCost,  int OriginNodeNo, int DestinationNodeNo)
{

	// build a network from the current zone centroid (1 centroid here) to all the other zones' centroids (all the zones)
	float Perception_error_ratio = 0.7f;

	std::list<DTANode*>::iterator iterNode;
	std::list<DTALink*>::iterator iterLink;

	m_NodeSize = p_NodeSet->size();

	int FromNodeNo, ToNodeNo;

	int i;

	for(i=0; i< m_NodeSize; i++)
	{
		m_OutboundSizeAry[i] = 0;
		m_InboundSizeAry[i] = 0;

	}

	// add physical links

	for(iterLink = p_LinkSet->begin(); iterLink != p_LinkSet->end(); iterLink++)
	{

		FromNodeNo = (*iterLink)->m_FromNodeNo;
		ToNodeNo   = (*iterLink)->m_ToNodeNo;

		if (FromNodeNo == 19)
		{
		TRACE("");
		}

		if ((*iterLink)->m_bConnector || (*iterLink)->m_bTransit || (*iterLink)->m_bWalking)  // no connectors: here we might have some problems here, as the users cannot select a zone centroid as origin/destination
		{
			if(FromNodeNo!=OriginNodeNo && ToNodeNo !=DestinationNodeNo)  // if not the first link or last link, skip
				continue; 
		}

		if((*iterLink)->m_AdditionalCost >1)  // skip prohibited links (defined by users)
			continue;

		int link_type = (*iterLink)->m_link_type ;


		m_FromIDAry[(*iterLink)->m_LinkNo] = FromNodeNo;
		m_ToIDAry[(*iterLink)->m_LinkNo]   = ToNodeNo;

		//      TRACE("FromNodeNo %d -> ToNodeNo %d \n", FromNodeNo, ToNodeNo);
		m_OutboundNodeAry[FromNodeNo][m_OutboundSizeAry[FromNodeNo]] = ToNodeNo;
		m_OutboundLinkAry[FromNodeNo][m_OutboundSizeAry[FromNodeNo]] = (*iterLink)->m_LinkNo ;
		m_OutboundSizeAry[FromNodeNo] +=1;

		m_InboundLinkAry[ToNodeNo][m_InboundSizeAry[ToNodeNo]] = (*iterLink)->m_LinkNo  ;
		m_InboundSizeAry[ToNodeNo] +=1;

		m_LinkTimeAry[(*iterLink)->m_LinkNo] = (*iterLink)->m_Length + (*iterLink)->m_AdditionalCost;

	}

	m_LinkSize = p_LinkSet->size();
}





int DTANetworkForSP::SimplifiedTDLabelCorrecting_DoubleQueue(int origin, int departure_time, int destination, int pricing_type, float VOT,
															 int PathLinkList[MAX_NODE_SIZE_IN_A_PATH],float &TotalCost, bool distance_flag,bool check_connectivity_flag, bool debug_flag, float RandomCostCoef)   // Pointer to previous node (node)
// time -dependent label correcting algorithm with deque implementation
{

	float CostUpperBound = MAX_SPLABEL;

    int	temp_reversed_PathLinkList[MAX_NODE_SIZE_IN_A_PATH];
	int i;
	debug_flag = 1;

	if(m_OutboundSizeAry[origin]== 0)
		return 0;

	if(origin == destination)
		return 0;

	for(i=0; i <m_NodeSize; i++) // Initialization for all nodes
	{
		NodePredAry[i]  = -1;
		NodeStatusAry[i] = 0;

		LabelTimeAry[i] = MAX_SPLABEL;
		LabelCostAry[i] = MAX_SPLABEL;

	}
 
	// Initialization for origin node
	LabelTimeAry[origin] = float(departure_time);
	LabelCostAry[origin] = 0;

	SEList_clear();
	SEList_push_front(origin);

	int FromNodeNo, LinkNo, ToNodeNo;


	float NewTime, NewCost;
	while(!SEList_empty())
	{
		FromNodeNo  = SEList_front();
		SEList_pop_front();

		if(debug_flag)
			TRACE("\nScan from node %d",FromNodeNo);

		NodeStatusAry[FromNodeNo] = 2;        //scaned

		for(i=0; i<m_OutboundSizeAry[FromNodeNo];  i++)  // for each arc (i,j) belong A(j)
		{
			LinkNo = m_OutboundLinkAry[FromNodeNo][i];
			ToNodeNo = m_OutboundNodeAry[FromNodeNo][i];

			if(ToNodeNo == origin)
				continue;


			  TRACE("\n   to node %d",ToNodeNo);
			// need to check here to make sure  LabelTimeAry[FromNodeNo] is feasible.

			  float travel_cost = m_LinkTimeAry[LinkNo];
			NewTime	 = LabelTimeAry[FromNodeNo] + travel_cost;  // time-dependent travel times come from simulator
			NewCost    = LabelCostAry[FromNodeNo] + travel_cost;       // costs come from time-dependent tolls, VMS, information provisions

			if(NewCost < LabelCostAry[ToNodeNo] &&  NewCost < CostUpperBound) // be careful here: we only compare cost not time
			{

				LabelTimeAry[ToNodeNo] = NewTime;
				LabelCostAry[ToNodeNo] = NewCost;
				NodePredAry[ToNodeNo]   = FromNodeNo;
				LinkNoAry[ToNodeNo] = LinkNo;

				if (ToNodeNo == destination) // special feature 7.2: update upper bound cost
				{
					CostUpperBound = LabelCostAry[ToNodeNo];
				}

				// Dequeue implementation
				//
				if(NodeStatusAry[ToNodeNo]==2) // in the SEList_TD before
				{
					SEList_push_front(ToNodeNo);
					NodeStatusAry[ToNodeNo] = 1;
				}
				if(NodeStatusAry[ToNodeNo]==0)  // not be reached
				{
					SEList_push_back(ToNodeNo);
					NodeStatusAry[ToNodeNo] = 1;
				}

				//another condition: in the SELite now: there is no need to put this node to the SEList, since it is already there.
			}

		}      // end of for each link

	} // end of while

	if(check_connectivity_flag) 
		return 0;


		int LinkSize = 0;
		int PredNode = NodePredAry[destination];	
		temp_reversed_PathLinkList[LinkSize++] = LinkNoAry[destination];

		while(PredNode != origin && PredNode!=-1 && LinkSize< MAX_NODE_SIZE_IN_A_PATH) // scan backward in the predessor array of the shortest path calculation results
			{
				ASSERT(LinkSize< MAX_NODE_SIZE_IN_A_PATH-1);
				temp_reversed_PathLinkList[LinkSize++] = LinkNoAry[PredNode];

				PredNode = NodePredAry[PredNode];
			}
	
		int j = 0;
		for(i = LinkSize-1; i>=0; i--)
		{
		PathLinkList[j++] = temp_reversed_PathLinkList[i];
		}

		TotalCost = LabelCostAry[destination];

		if(TotalCost > MAX_SPLABEL-10)
		{
			//ASSERT(false);
			return 0;
		}

		return LinkSize+1; // as }

}





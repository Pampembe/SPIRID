/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <iostream>
#include <cmath>

#include <SPIRID.h>
using namespace SPIRID;

int main()
{
#ifdef DEBUG
	std::cout << "collecting neighbor faces & nodes for special points: ";
	std::cout << std::endl;
	std::list<sGrid> starF = sGrid({0,0,0,1,1,1,1}).nodeNeighborFaces(2,2);
	std::list<std::pair<sGrid, unsigned short> > starN = sGrid({0,0,0,1,1,1,1}).nodeNeighborNodes(2,2);
	std::list<std::pair<sGrid, unsigned short> > starE = sGrid({0,0,0,1,1,1,1}).nodeConnectedEdges(2,2);
	std::list<std::pair<sGrid, unsigned short> > starO = sGrid({0,0,0,1,1,1,1}).nodeOuterRingEdges(2,2);
	std::cout << "Nodes           : ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starN.begin(); it != starN.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Connected Edges :         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starE.begin(); it != starE.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Outer Ring Edges:         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starO.begin(); it != starO.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Faces           :         ";
	for (std::list<sGrid>::const_iterator it = starF.begin(); it != starF.end(); ++it) std::cout << *it << "  ";
	std::cout << std::endl;

	starF = sGrid({0,0,0,1,1,1,1}).nodeNeighborFaces(2,3);
	starN = sGrid({0,0,0,1,1,1,1}).nodeNeighborNodes(2,3);
	starE = sGrid({0,0,0,1,1,1,1}).nodeConnectedEdges(2,3);
	starO = sGrid({0,0,0,1,1,1,1}).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starN.begin(); it != starN.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Connected Edges :         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starE.begin(); it != starE.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Outer Ring Edges:         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starO.begin(); it != starO.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Faces           :         ";
	for (std::list<sGrid>::const_iterator it = starF.begin(); it != starF.end(); ++it) std::cout << *it << "  ";
	std::cout << std::endl;

	starF = sGrid({0,0,0,0,0,0,1}).nodeNeighborFaces(2,3);
	starN = sGrid({0,0,0,0,0,0,1}).nodeNeighborNodes(2,3);
	starE = sGrid({0,0,0,0,0,0,1}).nodeConnectedEdges(2,3);
	starO = sGrid({0,0,0,0,0,0,1}).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starN.begin(); it != starN.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Connected Edges :         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starE.begin(); it != starE.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Outer Ring Edges:         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starO.begin(); it != starO.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Faces           :         ";
	for (std::list<sGrid>::const_iterator it = starF.begin(); it != starF.end(); ++it) std::cout << *it << "  ";
	std::cout << std::endl;

	starF = sGrid({0,0,0,0,0,0,1}).nodeNeighborFaces(2,2);
	starN = sGrid({0,0,0,0,0,0,1}).nodeNeighborNodes(2,2);
	starE = sGrid({0,0,0,0,0,0,1}).nodeConnectedEdges(2,2);
	starO = sGrid({0,0,0,0,0,0,1}).nodeOuterRingEdges(2,2);
	std::cout << "Nodes           : ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starN.begin(); it != starN.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Connected Edges :         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starE.begin(); it != starE.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Outer Ring Edges:         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starO.begin(); it != starO.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Faces           :         ";
	for (std::list<sGrid>::const_iterator it = starF.begin(); it != starF.end(); ++it) std::cout << *it << "  ";
	std::cout << std::endl;

	starF = sGrid({0,0,0,1,0,1,1}).nodeNeighborFaces(2,1);
	starN = sGrid({0,0,0,1,0,1,1}).nodeNeighborNodes(2,1);
	starE = sGrid({0,0,0,1,0,1,1}).nodeConnectedEdges(2,1);
	starO = sGrid({0,0,0,1,0,1,1}).nodeOuterRingEdges(2,1);
	std::cout << "Nodes           : ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starN.begin(); it != starN.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Connected Edges :         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starE.begin(); it != starE.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Outer Ring Edges:         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starO.begin(); it != starO.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Faces           :         ";
	for (std::list<sGrid>::const_iterator it = starF.begin(); it != starF.end(); ++it) std::cout << *it << "  ";
	std::cout << std::endl;

	starF = sGrid({0,0,0,1,0,1,1}).nodeNeighborFaces(2,3);
	starN = sGrid({0,0,0,1,0,1,1}).nodeNeighborNodes(2,3);
	starE = sGrid({0,0,0,1,0,1,1}).nodeConnectedEdges(2,3);
	starO = sGrid({0,0,0,1,0,1,1}).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starN.begin(); it != starN.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Connected Edges :         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starE.begin(); it != starE.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Outer Ring Edges:         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starO.begin(); it != starO.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Faces           :         ";
	for (std::list<sGrid>::const_iterator it = starF.begin(); it != starF.end(); ++it) std::cout << *it << "  ";
	std::cout << std::endl;

	starF = sGrid({0,0,0,0,0,1,1}).nodeNeighborFaces(2,3);
	starN = sGrid({0,0,0,0,0,1,1}).nodeNeighborNodes(2,3);
	starE = sGrid({0,0,0,0,0,1,1}).nodeConnectedEdges(2,3);
	starO = sGrid({0,0,0,0,0,1,1}).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starN.begin(); it != starN.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Connected Edges :         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starE.begin(); it != starE.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Outer Ring Edges:         ";
	for (std::list<std::pair<sGrid, unsigned short> >::const_iterator it = starO.begin(); it != starO.end(); ++it) std::cout << it->first << it->second << " ";
	std::cout << std::endl;
	std::cout << "Faces           :         ";
	for (std::list<sGrid>::const_iterator it = starF.begin(); it != starF.end(); ++it) std::cout << *it << "  ";
	std::cout << std::endl;

	std::cout << std::endl;
#endif //DEBUG

	angle::unitPi();

	/*
		std::cout << "Polar coordinates of special points in high grid level:" << std::endl;
		sGrid p0(5);
		sGrid p1(5);
		sGrid p2(5);
		sGrid p3(5);
		for (size_t it = 1; it<40; it++)
		{
			p0.setExtend(it,0);
			p1.setExtend(it,1);
			p2.setExtend(it,2);
			p3.setExtend(it,3);
		}
		std::cout << p0 << p0.toPolar() << std::endl;
		std::cout << p1 << p1.toPolar() << std::endl;
		std::cout << p2 << p2.toPolar() << std::endl;
		std::cout << p3 << p3.toPolar() << std::endl;
		std::cout << std::endl;

		std::cout << "Polar coordinates of nodes in grid level9:" << std::endl;
		std::cout << p0 << "node0: " << p0.toLocalPolar(9,3,1,12,0) << std::endl;
		std::cout << p0 << "node1: " << p0.toLocalPolar(9,3,1,12,1) << std::endl;
		std::cout << p0 << "node2: " << p0.toLocalPolar(9,3,1,12,2) << std::endl;
		std::cout << p0 << "node3: " << p0.toLocalPolar(9,3,1,12,3) << std::endl;
		std::cout << std::endl;

		std::cout << "Polar coordinates of nodes in grid level9, node level 15:" << std::endl;
		std::cout << p0 << "node0: " << p0.toLocalPolar(9,3,1,15,0) << std::endl;
		std::cout << p0 << "node1: " << p0.toLocalPolar(9,3,1,15,1) << std::endl;
		std::cout << p0 << "node2: " << p0.toLocalPolar(9,3,1,15,2) << std::endl;
		std::cout << p0 << "node3: " << p0.toLocalPolar(9,3,1,15,3) << std::endl;
		std::cout << std::endl;

		std::cout << "Polar coordinates of nodes in grid level13:" << std::endl;
		std::cout << p0 << "node0: " << p0.toLocalPolar(13,3,1,13,0) << std::endl;
		std::cout << p0 << "node1: " << p0.toLocalPolar(13,3,1,13,1) << std::endl;
		std::cout << p0 << "node2: " << p0.toLocalPolar(13,3,1,13,2) << std::endl;
		std::cout << p0 << "node3: " << p0.toLocalPolar(13,3,1,13,3) << std::endl;
		std::cout << std::endl;

		std::cout << "Polar coordinates of nodes in grid level13, node level 14:" << std::endl;
		std::cout << p0 << "node0: " << p0.toLocalPolar(13,3,1,14,0) << std::endl;
		std::cout << p0 << "node1: " << p0.toLocalPolar(13,3,1,14,1) << std::endl;
		std::cout << p0 << "node2: " << p0.toLocalPolar(13,3,1,14,2) << std::endl;
		std::cout << p0 << "node3: " << p0.toLocalPolar(13,3,1,14,3) << std::endl;
		std::cout << std::endl;

		std::cout << "Polar coordinates of nodes in grid level13, node level 15:" << std::endl;
		std::cout << p0 << "node0: " << p0.toLocalPolar(13,3,1,15,0) << std::endl;
		std::cout << p0 << "node1: " << p0.toLocalPolar(13,3,1,15,1) << std::endl;
		std::cout << p0 << "node2: " << p0.toLocalPolar(13,3,1,15,2) << std::endl;
		std::cout << p0 << "node3: " << p0.toLocalPolar(13,3,1,15,3) << std::endl;
		std::cout << std::endl;


		std::cout << "Polar coordinates of special points in different grid levels:" << std::endl;
		for (size_t it=0; it<40; it++)
		{
			std::cout << "Level " << it << ": " << p0.toPolar(it) << p1.toPolar(it) << p2.toPolar(it) << p3.toPolar(it) << std::endl;
		}
		std::cout << std::endl;


		//initialize Q as all faces in levels 0,1,2,3
		sGrid Q[512];
		for (size_t it0 = 0; it0<8; it0++)
		{
			for (size_t it1 = 0; it1<4; it1++)
			{
				for (size_t it2 = 0; it2<4; it2++)
				{
					for (size_t it3 = 0; it3<4; it3++)
					{
						Q[it0+8*it1+32*it2+128*it3].setExtend(0,it0);
						Q[it0+8*it1+32*it2+128*it3].setExtend(1,it1);
						Q[it0+8*it1+32*it2+128*it3].setExtend(2,it2);
						Q[it0+8*it1+32*it2+128*it3].setExtend(3,it3);
					}
				}
			}
		}




		std::cout << "Checking local polar coordiantes level0:" << std::endl;
		std::cout << Q[0] << ", node1, edge2: " << Q[0].toLocalPolar(0,1,2,0) << std::endl;
		std::cout << Q[0] << ", node1, edge3: " << Q[0].toLocalPolar(0,1,3,0) << std::endl;
		std::cout << Q[0] << ", node2, edge1: " << Q[0].toLocalPolar(0,2,1,0) << std::endl;
		std::cout << Q[0] << ", node2, edge3: " << Q[0].toLocalPolar(0,2,3,0) << std::endl;
		std::cout << Q[0] << ", node3, edge1: " << Q[0].toLocalPolar(0,3,1,0) << std::endl;
		std::cout << Q[0] << ", node3, edge2: " << Q[0].toLocalPolar(0,3,2,0) << std::endl;
		std::cout << Q[0] << ", node1, edge1: " << Q[0].toLocalPolar(0,1,1,0) << std::endl;
		std::cout << std::endl;

		std::cout << "Checking local polar coordiantes level1:" << std::endl;
		std::cout << Q[8] << ", node1, edge2: " << Q[8].toLocalPolar(1,1,2,1) << std::endl;
		std::cout << Q[8] << ", node1, edge3: " << Q[8].toLocalPolar(1,1,3,1) << std::endl;
		std::cout << Q[8] << ", node2, edge1: " << Q[8].toLocalPolar(1,2,1,1) << std::endl;
		std::cout << Q[8] << ", node2, edge3: " << Q[8].toLocalPolar(1,2,3,1) << std::endl;
		std::cout << Q[8] << ", node3, edge1: " << Q[8].toLocalPolar(1,3,1,1) << std::endl;
		std::cout << Q[8] << ", node3, edge2: " << Q[8].toLocalPolar(1,3,2,1) << std::endl;

		std::cout << Q[16] << ", node1, edge2: " << Q[16].toLocalPolar(1,1,2,1) << std::endl;
		std::cout << Q[16] << ", node1, edge3: " << Q[16].toLocalPolar(1,1,3,1) << std::endl;
		std::cout << Q[16] << ", node2, edge1: " << Q[16].toLocalPolar(1,2,1,1) << std::endl;
		std::cout << Q[16] << ", node2, edge3: " << Q[16].toLocalPolar(1,2,3,1) << std::endl;
		std::cout << Q[16] << ", node3, edge1: " << Q[16].toLocalPolar(1,3,1,1) << std::endl;
		std::cout << Q[16] << ", node3, edge2: " << Q[16].toLocalPolar(1,3,2,1) << std::endl;

		std::cout << Q[24] << ", node1, edge2: " << Q[24].toLocalPolar(1,1,2,1) << std::endl;
		std::cout << Q[24] << ", node1, edge3: " << Q[24].toLocalPolar(1,1,3,1) << std::endl;
		std::cout << Q[24] << ", node2, edge1: " << Q[24].toLocalPolar(1,2,1,1) << std::endl;
		std::cout << Q[24] << ", node2, edge3: " << Q[24].toLocalPolar(1,2,3,1) << std::endl;
		std::cout << Q[24] << ", node3, edge1: " << Q[24].toLocalPolar(1,3,1,1) << std::endl;
		std::cout << Q[24] << ", node3, edge2: " << Q[24].toLocalPolar(1,3,2,1) << std::endl;

		std::cout << Q[0] << ", node1, edge2: " << Q[0].toLocalPolar(1,1,2,1) << std::endl;
		std::cout << Q[0] << ", node1, edge3: " << Q[0].toLocalPolar(1,1,3,1) << std::endl;
		std::cout << Q[0] << ", node2, edge1: " << Q[0].toLocalPolar(1,2,1,1) << std::endl;
		std::cout << Q[0] << ", node2, edge3: " << Q[0].toLocalPolar(1,2,3,1) << std::endl;
		std::cout << Q[0] << ", node3, edge1: " << Q[0].toLocalPolar(1,3,1,1) << std::endl;
		std::cout << Q[0] << ", node3, edge2: " << Q[0].toLocalPolar(1,3,2,1) << std::endl;
		std::cout << std::endl;

		std::cout << "Face geometry level1:" << std::endl;
		std::cout << Q[0] << "edge1: " << Q[0].edgeLength(1, 1);
		std::cout << ", edge2: " << Q[0].edgeLength(1, 2);
		std::cout << ", edge3: " << Q[0].edgeLength(1, 3) << std::endl;
		std::cout << Q[0] << "intAngle1: " << Q[0].interiorAngle(1, 1);
		std::cout << ", intAngle2: " << Q[0].interiorAngle(1, 2);
		std::cout << ", intAngle3: " << Q[0].interiorAngle(1, 3) << std::endl;
		std::cout << Q[8] << "edge1: " << Q[8].edgeLength(1, 1);
		std::cout << ", edge2: " << Q[8].edgeLength(1, 2);
		std::cout << ", edge3: " << Q[8].edgeLength(1, 3) << std::endl;
		std::cout << Q[8] << "intAngle1: " << Q[8].interiorAngle(1, 1);
		std::cout << ", intAngle2: " << Q[8].interiorAngle(1, 2);
		std::cout << ", intAngle3: " << Q[8].interiorAngle(1, 3) << std::endl;
		std::cout << std::endl;


		std::cout << "Comparing distances between grid coordinates and polar coordinates:" << std::endl;
		size_t l1_max=3;
		size_t l2_max=4;
		size_t i_max=450;
		size_t j_max=453;
	*/
	/*
			fp_type difference = 0;
			fp_type max_difference = 0;
			for (size_t l1=0; l1<5; l1++)
			{
				for (size_t l2=0; l2<5; l2++)
				{
					for (size_t i = 0; i<512; i++)
					{
						for (size_t j = 0; j<512; j++)
						{
							difference = std::abs(SPIRID::sPolar::distance(Q[i].toPolar(l1),Q[j].toPolar(l2)) - SPIRID::sGrid::distance(l1,Q[i],l2,Q[j]));
							if (std::isnan(difference)) std::cerr << "nan" << std::endl;
							if( difference > max_difference)
							{
								max_difference = difference;
								i_max = i;
								j_max = j;
								l1_max = l1;
								l2_max = l2;
							}
						}
					}
				}
			}
	*/
	/*
		std::cout << "(" << l1_max << "," << l2_max << ")";
		std::cout << "(" << i_max << "," << j_max << "): ";
		std::cout << Q[i_max] << Q[i_max].toPolar(l1_max) << Q[j_max] << Q[j_max].toPolar(l2_max) << ": ";
		std::cout << "polar: " << SPIRID::sPolar::distance(Q[i_max].toPolar(l1_max),Q[j_max].toPolar(l2_max)) << ",";
		std::cout << " grid: " << SPIRID::sGrid::distance(l1_max,Q[i_max],l2_max,Q[j_max]) << ",";
		std::cout << " difference: " << SPIRID::sPolar::distance(Q[i_max].toPolar(l1_max),Q[j_max].toPolar(l2_max)) - SPIRID::sGrid::distance(l1_max,Q[i_max],l2_max,Q[j_max]);
		std::cout << std::endl;
		std::cout << std::endl;



		std::cout << "Neighbor faces:" << std::endl;
		std::cout << Q[0] << " neighbor 1, level 0:" << Q[0].neighborFace(0,1) << std::endl;
		std::cout << Q[0] << " neighbor 2, level 0:" << Q[0].neighborFace(0,2) << std::endl;
		std::cout << Q[0] << " neighbor 3, level 0:" << Q[0].neighborFace(0,3) << std::endl;
		std::cout << Q[20] << " neighbor 1, level 1:" << Q[20].neighborFace(1,1) << std::endl;
		std::cout << Q[20] << " neighbor 1, level 3:" << Q[20].neighborFace(3,1) << std::endl;
		for (size_t it = 0; it < 20; it++)
		{
			std::cout << Q[4*it] << " neighbor 3, level 2:" << Q[4*it].neighborFace(2,3) << std::endl;
		}
		std::cout << p1 << " neighbor 1, level 20:" << p1.neighborFace(20,1) << std::endl;
		std::cout << p1 << " neighbor 2, level 20:" << p1.neighborFace(20,2) << std::endl;
		std::cout << p1 << " neighbor 3, level 20:" << p1.neighborFace(20,3) << std::endl;
		std::cout << "distance to neighbor: " << SPIRID::sGrid::distance(10,p1,10,p1.neighborFace(10,3)) << std::endl;
		std::cout << std::endl;

		std::cout << "Face areas:" << std::endl;
		std::cout << Q[0] << " level 0: " << Q[0].area(0) << std::endl;
		std::cout << Q[0] << " level 1: " << Q[0].area(1) << std::endl;
		std::cout << Q[0].neighborFace(1,1) << " level 1: " << Q[0].neighborFace(1,1).area(1) << std::endl;
		std::cout << Q[0].neighborFace(1,2) << " level 1: " << Q[0].neighborFace(1,2).area(1) << std::endl;
		std::cout << Q[0].neighborFace(1,3) << " level 1: " << Q[0].neighborFace(1,3).area(1) << std::endl;
		std::cout << Q[0] << " level 2: " << Q[0].area(2) << std::endl;
		std::cout << Q[0].neighborFace(2,1) << " level 2: " << Q[0].neighborFace(2,1).area(2) << std::endl;
		std::cout << Q[0].neighborFace(2,2) << " level 2: " << Q[0].neighborFace(2,2).area(2) << std::endl;
		std::cout << Q[0].neighborFace(2,3) << " level 2: " << Q[0].neighborFace(2,3).area(2) << std::endl;
		std::cout << Q[0] << " level 3: " << Q[0].area(3) << std::endl;
		std::cout << Q[0] << " level 4: " << Q[0].area(4) << std::endl;
		std::cout << Q[0] << " level 5: " << Q[0].area(5) << std::endl;
		std::cout << Q[0] << " level 19: " << Q[0].area(19) << std::endl;
		std::cout << Q[0].neighborFace(19,1) << " level 19: " << Q[0].neighborFace(19,1).area(19) << std::endl;
		std::cout << Q[0].neighborFace(19,2) << " level 19: " << Q[0].neighborFace(19,2).area(19) << std::endl;
		std::cout << Q[0].neighborFace(19,3) << " level 19: " << Q[0].neighborFace(19,3).area(19) << std::endl;
		std::cout << Q[0] << " level 20: " << Q[0].area(20) << std::endl;
		std::cout << Q[0].neighborFace(20,1) << " level 20: " << Q[0].neighborFace(20,1).area(20) << std::endl;
		std::cout << Q[0].neighborFace(20,2) << " level 20: " << Q[0].neighborFace(20,2).area(20) << std::endl;
		std::cout << Q[0].neighborFace(20,3) << " level 20: " << Q[0].neighborFace(20,3).area(20) << std::endl;
	*/

	/*
	for (size_t it = 0; it < 10; it++)
	{
		std::cout << Q[it] << " level 0: " << Q[it].area(0) << std::endl;
		std::cout << Q[0] << " level " << it << ": " << Q[0].area(it) << std::endl;
		std::cout << Q[it] << " level " << it << ": " << Q[it].area(it) << std::endl;
	}

	std::cout << std::endl;
	//F03010211003030211113332313032333
	//F03010211003030211113332313032333.toPolar(33,2);
	std::cout << sGrid({0,0,0,1,1,0,0,1,0,0,0,0,1,1,0,1,0,0,0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,0,1,1,1,1,0,1,1,0,0,1,1,0,1,1,1,1,1,1,1}) << ": ";
	std::cout << sGrid({0,0,0,1,1,0,0,1,0,0,0,0,1,1,0,1,0,0,0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,0,1,1,1,1,0,1,1,0,0,1,1,0,1,1,1,1,1,1,1}).toPolar(33,2) << std::endl;
	*/

	sGrid X(7);
	X.setExtend(0,0);
	X.setExtend(1,3);
	X.setExtend(2,0);
	X.setExtend(3,1);
	X.setExtend(4,0);
	X.setExtend(5,2);
	X.setExtend(6,1);
	X.setExtend(7,0);
	std::cout << X << ": " << X.toPolar(7,3) << std::endl;

	X.resize(6);
	X.setExtend(5,0);
	X.setExtend(6,3);
	std::cout << X << X.neighborFace(6,1) << X.neighborFace(6,2) << X.neighborFace(6,3) << std::endl;

	size_t level = 30;
	size_t fullSearchLevel = 25;

	funcGraphPoint<> F(sGrid::searchMinPoint(level,SPIRID::sGrid::test));
	std::cout << F.dPoint.first << F.dPoint.second << ": " << F.fValue << " - " << F.dPoint.first.toPolar(level,F.dPoint.second) << std::endl;
	std::cout << std::endl;

	sGrid TMP(F.dPoint.first);
	sGrid TMP2(F.dPoint.first);
	unsigned short loc2 = 3;

	scaledFP minValue = sGrid::test(level,TMP,3);
	scaledFP dummy = sGrid::test(level,TMP,1);
/*
    for (size_t it = fullSearchLevel; it <= level; it++)
	{
		TMP.setExtend(it,0);
	}

	scaledFP minValue = sGrid::test(level,TMP,3);
	bool hasNextNode = TMP.stepToNextFace(level,fullSearchLevel);
//    scaledFP dummy = sGrid::test(level,TMP,0);
//    if (dummy < minValue) minValue = dummy;
	scaledFP dummy = sGrid::test(level,TMP,1);
	if (dummy < minValue) {
		minValue = dummy;
		TMP2=TMP;
		loc2=1;
	};
	dummy = sGrid::test(level,TMP,2);
	if (dummy < minValue) {
		minValue = dummy;
		TMP2=TMP;
		loc2=2;
	};
	dummy = sGrid::test(level,TMP,3);
	if (dummy < minValue) {
		minValue = dummy;
		TMP2=TMP;
		loc2=3;
	};
	while (hasNextNode)
	{
		hasNextNode = TMP.stepToNextFace(level,fullSearchLevel);
//        dummy = sGrid::test(level,TMP,0);
//        if (dummy < minValue) minValue = dummy;
		dummy = sGrid::test(level,TMP,1);
		if (dummy < minValue) {
			minValue = dummy;
			TMP2=TMP;
			loc2=1;
		};
		dummy = sGrid::test(level,TMP,2);
		if (dummy < minValue) {
			minValue = dummy;
			TMP2=TMP;
			loc2=2;
		};
		dummy = sGrid::test(level,TMP,3);
		if (dummy < minValue) {
			minValue = dummy;
			TMP2=TMP;
			loc2=3;
		};
	}
	std::cout << TMP2 << loc2 << " minValue: " << minValue << std::endl;
*/

	minValue = sGrid::test(level,TMP,3);
	sGrid::subGridScanner sTMP(TMP.begin(level,fullSearchLevel));
	while (sTMP != TMP.end())
	{
		dummy = sGrid::test(level,*sTMP,1);
		if (dummy < minValue) {
			minValue = dummy;
			TMP2=*sTMP;
			loc2=1;
		};
		dummy = sGrid::test(level,*sTMP,2);
		if (dummy < minValue) {
			minValue = dummy;
			TMP2=*sTMP;
			loc2=2;
		};
		dummy = sGrid::test(level,*sTMP,3);
		if (dummy < minValue) {
			minValue = dummy;
			TMP2=*sTMP;
			loc2=3;
		};
		++sTMP;
	}
	std::cout << TMP2 << loc2 << " minValue: " << minValue << std::endl;
	std::cout << TMP2 << 1 << " Value: " << sGrid::test(30,TMP2,1) << std::endl;
	std::cout << TMP2 << 2 << " Value: " << sGrid::test(30,TMP2,2) << std::endl;
	std::cout << TMP2 << 3 << " Value: " << sGrid::test(30,TMP2,3) << std::endl;
	std::cout << std::endl;
	std::cout << TMP2.neighborFace(30,1) << 1 << " Value: " << sGrid::test(30,TMP2.neighborFace(30,1),1) << std::endl;
	std::cout << TMP2.neighborFace(30,1) << 2 << " Value: " << sGrid::test(30,TMP2.neighborFace(30,1),2) << std::endl;
	std::cout << TMP2.neighborFace(30,1) << 3 << " Value: " << sGrid::test(30,TMP2.neighborFace(30,1),3) << std::endl;
	std::cout << TMP2.neighborFace(30,2) << 1 << " Value: " << sGrid::test(30,TMP2.neighborFace(30,2),1) << std::endl;
	std::cout << TMP2.neighborFace(30,2) << 2 << " Value: " << sGrid::test(30,TMP2.neighborFace(30,2),2) << std::endl;
	std::cout << TMP2.neighborFace(30,2) << 3 << " Value: " << sGrid::test(30,TMP2.neighborFace(30,2),3) << std::endl;
	std::cout << TMP2.neighborFace(30,3) << 1 << " Value: " << sGrid::test(30,TMP2.neighborFace(30,3),1) << std::endl;
	std::cout << TMP2.neighborFace(30,3) << 2 << " Value: " << sGrid::test(30,TMP2.neighborFace(30,3),2) << std::endl;
	std::cout << TMP2.neighborFace(30,3) << 3 << " Value: " << sGrid::test(30,TMP2.neighborFace(30,3),3) << std::endl;

	return 0;
}

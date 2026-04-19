/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <cmath>

#include <SPIRID.h>
using namespace SPIRID;

//#define MIN_SEARCH_CHECK
//#define DISTANCE_ERROR_CHECK
//#define GRID_NEIGHBOR_CHECK
//#define FACE_GEOMETRY_CHECK
//#define TO_POLAR_CHECK

class refPointTestFunction
{
	sPolar refPoint;
public:
	refPointTestFunction(const sPolar& P) : refPoint(P) {};
	scaledFP operator() (size_t level, const sGrid& P, unsigned short location) const
	{
		sPolar TP(P.toPolar(level,location));
		return scaledFP(SQRT((TP.getTheta()-refPoint.getTheta())*(TP.getTheta()-refPoint.getTheta())
		                     + (TP.getPhi()-refPoint.getPhi())*(TP.getPhi()-refPoint.getPhi())),0);
	};
};


int main()
{
	angle::unitDeg();
	size_t level = sGrid::getAccuracyBits();

#ifdef MIN_SEARCH_CHECK

	/********************  testing conversion from sPolar to sGrid  *********************/
	size_t pointCount = 1000;
	std::srand(0);
	std::cout << "generate " << pointCount << " new sGrid points from sPolar point - random coordinates" << std::endl;
	std::vector<sPolar> polarPoints(pointCount);
	std::for_each(polarPoints.begin(), polarPoints.end(), [](sPolar& P) {
		P = sPolar(pi*rand()/RAND_MAX, two_pi*rand()/RAND_MAX);
	});


	std::vector<sGrid> gridPointsRobustSearch(pointCount);
	auto t1 = std::chrono::high_resolution_clock::now();
	for (size_t it = 0; it < pointCount; ++it)
	{
		gridPointsRobustSearch[it] = sGrid(polarPoints[it], level, sGrid::minSearch<sGrid::polarDistanceToRef>);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> robustSearchTime = t2 - t1;
	fp_type maxDifference = 0;
	size_t max_it_Search = 0;
	for (size_t it = 0; it < pointCount; ++it)
	{
		if ( maxDifference < sPolar::distance(polarPoints[it],gridPointsRobustSearch[it].toPolar()) )
		{
			maxDifference = sPolar::distance(polarPoints[it],gridPointsRobustSearch[it].toPolar());
			max_it_Search = it;
		}
	}
	std::cout << "minSearch test: sGrid point with highest distance to given sPolar - execution time: " << robustSearchTime.count() << "ms" << std::endl;
	std::cout << "polar[" << max_it_Search << "]: " << polarPoints[max_it_Search] << std::endl;
	std::cout << " robust grid search: " << gridPointsRobustSearch[max_it_Search] << "-" << gridPointsRobustSearch[max_it_Search].toPolar() << std::endl;
	std::cout << " polar difference  : (" << polarPoints[max_it_Search].getTheta()-gridPointsRobustSearch[max_it_Search].toPolar().getTheta();
	std::cout << "," << polarPoints[max_it_Search].getPhi()-gridPointsRobustSearch[max_it_Search].toPolar().getPhi() << ")";
	std::cout << ", polar distance: " << sPolar::distance(polarPoints[max_it_Search],gridPointsRobustSearch[max_it_Search].toPolar()) << std::endl;
	std::cout << std::endl;


	std::vector<sGrid> gridPointsFastSearch(pointCount);
	t1 = std::chrono::high_resolution_clock::now();
	for (size_t it = 0; it < pointCount; ++it)
	{
		gridPointsFastSearch[it] = sGrid(polarPoints[it], level, sGrid::fastMinSearch<sGrid::polarDistanceToRef>);
	}
	t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> fastSearchTime = t2 - t1;
	maxDifference = 0;
	max_it_Search = 0;
	for (size_t it = 0; it < pointCount; ++it)
	{
		if ( maxDifference < sPolar::distance(polarPoints[it],gridPointsFastSearch[it].toPolar()) )
		{
			maxDifference = sPolar::distance(polarPoints[it],gridPointsFastSearch[it].toPolar());
			max_it_Search = it;
		}
	}
	std::cout << "fastMinSearch test: sGrid point with highest distance to given sPolar - execution time: " << fastSearchTime.count() << "ms" << std::endl;
	std::cout << "polar[" << max_it_Search << "]: " << polarPoints[max_it_Search] << std::endl;
	std::cout << " fast grid search  : " << gridPointsFastSearch[max_it_Search] << "-" << gridPointsFastSearch[max_it_Search].toPolar() << std::endl;
	std::cout << " robust grid search: " << gridPointsRobustSearch[max_it_Search] << "-" << gridPointsRobustSearch[max_it_Search].toPolar() << std::endl;
	std::cout << " polar difference  : (" << polarPoints[max_it_Search].getTheta()-gridPointsFastSearch[max_it_Search].toPolar().getTheta();
	std::cout << "," << polarPoints[max_it_Search].getPhi()-gridPointsFastSearch[max_it_Search].toPolar().getPhi() << ")";
	std::cout << ", polar distance: " << sPolar::distance(polarPoints[max_it_Search],gridPointsFastSearch[max_it_Search].toPolar()) << std::endl;
	std::cout << " grid distance fast vs robust search: " << sGrid::distance(gridPointsRobustSearch[max_it_Search],gridPointsFastSearch[max_it_Search].toPolar()) << std::endl;
	std::cout << std::endl;


	std::vector<sGrid> gridPointsHybridSearch(pointCount);
	t1 = std::chrono::high_resolution_clock::now();
	for (size_t it = 0; it < pointCount; ++it)
	{
		gridPointsHybridSearch[it] = sGrid(polarPoints[it], level, sGrid::hybridMinSearch<sGrid::polarDistanceToRef>);
	}
	t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> hybridSearchTime = t2 - t1;
	maxDifference = 0;
	max_it_Search = 0;
	for (size_t it = 0; it < pointCount; ++it)
	{
		if ( maxDifference < sPolar::distance(polarPoints[it],gridPointsHybridSearch[it].toPolar()) )
		{
			maxDifference = sPolar::distance(polarPoints[it],gridPointsHybridSearch[it].toPolar());
			max_it_Search = it;
		}
	}
	std::cout << "hybridMinSearch test: sGrid point with highest distance to given sPolar - execution time: " << hybridSearchTime.count() << "ms" << std::endl;
	std::cout << "polar[" << max_it_Search << "]: " << polarPoints[max_it_Search] << std::endl;
	std::cout << " hybrid grid search  : " << gridPointsHybridSearch[max_it_Search] << "-" << gridPointsHybridSearch[max_it_Search].toPolar() << std::endl;
	std::cout << " robust grid search: " << gridPointsRobustSearch[max_it_Search] << "-" << gridPointsRobustSearch[max_it_Search].toPolar() << std::endl;
	std::cout << " polar difference  : (" << polarPoints[max_it_Search].getTheta()-gridPointsHybridSearch[max_it_Search].toPolar().getTheta();
	std::cout << "," << polarPoints[max_it_Search].getPhi()-gridPointsHybridSearch[max_it_Search].toPolar().getPhi() << ")";
	std::cout << ", polar distance: " << sPolar::distance(polarPoints[max_it_Search],gridPointsHybridSearch[max_it_Search].toPolar()) << std::endl;
	std::cout << " grid distance robust vs hybrid search: " << sGrid::distance(gridPointsRobustSearch[max_it_Search],gridPointsHybridSearch[max_it_Search].toPolar()) << std::endl;
	std::cout << std::endl;


	scaledFP maxDifferenceScaled = {0,0};
	max_it_Search = 0;
	for (size_t it = 0; it < pointCount; ++it)
	{
		if ( maxDifferenceScaled < sGrid::distance(gridPointsFastSearch[it],gridPointsRobustSearch[it]) )
		{
			maxDifferenceScaled = sGrid::distance(gridPointsFastSearch[it],gridPointsRobustSearch[it]);
			max_it_Search = it;
		}
	}
	std::cout << "sGrid point with highest distance between fastMinSearch and minSearch: " << std::endl;
	std::cout << "polar[" << max_it_Search << "]: " << polarPoints[max_it_Search] << std::endl;
	std::cout << " fast search  : " << gridPointsFastSearch[max_it_Search] << "-" << gridPointsFastSearch[max_it_Search].toPolar();
	std::cout << " difference to polar: (" << polarPoints[max_it_Search].getTheta()-gridPointsFastSearch[max_it_Search].toPolar().getTheta();
	std::cout << "," << polarPoints[max_it_Search].getPhi()-gridPointsFastSearch[max_it_Search].toPolar().getPhi() << ")";
	std::cout << "," << sPolar::distance(polarPoints[max_it_Search],gridPointsFastSearch[max_it_Search].toPolar()) << std::endl;
	std::cout << " robust search: " << gridPointsRobustSearch[max_it_Search] << "-" << gridPointsRobustSearch[max_it_Search].toPolar();
	std::cout << " difference to polar: (" << polarPoints[max_it_Search].getTheta()-gridPointsRobustSearch[max_it_Search].toPolar().getTheta();
	std::cout << "," << polarPoints[max_it_Search].getPhi()-gridPointsRobustSearch[max_it_Search].toPolar().getPhi() << ")";
	std::cout << "," << sPolar::distance(polarPoints[max_it_Search],gridPointsRobustSearch[max_it_Search].toPolar()) << std::endl;
	std::cout << " difference between grid points: (" << gridPointsFastSearch[max_it_Search].toPolar().getTheta()-gridPointsRobustSearch[max_it_Search].toPolar().getTheta();
	std::cout << "," << gridPointsFastSearch[max_it_Search].toPolar().getPhi()-gridPointsRobustSearch[max_it_Search].toPolar().getPhi() << ")";
	std::cout << "," << sGrid::distance(gridPointsFastSearch[max_it_Search],gridPointsRobustSearch[max_it_Search]) << std::endl;
	std::cout << std::endl;



	/********************  testing different minimum search algorithms for the test function refPointTestFunction  *********************/
	std::cout << "compare different grid search algorithms how well minima of refPointTestFunction are found: check the " << pointCount << " points above:" << std::endl;
	std::vector<sGrid> gridPointsRobustSearchTest(pointCount);
	t1 = std::chrono::high_resolution_clock::now();
	for (size_t it = 0; it < pointCount; ++it)
	{
		gridPointsRobustSearchTest[it] = sGrid::minSearch(refPointTestFunction(polarPoints[it]));
	}
	t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> robustSearchTimeTest = t2 - t1;

    size_t largeOffsetCount = 0;
	maxDifference = 0;
	max_it_Search = 0;
	for (size_t it = 0; it < pointCount; ++it)
	{
	    fp_type distance = sPolar::distance(polarPoints[it],gridPointsRobustSearchTest[it].toPolar());
	    if (distance > 1e-3) largeOffsetCount++;
		if ( maxDifference < distance )
		{
			maxDifference = distance;
			max_it_Search = it;
		}
	}
	std::cout << "minSearch test function minimum - execution time: " << robustSearchTimeTest.count() << "ms" << std::endl;
	std::cout << "point count with large offset >1e-3: " << largeOffsetCount << std::endl;
	std::cout << "sGrid point with highest distance to given sPolar: ";
	std::cout << "polar[" << max_it_Search << "]: " << polarPoints[max_it_Search] << std::endl;
	std::cout << " robust grid search test  : " << gridPointsRobustSearchTest[max_it_Search] << "-" << gridPointsRobustSearchTest[max_it_Search].toPolar() << std::endl;
	std::cout << " robust grid search: " << gridPointsRobustSearch[max_it_Search] << "-" << gridPointsRobustSearch[max_it_Search].toPolar() << std::endl;
	std::cout << " polar difference  : (" << polarPoints[max_it_Search].getTheta()-gridPointsRobustSearchTest[max_it_Search].toPolar().getTheta();
	std::cout << "," << polarPoints[max_it_Search].getPhi()-gridPointsRobustSearchTest[max_it_Search].toPolar().getPhi() << ")";
	std::cout << ", polar distance: " << sPolar::distance(polarPoints[max_it_Search],gridPointsRobustSearchTest[max_it_Search].toPolar()) << std::endl;
	std::cout << std::endl;



	std::vector<sGrid> gridPointsFastSearchTest(pointCount);
	t1 = std::chrono::high_resolution_clock::now();
	for (size_t it = 0; it < pointCount; ++it)
	{
		gridPointsFastSearchTest[it] = sGrid::fastMinSearch(refPointTestFunction(polarPoints[it]));
	}
	t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> fastSearchTimeTest = t2 - t1;

    largeOffsetCount = 0;
	maxDifference = 0;
	max_it_Search = 0;
	for (size_t it = 0; it < pointCount; ++it)
	{
	    fp_type distance = sPolar::distance(polarPoints[it],gridPointsFastSearchTest[it].toPolar());
	    if (distance > 1e-3) largeOffsetCount++;
		if ( maxDifference < distance )
		{
			maxDifference = distance;
			max_it_Search = it;
		}
	}
	std::cout << "fastMinSearch test function minimum - execution time: " << fastSearchTimeTest.count() << "ms" << std::endl;
	std::cout << "point count with large offset >1e-3: " << largeOffsetCount << std::endl;
	std::cout << "sGrid point with highest distance to given sPolar: ";
	std::cout << "polar[" << max_it_Search << "]: " << polarPoints[max_it_Search] << std::endl;
	std::cout << " fast grid search test  : " << gridPointsFastSearchTest[max_it_Search] << "-" << gridPointsFastSearchTest[max_it_Search].toPolar() << std::endl;
	std::cout << " polar difference  : (" << polarPoints[max_it_Search].getTheta()-gridPointsFastSearchTest[max_it_Search].toPolar().getTheta();
	std::cout << "," << polarPoints[max_it_Search].getPhi()-gridPointsFastSearchTest[max_it_Search].toPolar().getPhi() << ")";
	std::cout << ", polar distance: " << sPolar::distance(polarPoints[max_it_Search],gridPointsFastSearchTest[max_it_Search].toPolar()) << std::endl;
	std::cout << std::endl;



	std::vector<sGrid> gridPointsHybridSearchTest(pointCount);
	t1 = std::chrono::high_resolution_clock::now();
	for (size_t it = 0; it < pointCount; ++it)
	{
		gridPointsHybridSearchTest[it] = sGrid::hybridMinSearch(refPointTestFunction(polarPoints[it]));
	}
	t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> hybridSearchTimeTest = t2 - t1;

    largeOffsetCount = 0;
	maxDifference = 0;
	max_it_Search = 0;
	for (size_t it = 0; it < pointCount; ++it)
	{
	    fp_type distance = sPolar::distance(polarPoints[it],gridPointsHybridSearchTest[it].toPolar());
	    if (distance > 1e-3) largeOffsetCount++;
		if ( maxDifference < distance )
		{
			maxDifference = distance;
			max_it_Search = it;
		}
	}
	std::cout << "hybridMinSearch test function minimum - execution time: " << hybridSearchTimeTest.count() << "ms" << std::endl;
	std::cout << "point count with large offset >1e-3: " << largeOffsetCount << std::endl;
	std::cout << "sGrid point with highest distance to given sPolar: ";
	std::cout << "polar[" << max_it_Search << "]: " << polarPoints[max_it_Search] << std::endl;
	std::cout << " hybrid grid search test  : " << gridPointsHybridSearchTest[max_it_Search] << "-" << gridPointsHybridSearchTest[max_it_Search].toPolar() << std::endl;
	std::cout << " polar difference  : (" << polarPoints[max_it_Search].getTheta()-gridPointsHybridSearchTest[max_it_Search].toPolar().getTheta();
	std::cout << "," << polarPoints[max_it_Search].getPhi()-gridPointsHybridSearchTest[max_it_Search].toPolar().getPhi() << ")";
	std::cout << ", polar distance: " << sPolar::distance(polarPoints[max_it_Search],gridPointsHybridSearchTest[max_it_Search].toPolar()) << std::endl;
	std::cout << std::endl;



#endif //MIN_SEARCH_CHECK



#ifdef DISTANCE_ERROR_CHECK
	/********************  point distance calculations: comparing sGrid vs sPolar results  *********************/
	std::cout << "Comparing distances between grid coordinates and polar coordinates (up to level 4):" << std::endl;
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

	size_t l1_max=3;
	size_t l2_max=4;
	size_t i_max=352;
	size_t j_max=359;

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
					difference = std::abs(SPIRID::sPolar::distance(Q[i].toPolar(l1),Q[j].toPolar(l2))
					                      - SPIRID::sGrid::distance(l1,Q[i],l2,Q[j]).toFPType());
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

	std::cout << "(" << l1_max << "," << l2_max << ")";
	std::cout << "(" << i_max << "," << j_max << "): ";
	std::cout << Q[i_max] << Q[i_max].toPolar(l1_max) << Q[j_max] << Q[j_max].toPolar(l2_max) << ": ";
	std::cout << "polar: " << SPIRID::sPolar::distance(Q[i_max].toPolar(l1_max),Q[j_max].toPolar(l2_max)) << ",";
	std::cout << " grid: " << SPIRID::sGrid::distance(l1_max,Q[i_max],l2_max,Q[j_max]) << ",";
	std::cout << " difference: " << scaledFP(SPIRID::sPolar::distance(Q[i_max].toPolar(l1_max),Q[j_max].toPolar(l2_max)),0) - SPIRID::sGrid::distance(l1_max,Q[i_max],l2_max,Q[j_max]);
	std::cout << std::endl;
	std::cout << std::endl;
#endif //DISTANCE_ERROR_CHECK



#ifdef FACE_GEOMETRY_CHECK
	/********************  testing face geometry functions  *********************/
	std::cout << "checking edge lengths of the same edge in different faces: ";
	std::cout << std::endl;
	sGrid GF0(std::vector<unsigned short>({0,3,1,2,1,2,2,1,2,3,1,3,3,1,0,0,0,1,2,3,1,2,2,1,2,2,2,1,1,3,3}));
	sGrid GF1(std::vector<unsigned short>({0,3,1,2,1,2,2,1,2,3,1,3,3,1,0,0,0,1,2,3,1,2,2,1,2,2,2,1,0,2,2}));
	sGrid GF2(std::vector<unsigned short>({0,3,1,2,1,2,2,1,2,3,1,3,3,1,0,0,0,1,2,3,1,2,2,1,2,2,0,3,3,1,1}));
	sGrid GF3(std::vector<unsigned short>({0,3,1,2,1,2,2,1,2,3,1,3,3,1,0,0,0,1,2,3,1,2,2,1,2,2,2,1,1,3,0}));
	std::cout << "GF0: " << GF0;
	std::cout << std::endl;
	std::cout << "GF1: " << GF1;
	std::cout << std::endl;
	std::cout << "GF2: " << GF2;
	std::cout << std::endl;
	std::cout << "GF3: " << GF3;
	std::cout << std::endl;
	std::cout << "level 30 GF0 edgeLength 1: " << GF0.edgeLength(30,1);
	std::cout << " edgeLength 2: " << GF0.edgeLength(30,2);
	std::cout << " edgeLength 3: " << GF0.edgeLength(30,3);
	std::cout << std::endl;
	std::cout << "level 30 GF1 edgeLength 1: " << GF1.edgeLength(30,1);
	std::cout << " edgeLength 2: " << GF1.edgeLength(30,2);
	std::cout << " edgeLength 3: " << GF1.edgeLength(30,3);
	std::cout << std::endl;
	std::cout << "level 30 GF2 edgeLength 1: " << GF2.edgeLength(30,1);
	std::cout << " edgeLength 2: " << GF2.edgeLength(30,2);
	std::cout << " edgeLength 3: " << GF2.edgeLength(30,3);
	std::cout << std::endl;
	std::cout << "level 30 GF3 edgeLength 1: " << GF3.edgeLength(30,1);
	std::cout << " edgeLength 2: " << GF3.edgeLength(30,2);
	std::cout << " edgeLength 3: " << GF3.edgeLength(30,3);
	std::cout << std::endl;
	std::cout << "lengths of same edges in different faces: ";
	std::cout << std::endl;
	std::cout << "GF0 edge1 - GF1 edge1: " << GF0.edgeLength(30,1) - GF1.edgeLength(30,1);
	std::cout << std::endl;
	std::cout << "GF0 edge2 - GF2 edge2: " << GF0.edgeLength(30,2) - GF2.edgeLength(30,2);
	std::cout << std::endl;
	std::cout << "GF0 edge3 - GF3 edge3: " << GF0.edgeLength(30,3) - GF3.edgeLength(30,3);
	std::cout << std::endl;
	std::cout << "lengths of different nearby edges: ";
	std::cout << std::endl;
	std::cout << "GF2 edge1 - GF1 edge1: " << GF2.edgeLength(30,1) - GF1.edgeLength(30,1);
	std::cout << std::endl;
	std::cout << "GF2 edge3 - GF1 edge3: " << GF2.edgeLength(30,3) - GF1.edgeLength(30,3);
	std::cout << std::endl;
	std::cout << "GF3 edge1 - GF2 edge1: " << GF3.edgeLength(30,1) - GF2.edgeLength(30,1);
	std::cout << std::endl;
	std::cout << "GF3 edge3 - GF2 edge3: " << GF3.edgeLength(30,3) - GF2.edgeLength(30,3);
	std::cout << std::endl;
	std::cout << "GF1 edge1 - GF3 edge1: " << GF1.edgeLength(30,1) - GF3.edgeLength(30,1);
	std::cout << std::endl;
	std::cout << std::endl;

#endif //FACE_GEOMETRY_CHECK



#ifdef TO_POLAR_CHECK
	/********************  testing conversion from sGrid to sPolar  *********************/
	sGrid F0(std::vector<unsigned short>({0,3,1,2,1,2,2,1,2,3,1,3,3,1,0,0,0,1,2,3,1,2,2,1,2,2,2,1,1,3,3}));
	sGrid F1(std::vector<unsigned short>({0,3,1,2,1,2,2,1,2,3,1,3,3,1,0,0,0,1,2,3,1,2,2,1,2,2,2,1,0,2,2}));
	sGrid F2(std::vector<unsigned short>({0,3,1,2,1,2,2,1,2,3,1,3,3,1,0,0,0,1,2,3,1,2,2,1,2,2,0,3,3,1,1}));
	sGrid F3(std::vector<unsigned short>({0,3,1,2,1,2,2,1,2,3,1,3,3,1,0,0,0,1,2,3,1,2,2,1,2,2,2,1,1,3,0}));
	std::cout << "checking polar coordinates in Octant0: ";
	std::cout << std::endl;
	sGrid O0(std::vector<unsigned short>({0}));
	std::cout << "O0 center:   " << O0.toPolar(0,0);
	std::cout << " O0 front:   " << O0.toPolar(0,1);
	std::cout << "          O0 east:   " << O0.toPolar(0,2);
	std::cout << "            O0 north:   " << O0.toPolar(0,3);
	std::cout << std::endl;
	O0.setExtend(1,0);
	std::cout << "O00 center:  " << O0.toPolar(1,0);
	std::cout << " O00 east:   " << O0.toPolar(1,1);
	std::cout << "           O00 front: " << O0.toPolar(1,2);
	std::cout << "         O00 south:  " << O0.toPolar(1,3);
	std::cout << std::endl;
	O0.setExtend(2,0);
	std::cout << "O000 center: " << O0.toPolar(2,0);
	std::cout << " O000 front: " << O0.toPolar(2,1);
	std::cout << " O000 east: " << O0.toPolar(2,2);
	std::cout << " O000 north: " << O0.toPolar(2,3);
	std::cout << std::endl;
	std::cout << std::endl;

	std::cout << "polar coordinates of the same node in different faces: ";
	std::cout << std::endl;
	sPolar F0p = F0.toPolar(30,2);
	sPolar F1p = F1.toPolar(30,3);
	std::cout << "[F0,2] polar: " << F0p;
	std::cout << " [F1,3] polar: " << F1p;
	std::cout << " difference: (" << F0p.getTheta()-F1p.getTheta() << "," << F0p.getPhi()-F1p.getPhi() << ")";
	std::cout << std::endl;
	std::cout << "[F0,3] polar: " << F0.toPolar(30,3);
	std::cout << " [F1,2] polar: " << F1.toPolar(30,2);
	std::cout << " difference: (" << F0.toPolar(30,3).getTheta()-F1.toPolar(30,2).getTheta() << "," << F0.toPolar(30,3).getPhi()-F1.toPolar(30,2).getPhi() << ")";
	std::cout << std::endl;
	std::cout << "[F0,1] polar: " << F0.toPolar(30,1);
	std::cout << " [F2,3] polar: " << F2.toPolar(30,3);
	std::cout << " difference: (" << F0.toPolar(30,1).getTheta()-F2.toPolar(30,3).getTheta() << "," << F0.toPolar(30,1).getPhi()-F2.toPolar(30,3).getPhi() << ")";
	std::cout << std::endl;
	std::cout << "[F0,3] polar: " << F0.toPolar(30,3);
	std::cout << " [F2,1] polar: " << F2.toPolar(30,1);
	std::cout << " difference: (" << F0.toPolar(30,3).getTheta()-F2.toPolar(30,1).getTheta() << "," << F0.toPolar(30,3).getPhi()-F2.toPolar(30,1).getPhi() << ")";
	std::cout << std::endl;
	std::cout << "[F0,1] polar: " << F0.toPolar(30,1);
	std::cout << " [F3,2] polar: " << F3.toPolar(30,2);
	std::cout << " difference: (" << F0.toPolar(30,1).getTheta()-F3.toPolar(30,2).getTheta() << "," << F0.toPolar(30,1).getPhi()-F3.toPolar(30,2).getPhi() << ")";
	std::cout << std::endl;
	std::cout << "[F0,2] polar: " << F0.toPolar(30,2);
	std::cout << " [F3,1] polar: " << F3.toPolar(30,1);
	std::cout << " difference: (" << F0.toPolar(30,2).getTheta()-F3.toPolar(30,1).getTheta() << "," << F0.toPolar(30,2).getPhi()-F3.toPolar(30,1).getPhi() << ")";
	std::cout << std::endl;
	std::cout << std::endl;

	size_t refLevel = 29;
	std::cout << "high level nodes polar coordinates in flat and spherical geometry: ";
	std::cout << "{" << F0.calcFaceGeometry(refLevel)[1];
	std::cout << "," << F0.calcFaceGeometry(refLevel)[2];
	std::cout << "," << F0.calcFaceGeometry(refLevel)[3] << "}";
	std::cout << std::endl;
	sGrid::inFacePolar testSpher(F0.calcFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,0));
	sGrid::inFacePolar testFlat(F0.calcFlatFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,0));
	std::cout << "[F0,0] level 30 local polar (1,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,0);
	testFlat = F0.calcFlatFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,0);
	std::cout << "[F0,0] level 30 local polar (1,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,0);
	testFlat = F0.calcFlatFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,0);
	std::cout << "[F0,0] level 30 local polar (2,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,0);
	testFlat = F0.calcFlatFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,0);
	std::cout << "[F0,0] level 30 local polar (2,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,0);
	testFlat = F0.calcFlatFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,0);
	std::cout << "[F0,0] level 30 local polar (3,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,0);
	testFlat = F0.calcFlatFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,0);
	std::cout << "[F0,0] level 30 local polar (3,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,1);
	testFlat = F0.calcFlatFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,1);
	std::cout << "[F0,1] level 30 local polar (1,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,1);
	testFlat = F0.calcFlatFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,1);
	std::cout << "[F0,1] level 30 local polar (1,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,1);
	testFlat = F0.calcFlatFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,1);
	std::cout << "[F0,1] level 30 local polar (2,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,1);
	testFlat = F0.calcFlatFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,1);
	std::cout << "[F0,1] level 30 local polar (2,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,1);
	testFlat = F0.calcFlatFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,1);
	std::cout << "[F0,1] level 30 local polar (3,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,1);
	testFlat = F0.calcFlatFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,1);
	std::cout << "[F0,1] level 30 local polar (3,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,2);
	testFlat = F0.calcFlatFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,2);
	std::cout << "[F0,2] level 30 local polar (1,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,2);
	testFlat = F0.calcFlatFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,2);
	std::cout << "[F0,2] level 30 local polar (1,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,2);
	testFlat = F0.calcFlatFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,2);
	std::cout << "[F0,2] level 30 local polar (2,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,2);
	testFlat = F0.calcFlatFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,2);
	std::cout << "[F0,2] level 30 local polar (2,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,2);
	testFlat = F0.calcFlatFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,2);
	std::cout << "[F0,2] level 30 local polar (3,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,2);
	testFlat = F0.calcFlatFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,2);
	std::cout << "[F0,2] level 30 local polar (3,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,3);
	testFlat = F0.calcFlatFacePolar(refLevel,1,2,F0.calcFaceGeometry(refLevel),30,3);
	std::cout << "[F0,3] level 30 local polar (1,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,3);
	testFlat = F0.calcFlatFacePolar(refLevel,1,3,F0.calcFaceGeometry(refLevel),30,3);
	std::cout << "[F0,3] level 30 local polar (1,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,3);
	testFlat = F0.calcFlatFacePolar(refLevel,2,1,F0.calcFaceGeometry(refLevel),30,3);
	std::cout << "[F0,3] level 30 local polar (2,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,3);
	testFlat = F0.calcFlatFacePolar(refLevel,2,3,F0.calcFaceGeometry(refLevel),30,3);
	std::cout << "[F0,3] level 30 local polar (2,3): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,3);
	testFlat = F0.calcFlatFacePolar(refLevel,3,1,F0.calcFaceGeometry(refLevel),30,3);
	std::cout << "[F0,3] level 30 local polar (3,1): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	testSpher = F0.calcFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,3);
	testFlat = F0.calcFlatFacePolar(refLevel,3,2,F0.calcFaceGeometry(refLevel),30,3);
	std::cout << "[F0,3] level 30 local polar (3,2): (" << testSpher.SinDistSq << "," << testSpher.angle << ") ";
	std::cout << "; flat: (" << testFlat.SinDistSq << "," << testFlat.angle << ") ";
	std::cout << "; difference: (" << testSpher.SinDistSq-testFlat.SinDistSq << "," << testSpher.angle-testFlat.angle << ") ";
	std::cout << std::endl;
	std::cout << std::endl;
#endif //TO_POLAR_CHECK



#ifdef GRID_NEIGHBOR_CHECK
	/********************  testing functions to get neighbor faces, nodes and edges  *********************/
	std::cout << "collecting neighbor faces & nodes for special points: ";
	std::cout << std::endl;
	std::list<sGrid> starF = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeNeighborFaces(2,2);
	std::list<std::pair<sGrid, unsigned short> > starN = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeNeighborNodes(2,2);
	std::list<std::pair<sGrid, unsigned short> > starE = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeConnectedEdges(2,2);
	std::list<std::pair<sGrid, unsigned short> > starO = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeOuterRingEdges(2,2);
	std::cout << "Nodes           : " << sGrid(std::vector<bool>({0,0,0,1,1,1,1})) << 2 << " ";
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

	starF = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeNeighborFaces(2,3);
	starN = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeNeighborNodes(2,3);
	starE = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeConnectedEdges(2,3);
	starO = sGrid(std::vector<bool>({0,0,0,1,1,1,1})).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : " << sGrid(std::vector<bool>({0,0,0,1,1,1,1})) << 3 << " ";
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

	starF = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeNeighborFaces(2,3);
	starN = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeNeighborNodes(2,3);
	starE = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeConnectedEdges(2,3);
	starO = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : " << sGrid(std::vector<bool>({0,0,0,0,0,0,1})) << 3 << " ";
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

	starF = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeNeighborFaces(2,2);
	starN = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeNeighborNodes(2,2);
	starE = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeConnectedEdges(2,2);
	starO = sGrid(std::vector<bool>({0,0,0,0,0,0,1})).nodeOuterRingEdges(2,2);
	std::cout << "Nodes           : " << sGrid(std::vector<bool>({0,0,0,0,0,0,1})) << 2 << " ";
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

	starF = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeNeighborFaces(2,1);
	starN = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeNeighborNodes(2,1);
	starE = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeConnectedEdges(2,1);
	starO = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeOuterRingEdges(2,1);
	std::cout << "Nodes           : " << sGrid(std::vector<bool>({0,0,0,1,0,1,1})) << 1 << " ";
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

	starF = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeNeighborFaces(2,3);
	starN = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeNeighborNodes(2,3);
	starE = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeConnectedEdges(2,3);
	starO = sGrid(std::vector<bool>({0,0,0,1,0,1,1})).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : " << sGrid(std::vector<bool>({0,0,0,1,0,1,1})) << 3 << " ";
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

	starF = sGrid(std::vector<bool>({0,0,0,0,0,1,1})).nodeNeighborFaces(2,3);
	starN = sGrid(std::vector<bool>({0,0,0,0,0,1,1})).nodeNeighborNodes(2,3);
	starE = sGrid(std::vector<bool>({0,0,0,0,0,1,1})).nodeConnectedEdges(2,3);
	starO = sGrid(std::vector<bool>({0,0,0,0,0,1,1})).nodeOuterRingEdges(2,3);
	std::cout << "Nodes           : " << sGrid(std::vector<bool>({0,0,0,0,0,1,1})) << 3 << " ";
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

#endif //GRID_NEIGHBOR_CHECK

	return 0;
}

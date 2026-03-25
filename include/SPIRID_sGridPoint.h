#ifndef SPIRID_SGRIDPOINT_H
#define SPIRID_SGRIDPOINT_H

#include <SPIRID.h>

namespace SPIRID
{

/* A point is characterized by a grid level, a face (sGrid code) and a location inside the face.
   For the type sGridPoint we extend the depth of an sGird by one level and use the last digit for the location.
*/
class sGridPoint : public sGrid
{
	unsigned short inFaceLoc;
public:
	static const sGridPoint North;
	static const sGridPoint South;

	inline void set(const sGridPoint& Q)
	{
		sGrid::set(Q);
		inFaceLoc = Q.inFaceLoc;
	};

	static sGridPoint searchPoint(scaledFP (*)(const sGridPoint&));
	static sGridPoint localSearchPoint(size_t level, const sGridPoint& P, scaledFP minFunctionValue, scaledFP (*minFunc)(const sGridPoint&));
	static scaledFP test(const sGridPoint& P) {
		return scaledFP(P.inFaceLoc+P.at(0),0);
	};

	inline sGridPoint(size_t depth) : sGrid(depth) {
		inFaceLoc = 0;
	};
	inline sGridPoint(const sGrid& P) : sGrid(P) {
		inFaceLoc = 0;
	};
	inline sGridPoint(const sGridPoint& P) : sGrid(P) {
		inFaceLoc = P.inFaceLoc;
	};
	sGridPoint(const std::vector<bool>& P, unsigned short location);

	inline unsigned short location() const {
		return inFaceLoc;
	}
	inline sGridPoint& setLocation(unsigned short x) {
		inFaceLoc = x;
		return *this;
	}

	inline static angle distance(const sGridPoint& P1, const sGridPoint& P2)
	{
		return sGrid::distance(P1.depth(), P1, P1.location(), P2.depth(), P2, P2.location());
	};

	inline sGridPoint& assignNeighborFace(size_t level, unsigned short edgeCode, sGridPoint& neighbor, bool& orientationMatch)
	{
		sGrid::assignNeighborFace(level, edgeCode, neighbor, orientationMatch);
		return *this;
	};

};
//output for grid coordinates as face codes
//std::ostream& operator << (std::ostream& out, const sGridPoint& P);

} // namespace SPIRID

#endif //SPIRID_SGRIDPOINT_H
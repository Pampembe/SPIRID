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
public:
	inline sGridPoint(size_t depth) : sGrid(depth+1) {};
	inline sGridPoint(const sGrid& P) : sGrid(P) {
		resize(depth()+1);
	};
	sGridPoint(const std::vector<bool>& P, unsigned short location);

	inline size_t depth() const {
		return operator[](sGrid::depth()-1);
	}
	inline unsigned short at(size_t level) const {
	    if (level==sGrid::depth()) return 0;
	    return at(level);
	}
	inline unsigned short location() const {
		return operator[](sGrid::depth());
	}
	inline void setLocation(unsigned short x) {
	    set(sGrid::depth(),x);
	}

	inline static angle distance(const sGridPoint& P1, const sGridPoint& P2)
	{
		return sGrid::distance(P1.depth(), P1, P1.location(), P2.depth(), P2, P2.location());
	};
};


} // namespace SPIRID

#endif //SPIRID_SGRIDPOINT_H
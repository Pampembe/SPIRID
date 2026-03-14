#include <SPIRID_sGridPoint.h>

SPIRID::sGridPoint::sGridPoint(const std::vector<bool>& P, unsigned short location) : sGridPoint(P)
{
    setLocation(location);
}

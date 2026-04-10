#include <SPIRID_aux.h>

fp_type SPIRID::angle::unitScale=1.;
std::string SPIRID::angle::unitSymbol="";

void SPIRID::angle::unitPi() {
	unitScale=1/pi;
	unitSymbol="Pi";
};
void SPIRID::angle::unitRad() {
	unitScale=1;
	unitSymbol="";
};
void SPIRID::angle::unitDeg() {
	unitScale=180/pi;
	unitSymbol="\u00B0";
};

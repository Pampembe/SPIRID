#include <SPIRID.h>

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
	unitSymbol="°";
};

/*
SPIRID::angle SPIRID::angle::operator + (const SPIRID::angle & beta) const
{
    size_t exponentDiff = beta.exponent;
    if (exponent <= exponentDiff)
    {
        exponentDiff -= exponent;
        return angle(mantissa + beta.mantissa/pow(2,exponentDiff),exponent);
    }
    else
    {
        exponentDiff = exponent-exponentDiff;
        return angle(beta.mantissa + mantissa/pow(2,exponentDiff),beta.exponent);
    }
}
SPIRID::angle SPIRID::angle::operator - (const SPIRID::angle & beta) const
{
    size_t exponentDiff = beta.exponent;
    if (exponent <= exponentDiff)
    {
        exponentDiff -= exponent;
        return angle(mantissa - beta.mantissa/pow(2,exponentDiff),exponent);
    }
    else
    {
        exponentDiff = exponent-exponentDiff;
        return angle(-beta.mantissa + mantissa/pow(2,exponentDiff),beta.exponent);
    }
}
SPIRID::angle SPIRID::angle::operator * (const SPIRID::angle & beta) const
{
    return angle(mantissa*beta.mantissa, exponent+beta.exponent);
}
fp_type SPIRID::angle::operator / (const SPIRID::angle & beta) const
{
    size_t exponentDiff = beta.exponent;
    if (exponent >= exponentDiff)
    {
        return mantissa/beta.mantissa/pow(2,mantissa-exponentDiff);
    }
    else
    {
        return mantissa/beta.mantissa*pow(2,exponentDiff-mantissa);
    }
}
*/

std::ostream& SPIRID::operator << (std::ostream& out, const SPIRID::angle& alpha)
{
    out << alpha.getMantissa()*angle::unitScale;
    if(alpha.getExponent() != 0) out << "/2^" << alpha.getExponent();
    out << angle::unitSymbol;
	return out;
}

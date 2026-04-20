#include <limits>
#include <SPIRID_aux.h>

const size_t SPIRID::scaledFP::FPDigits = std::numeric_limits<fp_type>::digits;
const scaleExp_type SPIRID::scaledFP::FPMaxExponent = -std::numeric_limits<fp_type>::min_exponent;

//standard arithmetic operators
SPIRID::scaledFP
SPIRID::scaledFP::operator + (const scaledFP& X) const
{
	if (X.scaleExponent > scaleExponent)
	{
		return {mantissa + LDEXP(X.mantissa,scaleExponent-X.scaleExponent), scaleExponent};
	}
	return {LDEXP(mantissa,X.scaleExponent-scaleExponent) + X.mantissa, X.scaleExponent};
};
SPIRID::scaledFP
SPIRID::scaledFP::operator - (const scaledFP& X) const
{
	if (X.scaleExponent > scaleExponent)
	{
		return {mantissa - LDEXP(X.mantissa,scaleExponent-X.scaleExponent), scaleExponent};
	}
	return {LDEXP(mantissa,X.scaleExponent-scaleExponent) - X.mantissa, X.scaleExponent};
};
SPIRID::scaledFP
SPIRID::scaledFP::operator * (const scaledFP& X) const
{
	return {mantissa*X.mantissa, scaleExponent+X.scaleExponent};
};
SPIRID::scaledFP
SPIRID::scaledFP::operator / (const scaledFP& X) const
{
	if (X.scaleExponent > scaleExponent)
	{
		return {LDEXP(mantissa/X.mantissa,scaleExponent-X.scaleExponent), 0};
	}
	return {mantissa/X.mantissa,scaleExponent-X.scaleExponent};
};
SPIRID::scaledFP
SPIRID::scaledFP::abs() const
{
	return {ABS(mantissa),scaleExponent};
};

//conversion to standard fp_type
fp_type
SPIRID::scaledFP::toFPType() const
{
	return LDEXP(mantissa,-scaleExponent);
};

//output operator
std::ostream& SPIRID::operator << (std::ostream& out, const SPIRID::scaledFP& x)
{
	out << x.getMantissa();
	if(x.getExponent() != 0) out << "*2^-" << x.getExponent();
	return out;
}

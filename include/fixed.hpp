#pragma once

/* Fixed-point arithmetic using signed 32-bit integers */

#include <stdint.h>

template<class B, class N>
struct FixedPConvert {
	static B convert(const N& rhs) { return static_cast<B>(rhs); }
};

class SFixed32 {
	/* Based on https://gist.github.com/dflemstr/294959 */

public:
	static const size_t SIZE = sizeof(signed long);
	static const size_t INTEGRAL_SIZE = 16; //16 bits of integer
	static const size_t FRACTION_SIZE = 16; //16 bits of fraction
	static const size_t TOTAL_SIZE = (INTEGRAL_SIZE + FRACTION_SIZE); //32 bits total

	static const signed long one = 1 << FRACTION_SIZE;

	/* Constructors */
	SFixed32() {}; //don't automatically initialize this with anything
	explicit SFixed32(int val)		: data(val << FRACTION_SIZE) {};
	explicit SFixed32(float val)	    : data(static_cast<signed long>(val * one)) {}
	explicit SFixed32(double val)	    : data(static_cast<signed long>(val * one)) {}
    explicit SFixed32(long val)	    : data(static_cast<signed long>(val * one)) {}

	/* Operators */
#define SFixed32_MK_CMP_OP(op) inline bool operator op(const SFixed32 &o) const { return data op o.data; }
	SFixed32_MK_CMP_OP(== )
	SFixed32_MK_CMP_OP(!= )
	SFixed32_MK_CMP_OP(< )
	SFixed32_MK_CMP_OP(> )
	SFixed32_MK_CMP_OP(>= )
	SFixed32_MK_CMP_OP(<= )
#undef SFixed32_MK_CMP_OP

	inline bool operator !() const { return !data; };
	inline SFixed32 operator ~() const { return SFixed32(~data); }
	inline SFixed32 &operator ++() { data += one; return *this; }
	inline SFixed32 &operator --() { data -= one; return *this; }

#define SFixed32_MK_BIN_OP(op) inline SFixed32 &operator op##=(const SFixed32 &n) { data op##= n.data; return *this; } \
	inline SFixed32 operator op(const SFixed32 &n) const { SFixed32 x(*this); x op##= n; return x; }
	SFixed32_MK_BIN_OP(+)
	SFixed32_MK_BIN_OP(-)
	SFixed32_MK_BIN_OP(&)
	SFixed32_MK_BIN_OP(|)
	SFixed32_MK_BIN_OP(^)
#undef SFixed32_MK_BIN_OP

	inline SFixed32 &operator >>=(const SFixed32 &n) { data >>= n.toInt(); return *this; }
	inline SFixed32 operator >> (const SFixed32 &n) { SFixed32 x(*this); x >>= n; return x; }

	inline SFixed32 &operator <<=(const SFixed32 &n) { data <<= n.toInt(); return *this; }
	inline SFixed32 operator <<(const SFixed32 &n) { SFixed32 x(*this); x <<= n; return x; }

	//int toInt() const { return ((data & (signed long(-1))) >> FRACTION_SIZE); }
    int toInt() const { return data >> FRACTION_SIZE; }
	float toFloat() const { return static_cast<float>(data) / one; }
	double toDouble() const { return static_cast<double>(data) / one; }
	signed long raw() const { return data; }

	inline SFixed32 &operator *=(const SFixed32 &n) {
		signed long long t(static_cast<signed long long>(data) * static_cast<signed long long>(n.data));
		t >>= FRACTION_SIZE;
		data = FixedPConvert<signed long, signed long long>::convert(t);
		return *this;
	}
	inline SFixed32 operator *(const SFixed32 &n) { SFixed32 x(*this); x *= n; return x; }

	inline SFixed32 &operator /=(const SFixed32 &n) {
		signed long long t(data);
		t <<= FRACTION_SIZE;
		t /= n.data;
		data = FixedPConvert<signed long, signed long long>::convert(t);
		return *this;
	}
	inline SFixed32 operator /(const SFixed32 &n) { SFixed32 x(*this); x /= n; return x; }

private:
	signed long data;
};
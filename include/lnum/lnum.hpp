/** Created by Fekete
 * 17.05.2018
 */

#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <utility>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace lnum {

class Lnum {
	const int base = 1e9;
	std::vector<int> digit;
	int sign;
	void normalize();
public:
	Lnum();
	Lnum(long long);
	Lnum(std::string);
	Lnum(std::vector<int>, int);
	Lnum(const Lnum&);
	Lnum(Lnum&&) noexcept;

	Lnum& operator=(const Lnum&);
	Lnum& operator=(Lnum&&) noexcept;

	bool operator==(const Lnum&) const;
	bool operator!=(const Lnum&) const;
	bool operator<(const Lnum&) const;
	bool operator<=(const Lnum&) const;
	bool operator>(const Lnum&) const;
	bool operator>=(const Lnum&) const;

	Lnum operator+(const Lnum&) const;
	Lnum operator-(const Lnum&) const;
	Lnum operator-() const;
	Lnum operator*(const Lnum&) const;
	Lnum operator/(const Lnum&) const;
	Lnum operator%(const Lnum&) const;

	Lnum operator+=(const Lnum&);
	Lnum operator-=(const Lnum&);
	Lnum operator*=(const Lnum&);
	Lnum operator/=(const Lnum&);
	Lnum operator%=(const Lnum&);


	Lnum& operator=(const long long&);

	bool operator==(const long long&) const;
	bool operator!=(const long long&) const;
	bool operator<(const long long&) const;
	bool operator<=(const long long&) const;
	bool operator>(const long long&) const;
	bool operator>=(const long long&) const;

	Lnum operator+(const long long&) const;
	Lnum operator-(const long long&) const;
	Lnum operator*(const long long&) const;
	Lnum operator/(const long long&) const;
	Lnum operator%(const long long&) const;

	Lnum operator+=(const long long&);
	Lnum operator-=(const long long&);
	Lnum operator*=(const long long&);
	Lnum operator/=(const long long&);
	Lnum operator%=(const long long&);

	std::pair<Lnum, Lnum> divmod(const Lnum&) const;

	int getDigit(int) const;
	int length() const;
	int getSign() const;
	std::vector<int> getDigits() const;
	std::string toString() const;
};

Lnum lPow(Lnum a, long long b);
Lnum lPow(long long a, long long b);


//CONSTRUCTORS

inline Lnum::Lnum() :
	sign(1), digit(std::vector<int>(1, 0)) {}

inline Lnum::Lnum(long long x) {
	unsigned long long ux;
	if(x < 0) {
		sign = -1;
		// Negating LLONG_MIN as a signed long long is UB (its magnitude
		// doesn't fit in long long). Reinterpret as unsigned first, where
		// negation wraps around and yields the correct magnitude.
		ux = -static_cast<unsigned long long>(x);
	}
	else {
		sign = 1;
		ux = static_cast<unsigned long long>(x);
	}

	digit.clear();
	while(ux > 0) {
		digit.push_back(static_cast<int>(ux % base));
		ux /= base;
	}
	if(digit.empty()) {
		digit.push_back(0);
	}
}

inline Lnum::Lnum(std::string s) {
	// Only accept a strict integer literal: an optional sign followed by one
	// or more ASCII digits. Anything else (letters, whitespace, stray signs,
	// embedded punctuation) is rejected rather than silently mis-parsed.
	if(s.empty()) {
		throw std::invalid_argument("lnum::Lnum: \"" + s + "\" is not a valid integer literal");
	}

	std::size_t firstDigit = 0;
	sign = 1;
	if(s[0] == '-' || s[0] == '+') {
		sign = (s[0] == '-') ? -1 : 1;
		firstDigit = 1;
	}
	if(firstDigit == s.size()) {
		throw std::invalid_argument("lnum::Lnum: \"" + s + "\" is not a valid integer literal");
	}
	for(std::size_t i = firstDigit; i < s.size(); i++) {
		if(!isdigit(static_cast<unsigned char>(s[i]))) {
			throw std::invalid_argument("lnum::Lnum: \"" + s + "\" is not a valid integer literal");
		}
	}

	digit.clear();
	std::string magnitude = s.substr(firstDigit);
	reverse(magnitude.begin(), magnitude.end());
	int cnt = 0, x = 0, pw = 1;
	for(auto c : magnitude) {
		cnt++;
		x += pw * int(c - '0');
		pw *= 10;
		if(cnt == 9) {
			digit.push_back(x);
			cnt = 0;
			x = 0;
			pw = 1;
		}
	}
	if(x != 0) {
		digit.push_back(x);
	}
	normalize();
}

inline Lnum::Lnum(std::vector<int> digit, int sign) :
		digit(digit), sign(sign) {
	normalize();
}


inline Lnum::Lnum(const Lnum& x) : digit(x.getDigits()), sign(x.getSign()) {
	normalize();
}

inline Lnum::Lnum(Lnum&& x) noexcept = default;


//OPERATORS

inline Lnum& Lnum::operator=(const Lnum& x){
	digit = x.getDigits();
	sign = x.getSign();
	return *this;
}

// Can't be `= default`: base is const, and a class with a const data member
// has its defaulted move-assignment operator implicitly deleted (assignment
// would require reassigning `base`, which is impossible). Write it by hand
// and simply leave `base` untouched, same as the copy-assignment operator
// above.
inline Lnum& Lnum::operator=(Lnum&& x) noexcept {
	digit = std::move(x.digit);
	sign = x.sign;
	return *this;
}

//// IN|OUT

inline std::istream& operator>>(std::istream& in, Lnum& x) {
	std::string s;
	in >> s;
	x = Lnum(s);
	return in;
}

inline std::ostream& operator<<(std::ostream& out, const Lnum& x) {
	out << x.toString();
	return out;
}

////FOR COMPARING

inline bool Lnum::operator==(const Lnum& x) const {
	return sign == x.getSign() && digit == x.getDigits();
}

inline bool Lnum::operator!=(const Lnum& x) const {
	return !(*this == x);
}

inline bool Lnum::operator<(const Lnum& x) const {
	if(sign != x.getSign()) {
		return sign < x.getSign();
	}
	else {
		bool condition;
		if(length() != x.length()) {
			condition = length() < x.length();
		}
		else {
			std::vector<int> v1 = digit, v2 = x.getDigits();
			reverse(v1.begin(), v1.end());
			reverse(v2.begin(), v2.end());
			condition = v1 < v2;
		}
		if(sign == -1) condition ^= 1;
		return condition;
	}
}

inline bool Lnum::operator<=(const Lnum& x) const {
	return *this < x || *this == x;
}

inline bool Lnum::operator>(const Lnum& x) const {
	return !(*this <= x);
}

inline bool Lnum::operator>=(const Lnum& x) const {
	return !(*this < x);
}

////ARITHMETIC

inline Lnum Lnum::operator-() const {
	return Lnum(digit, -sign);
}

inline Lnum Lnum::operator+(const Lnum& x) const {
	if(sign == x.getSign()) {
		std::vector<int> c(std::max(x.length(), length()) + 1, 0);
		for(int i = 0, carry = 0; i < int(c.size()); i++) {
			carry += getDigit(i) + x.getDigit(i);
			c[i] = carry % base;
			carry /= base;
		}
		return Lnum(c, sign);
	}
	else {
		if(*this > x) {
			return *this - (-x);
		}
		else {
			return (x - (-*this));
		}
	}
}

inline Lnum Lnum::operator-(const Lnum& x) const {
	if(sign != x.getSign()) {
		return *this + (-x);
	}
	else {
		if(sign == -1) {
			return -(-*this - (-x));
		}
		else {
			if(*this < x) {
				return -(x - *this);
			}
			else {
				std::vector<int> c(digit.size(), 0);
				for(int i = 0; i < int(c.size()); i++) {
					c[i] += getDigit(i) - x.getDigit(i);
					if(c[i] < 0) {
						c[i] += base;
						c[i + 1]--;
					}
				}
				return Lnum(c, 1);
			}
		}
	}
}


inline Lnum Lnum::operator*(const Lnum& x) const {
	std::vector<int> c(length() + x.length() + 10, 0);
	for(int i = 0, carry = 0; i < int(digit.size()); i++) {
		for(int j = 0; i + j < int(c.size()) || carry > 0; j++) {
			long long cur = c[i + j] + getDigit(i) * 1ll * x.getDigit(j) + carry;
			carry = cur / base;
			c[i + j] = cur % base;
		}
	}
	return Lnum(c, sign * x.getSign());
}

inline Lnum Lnum::operator/(const Lnum& x) const {
	return divmod(x).first;
}
inline Lnum Lnum::operator%(const Lnum& x) const {
	return divmod(x).second;
}

inline std::pair<Lnum, Lnum> Lnum::divmod(const Lnum& x) const {
	if(x == 0) {
		throw std::invalid_argument("lnum::Lnum::divmod: division by zero");
	}

	std::vector<int> a = digit;
	reverse(a.begin(), a.end());
	Lnum y(x.getDigits(), 1);
	std::vector<int> c;
	Lnum carry = 0;

	for(int i = 0; i < a.size(); i++) {
		carry *= base;
		carry += a[i];

		int l = 0, r = base - 1;
		while(l < r) {
			int m = (l + r + 1) / 2;
			if(y * m <= carry) {
				l = m;
			}
			else {
				r = m - 1;
			}
		}
		c.push_back(l);
		carry -= y * l;
	}

	reverse(c.begin(), c.end());

	return std::make_pair(Lnum(c, sign * x.getSign()), carry * sign);
}

inline Lnum Lnum::operator+=(const Lnum& x) {
	return (*this = *this + x);
}

inline Lnum Lnum::operator-=(const Lnum& x) {
	return (*this = *this - x);
}

inline Lnum Lnum::operator*=(const Lnum& x) {
	return (*this = *this * x);
}

inline Lnum Lnum::operator/=(const Lnum& x) {
	return (*this = *this / x);
}

inline Lnum Lnum::operator%=(const Lnum& x) {
	return (*this = *this % x);
}


////CLONES FOR INT

inline Lnum& Lnum::operator=(const long long &x){
	return (*this = Lnum(x));
}


inline bool Lnum::operator==(const long long& x) const {
	return *this == Lnum(x);
}

inline bool Lnum::operator!=(const long long& x) const {
	return *this != Lnum(x);
}

inline bool Lnum::operator<(const long long& x) const {
	return *this < Lnum(x);
}

inline bool Lnum::operator<=(const long long& x) const {
	return *this <= Lnum(x);
}

inline bool Lnum::operator>(const long long& x) const {
	return *this > Lnum(x);
}

inline bool Lnum::operator>=(const long long& x) const {
	return *this >= Lnum(x);
}


inline Lnum Lnum::operator+(const long long& x) const {
	return *this + Lnum(x);
}

inline Lnum Lnum::operator-(const long long& x) const {
	return *this - Lnum(x);
}

inline Lnum Lnum::operator*(const long long& x) const {
	return *this * Lnum(x);
}

inline Lnum Lnum::operator/(const long long& x) const {
	return *this / Lnum(x);
}

inline Lnum Lnum::operator%(const long long& x) const {
	return *this % Lnum(x);
}


inline Lnum Lnum::operator+=(const long long& x) {
	return (*this = *this + x);
}

inline Lnum Lnum::operator-=(const long long& x) {
	return (*this = *this - x);
}

inline Lnum Lnum::operator*=(const long long& x) {
	return (*this = *this * x);
}

inline Lnum Lnum::operator/=(const long long& x) {
	return (*this = *this / x);
}

inline Lnum Lnum::operator%=(const long long& x) {
	return (*this = *this % x);
}

//OTHERS

inline int Lnum::getDigit(int pos) const {
	return 0 <= pos && pos < int(digit.size()) ? digit[pos] : 0;
}

inline int Lnum::length() const {
	return digit.size() == 1 && digit[0] == 0 ? 1 :
		digit.size() + (sign == -1);
}

inline int Lnum::getSign() const {
	return sign;
}

inline std::vector<int> Lnum::getDigits() const {
	return digit;
}

inline void Lnum::normalize() {
	while(digit.size() > 1 && digit.back() == 0) {
		digit.pop_back();
	}
	if(digit.size() == 0) {
		sign = 1;
		digit.push_back(0);
	}
	if(digit.size() == 1 && digit[0] == 0) {
		sign = 1;
	}
}

inline std::string Lnum::toString() const {
	if(*this == 0) {
		return "0";
	}
	else {
		std::string result = "";
		if(this->getSign() == -1) {
			result += "-";
		}
		std::vector<int> digits = this->getDigits();
		std::stringstream leading;
		leading << digits.back();
		result += leading.str();
		digits.pop_back();
		reverse(digits.begin(), digits.end());

		struct Helper {
			std::string addLeadingZeroes(int x) {
				std::stringstream stream;
				stream << x;
				std::string s = stream.str();
				while(int(s.size()) < 9) {
					s = "0" + s;
				}
				return s;
			}
		} h;

		for(auto x : digits) {
			result += h.addLeadingZeroes(x);
		}

		return result;
	}
}

inline Lnum lPow(Lnum a, long long b) {
	Lnum res = 1;
	while(b > 0) {
		if(b % 2) res *= a;
		a *= a;
		b /= 2;
	}
	return res;
}

inline Lnum lPow(long long a, long long b) {
	return lPow(Lnum(a), b);
}

} // namespace lnum

/** Created by Fekete
 * 17.05.2018
 */

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

using namespace std;

class Lnum {
	vector<int> digit;
	int sign;
	void normalize();
public:
	Lnum();
	Lnum(long long);
	Lnum(string);
	Lnum(vector<int>, int);
	Lnum(const Lnum&);

	Lnum& operator=(const Lnum&);

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

	Lnum operator+=(const Lnum&);
	Lnum operator-=(const Lnum&);
	Lnum operator*=(const Lnum&);


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

	Lnum operator+=(const long long&);
	Lnum operator-=(const long long&);
	Lnum operator*=(const long long&);


	int getDigit(int) const;
	int length() const;
	int getSign() const;
	vector<int> getDigits() const;
};

//CONSTRUCTORS

Lnum::Lnum() :
	sign(1), digit(vector<int>(1, 0)) {}

Lnum::Lnum(long long x) {
	if(x < 0) {
		sign = -1;
		x *= -1;
	}
	else {
		sign = 1;
	}

	digit.clear();
	while(x > 0) {
		digit.push_back(x % 10);
		x /= 10;
	}
	if(digit.empty()) {
		digit.push_back(0);
	}
}

Lnum::Lnum(string s) {
	digit.clear();
	sign = 1;
	reverse(s.begin(), s.end());

	for(auto c : s) {
		if(isdigit(c)) {
			digit.push_back(c - '0');
		}
		else {
			if(c == '-') {
				sign *= -1;
			}
		}
	}
	normalize();
}

Lnum::Lnum(vector<int> digit, int sign) :
		digit(digit), sign(sign) {
	normalize();
}


Lnum::Lnum(const Lnum& x) : digit(x.getDigits()), sign(x.getSign()) {
	normalize();
}

// IN|OUT METHODS

istream& operator>>(istream& in, Lnum& x) {
	string s;
	in >> s;
	x = Lnum(s);
	return in;
}

ostream& operator<<(ostream& out, const Lnum& x) {
	if(x == 0) {
		out << 0;
	}
	else {
		if(x.getSign() == -1) {
			out << "-";
		}
		vector<int> digits = x.getDigits();
		reverse(digits.begin(), digits.end());
		for(auto x : digits) {
			out << x;
		}
	}
	return out;
}

//OPERATORS

Lnum& Lnum::operator=(const Lnum& x){
	digit = x.digit;
	return *this;
}

bool Lnum::operator==(const Lnum& x) const {
	return sign == x.getSign() && digit == x.getDigits();
}

bool Lnum::operator!=(const Lnum& x) const {
	return !(*this == x);
}

bool Lnum::operator<(const Lnum& x) const {
	if(sign != x.getSign()) {
		return sign < x.getSign();
	}
	else {
		bool condition;
		if(length() != x.length()) {
			condition = length() < x.length();
		}
		else {
			vector<int> v1 = digit, v2 = x.getDigits();
			reverse(v1.begin(), v1.end());
			reverse(v2.begin(), v2.end());
			condition = v1 < v2;
		}
		if(sign == -1) condition ^= 1;
		return condition;
	}
}

bool Lnum::operator<=(const Lnum& x) const {
	return *this < x || *this == x;
}

bool Lnum::operator>(const Lnum& x) const {
	return !(*this <= x);
}

bool Lnum::operator>=(const Lnum& x) const {
	return !(*this < x);
}

////ARYTHMETIC

Lnum Lnum::operator-() const {
	return Lnum(digit, -sign);
}

Lnum Lnum::operator+(const Lnum& x) const {
	if(sign == x.getSign()) {
		vector<int> c(max(x.length(), length()) + 1, 0);
		for(int i = 0, carry = 0; i < c.size(); i++) {
			carry += getDigit(i) + x.getDigit(i);
			c[i] = carry % 10;
			carry /= 10;
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

Lnum Lnum::operator-(const Lnum& x) const {
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
				vector<int> c(digit.size(), 0);
				for(int i = 0; i < c.size(); i++) {
					c[i] += getDigit(i) - x.getDigit(i);
					if(c[i] < 0) {
						c[i] += 10;
						c[i + 1]--;
					}
				}
				return Lnum(c, 1);
			}
		}
	}
}


Lnum Lnum::operator*(const Lnum& x) const {
	vector<int> c(length() + x.length() + 10, 0);
	for(int i = 0, carry = 0; i < digit.size(); i++) {
		for(int j = 0; i + j < c.size() || carry; j++) {
			c[i + j] += getDigit(i) * x.getDigit(j) + carry;
			carry = c[i + j] / 10;
			c[i + j] %= 10;
		}
	}
	return Lnum(c, sign * x.getSign());
}

Lnum Lnum::operator+=(const Lnum& x) {
	return (*this = *this + x);
}

Lnum Lnum::operator-=(const Lnum& x) {
	return (*this = *this - x);
}

Lnum Lnum::operator*=(const Lnum& x) {
	return (*this = *this * x);
}


////CLONES FOR INT

Lnum& Lnum::operator=(const long long &x){
	return (*this = Lnum(x));
}


bool Lnum::operator==(const long long& x) const {
	return *this == Lnum(x);
}

bool Lnum::operator!=(const long long& x) const {
	return *this != Lnum(x);
}

bool Lnum::operator<(const long long& x) const {
	return *this < Lnum(x);
}

bool Lnum::operator<=(const long long& x) const {
	return *this <= Lnum(x);
}

bool Lnum::operator>(const long long& x) const {
	return *this > Lnum(x);
}

bool Lnum::operator>=(const long long& x) const {
	return *this >= Lnum(x);
}


Lnum Lnum::operator+(const long long& x) const {
	return *this + Lnum(x);
}

Lnum Lnum::operator-(const long long& x) const {
	return *this - Lnum(x);
}

Lnum Lnum::operator*(const long long& x) const {
	return *this * Lnum(x);
}


Lnum Lnum::operator+=(const long long& x) {
	return (*this = *this + x);
}

Lnum Lnum::operator-=(const long long& x) {
	return (*this = *this - x);
}

Lnum Lnum::operator*=(const long long& x) {
	return (*this = *this * x);
}

//OTHERS

int Lnum::getDigit(int pos) const {
	return 0 <= pos && pos < digit.size() ? digit[pos] : 0;
}

int Lnum::length() const {
	return digit.size() == 1 && digit[0] == 0 ? 1 :
		digit.size() + (sign == -1);
}

int Lnum::getSign() const {
	return sign;
}

vector<int> Lnum::getDigits() const {
	return digit;
}

void Lnum::normalize() {
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

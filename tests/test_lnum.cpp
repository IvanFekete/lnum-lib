#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <lnum/lnum.hpp>
#include <climits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using lnum::Lnum;

TEST_CASE("construction and toString round-trip") {
	CHECK(Lnum().toString() == "0");
	CHECK(Lnum(0LL).toString() == "0");
	CHECK(Lnum(5LL).toString() == "5");
	CHECK(Lnum(-5LL).toString() == "-5");
	CHECK(Lnum(999999999LL).toString() == "999999999");
	CHECK(Lnum(1000000000LL).toString() == "1000000000");
	CHECK(Lnum("123456789123456789").toString() == "123456789123456789");
	CHECK(Lnum("-123456789123456789").toString() == "-123456789123456789");
	CHECK(Lnum("999999999999999999999999999").toString() == "999999999999999999999999999");
	CHECK(Lnum("000123").toString() == "123");
	CHECK(Lnum("-0").toString() == "0");
	CHECK(Lnum("-0").getSign() == 1);
	CHECK(Lnum("+5").toString() == "5"); // leading '+' is accepted like leading '-'
}

TEST_CASE("string construction rejects malformed input") {
	CHECK_THROWS_AS(Lnum(""), std::invalid_argument);
	CHECK_THROWS_AS(Lnum("-"), std::invalid_argument);
	CHECK_THROWS_AS(Lnum("+"), std::invalid_argument);
	CHECK_THROWS_AS(Lnum("--5"), std::invalid_argument);
	CHECK_THROWS_AS(Lnum("+-5"), std::invalid_argument);
	CHECK_THROWS_AS(Lnum("5-"), std::invalid_argument);
	CHECK_THROWS_AS(Lnum("12a34"), std::invalid_argument);
	CHECK_THROWS_AS(Lnum("12 34"), std::invalid_argument);
	CHECK_THROWS_AS(Lnum(" 123"), std::invalid_argument);
	CHECK_THROWS_AS(Lnum("123 "), std::invalid_argument);
	CHECK_THROWS_AS(Lnum("1,234"), std::invalid_argument);
	CHECK_THROWS_AS(Lnum("1.5"), std::invalid_argument);
}

TEST_CASE("comparisons") {
	CHECK(Lnum(5LL) == Lnum(5LL));
	CHECK(Lnum(5LL) != Lnum(6LL));
	CHECK(Lnum(-5LL) < Lnum(5LL));
	CHECK(Lnum(-5LL) < Lnum(-1LL));
	CHECK(Lnum(0LL) == Lnum(-0LL));
	CHECK(Lnum("123456789123456789") > Lnum("99999999999999999"));
	CHECK(Lnum(3LL) <= Lnum(3LL));
	CHECK(Lnum(3LL) >= Lnum(3LL));

	CHECK(Lnum(5LL) == 5LL);
	CHECK(Lnum(5LL) != 6LL);
	CHECK(Lnum(-1LL) < 0LL);
	CHECK(Lnum(-1LL) <= -1LL);
	CHECK(Lnum(1LL) >= 1LL);
	CHECK(Lnum(2LL) > 1LL);
}

TEST_CASE("addition") {
	CHECK((Lnum(2LL) + Lnum(3LL)) == Lnum(5LL));
	CHECK((Lnum(-2LL) + Lnum(3LL)) == Lnum(1LL));
	CHECK((Lnum(2LL) + Lnum(-3LL)) == Lnum(-1LL));
	CHECK((Lnum(-2LL) + Lnum(-3LL)) == Lnum(-5LL));
	CHECK((Lnum(999999999LL) + Lnum(1LL)).toString() == "1000000000"); // carry across a base-1e9 chunk
	CHECK((Lnum(5LL) + 3LL) == Lnum(8LL));
}

TEST_CASE("subtraction") {
	CHECK((Lnum(5LL) - Lnum(3LL)) == Lnum(2LL));
	CHECK((Lnum(3LL) - Lnum(5LL)) == Lnum(-2LL));
	CHECK((Lnum(-3LL) - Lnum(5LL)) == Lnum(-8LL));
	CHECK((Lnum(-3LL) - Lnum(-5LL)) == Lnum(2LL));
	CHECK((Lnum(1000000000LL) - Lnum(1LL)).toString() == "999999999"); // borrow across a base-1e9 chunk
	CHECK((Lnum(5LL) - 5LL) == 0LL);
}

TEST_CASE("multiplication") {
	CHECK((Lnum(6LL) * Lnum(7LL)) == Lnum(42LL));
	CHECK((Lnum(-6LL) * Lnum(7LL)) == Lnum(-42LL));
	CHECK((Lnum(-6LL) * Lnum(-7LL)) == Lnum(42LL));
	CHECK((Lnum(0LL) * Lnum(12345LL)) == Lnum(0LL));
	CHECK((Lnum(123456789LL) * Lnum(987654321LL)) == Lnum(123456789LL * 987654321LL));
	CHECK((Lnum(1000000000LL) * Lnum(1000000000LL)).toString() == "1000000000000000000");
}

TEST_CASE("division and modulo: truncate toward zero, matching built-in / and %") {
	// A handful of hand-checked cases across sign combinations.
	CHECK((Lnum(7LL) / Lnum(2LL)) == Lnum(3LL));
	CHECK((Lnum(7LL) % Lnum(2LL)) == Lnum(1LL));
	CHECK((Lnum(-7LL) / Lnum(2LL)) == Lnum(-3LL));
	CHECK((Lnum(-7LL) % Lnum(2LL)) == Lnum(-1LL));
	CHECK((Lnum(7LL) / Lnum(-2LL)) == Lnum(-3LL));
	CHECK((Lnum(7LL) % Lnum(-2LL)) == Lnum(1LL));
	CHECK((Lnum(-7LL) / Lnum(-2LL)) == Lnum(3LL));
	CHECK((Lnum(-7LL) % Lnum(-2LL)) == Lnum(-1LL));
	CHECK((Lnum(6LL) / Lnum(3LL)) == Lnum(2LL));
	CHECK((Lnum(6LL) % Lnum(3LL)) == Lnum(0LL));
	CHECK((Lnum(2LL) / Lnum(7LL)) == Lnum(0LL));
	CHECK((Lnum(2LL) % Lnum(7LL)) == Lnum(2LL));

	// Cross-check a grid of sign/magnitude combinations against C++'s own
	// (well-defined, truncating) integer division as an independent oracle,
	// and confirm quotient*divisor + remainder always reconstructs the dividend.
	const std::vector<long long> dividends = {0LL, 7LL, -7LL, 100LL, -100LL, 123456789123LL, -123456789123LL};
	const std::vector<long long> divisors = {2LL, -2LL, 3LL, -3LL, 1000000000LL, -1000000000LL};
	for (long long a : dividends) {
		for (long long b : divisors) {
			auto qr = Lnum(a).divmod(Lnum(b));
			CHECK((qr.first * Lnum(b) + qr.second) == Lnum(a));
			CHECK(qr.first == Lnum(a / b));
			CHECK(qr.second == Lnum(a % b));
		}
	}
}

TEST_CASE("LLONG_MIN construction does not trigger UB") {
	// -LLONG_MIN doesn't fit in long long, so naive negation overflows;
	// the constructor must negate via unsigned arithmetic instead.
	CHECK(Lnum(LLONG_MIN).toString() == "-9223372036854775808");
	CHECK(Lnum(LLONG_MIN).getSign() == -1);
	CHECK((-Lnum(LLONG_MIN)).toString() == "9223372036854775808");
	CHECK(Lnum(LLONG_MIN) < Lnum(0LL));
	CHECK((Lnum(LLONG_MIN) - Lnum(LLONG_MIN)) == Lnum(0LL));
	CHECK((Lnum(LLONG_MIN) + Lnum(1LL)).toString() == "-9223372036854775807");
}

TEST_CASE("division by zero throws") {
	CHECK_THROWS_AS(Lnum(5LL) / Lnum(0LL), std::invalid_argument);
	CHECK_THROWS_AS(Lnum(5LL) % Lnum(0LL), std::invalid_argument);
	CHECK_THROWS_AS(Lnum(5LL).divmod(Lnum(0LL)), std::invalid_argument);
	CHECK_THROWS_AS(Lnum(0LL) / Lnum(0LL), std::invalid_argument);
	CHECK_THROWS_AS(Lnum(-5LL) / Lnum(0LL), std::invalid_argument);
}

TEST_CASE("compound assignment") {
	Lnum a(10LL);
	a += Lnum(5LL); CHECK(a == Lnum(15LL));
	a -= Lnum(3LL); CHECK(a == Lnum(12LL));
	a *= Lnum(2LL); CHECK(a == Lnum(24LL));
	a /= Lnum(5LL); CHECK(a == Lnum(4LL));
	a %= Lnum(3LL); CHECK(a == Lnum(1LL));

	Lnum b(10LL);
	b += 5LL; CHECK(b == Lnum(15LL));
	b -= 3LL; CHECK(b == Lnum(12LL));
	b *= 2LL; CHECK(b == Lnum(24LL));
	b /= 5LL; CHECK(b == Lnum(4LL));
	b %= 3LL; CHECK(b == Lnum(1LL));
}

TEST_CASE("unary minus") {
	CHECK(-Lnum(5LL) == Lnum(-5LL));
	CHECK(-Lnum(-5LL) == Lnum(5LL));
	CHECK(-Lnum(0LL) == Lnum(0LL));
	CHECK((-Lnum(0LL)).getSign() == 1);
}

TEST_CASE("lPow") {
	CHECK(lnum::lPow(Lnum(2LL), 0) == Lnum(1LL));
	CHECK(lnum::lPow(Lnum(2LL), 10) == Lnum(1024LL));
	CHECK(lnum::lPow(2LL, 10) == Lnum(1024LL));
	CHECK(lnum::lPow(Lnum(-2LL), 3) == Lnum(-8LL));
	CHECK(lnum::lPow(Lnum(10LL), 18).toString() == "1000000000000000000");
}

TEST_CASE("stream operators") {
	std::ostringstream out;
	out << Lnum(-123456789123456789LL);
	CHECK(out.str() == "-123456789123456789");

	std::istringstream in("987654321987654321");
	Lnum x;
	in >> x;
	CHECK(x.toString() == "987654321987654321");
}

TEST_CASE("accessors") {
	Lnum x("5000000004"); // chunk 0 (least significant) = 4, chunk 1 = 5
	CHECK(x.getSign() == 1);
	CHECK(x.getDigits().size() == 2);
	CHECK(x.getDigit(0) == 4);
	CHECK(x.getDigit(1) == 5);
	CHECK(x.getDigit(5) == 0); // out of range reads as 0

	CHECK(Lnum(0LL).length() == 1);
	CHECK(Lnum(5LL).length() == 1);
	CHECK(Lnum(-5LL).getSign() == -1);
}

TEST_CASE("zero normalization") {
	CHECK(Lnum(std::vector<int>{0, 0, 0}, -1) == Lnum(0LL));
	CHECK(Lnum(std::vector<int>{0, 0, 0}, -1).getSign() == 1);
	CHECK(Lnum(std::vector<int>{5, 0}, 1).toString() == "5"); // trailing zero chunk trimmed
}

// Everything below deals in numbers with tens to hundreds of digits -- far
// past what long long (~19 digits), unsigned long long (~20 digits), or
// __int128 (~38-39 digits) can hold. Expected values were computed
// independently with Python's arbitrary-precision integers (adjusting its
// floor-based `//`/`%` to match this library's truncate-toward-zero
// convention) rather than derived from the implementation under test.

TEST_CASE("arithmetic on numbers far beyond long long / unsigned long long / __int128 range") {
	// 60-63 digit operands.
	const Lnum A("123456789012345678901234567890123456789012345678901234567890");
	const Lnum B("987654321098765432109876543210987654321098765432109876543210");

	CHECK((A + B).toString() == "1111111110111111111011111111101111111110111111111011111111100");
	CHECK((A - B).toString() == "-864197532086419753208641975320864197532086419753208641975320");
	CHECK((B - A).toString() == "864197532086419753208641975320864197532086419753208641975320");
	CHECK((A * B).toString() ==
		"121932631137021795226185032733866788594511507391563633592367367779295611949397448712086533622"
		"923332237463801111263526900");

	CHECK((A / B) == Lnum(0LL));
	CHECK((A % B) == A);
	CHECK((B / A).toString() == "8");
	CHECK((B % A).toString() == "9000000000900000000090000000009000000000900000000090");

	CHECK((-A / B) == Lnum(0LL));
	CHECK((-A % B) == -A);
	CHECK((A / -B) == Lnum(0LL));
	CHECK((A % -B) == A);
	CHECK((-A / -B) == Lnum(0LL));
	CHECK((-A % -B) == -A);

	CHECK(B > A);
	CHECK(A < B);
	CHECK(-A < A);
	CHECK(-B < -A);

	// quotient*divisor + remainder must reconstruct the dividend at this scale too.
	auto qr1 = A.divmod(B);
	CHECK((qr1.first * B + qr1.second) == A);
	auto qr2 = B.divmod(A);
	CHECK((qr2.first * A + qr2.second) == B);
}

TEST_CASE("carry and borrow cascades across many base-1e9 chunks") {
	const Lnum nines(std::string(40, '9'));
	CHECK((nines + Lnum(1LL)).toString() == "10000000000000000000000000000000000000000");

	const Lnum onezeros("1" + std::string(40, '0'));
	CHECK((onezeros - Lnum(1LL)).toString() == "9999999999999999999999999999999999999999");
}

TEST_CASE("multiplication of two 36-digit numbers") {
	const Lnum X("123456789123456789123456789123456789");
	const Lnum Y("987654321987654321987654321987654321");
	CHECK((X * Y).toString() ==
		"121932631356500531591068431825636331816338969581771069347203169112635269");
}

TEST_CASE("lPow producing results well beyond 128 bits") {
	CHECK(lnum::lPow(Lnum(2LL), 200).toString() ==
		"1606938044258990275541962092341162602522202993782792835301376");
	CHECK(lnum::lPow(Lnum(3LL), 150).toString() ==
		"369988485035126972924700782451696644186473100389722973815184405301748249");
	CHECK(lnum::lPow(Lnum(10LL), 100).toString() == "1" + std::string(100, '0'));
}

TEST_CASE("division edge cases at large scale") {
	const Lnum Z("55555555555555555555555555555555555555");
	CHECK((Z / Z) == Lnum(1LL));
	CHECK((Z % Z) == Lnum(0LL));

	// Divisor far larger than the (small, native-range) dividend: quotient 0,
	// remainder equal to the dividend, sign following the dividend.
	const Lnum big("999999999999999999999999999999999999999");
	CHECK((Lnum(42LL) / big) == Lnum(0LL));
	CHECK((Lnum(42LL) % big) == Lnum(42LL));
	CHECK((Lnum(-42LL) / big) == Lnum(0LL));
	CHECK((Lnum(-42LL) % big) == Lnum(-42LL));
}

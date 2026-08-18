#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <lnum/lnum.hpp>
#include <sstream>
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

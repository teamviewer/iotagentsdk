//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2024 TeamViewer Germany GmbH                                     //
//                                                                                //
// Permission is hereby granted, free of charge, to any person obtaining a copy   //
// of this software and associated documentation files (the "Software"), to deal  //
// in the Software without restriction, including without limitation the rights   //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      //
// copies of the Software, and to permit persons to whom the Software is          //
// furnished to do so, subject to the following conditions:                       //
//                                                                                //
// The above copyright notice and this permission notice shall be included in all //
// copies or substantial portions of the Software.                                //
//                                                                                //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  //
// SOFTWARE.                                                                      //
//********************************************************************************//
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/VersionNumber.h>
#include <iostream>

using namespace TVRemoteScreenSDKCommunication;

namespace
{

std::ostream& operator<<(std::ostream& os, const VersionNumber& versionNumber)
{
	os << "{" << versionNumber.first << ", " << versionNumber.second << "}";
	return os;
}

struct FromStringTestCase
{
	const char* givenCString;
	VersionNumber expectVersionNumber;
	bool expectConversionSuccess;
};

constexpr FromStringTestCase FromStringTestCases[] = {
	{"0.0",                     {0, 0},                     true},
	{"0.1",                     {0, 1},                     true},
	{"0.65535",                 {0, 65535},                 true},
	{"0.4294967295",            {0, 4294967295},            true},
	{"0.4294967296",            {0, 0},                     true},
	{"1.1",                     {1, 1},                     true},
	{"65535.65535",             {65535, 65535},             true},
	{"4294967295.4294967295",   {4294967295, 4294967295},   true},
	{"4294967296.4294967296",   {0, 0},                     true},
	{"-0.-0",                   {0, 0},                     true},
	{"-1.-1",                   {4294967295, 4294967295},   true},

	{"",                        {0, 0},                     false},
	{"1.2.3",                   {0, 0},                     false},
	{"asd.fff",                 {0, 0},                     false},
	{".4294967295",             {0, 0},                     false},
	{"0.",                      {0, 0},                     false},
	{".",                       {0, 0},                     false},
	{"a.b",                     {0, 0},                     false},
	{"-9123",                   {0, 0},                     false},
};

struct ToStringTestCase
{
	VersionNumber givenVersionNumber;
	const char* expectCString;
};

constexpr ToStringTestCase ToStringTestCases[] = {
	{{},                        "0.0"                   },
	{{0, 0},                    "0.0"                   },
	{{0, 1},                    "0.1"                   },
	{{0, 65535},                "0.65535"               },
	{{0, 4294967295},           "0.4294967295"          },
	{{1, 1},                    "1.1"                   },
	{{65535, 65535},            "65535.65535"           },
	{{4294967295, 4294967295},  "4294967295.4294967295" },
};

enum ComparisonType
{
	EQ,
	GT,
	GE,
	LT,
	LE,
	ComparisonTypeCount,
};

constexpr const char* ComparisonOperatorStrings[ComparisonTypeCount] = {
	"==",
	"> ",
	">=",
	"< ",
	"<=",
};

struct ComparisonTestCase
{
	VersionNumber lhs;
	VersionNumber rhs;
	bool expectedComparisonResults[ComparisonTypeCount];
};

using ComparisonOperator = bool(*)(const VersionNumber& lhs, const VersionNumber& rhs);
constexpr ComparisonOperator ComparisonOperaors[ComparisonTypeCount] = {
	&std::operator==,
	&std::operator>,
	&std::operator>=,
	&std::operator<,
	&std::operator<=,
};

constexpr ComparisonTestCase ComparisonTestCases[] = {
	//                                                    EQ GT GE LT LE
	{{},                        {},                       {1, 0, 1, 0, 1}},
	{{0, 0},                    {0, 0},                   {1, 0, 1, 0, 1}},
	{{4294967295, 4294967295},  {4294967295, 4294967295}, {1, 0, 1, 0, 1}},
	{{0, 5},                    {0, 0},                   {0, 1, 1, 0, 0}},
	{{5, 0},                    {0, 0},                   {0, 1, 1, 0, 0}},
	{{5, 5},                    {0, 10},                  {0, 1, 1, 0, 0}},
	{{5, 10},                   {5, 5},                   {0, 1, 1, 0, 0}},
	{{0, 0},                    {0, 1},                   {0, 0, 0, 1, 1}},
	{{0, 5},                    {0, 10},                  {0, 0, 0, 1, 1}},
	{{5, 0},                    {10, 0},                  {0, 0, 0, 1, 1}},
	{{5, 5},                    {10, 0},                  {0, 0, 0, 1, 1}},
	{{5, 5},                    {10, 10},                 {0, 0, 0, 1, 1}},
};

}

int main()
{
	std::cout << "VersionNumber Tests" << "\n";
	std::cout << "--- Test: FromString ---\n";

	for (const FromStringTestCase& testCase : FromStringTestCases)
	{
		VersionNumber versionNumber;
		const bool conversionSuccess = VersionNumberFromString(testCase.givenCString, versionNumber);

		std::cout << "* Given CString: '" << testCase.givenCString << "'\n"
				<< "EXPECT    conversion success: " << testCase.expectConversionSuccess << ", "
				<< "VersionNumber: " << testCase.expectVersionNumber << "\n"
				<< "RESULTING conversion success: " << conversionSuccess << ", "
				<< "VersionNumber: " << versionNumber << "\n";

		if (versionNumber == testCase.expectVersionNumber)
		{
			std::cout << "OK\n";
		}
		else
		{
			std::cerr << "FAILED\n";
			return EXIT_FAILURE;
		}
	}

	std::cout << "\n--- Test: FromString ---\n";

	for (const ToStringTestCase& testCase : ToStringTestCases)
	{
		const std::string resultingString = VersionNumberToString(testCase.givenVersionNumber);

		std::cout << "* Given VersionNumber: " << testCase.givenVersionNumber << "\n"
			    << "EXPECT string:         '" << testCase.expectCString << "'\n"
				<< "RESULTING string:      '" << resultingString << "'\n";

		if (resultingString == testCase.expectCString)
		{
			std::cout << "OK\n";
		}
		else
		{
			std::cerr << "FAILED\n";
			return EXIT_FAILURE;
		}
	}

	std::cout << "\n--- Test: comarison operators ---\n";

	for (const ComparisonTestCase& testCase : ComparisonTestCases)
	{
		std::cout << "* Given VersionNumbers: " << testCase.lhs << ", " <<  testCase.rhs << "\n";

		for (int i = 0; i < ComparisonTypeCount; ++i)
		{
			const ComparisonOperator op = ComparisonOperaors[i];
			const bool result = op(testCase.lhs, testCase.rhs);
			const bool expectedResult = testCase.expectedComparisonResults[i];
			std::cout << testCase.lhs << " " << ComparisonOperatorStrings[i] << " " << testCase.rhs << " resulting: " << result << ", expected: " << expectedResult << " -> ";

			if (result == expectedResult)
			{
				std::cout << "OK\n";
			}
			else
			{
				std::cerr << "FAILED\n";
				return EXIT_FAILURE;
			}
		}
		std::cout << "OK\n";
	}

	std::cout << "All tests passed\n";

	return EXIT_SUCCESS;
}

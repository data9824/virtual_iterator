#include "pch.h"
#include "virtual_random_access_iteratorTest.h"


CPPUNIT_TEST_SUITE_REGISTRATION( virtual_random_access_iteratorTest );

// vsortをオーバーロードすると暗黙の型変換が行われないので、オーバーロードは避けました。

static void vsorti(virtual_random_access_iterator<int> first, virtual_random_access_iterator<int> last)
{
	std::sort(first, last);
}

static void vsorts(virtual_random_access_iterator<std::string> first, virtual_random_access_iterator<std::string> last)
{
	std::sort(first, last);
}

virtual_random_access_iteratorTest::virtual_random_access_iteratorTest()
{
}


virtual_random_access_iteratorTest::~virtual_random_access_iteratorTest()
{
}

void virtual_random_access_iteratorTest::testDefaultConstruct()
{
	{
		virtual_random_access_iterator<int> it;
		virtual_random_access_iterator<int> it2;
		it = it2;
	}
	{
		virtual_random_access_iterator<std::string> it;
		virtual_random_access_iterator<std::string> it2;
		it = it2;
	}
}

void virtual_random_access_iteratorTest::testTemplateConstruct()
{
	{
		std::vector<int> l;
		l.push_back(1);
		virtual_random_access_iterator<int> it1(l.begin());
		virtual_random_access_iterator<int> it2(l.begin());
		CPPUNIT_ASSERT(it1 == it2);
	}
	{
		std::vector<std::string> l;
		l.push_back("test");
		virtual_random_access_iterator<std::string> it1(l.begin());
		virtual_random_access_iterator<std::string> it2(l.begin());
		CPPUNIT_ASSERT(it1 == it2);
	}
}

void virtual_random_access_iteratorTest::testCopyConstruct()
{
	{
		std::vector<int> l;
		l.push_back(1);
		virtual_random_access_iterator<int> it1(l.begin());
		virtual_random_access_iterator<int> it2(it1);
		CPPUNIT_ASSERT(it1 == it2);
	}
	{
		std::vector<std::string> l;
		l.push_back("test");
		virtual_random_access_iterator<std::string> it1(l.begin());
		virtual_random_access_iterator<std::string> it2(it1);
		CPPUNIT_ASSERT(it1 == it2);
	}
}

void virtual_random_access_iteratorTest::testAssign()
{
	{
		std::vector<int> l;
		l.push_back(1);
		virtual_random_access_iterator<int> it1(l.begin());
		virtual_random_access_iterator<int> it2;
		it2 = it1;
		CPPUNIT_ASSERT(it1 == it2);
	}
	{
		std::vector<std::string> l;
		l.push_back("test");
		virtual_random_access_iterator<std::string> it1(l.begin());
		virtual_random_access_iterator<std::string> it2;
		it2 = it1;
		CPPUNIT_ASSERT(it1 == it2);
	}
}

void virtual_random_access_iteratorTest::testInt()
{
	int source[100];
	size_t num = sizeof(source) / sizeof(source[0]);
	for (size_t i = 0; i < num; ++i)
	{
		source[i] = i;
	}
	std::random_shuffle(source, source + num);
	{
		std::vector<int> correct(source, source + num);
		std::vector<int> test(source, source + num);
		std::sort(correct.begin(), correct.end());
		vsorti(test.begin(), test.end());
		CPPUNIT_ASSERT(correct == test);
	}
	{
		int *correct = new int[num];
		int *test = new int[num];
		std::copy(source, source + num, correct);
		std::copy(source, source + num, test);
		std::sort(correct, correct + num);
		vsorti(test, test + num);
		CPPUNIT_ASSERT(std::equal(correct, correct + num, test));
		delete [] correct;
		delete [] test;
	}
}

void virtual_random_access_iteratorTest::testString()
{
	std::string source[100];
	size_t num = sizeof(source) / sizeof(source[0]);
	for (size_t i = 0; i < num; ++i)
	{
		std::ostringstream ostr;
		ostr << "stringnumber:" << i;
		source[i] = ostr.str();
	}
	std::random_shuffle(source, source + num);
	{
		std::vector<std::string> correct(source, source + num);
		std::vector<std::string> test(source, source + num);
		std::sort(correct.begin(), correct.end());
		vsorts(test.begin(), test.end());
		CPPUNIT_ASSERT(correct == test);
	}
	{
		std::string *correct = new std::string[num];
		std::string *test = new std::string[num];
		std::copy(source, source + num, correct);
		std::copy(source, source + num, test);
		std::sort(correct, correct + num);
		vsorts(test, test + num);
		CPPUNIT_ASSERT(std::equal(correct, correct + num, test));
		delete [] correct;
		delete [] test;
	}
}

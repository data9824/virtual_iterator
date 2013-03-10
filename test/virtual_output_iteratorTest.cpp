#include "pch.h"
#include "virtual_output_iteratorTest.h"


CPPUNIT_TEST_SUITE_REGISTRATION( virtual_output_iteratorTest );

static void vcopy(int *first, int *last, virtual_output_iterator<int> out)
{
	std::copy(first, last, out);
}

static void vcopy(std::string *first, std::string *last, virtual_output_iterator<std::string> out)
{
	std::copy(first, last, out);
}

virtual_output_iteratorTest::virtual_output_iteratorTest()
{
}


virtual_output_iteratorTest::~virtual_output_iteratorTest()
{
}


void virtual_output_iteratorTest::testDefaultConstruct()
{
	{
		virtual_output_iterator<int> it;
		virtual_output_iterator<int> it2;
		it = it2;
	}
	{
		virtual_output_iterator<std::string> it;
		virtual_output_iterator<std::string> it2;
		it = it2;
	}
}

void virtual_output_iteratorTest::testTemplateConstruct()
{
	{
		std::list<int> l;
		virtual_output_iterator<int> it(std::back_inserter(l));
		*it++ = 1;
		*it++ = 2;
		CPPUNIT_ASSERT(l.size() == 2);
	}
	{
		std::list<std::string> l;
		virtual_output_iterator<std::string> it(std::back_inserter(l));
		*it++ = std::string("str1");
		*it++ = std::string("str2");
		CPPUNIT_ASSERT(l.size() == 2);
	}
}

void virtual_output_iteratorTest::testCopyConstruct()
{
	{
		std::list<int> l;
		virtual_output_iterator<int> it1(std::back_inserter(l));
		virtual_output_iterator<int> it(it1);
		*it++ = 1;
		*it++ = 2;
		CPPUNIT_ASSERT(l.size() == 2);
	}
	{
		std::list<std::string> l;
		virtual_output_iterator<std::string> it1(std::back_inserter(l));
		virtual_output_iterator<std::string> it(it1);
		*it++ = std::string("str1");
		*it++ = std::string("str2");
		CPPUNIT_ASSERT(l.size() == 2);
	}
}

void virtual_output_iteratorTest::testAssign()
{
	{
		std::list<int> l;
		virtual_output_iterator<int> it1(std::back_inserter(l));
		virtual_output_iterator<int> it;
		it = it1;
		*it++ = 1;
		*it++ = 2;
		CPPUNIT_ASSERT(l.size() == 2);
	}
	{
		std::list<std::string> l;
		virtual_output_iterator<std::string> it1(std::back_inserter(l));
		virtual_output_iterator<std::string> it;
		it = it1;
		*it++ = std::string("str1");
		*it++ = std::string("str2");
		CPPUNIT_ASSERT(l.size() == 2);
	}
}

void virtual_output_iteratorTest::testInt()
{
	int source[100];
	size_t num = sizeof(source) / sizeof(source[0]);
	for (size_t i = 0; i < num; ++i)
	{
		source[i] = i;
	}
	std::random_shuffle(source, source + num);
	{
		std::list<int> l;
		std::list<int> l2;
		std::copy(source, source + num, std::back_inserter(l));
		vcopy(source, source + num, std::back_inserter(l2));
		CPPUNIT_ASSERT(l == l2);
	}
	{
		std::set<int> s;
		std::set<int> s2;
		std::copy(source, source + num, std::inserter(s, s.begin()));
		vcopy(source, source + num, std::inserter(s2, s2.begin()));
		CPPUNIT_ASSERT(s == s2);
	}
	{
		std::list<int> v;
		std::list<int> v2;
		std::copy(source, source + num, std::back_inserter(v));
		vcopy(source, source + num, std::back_inserter(v2));
		CPPUNIT_ASSERT(v == v2);
	}
	{
		int *a;
		int *a2;
		a = new int[num];
		a2 = new int[num];
		std::copy(source, source + num, a);
		vcopy(source, source + num, a2);
		CPPUNIT_ASSERT(std::equal(a, a + num, a2));
		delete [] a;
		delete [] a2;
	}
}

void virtual_output_iteratorTest::testString()
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
		std::list<std::string> l;
		std::list<std::string> l2;
		std::copy(source, source + num, std::back_inserter(l));
		vcopy(source, source + num, std::back_inserter(l2));
		CPPUNIT_ASSERT(l == l2);
	}
	{
		std::set<std::string> s;
		std::set<std::string> s2;
		std::copy(source, source + num, std::inserter(s, s.begin()));
		vcopy(source, source + num, std::inserter(s2, s2.begin()));
		CPPUNIT_ASSERT(s == s2);
	}
	{
		std::list<std::string> v;
		std::list<std::string> v2;
		std::copy(source, source + num, std::back_inserter(v));
		vcopy(source, source + num, std::back_inserter(v2));
		CPPUNIT_ASSERT(v == v2);
	}
	{
		std::string *a;
		std::string *a2;
		a = new std::string[num];
		a2 = new std::string[num];
		std::copy(source, source + num, a);
		vcopy(source, source + num, a2);
		CPPUNIT_ASSERT(std::equal(a, a + num, a2));
		delete [] a;
		delete [] a2;
	}
}


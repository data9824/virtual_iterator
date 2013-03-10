#include "pch.h"
#include "virtual_forward_iteratorTest.h"


CPPUNIT_TEST_SUITE_REGISTRATION( virtual_forward_iteratorTest );

static void vreplace(virtual_forward_iterator<int> first, virtual_forward_iterator<int> last, int x, int y)
{
	std::replace(first, last, x, y);
}

static void vreplace(virtual_forward_iterator<std::string> first, virtual_forward_iterator<std::string> last, std::string x, std::string y)
{
	std::replace(first, last, x, y);
}

virtual_forward_iteratorTest::virtual_forward_iteratorTest()
{
}


virtual_forward_iteratorTest::~virtual_forward_iteratorTest()
{
}

void virtual_forward_iteratorTest::testDefaultConstruct()
{
	{
		virtual_forward_iterator<int> it;
		virtual_forward_iterator<int> it2;
		it = it2;
	}
	{
		virtual_forward_iterator<std::string> it;
		virtual_forward_iterator<std::string> it2;
		it = it2;
	}
}

void virtual_forward_iteratorTest::testTemplateConstruct()
{
	{
		std::list<int> l;
		l.push_back(1);
		virtual_forward_iterator<int> it1(l.begin());
		virtual_forward_iterator<int> it2(l.begin());
		CPPUNIT_ASSERT(it1 == it2);
	}
	{
		std::list<std::string> l;
		l.push_back("test");
		virtual_forward_iterator<std::string> it1(l.begin());
		virtual_forward_iterator<std::string> it2(l.begin());
		CPPUNIT_ASSERT(it1 == it2);
	}
}

void virtual_forward_iteratorTest::testCopyConstruct()
{
	{
		std::list<int> l;
		l.push_back(1);
		virtual_forward_iterator<int> it1(l.begin());
		virtual_forward_iterator<int> it2(it1);
		CPPUNIT_ASSERT(it1 == it2);
	}
	{
		std::list<std::string> l;
		l.push_back("test");
		virtual_forward_iterator<std::string> it1(l.begin());
		virtual_forward_iterator<std::string> it2(it1);
		CPPUNIT_ASSERT(it1 == it2);
	}
}

void virtual_forward_iteratorTest::testAssign()
{
	{
		std::list<int> l;
		l.push_back(1);
		virtual_forward_iterator<int> it1(l.begin());
		virtual_forward_iterator<int> it2;
		it2 = it1;
		CPPUNIT_ASSERT(it1 == it2);
	}
	{
		std::list<std::string> l;
		l.push_back("test");
		virtual_forward_iterator<std::string> it1(l.begin());
		virtual_forward_iterator<std::string> it2;
		it2 = it1;
		CPPUNIT_ASSERT(it1 == it2);
	}
}

void virtual_forward_iteratorTest::testUpCast()
{
	{
		std::vector<int> cont;
		virtual_random_access_iterator<int> random_access(cont.begin());
		virtual_forward_iterator<int> me(random_access);
		CPPUNIT_ASSERT(me == static_cast<virtual_forward_iterator<int> >(random_access));
	}
	{
		std::list<int> cont;
		virtual_bidirectional_iterator<int> bidirectional(cont.begin());
		virtual_forward_iterator<int> me(bidirectional);
		CPPUNIT_ASSERT(me == static_cast<virtual_forward_iterator<int> >(bidirectional));
	}
}

void virtual_forward_iteratorTest::testInt()
{
	int source[100];
	size_t num = sizeof(source) / sizeof(source[0]);
	for (size_t i = 0; i < num; ++i)
	{
		source[i] = i;
	}
	std::random_shuffle(source, source + num);
	{
		std::list<int> correct(source, source + num);
		std::list<int> test(source, source + num);
		std::replace(correct.begin(), correct.end(), source[50], source[70]);
		vreplace(test.begin(), test.end(), source[50], source[70]);
		CPPUNIT_ASSERT(correct == test);
	}
	{
		std::vector<int> correct(source, source + num);
		std::vector<int> test(source, source + num);
		std::replace(correct.begin(), correct.end(), source[50], source[70]);
		vreplace(test.begin(), test.end(), source[50], source[70]);
		CPPUNIT_ASSERT(correct == test);
	}
	{
		int *correct = new int[num];
		int *test = new int[num];
		std::copy(source, source + num, correct);
		std::copy(source, source + num, test);
		std::replace(correct, correct + num, source[50], source[70]);
		vreplace(test, test + num, source[50], source[70]);
		CPPUNIT_ASSERT(std::equal(correct, correct + num, test));
		delete [] correct;
		delete [] test;
	}
}

void virtual_forward_iteratorTest::testString()
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
		std::list<std::string> correct(source, source + num);
		std::list<std::string> test(source, source + num);
		std::replace(correct.begin(), correct.end(), source[50], source[70]);
		vreplace(test.begin(), test.end(), source[50], source[70]);
		CPPUNIT_ASSERT(correct == test);
	}
	{
		std::vector<std::string> correct(source, source + num);
		std::vector<std::string> test(source, source + num);
		std::replace(correct.begin(), correct.end(), source[50], source[70]);
		vreplace(test.begin(), test.end(), source[50], source[70]);
		CPPUNIT_ASSERT(correct == test);
	}
	{
		std::string *correct = new std::string[num];
		std::string *test = new std::string[num];
		std::copy(source, source + num, correct);
		std::copy(source, source + num, test);
		std::replace(correct, correct + num, source[50], source[70]);
		vreplace(test, test + num, source[50], source[70]);
		CPPUNIT_ASSERT(std::equal(correct, correct + num, test));
		delete [] correct;
		delete [] test;
	}
}

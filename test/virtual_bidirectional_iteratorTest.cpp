#include "pch.h"
#include "virtual_bidirectional_iteratorTest.h"


CPPUNIT_TEST_SUITE_REGISTRATION( virtual_bidirectional_iteratorTest );


static void vrcopy(virtual_bidirectional_iterator<const int> first, virtual_bidirectional_iterator<const int> last, int *out)
{
	std::reverse_copy(first, last, out);
}


static void vrcopy(virtual_bidirectional_iterator<const std::string> first, virtual_bidirectional_iterator<const std::string> last, std::string *out)
{
	std::reverse_copy(first, last, out);
}


virtual_bidirectional_iteratorTest::virtual_bidirectional_iteratorTest()
{
}


virtual_bidirectional_iteratorTest::~virtual_bidirectional_iteratorTest()
{
}

void virtual_bidirectional_iteratorTest::testDefaultConstruct()
{
	{
		virtual_bidirectional_iterator<int> it;
		virtual_bidirectional_iterator<int> it2;
		it = it2;
	}
	{
		virtual_bidirectional_iterator<std::string> it;
		virtual_bidirectional_iterator<std::string> it2;
		it = it2;
	}
}

void virtual_bidirectional_iteratorTest::testTemplateConstruct()
{
	{
		std::list<int> l;
		l.push_back(1);
		virtual_bidirectional_iterator<int> it1(l.begin());
		virtual_bidirectional_iterator<int> it2(l.begin());
		CPPUNIT_ASSERT(it1 == it2);
	}
	{
		std::list<std::string> l;
		l.push_back("test");
		virtual_bidirectional_iterator<std::string> it1(l.begin());
		virtual_bidirectional_iterator<std::string> it2(l.begin());
		CPPUNIT_ASSERT(it1 == it2);
	}
}

void virtual_bidirectional_iteratorTest::testCopyConstruct()
{
	{
		std::list<int> l;
		l.push_back(1);
		virtual_bidirectional_iterator<int> it1(l.begin());
		virtual_bidirectional_iterator<int> it2(it1);
		CPPUNIT_ASSERT(it1 == it2);
	}
	{
		std::list<std::string> l;
		l.push_back("test");
		virtual_bidirectional_iterator<std::string> it1(l.begin());
		virtual_bidirectional_iterator<std::string> it2(it1);
		CPPUNIT_ASSERT(it1 == it2);
	}
}

void virtual_bidirectional_iteratorTest::testAssign()
{
	{
		std::list<int> l;
		l.push_back(1);
		virtual_bidirectional_iterator<int> it1(l.begin());
		virtual_bidirectional_iterator<int> it2;
		it2 = it1;
		CPPUNIT_ASSERT(it1 == it2);
	}
	{
		std::list<std::string> l;
		l.push_back("test");
		virtual_bidirectional_iterator<std::string> it1(l.begin());
		virtual_bidirectional_iterator<std::string> it2;
		it2 = it1;
		CPPUNIT_ASSERT(it1 == it2);
	}
}

void virtual_bidirectional_iteratorTest::testUpCast()
{
	{
		std::vector<int> cont;
		virtual_random_access_iterator<int> random_access(cont.begin());
		virtual_bidirectional_iterator<int> me(random_access);
		CPPUNIT_ASSERT(me == static_cast<virtual_bidirectional_iterator<int> >(random_access));
	}
}

void virtual_bidirectional_iteratorTest::testInt()
{
	int source[100];
	int correct[100];
	int test[100];
	size_t num = sizeof(source) / sizeof(source[0]);
	for (size_t i = 0; i < num; ++i)
	{
		source[i] = i;
	}
	std::random_shuffle(source, source + num);
	{
		std::list<int> cont(source, source + num);
		std::reverse_copy(cont.begin(), cont.end(), correct);
		vrcopy(cont.begin(), cont.end(), test);
		CPPUNIT_ASSERT(std::equal(correct, correct + num, test));
	}
	{
		std::set<int> cont(source, source + num);
		std::reverse_copy(cont.begin(), cont.end(), correct);
		vrcopy(cont.begin(), cont.end(), test);
		CPPUNIT_ASSERT(std::equal(correct, correct + num, test));
	}
	{
		std::vector<int> cont(source, source + num);
		std::reverse_copy(cont.begin(), cont.end(), correct);
		vrcopy(cont.begin(), cont.end(), test);
		CPPUNIT_ASSERT(std::equal(correct, correct + num, test));
	}
	{
		int *cont = new int[num];
		std::copy(source, source + num, cont);
		std::reverse_copy(cont, cont + num, correct);
		vrcopy(cont, cont + num, test);
		CPPUNIT_ASSERT(std::equal(correct, correct + num, test));
		delete [] cont;
	}
}

void virtual_bidirectional_iteratorTest::testString()
{
	std::string source[100];
	std::string correct[100];
	std::string test[100];
	size_t num = sizeof(source) / sizeof(source[0]);
	for (size_t i = 0; i < num; ++i)
	{
		std::ostringstream ostr;
		ostr << "stringnumber:" << i;
		source[i] = ostr.str();
	}
	std::random_shuffle(source, source + num);
	{
		std::list<std::string> cont(source, source + num);
		std::reverse_copy(cont.begin(), cont.end(), correct);
		vrcopy(cont.begin(), cont.end(), test);
		CPPUNIT_ASSERT(std::equal(correct, correct + num, test));
	}
	{
		std::set<std::string> cont(source, source + num);
		std::reverse_copy(cont.begin(), cont.end(), correct);
		vrcopy(cont.begin(), cont.end(), test);
		CPPUNIT_ASSERT(std::equal(correct, correct + num, test));
	}
	{
		std::vector<std::string> cont(source, source + num);
		std::reverse_copy(cont.begin(), cont.end(), correct);
		vrcopy(cont.begin(), cont.end(), test);
		CPPUNIT_ASSERT(std::equal(correct, correct + num, test));
	}
	{
		std::string *cont = new std::string[num];
		std::copy(source, source + num, cont);
		std::reverse_copy(cont, cont + num, correct);
		vrcopy(cont, cont + num, test);
		CPPUNIT_ASSERT(std::equal(correct, correct + num, test));
		delete [] cont;
	}
}

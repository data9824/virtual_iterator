#include "pch.h"
#include "virtual_input_iteratorTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( virtual_input_iteratorTest );

static void vcopy(virtual_input_iterator<int> first, virtual_input_iterator<int> last, int *out)
{
	std::copy(first, last, out);
}

static void vcopy(virtual_input_iterator<std::string> first, virtual_input_iterator<std::string> last, std::string *out)
{
	std::copy(first, last, out);
}

virtual_input_iteratorTest::virtual_input_iteratorTest()
{
}


virtual_input_iteratorTest::~virtual_input_iteratorTest()
{
}

void virtual_input_iteratorTest::testDefaultConstruct()
{
	{
		virtual_input_iterator<int> it;
		virtual_input_iterator<int> it2;
		it = it2;
	}
	{
		virtual_input_iterator<std::string> it;
		virtual_input_iterator<std::string> it2;
		it = it2;
	}
}

void virtual_input_iteratorTest::testTemplateConstruct()
{
	{
		std::set<int> l;
		l.insert(1);
		virtual_input_iterator<int> it1(l.begin());
		virtual_input_iterator<int> it2(l.begin());
		CPPUNIT_ASSERT(it1 == it2);
	}
	{
		std::set<std::string> l;
		l.insert("test");
		virtual_input_iterator<std::string> it1(l.begin());
		virtual_input_iterator<std::string> it2(l.begin());
		CPPUNIT_ASSERT(it1 == it2);
	}
}

void virtual_input_iteratorTest::testCopyConstruct()
{
	{
		std::set<int> l;
		l.insert(1);
		virtual_input_iterator<int> it1(l.begin());
		virtual_input_iterator<int> it2(it1);
		CPPUNIT_ASSERT(it1 == it2);
	}
	{
		std::set<std::string> l;
		l.insert("test");
		virtual_input_iterator<std::string> it1(l.begin());
		virtual_input_iterator<std::string> it2(it1);
		CPPUNIT_ASSERT(it1 == it2);
	}
}

void virtual_input_iteratorTest::testAssign()
{
	{
		std::set<int> l;
		l.insert(1);
		virtual_input_iterator<int> it1(l.begin());
		virtual_input_iterator<int> it2;
		it2 = it1;
		CPPUNIT_ASSERT(it1 == it2);
	}
	{
		std::set<std::string> l;
		l.insert("test");
		virtual_input_iterator<std::string> it1(l.begin());
		virtual_input_iterator<std::string> it2;
		it2 = it1;
		CPPUNIT_ASSERT(it1 == it2);
	}
}

void virtual_input_iteratorTest::testUpCast()
{
	{
		std::vector<int> cont;
		virtual_random_access_iterator<int> random_access(cont.begin());
		virtual_input_iterator<int> me(random_access);
		CPPUNIT_ASSERT(me == static_cast<virtual_input_iterator<int> >(random_access));
	}
	{
		std::list<int> cont;
		virtual_bidirectional_iterator<int> bidirectional(cont.begin());
		virtual_input_iterator<int> me(bidirectional);
		CPPUNIT_ASSERT(me == static_cast<virtual_input_iterator<int> >(bidirectional));
	}
	{
		std::list<int> cont;
		virtual_forward_iterator<int> forward(cont.begin());
		virtual_input_iterator<int> me(forward);
		CPPUNIT_ASSERT(me == static_cast<virtual_input_iterator<int> >(forward));
	}
}

class CBase
{
public:
	CBase(int n) : m_n(n) {}
	bool operator==(const CBase& rhs)
	{
		return m_n == rhs.m_n;
	}
	int m_n;
};

class CDerived : public CBase
{
public:
	CDerived(int n) : CBase(n) {}
};

void virtual_input_iteratorTest::testContainDerived()
{
	std::set<CDerived *> cont;
	CDerived d(1);
	cont.insert(&d);
	virtual_input_iterator<CBase *, const CBase *, void> base(cont.begin());
	virtual_input_iterator<CBase *, const CBase *, void> derived(cont.begin());
	CPPUNIT_ASSERT(*base == *derived);
}

void virtual_input_iteratorTest::testInt()
{
	int source[100];
	int dest[100];
	size_t num = sizeof(source) / sizeof(source[0]);
	for (size_t i = 0; i < num; ++i)
	{
		source[i] = i;
	}
	std::random_shuffle(source, source + num);
	{
		std::list<int> correct(source, source + num);
		std::fill_n(dest, num, 0);
		vcopy(correct.begin(), correct.end(), dest);
		CPPUNIT_ASSERT(std::equal(correct.begin(), correct.end(), dest));
	}
	{
		std::set<int> correct(source, source + num);
		std::fill_n(dest, num, 0);
		vcopy(correct.begin(), correct.end(), dest);
		CPPUNIT_ASSERT(std::equal(correct.begin(), correct.end(), dest));
	}
	{
		std::vector<int> correct(source, source + num);
		std::fill_n(dest, num, 0);
		vcopy(correct.begin(), correct.end(), dest);
		CPPUNIT_ASSERT(std::equal(correct.begin(), correct.end(), dest));
	}
	{
		int *correct = new int[num];
		std::copy(source, source + num, correct);
		std::fill_n(dest, num, 0);
		vcopy(correct, correct + num, dest);
		CPPUNIT_ASSERT(std::equal(correct, correct + num, dest));
		delete [] correct;
	}
}

void virtual_input_iteratorTest::testString()
{
	std::string source[100];
	std::string dest[100];
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
		std::fill_n(dest, num, 0);
		vcopy(correct.begin(), correct.end(), dest);
		CPPUNIT_ASSERT(std::equal(correct.begin(), correct.end(), dest));
	}
	{
		std::set<std::string> correct(source, source + num);
		std::fill_n(dest, num, 0);
		vcopy(correct.begin(), correct.end(), dest);
		CPPUNIT_ASSERT(std::equal(correct.begin(), correct.end(), dest));
	}
	{
		std::vector<std::string> correct(source, source + num);
		std::fill_n(dest, num, 0);
		vcopy(correct.begin(), correct.end(), dest);
		CPPUNIT_ASSERT(std::equal(correct.begin(), correct.end(), dest));
	}
	{
		std::string *correct = new std::string[num];
		std::copy(source, source + num, correct);
		std::fill_n(dest, num, 0);
		vcopy(correct, correct + num, dest);
		CPPUNIT_ASSERT(std::equal(correct, correct + num, dest));
		delete [] correct;
	}
}

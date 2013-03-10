#ifndef virtual_forward_iteratorTestDEF
#define virtual_forward_iteratorTestDEF

#include <cppunit\testcase.h>
#include <cppunit\extensions\HelperMacros.h>
class virtual_forward_iteratorTest:public CppUnit::TestCase
{
	CPPUNIT_TEST_SUITE( virtual_forward_iteratorTest );
	CPPUNIT_TEST( testDefaultConstruct );
	CPPUNIT_TEST( testTemplateConstruct );
	CPPUNIT_TEST( testCopyConstruct );
	CPPUNIT_TEST( testAssign );
	CPPUNIT_TEST( testUpCast );
	CPPUNIT_TEST( testInt );
	CPPUNIT_TEST( testString );
	CPPUNIT_TEST_SUITE_END();
public:
	virtual_forward_iteratorTest();
	virtual ~virtual_forward_iteratorTest();
	void testDefaultConstruct();
	void testTemplateConstruct();
	void testCopyConstruct();
	void testAssign();
	void testUpCast();
	void testInt();
	void testString();
};
#endif

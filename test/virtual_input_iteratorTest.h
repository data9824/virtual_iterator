#ifndef virtual_input_iteratorTestDEF
#define virtual_input_iteratorTestDEF

#include <cppunit\testcase.h>
#include <cppunit\extensions\HelperMacros.h>

class virtual_input_iteratorTest:public CppUnit::TestCase
{
	CPPUNIT_TEST_SUITE( virtual_input_iteratorTest );
	CPPUNIT_TEST( testDefaultConstruct );
	CPPUNIT_TEST( testTemplateConstruct );
	CPPUNIT_TEST( testCopyConstruct );
	CPPUNIT_TEST( testAssign );
	CPPUNIT_TEST( testUpCast );
	CPPUNIT_TEST( testContainDerived );
	CPPUNIT_TEST( testInt );
	CPPUNIT_TEST( testString );
	CPPUNIT_TEST_SUITE_END();
public:
	virtual_input_iteratorTest();
	virtual ~virtual_input_iteratorTest();
	void testDefaultConstruct();
	void testTemplateConstruct();
	void testCopyConstruct();
	void testAssign();
	void testUpCast();
	void testContainDerived();
	void testInt();
	void testString();
};
#endif

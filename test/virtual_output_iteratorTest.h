#ifndef virtual_output_iteratorTestDEF
#define virtual_output_iteratorTestDEF

#include <cppunit\testcase.h>
#include <cppunit\extensions\HelperMacros.h>
class virtual_output_iteratorTest:public CppUnit::TestCase
{
	CPPUNIT_TEST_SUITE( virtual_output_iteratorTest );
	CPPUNIT_TEST( testDefaultConstruct );
	CPPUNIT_TEST( testTemplateConstruct );
	CPPUNIT_TEST( testCopyConstruct );
	CPPUNIT_TEST( testAssign );
	CPPUNIT_TEST( testInt );
	CPPUNIT_TEST( testString );
	CPPUNIT_TEST_SUITE_END();
public:
	virtual_output_iteratorTest();
	virtual ~virtual_output_iteratorTest();
	void testDefaultConstruct();
	void testTemplateConstruct();
	void testCopyConstruct();
	void testAssign();
	void testInt();
	void testString();
};
#endif

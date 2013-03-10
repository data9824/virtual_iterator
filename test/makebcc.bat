mkdir bcc
bcc32 -I%CPPUNIT%\include -nbcc -DCPPUNIT_FUNC_STRING_COMPARE_STRING_FIRST=0 -c %CPPUNIT%\src\cppunit\*.cpp
bcc32 -I%CPPUNIT%\include -nbcc -c *.cpp
bcc32 -nbcc -etest.exe bcc\*.obj

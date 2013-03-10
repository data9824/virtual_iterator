mkdir vc
cl -I%CPPUNIT%\include -Fovc\ -GX -GR -c -DCPPUNIT_FUNC_STRING_COMPARE_STRING_FIRST=0 %CPPUNIT%\src\cppunit\*.cpp
cl -I%CPPUNIT%\include -Fovc\ -GX -GR -c *.cpp
link /out:vc\test.exe vc\*.obj

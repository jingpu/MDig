set OCV=/nobackup/jingpu/tools/opencv-2.4.11/install
set PTOOLS=/nobackup/jingpu/tools/PicklingTools153Release/C++

if ( ($?LD_LIBRARY_PATH) ) then
    setenv LD_LIBRARY_PATH ${PTOOLS}:${OCV}/lib:${LD_LIBRARY_PATH}
else
    setenv LD_LIBRARY_PATH ${PTOOLS}:${OCV}/lib
endif

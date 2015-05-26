set OCV=/nobackup/jingpu/tools/opencv-2.4.11/install

if ( ($?LD_LIBRARY_PATH) ) then
    setenv LD_LIBRARY_PATH $OCV/lib:$LD_LIBRARY_PATH
else
    setenv LD_LIBRARY_PATH $OCV/lib
endif

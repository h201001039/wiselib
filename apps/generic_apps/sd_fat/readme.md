
In this I have made 2 classes.First one is SdFileSystemLibrary class and other one is File class.
In SdFileSystemLibrary class I have included the functions which deals with accessing the SD 
card and manipulating directories.In File class I have include all the functions which are
related to operations on individual file. 

I have checked it on my pc having linux platform by g++ and also on arduino board. 
It is successfully compiling and running on both.

For testing in Arduino I just combine the File.cpp, sd_filesys_lib.cpp, and 
sd_filesys_lib.h into one file sd_demo_arduino_code. I have tested the output
by printing return values of different functions by using Serial.println() in
setup() method.


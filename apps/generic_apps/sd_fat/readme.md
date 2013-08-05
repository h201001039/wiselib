
-> open()

Description

open a file on SD card. A file can be open in read mode or write mode. If user want to open file in read mode he/she must provide 0
in mode and if he want to open file in write mode he/she should provide 1 in mode. If user is opening file in read mode and file 
exists, File object will return. If user is opening file in write mode and file exists, the original contents disappear,
and get an empty file, pointer at beginning. If the file dont exist a new file will be created. If user will not define any mode,
it will treated read mode by default.

Syntax

wiselib::File<Os> x=f.open("soa.txt"); 	 // open file in read mode.
wiselib::File<Os> x=f.open("soa.txt",0); // open file in read mode.
wiselib::File<Os> x=f.open("soa.txt",1); // open file in write mode.

Returns

A file object referring to the opened file.

-> f_read()

Description

read a file on SD card. User can read a file if he/she opens a file in read mode.

Syntax

len=x.f_read(buffer,len1); // buffer is the block_data_t array in which data from the file will be read. len1 is length user want to read.

Returns

Number of bytes read by the method

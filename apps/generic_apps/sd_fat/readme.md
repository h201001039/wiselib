<h3>-> open()</h3>

<h4>Description</h4>

open a file on SD card. A file can be open in read mode or write mode. If user want to open file in read mode he/she must provide 0
in mode and if he want to open file in write mode he/she should provide 1 in mode. If user is opening file in read mode and file 
exists, File object will return. If user is opening file in write mode and file exists, the original contents disappear,
and get an empty file, pointer at beginning. If the file dont exist a new file will be created. If user will not define any mode,
it will treated read mode by default.

<h4>Syntax</h4>

* wiselib::File<Os> x=f.open("soa.txt"); 	 // open file in read mode.
* wiselib::File<Os> x=f.open("soa.txt",0); // open file in read mode.
* wiselib::File<Os> x=f.open("soa.txt",1); // open file in write mode.

<h4>Returns</h4>

A file object referring to the opened file.

<h3>-> f_read()</h3>

<h4>Description</h4>

read a file on SD card. User can read a file if he/she opens a file in read mode.

<h4>Syntax</h4>

len=x.f_read(buffer,len1); // buffer is the block_data_t array in which data from the file will be read. len1 is length user want to read.

<h4>Returns</h4>

Number of bytes read by the method

<h3>-> f_write()</h3>

<h4>Description</h4>

write a file on SD card. User can read a file if he/she opens a file in write mode.

<h4>Syntax</h4>

len=x.f_write(buffer,len1); // buffer is the block_data_t array in which data is there which will get written in file. 
len1 is length user want to write.

<h4>Returns</h4>

Number of bytes written by the method

<h3>-> f_delete()</h3>

<h4>Description</h4>

delete a file on SD card if the file exist otherwise return ERR_UNSPEC.

<h4>Syntax</h4>

res=f.delete_file(file_name); // file_name is the name of the file user wants to delete.

<h4>Returns</h4>

Return SUCCESS if file exist in SD card is get deleted, otherwise return ERR_UNSPEC.

<h3>-> seek()</h3>

<h4>Description</h4>

Seek to a new position in the file, which must be between 0 and the size of the file (inclusive). 

<h4>Syntax</h4>

len=x.seek(value); // seek the file pointer to a specific value between 0 and file_size.

<h4>Returns</h4>

Return SUCCESS if value is between 0 to file_size. Other wise seek function will return ERR_UNSPEC.

<h3>-> close()</h3>

<h4>Description</h4>

Close the file

<h4>Syntax</h4>

a.f_close(); //where a is File object.

<h4>Returns</h4>

Return SUCCESS if file closes successfully. Other wise seek function will return ERR_UNSPEC.

<h3>-> exist()</h3>

<h4>Description</h4>

Tests whether a file exists on the SD card. 

<h4>Syntax</h4>

f.exist(); //where f is FATFileSystem object.

<h4>Returns</h4>

Return true if file exists otherwise return false.

<h3>-> position()</h3>

<h4>Description</h4>

Get the current position within the file

<h4>Syntax</h4>

x.position() //where x is File object.

<h4>Returns</h4>

Return the position within the file (unsigned long) .

<h3>-> available()</h3>

<h4>Description</h4>

Check if there are any bytes available for reading from the file. 

<h4>Syntax</h4>

x.available() //where x is File object.

<h4>Returns</h4>

Return true if bytes available for reading otherwise false.

<h3>-> size()</h3>

<h4>Description</h4>

Get the size of the file.

<h4>Syntax</h4>

x.size() //where x is File object.

<h4>Returns</h4>

the size of the file in bytes (unsigned long).

<h3>-> fat_type()</h3>

<h4>Description</h4>

Give the information about the 

FAT file system in SD card, whether it is 16 bit or 32 bit.

<h4>Syntax</h4>

val=f.fat_type() //f is the FATFileSystem object.

<h4>Returns</h4>
Returns 16 if the FAT type is 16 bit , return 32 if FAT type is 32 bit.

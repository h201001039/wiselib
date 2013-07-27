#include <iostream>
#include "sd_filesys_lib.h"
#include "/home/mindfuck/wiselib/wiselib.testing/external_interface/pc/pc_os_model.h"
//namespace wiselib
//{
typedef wiselib::PCOsModel Os;
int main()
{
	std::cout  << std::endl;
	wiselib::SdFileSystemLibrary<Os> sd;
	wiselib::File<Os> f=sd.open("harsh",1);
	std::cout << "Name of created file is " << f.name() <<std::endl;
	std::cout << "Size of file harsh is "<<f.size()  << std::endl;
	wiselib::File<Os> *x=f.open_next_file(1);
	if((x->is_directory()))
	std::cout << "next file of harsh is a directory "  << std::endl;
	else 
	std::cout << "next file of harsh is not a directory "  << std::endl;
	std::cout << "... "  << std::endl;
	std::cout << "... "  << std::endl;
	return 0;

}
//}

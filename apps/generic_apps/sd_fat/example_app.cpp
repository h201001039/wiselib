#define pc
#define USE_FILE_BLOCK_MEMORY 1
/*
* Simple Wiselib Example
*/
#include "external_interface/external_interface.h"
#include "algorithms/routing/tree/tree_routing.h"
#include "external_interface/pc/pc_os_model.h"
#include "external_interface/pc/pc_wiselib_application.h"
#include "external_interface/external_interface.h"
#include "algorithms/file_system/FATFileSystem.h"

typedef wiselib::PCOsModel Os;
typedef typename Os::block_data_t block_data_t;
block_data_t buffer[1024]="gsdsdgsgsdgsdgd";
block_data_t buffer1[1024];

class ExampleApplication
{
   public:
      void init( Os::AppMainParameter& value )
      {
		int len,i,len1;
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );
		debug_->debug( "Reading BIOS Stuff from SD card!" );
		//typedef wiselib::ArduinoOsModel Os;
		wiselib::FATFileSystem<Os> f;
		f.init("/home/mindfuck/Downloads/hello.img");
			//f.delete_file("harsh");
			//res=f.delete_file("some.txt");
			//if(res==0)
			//debug_->debug("succesfully deleted\n");
			//else
			//debug_->debug("file you want to delete does not exist\n");
		wiselib::File<Os> a=f.open("soa.txt",1);
		len1=a.write(buffer,50);
		if(len1<0)
		debug_->debug("error in writing\n");
		a.write(buffer,50);
		if(len1<0)
		debug_->debug("error in writing\n");
		a.write(buffer,50);
		if(len1<0)
		debug_->debug("error in writing\n");		
		a.write(buffer,50);
		if(len1<0)
		debug_->debug("error in writing\n");
		a.write(buffer,50);
		if(len1<0)
		debug_->debug("error in writing\n");
		a.close();
		wiselib::File<Os> x=f.open("soa.txt",0);
		printf("FTYPE=%d\n",f.fat_type());	
		if(x.exist())
		{
		len=x.read(buffer1,103);
		printf("hello harsh len=%d\n",len);
		if(len>0)
		{
			for(i=0;i<len;i++)
			{
				printf("%c",buffer1[i]);
				}
				printf("\n");
		 debug_->debug("reading %d from the file\n",len);
			 debug_->debug("%s\n",buffer1);
		 }
		 else if(len==-2)
		 debug_->debug("you are at end of the file\n");
		len=x.seek(210);
		if(len==-1)
		debug_->debug("you are seeking to a ptr which is greater than file size or less than 0\n");
		len=x.read(buffer1,200);
		if(len>0)
		{

		 debug_->debug("reading %d from the file\n",len);
		debug_->debug("%s\n",buffer1);
		 }
		 else if(len==-2)
		 debug_->debug("you are at end of the file\n");
		 }
		 else
		 debug_->debug("file you want to read dont exist\n");
		  }
			 
		// --------------------------------------------------------------------
		   private:
			  Os::Debug::self_pointer_t debug_;
};
		// --------------------------------------------------------------------------
		wiselib::WiselibApplication<Os, ExampleApplication> example_app;
		// --------------------------------------------------------------------------
		void application_main( Os::AppMainParameter& value )
		{
		 example_app.init( value );
		}

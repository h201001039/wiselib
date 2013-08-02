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
block_data_t buffer[5000];
class ExampleApplication
{
   public:
      void init( Os::AppMainParameter& value )
      {
		int len;
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );
		debug_->debug( "Reading BIOS Stuff from SD card!" );
		//typedef wiselib::ArduinoOsModel Os;
		wiselib::FATFileSystem<Os> f;
		f.init("/home/mindfuck/disk123.img");
			//f.delete_file("harsh");
			//	f.delete_file("wooo");
		wiselib::File<Os> x=f.open("wooo");
		printf("FTYPE=%d\n",f.fat_type());
	
		if(x.exist())
		{
		len=x.read(buffer,5000);
		if(len>0)
		{
		 debug_->debug("reading %d from the file\n",len);
			 debug_->debug("%s\n",buffer);
		 }
		 else if(len==-2)
		 debug_->debug("you are at end of the file\n");
		len=x.read(buffer,10);
		if(len>0)
		{

		 debug_->debug("reading %d from the file\n",len);
		debug_->debug("%s\n",buffer);
		 }
		 else if(len==-2)
		 debug_->debug("you are at end of the file\n");
		 }
		 else
		 debug_->debug("file dont exist\n");
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

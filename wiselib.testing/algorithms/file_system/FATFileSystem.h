	#include <stdint.h>
	#include <stddef.h>
	//#include "file_block_memory.h"
	#define IsUpper(c) (((c)>='A')&&((c)<='Z'))
	#define IsLower(c) (((c)>='a')&&((c)<='z'))
	#define LD_INT(ptr) (unsigned int)(*(unsigned int*)(unsigned char*)(ptr))
	#define LD_LONG(ptr) (unsigned long)(*(unsigned long*)(unsigned char*)(ptr))
	#define LD_SHORT(ptr) (unsigned short)(*(unsigned short*)(unsigned char*)(ptr))
	#define LD_CLUST(dir) LD_SHORT(dir+26)
	#define	ST_SHORT(ptr,val)	*(unsigned char*)(ptr)=(unsigned char)(val); *((unsigned char*)(ptr)+1)=(unsigned char)((unsigned short)(val)>>8)
	#define	ST_INT(ptr,val)	*(unsigned char*)(ptr)=(unsigned char)(val); *((unsigned char*)(ptr)+1)=(unsigned char)((unsigned short)(val)>>8); *((unsigned char*)(ptr)+2)=(unsigned char)((unsigned int)(val)>>16); *((unsigned char*)(ptr)+3)=(unsigned char)((unsigned int)(val)>>24)
	//#define	ST_SHORT(ptr,val)	*(unsigned short*)(unsigned char*)(ptr)=(unsigned short)(val)
	//#define	ST_INT(ptr,val)	*(unsigned int*)(unsigned char*)(ptr)=(unsigned int)(val)
	
	#ifndef __SD_FILE_SYSTEM_LIBRARY_H__
	#define __SD_FILE_SYSTEM_LIBRARY_H__
	//#include "pc_os_model.h"
	namespace wiselib
	{
	unsigned char fs_type; 		/* FAT sub type */
	unsigned char csize; 		/* Number of sectors per cluster */
	unsigned int n_rootdir; 	/* Number of root directory entries (0 on FAT32) */
	unsigned long n_fatent; 	/* Number of FAT entries (= number of clusters + 2) */
	unsigned long fatbase; 		/* FAT start sector */
	unsigned long dirbase; 		/* Root directory start sector (Cluster# on FAT32) */
	unsigned long database; 	/* Data start sector */
	unsigned long bps;			/*bytes per sectors  */
	unsigned long RootDirSectors;
	unsigned int FATSz;
	unsigned int FirstDataSector;
	unsigned int TotSec;
	unsigned long DataSec;
	unsigned long CountofClusters;
	unsigned int BPB_TotSec16;
	unsigned int BPB_NumFATs;
	unsigned int BPB_SecPerClus;
	unsigned int FATOffset;
	unsigned int ThisFATSecNum;
	unsigned int ThisFATEntOffset;
	unsigned long FirstRootDirSecNum;
	unsigned long max_clust;
	
	template<typename OsModel_P,typename BlockMemory_P = typename OsModel_P::BlockMemory>
	class File
	{
	    public:
	                typedef OsModel_P OsModel;
	                typedef typename OsModel::size_t size_t;
	                typedef typename OsModel::block_data_t block_data_t;
	                typedef BlockMemory_P BlockMemory;
	                unsigned long fptr; 		/* File R/W pointer */
	                unsigned long fsize; 		/* File size */
	                 long org_clust; 			/* File start cluster */
	                unsigned long curr_clust; 	/* File current cluster */
	                unsigned long dsect; 		/* File current data sector */
	                unsigned char flag; 		/* File status flags */
	                unsigned int sect3;
	                block_data_t *buffer4;
	                block_data_t *dir3;
	                enum {
						SUCCESS = OsModel::SUCCESS,
						ERR_UNSPEC = OsModel::ERR_UNSPEC
						};
	
	//------------------------------------------------------------------------
	                File(char* n, unsigned int cluster_no, BlockMemory& bm) : fd(bm) 
	                {
	                int i=0;
	                while(i<12 && n[i]!=0)
	                 {
	                        name_[i]=n[i];
	                        i++;
	                 }
	                name_[i]='\0';
	                file_cluster_no=cluster_no;
	                }
	
	//------------------------------------------------------------------------
	                File(BlockMemory& bm) : fd(bm) 
	                {
	                name_[0]=0;
	                }
	
	//------------------------------------------------------------------------
	                
	/* Fill memory */
	
					void mem_set (block_data_t* dst, block_data_t *src, int cnt) {
					while(cnt--)
					{
					*dst=*src;
					dst++;
					src++;
					}
					}
	//-----------------------------------------------------------------------
	
					int exist()
					{
					if(org_clust==-2)
					return 0;
					else return 1;
					}
	
	//-------------------------------------------------------------------------------------
					/* Compare memory to memory */
	
					int mem_cmp (const void* dst, const void* src, int cnt) {
					const char *d = (const char *)dst, *s = (const char *)src;
					int r = 0;
					while (cnt-- && (r = *d++ - *s++) == 0) ;
					//printf("r=%d\n",r);
					return r;
					}
	//-----------------------------------------------------------------------
	                        //Given any valid data cluster number N, the sector number of
	                        //the first sector of that cluster (again relative to sector 0 of the FAT volume)
	                        //is computed as follows:
	                unsigned int first_sector_of_cluster(unsigned int N )
	                {
					//printf("database=%ld\n",database);
	                return ((N - 2) * csize) + database;
	                }
	//------------------------------------------------------------------------
	                 unsigned int fat_type()
	                        {
	                                if(CountofClusters < 4085) {
	                                /* Volume is FAT12 */
	                                return 12;
	                                }
	                                else if(CountofClusters < 65525) {
	                                /* Volume is FAT16 */
	                                return 16;
	                                }
	                                else {
	                                /* Volume is FAT32 */
	                                return 32;
	                                }
	 
	                        }
	
	//-----------------------------------------------------------------------
	 
	                unsigned int get_fat_entry(int N)
	                {
	                   block_data_t buffer123[512];
	                if(fat_type() == 16)
	                        FATOffset = N * 2;
	                else if (fat_type() == 32)
	                        FATOffset = N * 4;
	 
	                 ThisFATSecNum = fatbase + (FATOffset / bps);
	                 ThisFATEntOffset = FATOffset % bps;
	                //printf("buffer123=%u\n",buffer123);
	                fd.read(buffer123,ThisFATSecNum); 
	                //printf("buffer123=%u\n",buffer123);
	                if(fat_type()==16)
	                return LD_SHORT(buffer123+ThisFATEntOffset);//change into word. eg 16 bit or 32 bit.
	                else if (fat_type() == 32)
	                {
	                return  LD_INT(buffer123+ThisFATEntOffset) & 0x0FFFFFFF;
				}
			else 
				return ERR_UNSPEC;	
	//-----------------------------------------------------------------------
	                }
	                void set_fat_entry(int N,unsigned int value)
	                {
						block_data_t buffer12[512];
						if(fat_type() == 16)
	                        FATOffset = N * 2;
						else if (fat_type() == 32)
	                        FATOffset = N * 4;
					//printf("buffer12====%u\n",buffer12);
	                 ThisFATSecNum = fatbase + (FATOffset / bps);
	                 ThisFATEntOffset = FATOffset % bps;
	                fd.read(buffer12,ThisFATSecNum);
	                //buffer12= ThisFATSecNum;
	                //printf("buffer12====%u\n",buffer12);
	                if(fat_type()==16)
	                {
	                //buffer12[ThisFATEntOffset]=buffer12[ThisFATEntOffset] &0x0000;
	                //buffer12[ThisFATEntOffset+1]=buffer12[ThisFATEntOffset+1] &0x0000;
	                //buffer12[ThisFATEntOffset]=value;
	                //buffer12[ThisFATEntOffset+1]=value>>8;
	                //*(unsigned short*)(unsigned char*)(buffer12+ThisFATEntOffset)=(unsigned short)(value);
	                ST_SHORT(buffer12+ThisFATEntOffset,value);
	               // printf("buffer12[ThisFATEntOffset]=%d\n",buffer12[ThisFATEntOffset]);
	                fd.write(buffer12,ThisFATSecNum);
					fd.read(buffer12,ThisFATSecNum);
				//	printf("buffer12[ThisFATEntOffset]=%d\n",buffer12[ThisFATEntOffset]);
					
				}
	                else if (fat_type() == 32)
	                {
						value=value& 0x0FFFFFFF;
						//printf("%d\n",LD_INT(buffer12+ThisFATEntOffset) & 0x0FFFFFFF);
					*(unsigned int*)(unsigned char*)(buffer12+ThisFATEntOffset)=*(unsigned int*)(unsigned char*)(buffer12+ThisFATEntOffset)&0xF0000000;
	                //buffer12[ThisFATEntOffset]=buffer12[ThisFATEntOffset] &0xF0;
	                *(unsigned int*)(unsigned char*)(buffer12+ThisFATEntOffset)=*(unsigned int*)(unsigned char*)(buffer12+ThisFATEntOffset)|value;
	                //buffer12[ThisFATEntOffset]=buffer12[ThisFATEntOffset] | value;
					//buffer12[ThisFATEntOffset+1]=buffer12[ThisFATEntOffset+1] | value>>8;
					//buffer12[ThisFATEntOffset+2]=buffer12[ThisFATEntOffset+2] | value>>16;
					//buffer12[ThisFATEntOffset+3]=buffer12[ThisFATEntOffset+3] | value>>24;
					fd.write(buffer12,ThisFATSecNum);
				}
				
					fd.write(buffer12,ThisFATSecNum);
	
	                }
	                
	//----------------------------------------------------------------------------   
					int remove_clust_chain (
						unsigned int clust			/* Cluster# to remove chain from */
					)
					{
						unsigned int nxt;
	
						while ((nxt = get_fat_entry(clust)) >= 2) {
							//printf("get_fat_entry(%d)=%d\n",clust,get_fat_entry(clust));
							set_fat_entry(clust, 0) ;
							clust = nxt;
						}
						//printf("get_fat_entry(%d)=%d\n",clust,get_fat_entry(clust));
						if(get_fat_entry(clust) == 0x0000)
						set_fat_entry(clust, 0);
						return SUCCESS;
					}
	//----------------------------------------------------------------------------
					/* Stretch or Create a Cluster Chain */
	
					
					unsigned int create_clust_chain (
						unsigned int clust			 /* Cluster# to stretch, 0 means create new */
					)
					{
						unsigned int ncl, ccl, mcl =max_clust;
	
	
						if (clust == 0) {	/* Create new chain */
							ncl = 1;
							do {
								ncl++;						
								if (ncl >= mcl) return 0;	
								ccl = get_fat_entry(ncl);		
								if (ccl == 1) return 0;		
							} while (ccl);				
						}
						else {				
							ncl = get_fat_entry(clust);	
							if (ncl < 2) return 0;		
							if (ncl < mcl) return ncl;	
							ncl = clust;				
							do {
								ncl++;						
								if (ncl >= mcl) ncl = 2;	
								if (ncl == clust) return 0;	
								ccl = get_fat_entry(ncl);	
								if (ccl == 1) return 0;		
							} while (ccl);				
						}
	
						set_fat_entry(ncl, 0xFFFFFFFF);		
						set_fat_entry(clust, ncl);	
	
						return ncl;
					}
	
	//----------------------------------------------------------------------------
					long f_read (
					block_data_t* buff,
					int btr
					)
					{
					int dr;
					int clst;
					long sect, remain;
					int rcnt,ret;
					block_data_t cs;
					block_data_t inter[bps];
					//printf("fsize=%ld,fptr=%ld\n",fsize,fptr);
					//fptr=0;
					remain = fsize - fptr;
					while(remain<=0)
					return -2;
					if (btr > remain) btr = (int)remain; /* Truncate btr by remaining bytes */
					ret=btr;
					while (btr) 
					{
					//printf("hello dude\n");
					//printf("fptr=%ld\n",fptr);
					if ((fptr % bps) == 0) {			/* On the sector boundary? */
					cs = (unsigned char)(fptr / bps & (csize - 1));
					//printf("cs=%ld\n",cs);
					if (!cs) {
					clst = (fptr == 0) ? org_clust : get_fat_entry(curr_clust);
					//printf("clst in read=%d\n",clst);
					//printf("clst=%d\n",clst);
					if (clst <= 1) return ERR_UNSPEC;
					curr_clust = clst;
					}
					sect = first_sector_of_cluster(curr_clust);
					//printf("sect in read=%ld\n",sect);
					if (!sect) return ERR_UNSPEC;
					dsect = sect + cs;
					}
					//printf("dsect=%ld\n",dsect);
					rcnt = (unsigned int)(bps - (fptr % bps));
					if (rcnt > btr) rcnt = btr;
					dr = fd.read(inter, dsect);
					mem_set(buff,inter+(fptr%bps),rcnt);
					if (dr) return ERR_UNSPEC;
					fptr += rcnt; buff += rcnt;		/* Update pointers and counters */
					btr -= rcnt;
					}
					*buff=0;
					return ret;
					}
	//----------------------------------------------------------------------------------------
	
	unsigned long f_write (
		block_data_t *buff,	
		unsigned long btw	
	)
	{
		//FATFileSystem<OsModel> fat();
		unsigned int sect1_clust=1,i,ret1;
		unsigned long clust;
		unsigned int sect=dsect;
		unsigned short wcnt;
		block_data_t cc;
		block_data_t buffer5[512];
		//FATFS *fs = FatFs;
		//printf("btw=%ld\n",btw);
		fsize=fsize+btw;
		ret1=btw;
		//printf("\n");
		//for(i=0;i<32;i++)
		//printf("%c ",dir3[i]);
		//printf("\n");
		ST_INT(dir3+28, fsize);
		//printf("\n");
		//for(i=0;i<32;i++)
		//printf("%x ",dir3[i]);
		printf(" ");
		//fd.write(buffer4,sect3);
		if (fsize + btw < fsize) btw = 0;			
		
		for ( ;  btw;buff += wcnt, fptr += wcnt,  btw -= wcnt) {
				//printf("fptr=%ld\n",fptr);
			if ((fptr & 511) == 0) {
				//printf("HELLO FPTR=0\n");							/* On the sector boundary */
				if (--(sect1_clust)) {				
					sect = dsect + 1;				
				} else {								
					if (fptr == 0) {				/* Top of the file */
					
						clust = org_clust;
						if (clust == 0)					
							org_clust = clust = create_clust_chain(0);	
						//			printf("CLUST=%ld\n",clust);
					} else {							
						clust = create_clust_chain(curr_clust);		
					}
					if ((clust < 2) || (clust >= max_clust)) break;
					curr_clust = clust;				
					sect = first_sector_of_cluster(clust);			
							//	printf("sect=%ld\n",sect);
					sect1_clust = csize;	
				}
				dsect = sect;					
				cc = btw / 512;							
				//printf("cc=%ld\n",cc);
				if (cc) {								
					if (cc > sect1_clust) cc = sect1_clust;
					for(i=cc;i>0;i--)
					{
					 fd.write(buff, sect); 
					sect++;
					}
					sect1_clust -= cc - 1;
					dsect += cc - 1;
					wcnt = cc * 512; 
					//printf("wcnt=%ld\n",wcnt);
					continue;
					
				}
				if ((fptr < fsize))  			
					fd.read(buffer5, sect);
			}
			wcnt = 512 - (fptr & 511);				
			if (wcnt > btw) wcnt = btw;
			mem_set(&buffer5[fptr & 511], buff, wcnt);
					fd.write(buffer5,sect);
	
		}
						ST_SHORT(dir3+26,org_clust);
						fd.write(buffer4,sect3);
						return ret1;
	
	}
	//----------------------------------------------------------------------------------------
			int f_close()
			{
			fptr=0;	
			return SUCCESS;
			} 
	
	//----------------------------------------------------------------------------------------
			int seek(unsigned int val)
			{
				if(val>fsize || val<0)
				return ERR_UNSPEC;
			else fptr=val;	
			return SUCCESS;
			} 
	
	//----------------------------------------------------------------------------------------
	
			unsigned long size()
			{
			
			return fsize;
			} 
	
	//----------------------------------------------------------------------------------------
	
			unsigned long position()
			{
			
			return fptr;
			} 
	
	//----------------------------------------------------------------------------------------
	
			bool available()
			{
			
			if(fptr<fsize)
			return true;
			else return false;
			} 
	
	//----------------------------------------------------------------------------------------
	        private:
	                char name_[13];
	                int file_open_mode;
	                BlockMemory& fd;
	                unsigned int file_cluster_no;
	                                
	};
	
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	
	template<typename OsModel_P,typename BlockMemory_P = typename OsModel_P::BlockMemory>
	class FATFileSystem
	{
	        public:
	                typedef OsModel_P OsModel;
	                typedef typename OsModel::size_t size_t;
	                typedef typename OsModel::block_data_t block_data_t;
	                typedef BlockMemory_P BlockMemory;
	                enum {
	                                BLOCK_SIZE = 512,
	                                SIZE = (1024UL * 1024UL * 1024UL / 512UL), ///< #blocks for 1GB
	                        };
	                        
	                        enum {
	                                SUCCESS = OsModel::SUCCESS,
	                                ERR_UNSPEC = OsModel::ERR_UNSPEC
	                        };
	                        
	                        typedef struct BPB_P
	         {
	                 unsigned char BS_jmpBoot[3];
	                 unsigned char BS_OEMName[8];
	                 unsigned short BPB_BytsPerSec;
	                 unsigned char BPB_SecPerClus;
	                 unsigned short BPB_RsvdSecCnt;
	                 unsigned char BPB_NumFATs;
	                 unsigned short BPB_RootEntCnt;
	                 unsigned short BPB_TotSec16;
	                 unsigned char BPB_Media;
	                 unsigned short BPB_FATSz16;
	                 unsigned short BPB_SecPerTrk;
	                 unsigned short BPB_NumHeads;
	                 unsigned int BPB_HiddSec;
	                 unsigned int BPB_TotSec32;
	                 } BPB;
	                 
	                typedef struct BS_16_P
	                {
	                unsigned char BS_DrvNum;
	                unsigned char BS_Reserved1;
	                unsigned char BS_BootSig;
	                unsigned int BS_VolID;
	                unsigned char BS_VolLab[11];
	                unsigned char BS_FilSysType[8];
	                }BS_16;
	                
	                typedef struct BS_32_P
	                {
	                unsigned int BPB_FATSz32;
	                unsigned short BPB_ExtFlags;
	                unsigned short BPB_FSVer;
	                unsigned long BPB_RootClus;
	                unsigned short BPB_FSInfo;
	                unsigned short BPB_BkBootSec;
	                unsigned char BPB_Reserved[12];
	                unsigned char BS_DrvNum;
	                unsigned char BS_Reserved1;
	                unsigned char BS_BootSig;
	                unsigned int BS_VolID;
	                unsigned char BS_VolLab[11];
	                unsigned char BS_FilSysType[8];
	                }BS_32;
	                        
	                unsigned int fat_type()
	                        {
	                                if(CountofClusters < 4085) {
	                                /* Volume is FAT12 */
	                                return 12;
	                                }
	                                else if(CountofClusters < 65525) {
	                                /* Volume is FAT16 */
	                                return 16;
	                                }
	                                else {
	                                /* Volume is FAT32 */
	                                return 32;
	                                }
	 
	                        }
	//-----------------------------------------------------------------------
					/* Fill memory */
	
					void mem_set (void* dst, int val, int cnt) {
					char *d = (char*)dst;
					while (cnt--) *d++ = (char)val;
					}
	
	//-----------------------------------------------------------------------
					/* Compare memory to memory */
	
					int mem_cmp (const void* dst, const void* src, int cnt) {
					const char *d = (const char *)dst, *s = (const char *)src;
					int r = 0;
					while (cnt-- && (r = *d++ - *s++) == 0) ;
					//printf("r=%d\n",r);
					return r;
					}
	//-----------------------------------------------------------------------
	
					/* length */
	
					int len(const char *name)
					{
					int i=0;
					while(*name!=0)
					{
					i++;
					name++;
					}
					return i;
					}
	//-----------------------------------------------------------------------
					char upper(char c) {
					if (c >= 'a' && c <= 'z') {
					return c + ('A' - 'a');
					} else {
					return c;
					}
					}
	//-----------------------------------------------------------------------
					char *mystrchr(const char *s, int c) {
					while (*s != (char) c) {
					if (!*s++) {
					return NULL;
					}
					}
					return (char *)s;
					}
	//-----------------------------------------------------------------------
	
					void tf_shorten_filename(char *dest,const char *src) {
					//char *dest1=dest;
					int i = 0;
					while (1) {
					if (i == 8)
					break;
					if ((i == 6) || (*src == '.') || (*src == 0))
					break;
					if ((*dest == ' ')) {
	
					}
					else
					{
					*(dest++) = upper(*(src++));
					}
					i += 1;
					}
					if (i == 6) {
					*(dest++) = '~';
					*(dest++) = '1';
					i += 2;
					}
					else {
					while (i < 8) {
					*(dest++) = ' ';
					i++;
					}
					}
	
	
					src = mystrchr(src, '.');
	
					*(dest++) = ' ';
					*(dest++) = ' ';
					*(dest++) = ' ';
					*(dest++) = '\x00';
					dest -= 4;
					if (src != NULL) {
					src += 1;
					while (i < 12) {
					if (*src == '\x00')
					break;
					*(dest++) = upper(*(src++));
					i += 1;
					}
					}
					}
	//-------------------------------------------------------------------------------------
									
					void to_upper( char *src)
					{
					while((*src)!=0)
					{
					if(*src >=97 && *src <=122)
					{
	
					*src-=32;
					}
					src++;
					}
					}
	//-------------------------------------------------------------------------------------
	                int init( const char *image=NULL) {
	
	                        CountofClusters=0;
	                        TotSec=0;
	                        DataSec=0;
	                        int i=0;
	                        if(image!=NULL)
	                        {
	                      
	                        i=fd.init(image);
	}
	                        else
	                        i=fd.init();
	                        if(i==ERR_UNSPEC)
	                        return ERR_UNSPEC;
	                        for(i=0;i<512;i++)
	                        buffer[i]=0;
	                         int a=fd.read(buffer,0);
	                        if(a==SUCCESS)
	                        {
	                        printf("succesfull read\n\n");
	                        sclust=0;
	                        b1.BPB_BytsPerSec=LD_INT(buffer+11);
	                        bps=b1.BPB_BytsPerSec;
	                      //  printf("bps=%d\n",bps);
	                        b1.BPB_SecPerClus=buffer[13];
	                        csize=b1.BPB_SecPerClus;
	                        b1.BPB_RsvdSecCnt=LD_INT(buffer+14);;
	                        fatbase=b1.BPB_RsvdSecCnt;
	                        b1.BPB_NumFATs=buffer[16];
	                        b1.BPB_RootEntCnt=LD_INT(buffer+17);
	                        n_rootdir=b1.BPB_RootEntCnt;
	                        b1.BPB_TotSec16=LD_INT(buffer+19);
	                        b1.BPB_FATSz16=LD_INT(buffer+22);
	                        b1.BPB_TotSec32=LD_INT(buffer+32);
	                        b3.BPB_FATSz32=LD_INT(buffer+36);
	                        b3.BPB_RootClus=LD_INT(buffer+44);
	                        if(b1.BPB_FATSz16 != 0)
	                        FATSz = b1.BPB_FATSz16;
	                        else
	                        FATSz = b3.BPB_FATSz32;
	                        int totsec;
	                        if(b1.BPB_TotSec16 != 0)
	                        totsec = b1.BPB_TotSec16;
	                        else
	                        totsec = b1.BPB_TotSec32;
	                        RootDirSectors = ((b1.BPB_RootEntCnt * 32) + (b1.BPB_BytsPerSec - 1)) / b1.BPB_BytsPerSec;
	                        database=fatbase+(b1.BPB_NumFATs*FATSz)+RootDirSectors;
	                        DataSec = totsec - (fatbase + (b1.BPB_NumFATs * FATSz) + RootDirSectors);
	                        CountofClusters = DataSec / csize;
	                        n_fatent=CountofClusters;
	                        if(fat_type()==16)
	                        FirstRootDirSecNum = fatbase + (b1.BPB_NumFATs * FATSz);
	                        else
	                        FirstRootDirSecNum = b3.BPB_RootClus;
	                        dirbase=FirstRootDirSecNum;
	                        max_clust=(totsec-database)/csize+2;
	                        return SUCCESS;
	                        }
	                  else return ERR_UNSPEC;
	                        }
	//-----------------------------------------------------------------------
	                        //Given any valid data cluster number N, the sector number of
	                        //the first sector of that cluster (againrelative to sector 0 of the FAT volume)
	                        //is computed as follows:
	                unsigned int first_sector_of_cluster(unsigned int N )
	                {
	                return ((N - 2) * csize) + database;
	                }
	//-----------------------------------------------------------------------
	 
	                unsigned int get_fat_entry(int N)
	                {
	                   block_data_t buffer123[512];
	                if(fat_type() == 16)
	                        FATOffset = N * 2;
	                else if (fat_type() == 32)
	                        FATOffset = N * 4;
	 
	                 ThisFATSecNum = fatbase + (FATOffset / bps);
	                 ThisFATEntOffset = FATOffset % bps;
	                //printf("buffer123=%u\n",buffer123);
	                fd.read(buffer123,ThisFATSecNum);
	                //printf("buffer123=%u\n",buffer123);
	                if(fat_type()==16)
	                return LD_SHORT(buffer123+ThisFATEntOffset);//change into word. eg 16 bit or 32 bit.
	                else if (fat_type() == 32)
	                {
	                return  LD_INT(buffer123+ThisFATEntOffset) & 0x0FFFFFFF;
				}
			else 
				return ERR_UNSPEC;	
	//-----------------------------------------------------------------------
	                }
	                void set_fat_entry(int N,unsigned int value)
	                {
						block_data_t buffer12[512];
						if(fat_type() == 16)
	                        FATOffset = N * 2;
						else if (fat_type() == 32)
	                        FATOffset = N * 4;
					//printf("buffer12====%u\n",buffer12);
	                 ThisFATSecNum = fatbase + (FATOffset / bps);
	                 ThisFATEntOffset = FATOffset % bps;
	                fd.read(buffer12,ThisFATSecNum);
	                //buffer12= ThisFATSecNum;
	                //printf("buffer12====%u\n",buffer12);
	                if(fat_type()==16)
	                {
	                //buffer12[ThisFATEntOffset]=buffer12[ThisFATEntOffset] &0x0000;
	                //buffer12[ThisFATEntOffset+1]=buffer12[ThisFATEntOffset+1] &0x0000;
	                //buffer12[ThisFATEntOffset]=value;
	                //buffer12[ThisFATEntOffset+1]=value>>8;
	                //*(unsigned short*)(unsigned char*)(buffer12+ThisFATEntOffset)=(unsigned short)(value);
	                ST_SHORT(buffer12+ThisFATEntOffset,value);
	                printf("buffer12[ThisFATEntOffset]=%d\n",buffer12[ThisFATEntOffset]);
	                fd.write(buffer12,ThisFATSecNum);
					fd.read(buffer12,ThisFATSecNum);
					printf("buffer12[ThisFATEntOffset]=%d\n",buffer12[ThisFATEntOffset]);
					
				}
	                else if (fat_type() == 32)
	                {
						value=value& 0x0FFFFFFF;
						//printf("%d\n",LD_INT(buffer12+ThisFATEntOffset) & 0x0FFFFFFF);
					*(unsigned int*)(unsigned char*)(buffer12+ThisFATEntOffset)=*(unsigned int*)(unsigned char*)(buffer12+ThisFATEntOffset)&0xF0000000;
	                //buffer12[ThisFATEntOffset]=buffer12[ThisFATEntOffset] &0xF0;
	                *(unsigned int*)(unsigned char*)(buffer12+ThisFATEntOffset)=*(unsigned int*)(unsigned char*)(buffer12+ThisFATEntOffset)|value;
	                //buffer12[ThisFATEntOffset]=buffer12[ThisFATEntOffset] | value;
					//buffer12[ThisFATEntOffset+1]=buffer12[ThisFATEntOffset+1] | value>>8;
					//buffer12[ThisFATEntOffset+2]=buffer12[ThisFATEntOffset+2] | value>>16;
					//buffer12[ThisFATEntOffset+3]=buffer12[ThisFATEntOffset+3] | value>>24;
					fd.write(buffer12,ThisFATSecNum);
				}
				
					fd.write(buffer12,ThisFATSecNum);
	
	                }
	                
	//----------------------------------------------------------------------------   
					int remove_clust_chain (
						unsigned int clust			/* Cluster# to remove chain from */
					)
					{
						unsigned int nxt;
	
						while ((nxt = get_fat_entry(clust)) >= 2) {
							//printf("get_fat_entry(%d)=%d\n",clust,get_fat_entry(clust));
							set_fat_entry(clust, 0) ;
							clust = nxt;
						}
						//printf("get_fat_entry(%d)=%d\n",clust,get_fat_entry(clust));
						if(get_fat_entry(clust) == 0x0000)
						set_fat_entry(clust, 0);
						return SUCCESS;
					}
	//----------------------------------------------------------------------------
					/* Stretch or Create a Cluster Chain */
	
					static
					unsigned int create_clust_chain (
						unsigned int clust			 /* Cluster# to stretch, 0 means create new */
					)
					{
						unsigned int ncl, ccl, mcl =max_clust;
	
	
						if (clust == 0) {	/* Create new chain */
							ncl = 1;
							do {
								ncl++;						
								if (ncl >= mcl) return 0;	
								ccl = get_fat_entry(ncl);		
								if (ccl == 1) return 0;		
							} while (ccl);				
						}
						else {				
							ncl = get_fat_entry(clust);	
							if (ncl < 2) return 0;		
							if (ncl < mcl) return ncl;	
							ncl = clust;				
							do {
								ncl++;						
								if (ncl >= mcl) ncl = 2;	
								if (ncl == clust) return 0;	
								ccl = get_fat_entry(ncl);	
								if (ccl == 1) return 0;		
							} while (ccl);				
						}
	
						if (!set_fat_entry(ncl, 0xFFFFFFFF)) return 0;		
						if (clust && !set_fat_entry(clust, ncl)) return 0;	
	
						return ncl;
					}
	
	//----------------------------------------------------------------------------
								 
									/* Directory handling - Move directory index next */
	
					int dir_next ()
					{
					unsigned long clst;
					unsigned short i;
					//block_data_t buffer1[512];
					i = index + 1;
					if (!i || !sect)
					return 0;
					int mod=(bps/32);
					if (!(i % mod)) { /* Sector changed? */
					sect++; /* Next sector */
	
					if (clust == 0) {
					if (i >= n_rootdir)
					return 0;
					}
					else {
					if (((i / mod) & (csize-1)) == 0) { /* Cluster changed? */
					clst = get_fat_entry(clust);
					if (clst <= 1) return 0;
					if (clst >= n_fatent)
					return 0;
					clust = clst;
					sect = first_sector_of_cluster(clst);
					}
					}
					}
	
					index = i;
					return 1;
					}
	//----------------------------------------------------------------------------
	
					/* Directory handling - Rewind directory index */
					int dir_rewind ()
					{
					unsigned long clst;//may be need change
	
	
					index = 0;
					clst = sclust;
					//printf("sclst=%ld\n",clst);
					if (clst == 1 || clst >= n_fatent) /* Check start cluster range */
					{
					return ERR_UNSPEC;//change
					}
					if ( !clst && fat_type()==32)//need changeds
					/* Replace cluster# 0 with root cluster# if in FAT32 */
					clst = (unsigned long)dirbase;
					clust = clst; /* Current cluster */
					sect = clst ? first_sector_of_cluster(clst) : dirbase; /* Current sector */
					//printf("clsusterin rewind=%ld\n",clst);
					return 1;
					}
	
	//-------------------------------------------------------------------------------------
					/* Directory handling - Find an object in the directory */
	
					int dir_find (char *name,int len)
					{
					//printf("Hi wooo\n");
					int res,c,flag1=0;
	
					res = dir_rewind(); /* Rewind directory object */
					//printf("res=%d\n",res);
					if (res != 1) return res;
	
					do {
					flag1=0;
					//printf("sect in find=%ld\n",sect);
					res=fd.read(buffer2,sect);
					//printf("buffer2=%u\n",buffer2);
					if(res==ERR_UNSPEC)
					return ERR_UNSPEC;
					dir1 =(buffer2+((index % 16) * 32));
					//printf("dir1=%u\n",dir1);
					c = dir1[0]; /* First character */
					if (c == 0) { break; }
					if(c==0xE5)
					{
					//printf("c=%d\n",c);	
					//printf("continue\n");
					res = dir_next();
					continue;
					}
					
					//for(i=0;i<11;i++)
					//printf("%c",dir1[i]);
					//printf("\n");
	
					if(!mem_cmp(dir1, name, 11))
					{
					//printf("awesome\n");
					flag1=1;
					break;
					}
					res = dir_next(); /* Next entry */
					} while (res == 1);
					if(flag1==1)
					return res;
					else return -2;
					}
	
		//-------------------------------------------------------------------------------------
					/* Directory handling - Find an object in the directory */
	
					int find_free_entry()
					{
					//printf("Hi wooo\n");
					int res,c;
	
					res = dir_rewind(); /* Rewind directory object */
					//printf("res=%d\n",res);
					if (res != 1) return res;
	
					do {
					//printf("sect in find=%ld\n",sect);
					res=fd.read(buffer2,sect);
					//printf("buffer2=%u\n",buffer2);
					if(res==ERR_UNSPEC)
					return ERR_UNSPEC;
					dir1 =(buffer2+((index % 16) * 32));
					//printf("dir1=%u\n",dir1);
					c = dir1[0]; /* First character */
					if (c == 0) { return 1; }
					if(c==0xE5)
					{
					//printf("c=%d\n",c);	
					//printf("continue\n");
					return 1;
					}
					
					res = dir_next(); /* Next entry */
					} while (res == 1);
					return -2;
					}
	
	//--------------------------------------------------------------------------------
					File<OsModel> open( const char* name, int mode=0)
					{
					//search for the file in root entries and if found create file object
					//pointing to that clusture otherwise create new file and return file object
					//pointing to new clusture
					//int i=0;
					int res,res1,i;
					char name1[15]="gffgggfgfgf";
					block_data_t buf;
					tf_shorten_filename(name1,name);
					File<OsModel> f(name1,0, fd);
					if(mode==0)
					{
					
					//for(i=0;i<15;i++)
					//printf("%c",name1[i]);
					//printf("\n");
					dir1=&buf;
					f.flag = 0;
					res=dir_find(name1,11);
					if(res==-2)
					{
					printf("res=%d\n",res);
					f.org_clust=-2;
					return f;
					}
					f.org_clust = LD_CLUST(dir1);
					//printf("f.org_clust=%ld",f.org_clust);
					f.fsize = LD_INT(dir1+28);
					f.fptr = 0;
					f.flag = 1;
	                return f;
					}
					else if(mode==1)
					{
					//tf_shorten_filename(name1,name);
					dir1=&buf;
					//File<OsModel> f(name1,0, fd);
					f.flag = 1;
					res=dir_find(name1,11);
					if(res==-2)
					{
					res1=find_free_entry();
					if(res1==1)
					{
					for(i=0;i<11;i++)
					*(dir1+i)=name1[i];
					*(dir1+i)=15;
					ST_INT(dir1+28, 0);
					ST_SHORT(dir1+26,0);
					fd.write(buffer2,sect);
					f.buffer4=buffer2;
					f.dir3=dir1;
					f.sect3=sect;
					f.org_clust = LD_CLUST(dir1);
					f.fsize = LD_INT(dir1+28);
					f.fptr = 0;
					f.flag = 1;
	                return f;
					}
					else f.org_clust=-3;
					}
					else
					{
					ST_INT(dir1+28, 0);
					ST_SHORT(dir1+26,0);
					fd.write(buffer2,sect);
					f.buffer4=buffer2;
					f.dir3=dir1;
					f.sect3=sect;
					f.org_clust = LD_CLUST(dir1);
					f.fsize = LD_INT(dir1+28);
					f.fptr = 0;
					f.flag = 1;
	                return f;
					}
					return f;
					}
					else 
					{
					//File<OsModel> f(name1,0, fd);
					f.flag = 0;
					return f;	
					}
	                }
	//-----------------------------------------------------------------------
	bool exist( const char* name)
					{
					int res;
					char name1[15]="gffgggfgfgf";
					tf_shorten_filename(name1,name);
					
					res=dir_find(name1,11);
					if(res==-2)
					{
					return false;
					}
					else 
					return true;
					
				}
	//-----------------------------------------------------------------------
					int delete_file( const char *name)
					{
						int i,res;
					char name12[15]="gffgggfgfgf";
					block_data_t buf;
					block_data_t buf1[512];
					unsigned int org_clustr;
					tf_shorten_filename(name12,name);
					//for(i=0;i<15;i++)
					//printf("%c",name12[i]);
					//printf("\n");
					dir1=&buf;								
					res=dir_find(name12,11);
					if(res==-2)
					{
					return ERR_UNSPEC;
					}
					//printf("dir1=%u\n",dir1);
					org_clustr = LD_CLUST(dir1);
					//printf("org_clustr=%d\n",org_clustr);
					remove_clust_chain(org_clustr);
					fd.read(buf1,sect);
					//*(dir1)=0xE5;
					//*(dir1+1)=0;
					//*(dir1+2)=0;
					//*(dir1+3)=0;
					//fd.read(buf12,sect);
					//*(buf1+((index % 16) * 32))=0xE5;
					ST_INT((buf1+((index % 16) * 32)),0XE5);
					//*((buf12+((index % 16) * 32))+1)=0;
					//printf("sect in delete=%ld\n",sect);
					fd.write(buf1,sect);
					fd.read(buf1,sect);
					dir1=buf1+((index % 16) * 32);
					for(i=0;i<32;i++)
					printf("%d",*(dir1+i));
					printf("\n");
					printf("hello dude\n");
					return SUCCESS;
					}
	
	               private:
	                        block_data_t buffer[512];
	                        BPB b1;
	                        BS_16 b2;
	                        BS_32 b3;
	                        BlockMemory fd;
	                        block_data_t buffer2[512];
	                        unsigned int index; /* Current read/write index number */
	                        unsigned long sclust; /*start cluster */
	                        unsigned long clust; /* Current cluster */
	                        unsigned long sect; /* Current sector */
	                        block_data_t *dir1;
	};
	 
	}
	                
	#endif
	

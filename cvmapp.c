#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include "common.h"
#define TARNAME 1
#define LOGNAME 2
#define LOGTARNAME 3

#define IMGMODE 1
#define LOGMODE 2
//#define LOGFILEMODE 3

//#define APPPATH "/mnt/usb/dm386-files"
//#define APPPATH "/home/supriya/godb"
#define APPPATH "/mnt/usb/dm386-files"
#define IMGPATH APPPATH"/seqimages"
#define LOGPATH APPPATH"/applog"
#define LOGFILEPATH APPPATH"/logfiles"

#define DOWNLOAD_URL "http://cvm.rwp.in/cvmrwp/CVMWebservice.aspx"
#define UPLOAD_URL "http://cvm.rwp.in/cvmrwp/gupload.aspx"
struct filelist
{
	int InodeNo[200];
	char filename[200][256];
	int No;
};
struct cvmseqData
{
        char ChannelShortName[150];
        char FrequencyShortCode[50];
        char region[50];
        char operatorId[50];
        char TimeStamp[50];
        char Duration[50];
        char ChannelNumber[50];
};
typedef enum
{
	//12AM = 0, 1AM, 2AM, 3AM, 4AM, 5AM, 6AM, 7AM, 8AM, 9AM, 10AM, 11AM,
	//12PM, 1PM, 2PM, 3PM, 4PM, 5PM, 6PM, 7PM, 8PM, 9PM, 10PM, 11PM	
	AM12 = 0, AM1, AM2, AM3, AM4, AM5, AM6, AM7, AM8, AM9, AM10, AM11,
	PM12, PM1, PM2, PM3, PM4, PM5, PM6, PM7, PM8, PM9, PM10, PM11	
}TIME;
//char TarFormat[100];
char hw[220];
char TarFormat[100];
char LogFileFormat[100];
char chrDate[50];
char url_addr[100];
char LogFileTarFormat[100];
int static FileCount = 1;
int LOGFileLineCount = 0;
	
struct filelist temp_tar;
char* NameCreate( int mode );
pthread_t t_intcheck;
/*****************************************************************************
 Functio Name   : GetIP 
 Prototype      : void GetIP()
 Arguments      : -
 Return Type    : char *   
 Purpose        : To get IP Address
******************************************************************************/
char* GetIP()
{
	struct ifreq s;
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char *addr, iface[] = "", hw[20];
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	//char* interface = "eth0";
	char* gateway = NULL;

	FILE* fp = popen("netstat -rn", "r");
	char line[256]={0x0};
	char *eth;
	while(fgets(line, sizeof(line), fp) != NULL)
	{
		char* iface;
		eth = iface = strndup(line + 73, 4);
		//eth = strndup(line + 73, 4);
	}
	//PARSER_PRINT( ("gateway = %s eth = %s\n",gateway,eth) );
	strcpy(s.ifr_name, eth);

	//get the ip address
	ioctl(fd, SIOCGIFADDR, &s);
	fclose(fp);
	//display ip
	char *ip = inet_ntoa(( (struct sockaddr_in *)&s.ifr_addr )->sin_addr);
	//PARSER_PRINT( ("IP %s - %s\n" , iface , ip ) );
	return ip;
}
int internet_check()
{
	int ret = 0;
	ret = system( "ping -c1 8.8.8.8" );
	if(ret != 0)
	{
		perror("ping");
		printf("--> no connection!!!\n");
		return -1;
	}
	else
	{
		printf("--> connection established!!!\n");
		return 0;
	}
#if 0
	sleep(2);
	char *hostname;   
	struct hostent *hostinfo;
	hostname = "gmail.com";
	hostinfo = gethostbyname(hostname);
	//if (hostinfo == NULL)
	if (hostinfo == NULL)
	{
		printf("--> no connection!!!\n");
		return -1;
	}
	else
	{
		printf("--> connection established!!!\n");
		return 0;
	}
#endif
}

char *MacAddr()
{
	struct ifreq s;
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char *addr, iface[] = "";//, hw[20];
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	char* gateway = NULL;

	FILE* fp = popen("netstat -rn", "r");
	char line[256]={0x0};
	char *eth;

	while(fgets(line, sizeof(line), fp) != NULL)
	{
		char* destination;
		destination = strndup(line, 15);
		char* iface;
		eth = iface = strndup(line + 73, 4);

	}
	strcpy(s.ifr_name, eth);
	if ( ioctl(fd, SIOCGIFHWADDR, &s) == 0)
	{
		int i;
		sprintf(hw,"%02x:%02x:%02x:%02x:%02x:%02x",(unsigned char)s.ifr_addr.sa_data[0],
				(unsigned char)s.ifr_addr.sa_data[1], (unsigned char)s.ifr_addr.sa_data[2],
				(unsigned char)s.ifr_addr.sa_data[3], (unsigned char)s.ifr_addr.sa_data[4],
				(unsigned char)s.ifr_addr.sa_data[5] );
		//printf("MAC ID = %s\n",hw);
	}
	fclose(fp);
	int i = 0;
	for(i=0;hw[i];i++)
	{
		if(hw[i] == ':')
		{
			hw[i] = '_';
		}
	}

	char *mac = hw;
	return mac;
}
/*---------------------------------------------------------------*/

#if 0
void LogFileCount()
{

	char *st;
	char LineCount[100];
	char FileCount1[100];
	char CountBuf[100];
	st = LogFileNameCreat();
	sprintf(CountBuf,"grep -c ^ %s ",st);
	system(CountBuf);
	FILE* file = popen(CountBuf, "r");
	fscanf(file, "%s", LineCount);
	pclose(file);

	if(atoi(LineCount) >= 10)
	{
		FileCount++;
		sprintf(FileCount1,"echo > FileCount.txt %d",FileCount);
		system(FileCount1);
		printf("FileCount = %d\n",FileCount);
	}
}
#endif 
/*--------------------------------------------------------------*/


int TarFileCheck( char *path )
{
	int k;
	char cmd[200];
	sprintf(cmd,"cd %s; find *.tar",path);
	//k = system("find *.tar*");
	k = system(cmd);
	if(k!=0)
	{
		printf(".tar file is not available\n");
		return -1;
	}
	else
	{
		printf(".tar file present\n");
		return 0;
	}	

}
#if 1
void LogFileCreat()
{
	FILE * fp;
	char *ptr;
	signed char c;
	char cmd[200];
	int count = 0;
	char *TarfileName;
	//char LogFileFormat[100];
	/*
	   struct tm *myTime;
	   time_t mytm;

	   time(&mytm);
	   myTime = localtime(&mytm);
	   strftime(chrDate, 50, "%d-%m-%Y%H:%M:%S", myTime);
	   ptr = NameCreate(LOGNAME);	
	 */
	sprintf( cmd,"%s/%s", LOGFILEPATH, NameCreate(LOGNAME) );
	//TarfileName = TarName();
up:
	fp = fopen(cmd, "r");
	if (fp != NULL)
	{
		for (c = getc(fp); c != EOF; c = getc(fp))
			if (c == '\n')
				count = count + 1;
		fclose(fp);
		//printf("lines == %d\n ",  count);
		if( count == 10 )
		{
			FileCount++;
			sprintf( cmd,"%s/%s", LOGFILEPATH, NameCreate(LOGNAME) );
			goto up;
		}
	}
	sprintf( cmd,"%s/%s", LOGFILEPATH, NameCreate(LOGNAME) );
	fp = fopen (cmd,"a+");
	fprintf(fp, "%s%s%s%s\n",chrDate,hw,":",TarFormat);
	fclose(fp);
}
#endif
char* NameCreate( int mode )
{
	if( mode == TARNAME )//tar name
	{
		char chrDate[50];
		struct tm *myTime;
		time_t mytm;
		time(&mytm);
		myTime = localtime(&mytm);
		//strftime(chrDate, 50, "%d-%m-%Y%H:%M:%S", myTime);
		strftime(chrDate, 50, "%d-%m-%Y_%H-%M-%S", myTime);

		printf("Time and Date=%s\n", chrDate);

		sprintf(TarFormat,"%s%c%s%c%s",hw,'_',chrDate,'_',"IMAGES.tar");
		int i = 0;
		for(i=0;TarFormat[i];i++)
		{
			if(TarFormat[i] == ':')
			{
				TarFormat[i] = '_';
			}
		}
		return TarFormat;
	}
	else
	{
		if( mode == LOGNAME )//log name
		{
			sprintf(LogFileFormat,"%s%c%s%c%d.log",hw,'_',"LogFileType",'_',FileCount);
			return LogFileFormat;
		}
		else
		{
#if 0
			if( mode == JPGNAME )//jpg name
			{
				sprintf(LogFileFormat,"%s_%s",hw,);
				return LogFileFormat;
			}
			else//10 log tar name
			{
#endif
				struct tm *myTime;
				time_t mytm;
				time(&mytm);
				myTime = localtime(&mytm);
				strftime(chrDate, 50, "%d-%m-%Y_%H-%M-%S", myTime);
				sprintf(LogFileTarFormat,"%s%c%s%cAPPLOGS.tar",hw,'_',chrDate,'_');
				return LogFileTarFormat;

			}
		}
	}

/*****************************************************************************
 Functio Name   : capture
 Prototype      : void capture ( char *input, char *ImageName, char crop, char *cropsize))
 Arguments      : char *input, char *ImageName, char crop, char *cropsize)
 Return Type    : -   
 Purpose        : To capture image
******************************************************************************/
void capture( char *input, char *ImageName, char crop, char *cropsize)
{
	char cmd[200];
	if( crop == 1 )
	{
		sprintf(cmd,"./ffmpeg -y -i %s  -vf crop=%s %s \n", input, cropsize, ImageName);
		system(cmd);
	}
	else
	{
		sprintf(cmd,"./ffmpeg -y -i %s  -vframes 1  %s \n", input, ImageName);
		system(cmd);
	}
}
int FileSort( struct filelist *f, char *dirpath, char *FileExtension , int TotalFilesLimit)
{
	DIR *dir;
	struct dirent *ent;
	//char *path = "/home/supriya/godb/seqimages";
	char *path = dirpath;
	//struct filelist f;
	int i = 0, j = 0;
	if ((dir = opendir (path)) != NULL)
	{
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL)
		{
			//printf ("%d ", ent->d_type);
			//printf ("%d ", ent->d_ino);
			//printf ("%s\n", ent->d_name);
			//if( strstr(ent->d_name, ".jpg") )
			if( ( ent->d_type == 8 ) && strstr( ent->d_name, FileExtension ) )
			{
				f->InodeNo[i] = ent->d_ino;
				strcpy( f->filename[i] , ent->d_name);
				i++;
			}
			//printf ("%s\n", ent->d_name);
		}
		f->No = i;
	}
	closedir (dir);
	if( TotalFilesLimit )
	if( f->No < TotalFilesLimit )
	
	return -1;

	for(i=0;i<f->No;i++) //Logic for Sorting the Channels
	{
		for(j=i+1;j<f->No/*(( Service .No)-i-1)*/;j++)
		{
			if( f->InodeNo[i] > f->InodeNo[j] )
			{
				int temp_no;
				char temp_name[256];
				temp_no = f->InodeNo[i];
				strcpy( temp_name, f->filename[i] );

				f->InodeNo[i] = f->InodeNo[j];
				strcpy( f->filename[i], f->filename[j] );

				f->InodeNo[j] = temp_no;
				strcpy( f->filename[j], temp_name );

			}
		}
		usleep(100);
	}
#if 0
	//printf("============================================\n");
	for(i=0;i<f->No;i++)
	{
		printf( "%d %s\n",f->InodeNo[i], f->filename[i] );
	}
#endif
	return 0;
}
int FileCompress( struct filelist *f, int TotalFile, char *dirpath, char *TarFileName )
{
	char name[3000],i, cmd[4000] ;
	int offset = 0;
	for(i=0;i<TotalFile;i++)
	{
		sprintf(name+offset,"%s  ",f->filename[i]);
		offset += ( strlen(f->filename[i])+2 );
		//printf( "%d %s %d\n",f->InodeNo[i], f->filename[i], offset );
	}
	//printf(" %s\n",name);
	//int ret;
#if 1
	sprintf(cmd,"cd %s;tar -cf ./%s %s", dirpath, TarFileName, name);
	printf(" %s\n",cmd);

	int ret = system(cmd);
	if(ret != 0)
	{
		perror("tar");
		return ret;
	}
	sprintf(cmd,"cd %s;rm %s\n",dirpath, name);
	printf(" %s\n",cmd);
	ret = system(cmd);
	if(ret != 0)
	{
		perror("rm");
		return ret;
	}
#endif
	return ret;
}
//int UploadFile( char *url, char *username, char *password, char *filepath, char *filename)
int UploadFile( char *url, char *filepath, char *filename)
{
	char cmd [1000];
	sprintf(cmd,"./curl %s --upload-file %s/%s", url, filepath, filename);
	//./curl -u asif:welcome http://192.168.1.127 --upload-file act.tar.gz
	int ret = system(cmd);	
	printf("==%s\n",cmd);
	printf("==%d\n",ret);
	return ret;
}
int DownloadFile( char *url, char *filename)
{
	char cmd [1000];
	sprintf(cmd,"./curl --remote-name %s/%s", url, filename);
	//curl --remote-name http://192.168.1.15/5.jpg
	int ret = system(cmd);	
	printf("==%s\n",cmd);
	printf("==%d\n",ret);
	return ret;
}
char *jpgname(char *chcode, char *frqcode)
{
	char ImageName[500];
	struct tm *myTime;
	time_t mytm;
	char chrDate[100];
	time(&mytm);
	myTime = localtime(&mytm);
	strftime(chrDate, 50, "%d-%m-%Y_%H-%M-%S", myTime);
	sprintf( ImageName, "%s_%s_%s_%s.jpg",hw, chcode, frqcode, chrDate );
	char *name = ImageName;
	return name;
}
#if 1
void cvmSequenceCapture()
{
	time_t mytm;
	struct tm *myTime;

	char Hour[100];
	time(&mytm);
	myTime = localtime(&mytm);
	strftime(Hour, 50, "%H", myTime);
	printf("h = %s\n",Hour);
	char *imgname;
	char ImagePath[500];
	//thumbcheck();
	//channeltune();
        int linenumber = 0;

	//cvmseq read();
	//capture();

	//sprintf( ImagePath,"%s/%s",IMGPATH, jpgname);
	//capture( url_addr, ImagePath, 1, "200:200:0:0");  //1
	//capture( url_addr, "seqimages/7.jpg", 1, "200:200:1080:0"); //3
	int i = 0, lno = 0, count = 1;
	struct cvmseqData *cvmdata;
	cvmdata = ( struct cvmseqData *)cvmRead(&lno);
	TIME curtime = AM1;
	for(i =0 ; i<lno;i++)
	{
		if(cvmdata[i].TimeStamp[curtime] == '1')
		{
#if 0
			printf("Data[no].ChannelShortName          = %s\n",cvmdata[i].ChannelShortName);
			printf("Data[no].FrequencyShortCode        = %s\n",cvmdata[i].FrequencyShortCode);
			printf("Data[no].region                    = %s\n",cvmdata[i].region);
			printf("Data[no].operatorId                = %s\n",cvmdata[i].operatorId);
			printf("Data[no].TimeStamp                 = %s\n",cvmdata[i].TimeStamp);
			printf("Data[no].Duration                  = %s\n",cvmdata[i].Duration);
			printf("Data[no].ChannelNumber             = %s\n\n",cvmdata[i].ChannelNumber);
#endif			
			//channeltune( cvmdata[i].ChannelNumber );
			imgname = jpgname( cvmdata[i].ChannelShortName, cvmdata[i].FrequencyShortCode );	
			printf( "imgname = %s\n", imgname );	
			sprintf( ImagePath,"%s/%s",IMGPATH, imgname);
			capture( url_addr, ImagePath, 0, "200:200:0:0");  //1
			sleep(1);
			printf("==============================================\n");
			printf("          Image Count = %d\n",count);
			printf("==============================================\n");
			if( count > 110 )
			break;
			count++;
		}
	}
}
#endif
#if 1
int cvmFileUploadSequence( int mode )
{
	int ret,i=0;
	//struct filelist temp_tar;
	if( internet_check() < 0 )
	{
		printf("No Internet\n");
		return -1;
	}
	switch( mode )
	{
		case IMGMODE:
			{
imgup:
				while(!TarFileCheck(IMGPATH))
				{	
					printf("mode = 1\n");
					ret = FileSort(&temp_tar, IMGPATH, ".tar", 0);
					for( i=0; i<temp_tar.No; i++ )
					{
						//http://CVMservername/webservices/fileupld.asp
						ret = UploadFile ( "http://192.168.1.15", IMGPATH, temp_tar.filename[i] );
						if(ret != 0)
						{
							printf(" %s Upload Failed\n", temp_tar.filename[i]);
						}
						else
						{
							char cmd[200];
							sprintf(cmd,"%s/%s",IMGPATH, temp_tar.filename[i] );
							remove(cmd);
						}
					}
				}
				ret = FileSort(&temp_tar, IMGPATH, ".jpg", 10);
				if( ret == 0 )
				{
					FileCompress( &temp_tar,10, IMGPATH, NameCreate ( TARNAME ));
					LogFileCreat();
					goto imgup;
				}
			}break;
		case LOGMODE:
			{
up:
				while(!TarFileCheck(LOGPATH))
				{	
					printf("mode = 2\n");
					//http://cvmservername/webservice/applogs.asp?filename=tarfilename
					ret = FileSort(&temp_tar, LOGPATH, ".tar", 0);
					for( i=0; i<temp_tar.No; i++ )
					{
						//http://CVMservername/webservices/fileupld.asp
						ret = UploadFile ( "http://192.168.1.15", LOGPATH, temp_tar.filename[i] );
						if(ret != 0)
						{
							printf(" %s Upload Failed\n", temp_tar.filename[i]);
						}
						else
						{
							char cmd[200];
							sprintf(cmd,"%s/%s",LOGPATH, temp_tar.filename[i] );
							remove(cmd);
						}
					}
					ret = FileSort( &temp_tar,LOGPATH, ".log", 10);
					if( ret == 0 )
					{
						FileCompress( &temp_tar, 10, LOGPATH, NameCreate ( LOGTARNAME ));
					}

				}
				memset(&temp_tar,0,sizeof(temp_tar));
				char cmd[200];	
				ret = FileSort(&temp_tar, LOGFILEPATH, ".log", 10);
				if( ret == 0 )
				{
					FileCompress( &temp_tar,10,LOGFILEPATH, NameCreate ( LOGTARNAME ));
					sprintf( cmd,"mv %s/*.tar %s/.", LOGFILEPATH, LOGPATH);
					system(cmd);
					goto up;
				}


			}break;
		default:
			{
			}
	}
}
#endif
int main()
{
	char cmd[500];
	int i = 0;
	internet_check();
	//pthread_create(&t_intcheck,NULL,internet_check,NULL);
	sleep(1);
	sprintf( url_addr, "rtsp://%s:8556/PSIA/Streaming/channels/2?videoCodecType=H.264", GetIP());
	//#define RTSP "rtsp://192.168.1.56:8556/PSIA/Streaming/channels/2?videoCodecType=H.264"
	printf("URL = %s\n",url_addr);
	printf("MAC = %s\n",MacAddr());
	//return 0;
	//TarName();
#if 0
	struct filelist f;
	char CountData[20];
	FILE *fpp = fopen("FileCount.txt","r+");
	if(fpp!=NULL)
	{
		fgets(CountData,1024,fpp);
		FileCount  = atoi(CountData);
		printf("File count = %d\n",FileCount);
		fclose(fpp);
	}
#endif
	FileCount = 0;
	cvmFileUploadSequence( IMGMODE );
	cvmFileUploadSequence( LOGMODE );
	cvmSequenceCapture();

	//printf("=%s\n",NameCreate(TARNAME));	
	//printf("=%s\n",NameCreate(LOGNAME));	
	//printf("=%s\n",NameCreate(LOGTARNAME));	
	//printf("=%s\n",jpgname("213","564"));	
	//printf(IMGPATH);

	//LogFileCreat();

	//cvmFileUploadSequence(1);
	//cvmFileUploadSequence(2);
	cvmFileUploadSequence( IMGMODE );
	cvmFileUploadSequence( LOGMODE );

	//	int ret = FileSort(&f, LOGPATH, ".log", 7);
	//	if( ret == 0 )
	//	{
	//		FileCompress( &f,7, LOGPATH, NameCreate ( LOGTARNAME ));
	//	}

	//		int ret = FileSort(&f, LOGFILEPATH, ".log", 10);
	//               if( ret == 0 )
	//             {
	//                      FileCompress( &f,10,LOGFILEPATH, NameCreate ( LOGTARNAME ));
	//LogFileCreat();
	//            }


#if 0

	//filename , crop 1/ no crop 0 , width,height,x,y


	capture( url_addr, "seqimages/1.jpg", 1, "200:200:0:0");  //1
	capture( url_addr, "seqimages/2.jpg", 1, "200:200:1080:0");  //2
	capture( url_addr, "seqimages/3.jpg", 1, "200:200:1080:0"); //3
	capture( url_addr, "seqimages/4.jpg", 1, "200:200:1080:0");  //4

	capture( url_addr, "seqimages/5.jpg", 1, "200:200:0:0");  //1
	capture( url_addr, "seqimages/6.jpg", 1, "200:200:1080:0");  //2
	capture( url_addr, "seqimages/7.jpg", 1, "200:200:1080:0"); //3
	capture( url_addr, "seqimages/8.jpg", 1, "200:200:1080:0");  //4

	capture( url_addr, "seqimages/9.jpg", 1, "200:200:0:0");  //1
	capture( url_addr, "seqimages/10.jpg", 1, "200:200:1080:0");  //2


	struct filelist f;
	//int ret = FileSort(&f, "/home/supriya/godb/seqimages",".jpg");
	int ret = FileSort(&f, "/mnt/usb/dm386-files/seqimages",".jpg");
	if( ret < 0 )
	{
		printf("Image Count is less than 10\n");
		return -1;
	}
#if 0
	for(i=0;i<f->No;i++)
	{
		printf( "%d %s\n",f->InodeNo[i], f->filename[i] );
	}
#endif
	char *TarfileName = TarName();
	//FileCompress( &f,10,"/home/supriya/godb/seqimages", "12.tar");
	//FileCompress( &f,10,"/home/supriya/godb/seqimages", TarfileName);
	FileCompress( &f,10,"/mnt/usb/dm386-files/seqimages", TarfileName);
	//UploadFile("http://192.168.1.15", "asif", "welcome","/home/supriya/godb/seqimages","12.tar");
	//UploadFile("http://192.168.1.15", "asif", "welcome","/home/supriya/godb/seqimages",TarfileName);
	//UploadFile("http://192.168.1.15","/home/supriya/godb/seqimages",TarfileName);
	UploadFile("http://192.168.1.15","/mnt/usb/dm386-files/seqimages",TarfileName);
#endif
	//DownloadFile("http://192.168.1.15","12.tar");
	//pthread_create(&t_maketar,NULL,TarFileCheck,NULL);
#if 0
	while(1)
	{
		LogFileCreat();
		//LogFileCreat();
		LogFileCount();
		LogFileTarNameCreat();
		sleep(2);
	}
#endif
	/*	while(1)
		{
		sleep(1);
		}	
	 */

	//		ret = FileSort(&temp_tar, IMGPATH, ".jpg", 10);
	//              if( ret == 0 )
	//            {
	//                  FileCompress( &temp_tar,10, IMGPATH, NameCreate ( TARNAME ));
	//                LogFileCreat();
	//      }

}

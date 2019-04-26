#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
//Key 17 untuk membaca index pertama ke 17

static const char *dirpath = "/home/insane/shift4";
static const char *charlist = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
//Panjangnya charlist 93 dimulai dari 0
void enc(char *path)
{
	int a;
	a=strlen(path);
	//Untuk mengambil panjang array (path)
	for(int i=0; i<a; i++)//Cek karakter
	{
		for(int j = 0; j< 94; j++)//cek karakter setelah back slash pertama
		{
			if(path[i] == '/')
				break;
				//Karena langsung ngecek ke berikutnya atau udh selesai
			else if(path[i] == charlist[j])
			{
				path[i] = charlist[(j+17)%94];
					break; 
				//j+17 adalah untuk mengganti huruf pertama ke huruf ke 17
				//modulus 94 untuk misalkan indexnya lebih biar bisa mulai dari awal
			}
		}
	}
}

void dec(char *path)
{
	int a;
	a=strlen(path);
	//Untuk mengambil panjang array (path)
	for(int i=0; i<a; i++) //Cek karakter
	{
		for(int j = 0; j< 94; j++)//cek karakter setelah back slash pertama
		{
			if(path[i] == '/')
				break;
				//Karena langsung ngecek ke berikutnya atau udh selesai
			else if(path[i] == charlist[j])
			{
				path[i] = charlist[(94+(j-17))%94];
					break;
				//94+ agar kembali ke belakang 
				//j-17 adalah dibalik
				//modulus 94 untuk misalkan indexnya lebih biar bisa mulai dari awal
			}
		}
	}
}

//Template ke 2 di modul 4 SISOP
static int xmp_getattr(const char *path, struct stat *stbuf)
{
	enc(path);//-
  	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	enc(path);//-
  	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		dec(de->d_name);//-
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	enc(path);//-
  	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;
    int fd = 0 ;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}

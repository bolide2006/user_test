#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
 
int main (int argc, char* argv[]) {
	int fd=0;
	int fp=0;
	ssize_t nr;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	long screensize=0;
	char *fbp = 0;
	char *fbp_bak = 0;
	int x = 0, y = 0;
	int i = 0;
	long location = 0;
	
	if (argc < 2) {
		printf ("please use like:fb_test /dev/fbX");
		exit(1);
	}
		
	fp = open (argv[1],O_RDWR);

	if (fp < 0){
		printf("Error : Can not open framebuffer device %s/n", argv[1]);
		exit(1);
	}

	if (ioctl(fp,FBIOGET_FSCREENINFO,&finfo)){
		printf("Error reading fixed information/n");
		exit(2);
	}

	if (ioctl(fp,FBIOGET_VSCREENINFO,&vinfo)){
		printf("Error reading variable information/n");
		exit(3);
	}
	
	printf("xres:%d, yres:%d, bits_per_pixel:%d", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	/*这就是把fp所指的文件中从开始到screensize大小的内容给映射出来，得到一个指向这块空间的指针*/
	fbp_bak = fbp =(char *) mmap (0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fp,0);
	if ((int) fbp == -1)
	{
		printf ("Error: failed to map framebuffer device to memory./n");
		exit (4);
	}
	
	/*fill the screen with pure blue color*/
	for (i = 0; i < vinfo.xres * vinfo.yres; i++)
	{
		memset(fbp_bak++, 0xff, 1); //B
		memset(fbp_bak++, 0x00, 1);	//R
		memset(fbp_bak++, 0x00, 1); //G
		memset(fbp_bak++, 0x00, 1); //dummy
	}
	
	/*fill the screen with a rgb picture*/
	{
		int len = 100;
		char buf[len];
		fbp_bak = fbp;
		fd = open("1.rgba", O_RDONLY);
		if (fd == -1)
		{
			printf("Error: failed to open file test.rgba/n");
			exit(5);
		}
		
		while (len != 0 && (nr = read(fd, buf, len)) != 0)
		{
			if (nr == -1)
			{
				printf("Error: failed to read!");
				exit(6);
			}
			memcpy(fbp_bak, buf, nr);		
			fbp_bak+=nr;
		}	
		close(fd);
	}
	
	/*这是你想画的点的位置坐标,(0，0)点在屏幕左上角*/
	x = 100;
	y = 100;
	location = x * (vinfo.bits_per_pixel / 8) + y * finfo.line_length;

	*(fbp + location) = 100;  /* 蓝色的色深 */  /*直接赋值来改变屏幕上某点的颜色*/
	*(fbp + location + 1) = 15; /* 绿色的色深*/   
	*(fbp + location + 2) = 200; /* 红色的色深*/   
	*(fbp + location + 3) = 0;  /* 是否透明*/  
	
	
	munmap (fbp, screensize); /*解除映射*/
	close (fp);    /*关闭文件*/
	return 0;
}

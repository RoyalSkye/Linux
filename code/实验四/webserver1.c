#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>
#define HOSTLEN 256
#define BACKLOG 1
int make_server_socket(int);
int make_server_socket_q(int,int);
void read_til_crnl(FILE *);
void process_rq(char *,int);
void header(FILE *,char *);
void cannot_do(int);
void do_404(char *,int);
int isadir(char *);
int not_exist(char *);
void do_ls(char *,int);
char * file_type(char *);
int ends_in_cgi(char *);
void do_exec(char *,int);
void do_cat(char *,int);
int make_server_socket(int portnum){
	make_server_socket_q(portnum,BACKLOG);	
}
int make_server_socket_q(int portnum,int backlog){
	struct sockaddr_in saddr;
	struct hostent *hp; //this is part of our
	char hostname[HOSTLEN]; //address
	int sock_id;
	sock_id=socket(PF_INET,SOCK_STREAM,0);	
	if(sock_id==-1){
		perror("cannot socket");
		return -1;
	}
	bzero((void*)&saddr,sizeof(saddr));
	gethostname(hostname,HOSTLEN);
	//printf("%s\n",hostname);
	hp=gethostbyname(hostname);
	if(hp==NULL){
		herror("hp is null:");  //etc/hosts
		return -1;
	}
	/*struct hostent { 
 		char  *h_name;            official name of host
               char **h_aliases;          alias list 
               int    h_addrtype;         host address type 
               int    h_length;           length of address 
               char **h_addr_list;        list of addresses
        }

	struct sockaddr_in{
        short sin_family;
        unsigned short sin_port;  Port number(必须要采用网络数据格式,普通数字可以用htons()函数转换成网络数据格式的数字)
        struct in_addr sin_addr;  IP address in network byte order（Internet address）
        unsigned char sin_zero[8];
	};*/
	bcopy(hp->h_addr,(struct sockaddr *)&saddr.sin_addr,hp->h_length);
	//saddr.sin_addr.s_addr=INADDR_ANY;
	saddr.sin_port=htons(portnum);//htons converts integer variables from host byte order to network byte order
	saddr.sin_family=AF_INET;
	if(bind(sock_id,(struct sockaddr *)&saddr,sizeof(saddr))!=0){
		perror("cannot bind");
		return -1;	
	}
	if(listen(sock_id,backlog)!=0){
		perror("cannot listen");
		return -1;	
	}
	return sock_id;	
}
void read_til_crnl(FILE *fp){
	char buf[BUFSIZ];
	while(fgets(buf,BUFSIZ,fp)!=NULL&&strcmp(buf,"\r\n")!=0);
}
void process_rq(char *rq,int fd){
	char cmd[BUFSIZ],arg[BUFSIZ];
	if(fork()!=0) return;
	strcpy(arg,"./");  //当前目录
	if(sscanf(rq,"%s%s",cmd,arg+2)!=2) return;  //int sscanf (const char *str,const char * format,...)会将参数str的字符串根据参数format字符串来转换并格式化数据,成功则返回参数数目,失败则返回0
	if(strcmp(cmd,"GET")!=0) cannot_do(fd);	
	else if(not_exist(arg)) do_404(arg,fd);
	else if(isadir(arg)) do_ls(arg,fd);
	else if(ends_in_cgi(arg)) do_exec(arg,fd);
	else do_cat(arg,fd);
	
}	
void header(FILE *fp,char *content_type){  //the reply header,if content_type is NULL then don't send content type
	fprintf(fp,"HTTP/1.0 200 OK\r\n");
	if(content_type){
		fprintf(fp,"Content-type:%s\r\n",content_type);
	}
}	
void cannot_do(int fd){
	FILE *fp=fdopen(fd,"w");
	fprintf(fp,"HTTP/1.0 501 Not Implemented\r\n");
	fprintf(fp,"Content-type:text/plain\r\n");
	fprintf(fp,"\r\n");
	fprintf(fp,"That command is not yet implemented\r\n");
	fclose(fp);
}
void do_404(char *item,int fd){
	FILE *fp=fdopen(fd,"w");
	fprintf(fp,"HTTP/1.0 404 Not Found\r\n");
	fprintf(fp,"Content-type:text/plain\r\n");
	fprintf(fp,"\r\n");
	fprintf(fp,"The item you requested:%s\r\nis not found\r\n",item);
	fclose(fp);
}
int isadir(char *f){
	struct stat info;
	return (stat(f,&info)!=-1&&S_ISDIR(info.st_mode));  //mode_t  st_mode; protection The following POSIX macros are defined to check the file type using the st_mode field: S_ISDIR(m)  directory?
}
int not_exist(char *f){
	struct stat info;
	return (stat(f,&info)==-1);  //int stat(const char *path, struct stat *buf) stats the file pointed to by path and fills in buf.On success, zero is returned. On error, -1 is returned
}
void do_ls(char *dir,int fd){
	FILE *fp;
	fp=fdopen(fd,"w");
	header(fp,"text/plain");
	fprintf(fp,"\r\n");
	fflush(fp);
	dup2(fd,1);  //1 stdout
	dup2(fd,2);  //2 stderr
	close(fd);
	execlp("ls","ls","-l",dir,NULL);  //int execlp(const char *file, const char *arg, ...)会从PATH 环境变量所指的目录中查找符合参数file的文件名，找到后便执行该文件，然后将第二个以后的参数当做该文件的argv[0]、argv[1]……，最后一个参数必须用空指针(NULL)作结束。
	perror(dir);
	exit(0);
}
char * file_type(char *f){  //return extension of file
	char *cp;
	if((cp=strrchr(f,'.'))!=NULL) return cp+1;   //char *strrchr(const char *s, int c) The strrchr() function returns a pointer to the last occurrence of the character c in the string s.
	return "";
}
int ends_in_cgi(char *f){
	return (strcmp(file_type(f),"cgi")==0);
}
void do_exec(char *prog,int fd){
	FILE *fp;
	fp=fdopen(fd,"w");
	header(fp,NULL);
	fflush(fp);
	dup2(fd,1);
	dup2(fd,2);
	close(fd);
	execl(prog,prog,NULL);  // int execl(const char *path, const char *arg, ...) 
	perror(prog);
}
void do_cat(char *f,int fd){
	char *extension = file_type(f);
	char *content = "text/plain";
	FILE *fp, *fpfile;
	int c;
	if ( strcmp(extension,"html") == 0 )
		content = "text/html";
	else if ( strcmp(extension, "gif") == 0 )
		content = "image/gif";
	else if ( strcmp(extension, "jpg") == 0 )
		content = "image/jpeg";
	else if ( strcmp(extension, "jpeg") == 0 )
		content = "image/jpeg";
	fp=fdopen(fd, "w");
	fpfile = fopen(f,"r");
	if(fp!= NULL && fpfile != NULL){
		header(fp,content);
		fprintf(fp, "\r\n");
		while((c=getc(fpfile))!=EOF) putc(c,fp);
		fclose(fpfile);
		fclose(fp);
	} 
	exit(0);
}
main(int ac,char *av[]){
	if(ac!=2){
		fprintf(stderr,"usage:ws portnum\n");
		exit(1);
	}
	int fd,sock,rv;
	FILE *fpin;
	char request[BUFSIZ];
	sock=make_server_socket(atoi(av[1]));
	if(sock==-1){
		exit(2);
	}	
	while(1){
		fd=accept(sock,NULL,NULL);
		fpin=fdopen(fd,"r");
		fgets(request,BUFSIZ,fpin);
		printf("get a call:request=%s",request);
		read_til_crnl(fpin);
		process_rq(request,fd);
		//printf("This is server,accept successfully!\n");
		fclose(fpin);
	}
}

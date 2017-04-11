/*
 * (c) 2008-2011 Daniel Halperin <dhalperi@cs.washington.edu>
 *
 *  Modified by Zhicheng Zeng, Bingxian Lu.
 *  Email: bingxian.lu@gmail.com
 */
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/connector.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/errno.h>

#define MAX_PAYLOAD 2048
#define SLOW_MSG_CNT 1

#define _READ_IP_PORT_FROM_

#define SERV_PORT 8090
int serv_port = -1;

#define SERV_IP "127.0.0.1"
char serv_ip[16];

int sock_fd = -1;							// the socket
FILE* out = NULL;

void check_usage(int argc, char** argv);

FILE* open_file(const char* filename, char* spec);

void caught_signal(int sig);

void exit_program(int code);
void exit_program_err(int code, char* func);

int Socket(int, int, int);
void Inet_pton(int , const char *, void *);
void Connect(int, struct sockaddr_in * , int);
size_t SockWrite(int, const void *, unsigned short);

#ifdef _READ_IP_PORT_FROM_
void read_ip_port_from_config(char * file_config)
{
	FILE * fconfig = NULL;
	fconfig = open_file(file_config, "r");
	fscanf(fconfig, "%d %s", &serv_port, serv_ip);
	fclose(fconfig);
}
#endif

int main(int argc, char** argv)
{
	/* Local variables */
	struct sockaddr_nl proc_addr, kern_addr;	// addrs for recv, send, bind
	struct cn_msg *cmsg;
	char buf[4096];
	int ret;
	unsigned short l, l2;
	int count = 0;
	struct sockaddr_in servaddr;
	int sock_cli_fd = -1;

	#ifdef _READ_IP_PORT_FROM_
	if(argc == 2)
	{
		printf("read ip port from %s\n", argv[1]);
		read_ip_port_from_config(argv[1]);
	}
	else
	{
		serv_port = SERV_PORT;
		strcpy(serv_ip, SERV_IP);
	}	
	#else
		serv_port = SERV_PORT;
		strcpy(serv_ip, SERV_IP);
	#endif
	
	printf("connect %s with port %d\n", serv_ip, serv_port);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(serv_port);	
	Inet_pton(AF_INET, serv_ip, &servaddr.sin_addr);

	sock_cli_fd = Socket(AF_INET, SOCK_STREAM, 0);
	Connect(sock_cli_fd, &servaddr, sizeof(servaddr));


	/* Make sure usage is correct */
	check_usage(argc, argv);

	/* Setup the socket */
	sock_fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);
	if (sock_fd == -1)
		exit_program_err(-1, "socket");

	/* Initialize the address structs */
	memset(&proc_addr, 0, sizeof(struct sockaddr_nl));
	proc_addr.nl_family = AF_NETLINK;
	proc_addr.nl_pid = getpid();			// this process' PID
	proc_addr.nl_groups = CN_IDX_IWLAGN;
	memset(&kern_addr, 0, sizeof(struct sockaddr_nl));
	kern_addr.nl_family = AF_NETLINK;
	kern_addr.nl_pid = 0;					// kernel
	kern_addr.nl_groups = CN_IDX_IWLAGN;

	/* Now bind the socket */
	if (bind(sock_fd, (struct sockaddr *)&proc_addr, sizeof(struct sockaddr_nl)) == -1)
		exit_program_err(-1, "bind");

	/* And subscribe to netlink group */
	{
		int on = proc_addr.nl_groups;
		ret = setsockopt(sock_fd, 270, NETLINK_ADD_MEMBERSHIP, &on, sizeof(on));
		if (ret)
			exit_program_err(-1, "setsockopt");
	}

	/* Set up the "caught_signal" function as this program's sig handler */
	signal(SIGINT, caught_signal);

	/* Poll socket forever waiting for a message */
	while (1)
	{
		/* Receive from socket with infinite timeout */
		ret = recv(sock_fd, buf, sizeof(buf), 0);
		if (ret == -1)
			exit_program_err(-1, "recv");
		/* Pull out the message portion and print some stats */
		cmsg = NLMSG_DATA(buf);
		if (count % SLOW_MSG_CNT == 0)
			printf("received %d bytes: id: %d val: %d seq: %d clen: %d\n", cmsg->len, cmsg->id.idx, cmsg->id.val, cmsg->seq, cmsg->len);
		/* Log the data to file */
		l = (unsigned short) cmsg->len;
		l2 = htons(l);

		/******** by chng : write &l2 by client socket ********/
		if( write(sock_cli_fd, &l2, sizeof(unsigned short)) < 0)
			;
		/******** by chng : end ********/

		/******** by chng : write cmsg->data by client socket ********/
		if (write(sock_cli_fd, cmsg->data, l) < 0)
			;
		/******** by chng : end ********/

		if (count % 20 == 0)
			printf("wrote %d bytes [msgcnt=%u]\n", ret, count);
		++count;
	}

	exit_program(0);
	return 0;
}

int Socket (int family, int type, int protocol)
{
	int n;
	if ( (n=socket(family, type, protocol)) < 0){
		fprintf(stderr, "socket error\n");
		exit_program(1);
	}
	return n;
}

void Inet_pton(int family, const char * ip, void * addptr)
{
	int n;
	if ( (n = inet_pton(family, ip, addptr)) <=0)
	{
		if(n == 0)
			fprintf(stdin, "inet_pton format error : %s\n", ip);
		else
			fprintf(stdin, "inet_pton error : %s\n", ip);
		exit_program(1);
	}
}

void Connect(int fd, struct sockaddr_in * sa, int size)
{
	if ( connect(fd, (struct sockaddr *)sa, size) < 0 )
	{
		fprintf(stderr, "connect error\n");
		exit_program(1);
	}
}
size_t SockWrite(int fd, const void * buf, unsigned short len)
{
	size_t nleft = len;
	size_t n;
	while (nleft > 0)
	{
		if ((n=write(fd, buf, len)) <= 0)
		{
			if(n < 0 && errno == EINTR)
				n = 0;
			else{
				fprintf(stderr, "write error: %s\n", (char *)buf);
				return (-1);
			}
		}
		nleft -= n;
		buf += n;
	}
	return (n);
}

void check_usage(int argc, char** argv)
{
	if (argc < 1)
	{
		fprintf(stderr, "Usage: log_to_file [config_file_with_ip_and_port_in_it]\n");
		exit_program(1);
	}
}

FILE* open_file(const char * filename, char* spec)
{
	FILE* fp = fopen(filename, spec);
	if (!fp)
	{
		perror("fopen");
		exit_program(1);
	}
	return fp;
}

void caught_signal(int sig)
{
	fprintf(stderr, "Caught signal %d\n", sig);
	exit_program(0);
}

void exit_program(int code)
{
	if (out)
	{
		fclose(out);
		out = NULL;
	}
	if (sock_fd != -1)
	{
		close(sock_fd);
		sock_fd = -1;
	}
	exit(code);
}

void exit_program_err(int code, char* func)
{
	perror(func);
	exit_program(code);
}

/*
 * smtp_main.c
 *
 * Starter code for Level I Networks Portfolio 1
 * Simple SMTP client for connecting to the outlook smtp mail server
 * The connection , TLS switch and authentication logic is all written
 * students only need to implement the SMTP command logic for:
 *      EMAIL FROM
 *      RCPT TO
 *      BEGIN DATA
 *      Sending the data line by line (including a SUBJECT header)
 *      QUIT
 *
 */

/* Include files needed for standard networking */
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include<netdb.h>
#include <stdlib.h>
/* Include files needed for SSL/TLS */
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

/*
 * Port 587 is the port outlook.com uses for a vanilla socket -> TLS connection
 * i.e. one that uses the STARTTLS command
 */
#define PORT 587

/* Utility functions for the client */
int hostname_to_ip(char * hostname , char* ip);
int send_line(char *msg, int len);
int read_reply(char *buffer, int capacity);
int get_code(char *msg, int len);
int init_ssl();

/* These functions implement the SMTP protocol commands and responses */
int start_tcp_session();
int send_ehlo(char *buffer, int buf_size);
int send_start_tls(char *buffer, int buf_size);
int send_helo(char *buffer, int buf_size);
int send_auth_request(char *buffer, int buf_size);
int send_username(char *buffer, int buf_size);
int send_password(char *buffer, int buf_size);

/********************* TODO You will have to implement the functions in this block *********************/
int send_email_from(char *msg, int len);
int send_rcpt_to(char *msg, int len);
int send_begin_data();
int send_message();
int send_quit();
/********************* TODO You will have to implement the functions in this block *********************/

/* Global variables */
char *server_name = "smtp-mail.outlook.com";
int  socket_handle = 0;
int client_fd;
struct sockaddr_in serv_addr;


int main(int argv, char **args) {

    char *reply_buffer = malloc(1024);

    int err = 0;
    int bytes_sent = 0;
    int bytes_read = 0;

    /* Start a TCP session - exit if it doesn't set up */
    if(0 != start_tcp_session()) {
        exit(1);
    }
    // Send EHLO
    bytes_read = send_ehlo(reply_buffer, 1024);
    // Send STARTTLS
    bytes_read = send_start_tls(reply_buffer, 1024);









}


/**********************************************************************************************************************/



/**********************************************************************************************************************/

/**
 * Sends the EHLO command writes the response into the passed in buffer
 * @return
 */
int send_ehlo(char *buffer, int buf_size) {
    memset(buffer, 1024, 0x00);
    int bytes_sent = send_line("ehlo smtp-mail.outlook.com\r\n", strlen("ehlo smtp-mail.outlook.com\r\n"));
    int bytes_read = read_reply(buffer, 1024);
    if(bytes_read < 200) {
        /* This is a bit of a bad heuristic, sometimes the reply gets segmented. */
        bytes_read += read_reply(buffer+bytes_read, 1024-bytes_read);
    }
    return bytes_read;
}

/**
 * Sends the STARTTLS request. If OK wrapps the socket in a
 * TLS handler
 * @param buffer
 * @param buf_size
 * @return
 */
int send_start_tls(char *buffer, int buf_size) {
    memset(buffer, 1024, 0x00);
    int bytes_sent = send_line("STARTTLS\r\n", 10);
    int bytes_read = read_reply(buffer, 1024);
    // Check that the return buffer has the success code '220'
    if(strncmp("220", buffer, 3) != 0) {
        printf("STARTTLS failed");
        exit(1);
    } else {
        init_ssl();
    };
    // Switch to TLS mode
    return bytes_read;
}






/**
 * Utility function - just sends one line through the socket
 * @param line
 * @param len
 * @return
 */
int send_line(char *line, int len) {
    return send(socket_handle, line, len, 0);

}

/**
 * Utility function - waits on the socket for a reply
 * @param buffer
 * @param capacity
 * @return
 */
int read_reply(char *buffer, int capacity) {
    int bytes_read = read(socket_handle, buffer, capacity);
    return bytes_read;
}

/**
 * Utility function - opens a client side socket
 * @return
 */
int start_tcp_session() {
    if ((socket_handle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Get the host IP from the name
    char ip[100] = {0};
    hostname_to_ip(server_name, ip);
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        return -1;
    } else {
        if ((client_fd = connect(socket_handle, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
            return -1;
        }
    }
    return 0;
}

int hostname_to_ip(char * hostname , char* ip) {
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
    if ( (he = gethostbyname( hostname ) ) == NULL) {
        // get the host info
        herror("gethostbyname");
        return 1;
    }
    addr_list = (struct in_addr **) he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++) {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
    return 1;
}

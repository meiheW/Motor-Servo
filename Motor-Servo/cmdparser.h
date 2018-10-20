#ifndef CMDPARSER_H
#define CMDPARSER_H

#include <stdlib.h>
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

// 套接字参数对象格式
struct am335x_socket_t;
typedef struct am335x_socket_t am335x_socket_t;

struct am335x_socket_t{

        int server_port;
        int queue_size;
};


// 网络数据的收发
int m_socket_read(void *buf, size_t count);
int m_socket_write(const void *buf, size_t count);
int message_send(const char* msg);

int listening_socket(int server_port, int queue_size);

#ifdef __cplusplus
}

#endif
#endif  // CMDPARSER_H

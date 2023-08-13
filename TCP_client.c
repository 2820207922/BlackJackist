#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

struct Message
{
    int messageLength;
    char data[256];
};

void read_str(char **str)
{
    int size = 1;
    *str = (char *)malloc(sizeof(char));

    char ch = getchar();
    while (ch != '\n')
    {
        (*str)[size - 1] = ch;
        *str = (char *)realloc(*str, ++size);
        ch = getchar();
    }
    (*str)[size - 1] = '\0';
}

DWORD WINAPI SendThread(LPVOID lpParameter)
{
    SOCKET clientSocket = *((SOCKET *)lpParameter);
    struct Message sendMessage;
    char sendBuffer[sizeof(struct Message)];

    while (1)
    {
        char *str = NULL;
        read_str(&str);
        sendMessage.messageLength = snprintf(sendMessage.data, sizeof(sendMessage.data), str);

        // 发送消息给服务器
        memcpy(sendBuffer, &sendMessage, sizeof(struct Message));
        if (send(clientSocket, sendBuffer, sizeof(struct Message), 0) < 0)
        {
            fprintf(stderr, "Send failed\n");
            break;
        }
    }

    return 0;
}

int main()
{
    WSADATA wsa;
    SOCKET clientSocket;
    struct sockaddr_in server;
    HANDLE hSendThread;
    DWORD sendThreadId;
    struct Message receivedMessage;
    char recvBuffer[sizeof(struct Message)];

    // 初始化Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "Failed to initialize Winsock\n");
        return 1;
    }

    // 创建套接字
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    // 设置服务器地址
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // 替换为服务器的IP地址
    server.sin_port = htons(8888);

    // 连接服务器
    if (connect(clientSocket, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        fprintf(stderr, "Connection failed\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    printf("\nBlackJackist\n");
    printf("----------------------------------------\n");
    printf("请输入用户名: ");

    // 创建发送线程
    hSendThread = CreateThread(NULL, 0, SendThread, &clientSocket, 0, &sendThreadId);
    if (hSendThread == NULL)
    {
        fprintf(stderr, "Failed to create send thread\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // 接收服务器的响应
    while (1)
    {
        if (recv(clientSocket, (char *)&receivedMessage, sizeof(struct Message), 0) < 0)
        {
            // 关闭发送线程
            WaitForSingleObject(hSendThread, INFINITE);
            CloseHandle(hSendThread);

            // 关闭套接字
            closesocket(clientSocket);
            WSACleanup();

            return 1;
        }

        // 处理服务器的响应
        printf("%s", receivedMessage.data);
    }

    // 关闭发送线程
    WaitForSingleObject(hSendThread, INFINITE);
    CloseHandle(hSendThread);

    // 关闭套接字
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}

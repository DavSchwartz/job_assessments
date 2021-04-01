#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <winsock2.h>

#define ADDRESS "127.0.0.1"
#define PORT 8080
#define BUFFER_LEN 1024
#define MAX_STRING_LEN 64
#define LISTEN_QUEUE_LEN 3
enum Command;
enum State;
struct Context;
char* commandStr(enum Command command);
char* stateStr(enum State state);
void executeCommand(struct Context* context);
void safeModeEnable(struct Context* context);
void safeModeDisable(struct Context* context);
void showNumCommands(struct Context* context);
void showNumSafeModes(struct Context* context);
void showUpTime(struct Context* context);
void resetCommandCounter(struct Context* context);
void terminate(struct Context* context);
void invalid(struct Context* context);
void startup(struct Context* context);
void initSock(SOCKET* sock, struct sockaddr_in* address);
void eventLoop();
void parseArgs(int argc, char **argv, char(* address)[MAX_STRING_LEN], int* port);

// available commands from client
enum Command
{
    Undefined = 1,
    SafeModeEnable,
    SafeModeDisable,
    ShowNumCommands,
    ShowNumSafeModes,
    ShowUpTime,
    ResetCommandCounter,
    Shutdown
};
char commands[8][MAX_STRING_LEN] = {"undefined", "safe mode enable", "safe mode disable", "show number of commands received", "show number of safe modes", "show up time", "reset command counter", "shutdown"};

// states of state machine
enum State
{
    Restarting = 1,
    Ready,
    SafeMode,
    BbqMode
};
char states[4][MAX_STRING_LEN] = {"Restarting", "Ready", "Safe Mode", "BBQ Mode"};

// contextual information of client connnection & finite state machine
struct Context
{
    enum Command command;
    enum State state;
    SOCKET socket;
    unsigned int num_valid_cmds;
    unsigned int num_seq_invalid_cmds;
    unsigned int num_safe_modes;
    time_t start_time_sec;
};

// string representation of Command Enum
char* commandStr(enum Command command)
{
    return commands[(int)command - 1];
}

// string representation of State Enum
char* stateStr(enum State state)
{
    return states[(int)state - 1];
}

// take action based on current state and command
void executeCommand(struct Context* context)
{
    switch(context->state)
    {
        case Ready:
            switch(context->command)
            {
                case SafeModeEnable:
                    safeModeEnable(context);
                    break;
                case SafeModeDisable:
                    safeModeDisable(context);
                    break;
                case ShowNumCommands:
                    showNumCommands(context);
                    break;
                case ShowNumSafeModes:
                    showNumSafeModes(context);
                    break;
                case ShowUpTime:
                    showUpTime(context);
                    break;
                case ResetCommandCounter:
                    resetCommandCounter(context);
                    break;
                case Shutdown:
                    terminate(context);
                    break;
                default:
                    invalid(context);
                    break;
            }
            break;
        case SafeMode:
            switch(context->command)
            {
                case SafeModeDisable:
                    safeModeDisable(context);
                    break;
                case ShowNumSafeModes:
                    showNumSafeModes(context);
                    break;
                case Shutdown:
                    terminate(context);
                    break;
                default:
                    invalid(context);
                    break;
            }
            break;
        case BbqMode:
            switch(context->command)
            {
                case SafeModeDisable:
                    safeModeDisable(context);
                    break;
                case Shutdown:
                    terminate(context);
                    break;
                default:
                    invalid(context);
                    break;
            }
            break;
    }
}

// action for "safe mode enable" command
void safeModeEnable(struct Context* context)
{
    context->num_seq_invalid_cmds = 0;
    context->num_valid_cmds++;
    context->state = SafeMode;
    context->num_safe_modes++;

    char response[BUFFER_LEN] = {0};
    sprintf(response, "{\"state\": \"%s\"}", stateStr(context->state));
    send(context->socket , response, strlen(response) , 0);
}

// action for "safe mode enable" command
void safeModeDisable(struct Context* context)
{
    context->num_seq_invalid_cmds = 0;
    context->num_valid_cmds++;
    context->state = Ready;

    char response[BUFFER_LEN] = {0};
    sprintf(response, "{\"state\": \"%s\"}", stateStr(context->state));
    send(context->socket , response, strlen(response) , 0);
}

// action for "safe mode disable" command
void showNumCommands(struct Context* context)
{
    context->num_seq_invalid_cmds = 0;
    context->num_valid_cmds++;

    char response[BUFFER_LEN] = {0};
    sprintf(response, "{\"state\": \"%s\", \"number of commands\": %i}", stateStr(context->state), context->num_valid_cmds);
    send(context->socket , response, strlen(response) , 0);
}

// action for "show number of safe modes" command
void showNumSafeModes(struct Context* context)
{
    context->num_seq_invalid_cmds = 0;
    context->num_valid_cmds++;

    char response[BUFFER_LEN] = {0};
    sprintf(response, "{\"state\": \"%s\", \"number safe modes\": %i}", stateStr(context->state), context->num_safe_modes);
    send(context->socket , response, strlen(response) , 0);
}

// action for "show up time" command
void showUpTime(struct Context* context)
{
    context->num_seq_invalid_cmds = 0;
    context->num_valid_cmds++;

    int time_sec = time(NULL) - context->start_time_sec;
    char response[BUFFER_LEN] = {0};
    sprintf(response, "{\"state\": \"%s\", \"up time\": %i}", stateStr(context->state), time_sec);
    send(context->socket, response, strlen(response) , 0);
}

// action for "reset command counter" command
void resetCommandCounter(struct Context* context)
{
    context->num_seq_invalid_cmds = 0;
    context->num_valid_cmds++;

    context->num_valid_cmds = 0;
    char response[BUFFER_LEN] = {0};
    sprintf(response, "{\"state\": \"%s\", \"number of commands\": %i}", stateStr(context->state), context->num_valid_cmds);
    send(context->socket , response, strlen(response) , 0);
}

// action for "shutdown" command
void terminate(struct Context* context)
{
    context->num_seq_invalid_cmds = 0;
    context->num_valid_cmds++;
    
    char response[BUFFER_LEN] = {0};
    sprintf(response, "{\"state\": \"%s\"}", stateStr(context->state));
    send(context->socket , response, strlen(response) , 0);

    closesocket(context->socket);

    printf("Shutting down...\n");
    WSACleanup();
    exit(0);
}

// action for invalid command
void invalid(struct Context* context)
{
    context->num_seq_invalid_cmds++;
    if ((context->state == Ready) && (context->num_seq_invalid_cmds >= 5))
    {
        context->state = SafeMode;
    }
    else if ((context->state == SafeMode) && (context->num_seq_invalid_cmds >= 8))
    {
        context->state = BbqMode;
    }

    char response[BUFFER_LEN] = {0};
    sprintf(response, "{\"state\": \"%s\"}", stateStr(context->state));
    send(context->socket , response, strlen(response) , 0);
}

// initialization of state machine
void startup(struct Context* context)
{
    sleep(10);
    context->state = Ready;
}

// create and bind socket; listen for connections
void initSock(SOCKET* sock, struct sockaddr_in* address)
{
    struct WSAData wsaData = {0};
    int result;
    int opt = 1;

    // Initialize Winsock
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        exit(1);
    }

    // Creating socket file descriptor
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == INVALID_SOCKET)
    {
        printf("socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }

    // Allow to forcibly bind to a port in use by another socket
    result = setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
    if (result == SOCKET_ERROR)
    {
        printf("setsockopt SO_REUSEADDR failed: %d\n", WSAGetLastError());
        closesocket(*sock);
        WSACleanup();
        exit(1);
    }
       
    // Forcefully attaching socket to the port 8080
    result = bind(*sock, (struct sockaddr *)address, sizeof(*address));
    if (result == SOCKET_ERROR)
    {
        printf("bind failed: %d\n", WSAGetLastError());
        closesocket(*sock);
        WSACleanup();
        exit(1);
    }

    // Listen for connections
    result = listen(*sock, LISTEN_QUEUE_LEN);
    if (result == SOCKET_ERROR)
    {
        printf("listen failed: %d\n", WSAGetLastError());
        closesocket(*sock);
        WSACleanup();
        exit(1);
    }
}

// event loop to serve clients
void eventLoop(SOCKET sock)
{
    struct Context context;
    context.command = Undefined;
    context.state = Restarting;
    context.num_valid_cmds = 0;
    context.num_seq_invalid_cmds = 0;
    context.num_safe_modes = 0;
    context.start_time_sec = time(NULL);

    for (;;)
    {
        // connect to client
        context.socket = accept(sock, NULL, NULL);
        if (context.socket == INVALID_SOCKET)
        {
            printf("accept: %d\n", WSAGetLastError());
            closesocket(sock);
            WSACleanup();
            exit(1);
        }
        printf("Client connected\n");

        startup(&context);

        char buffer[BUFFER_LEN] = {0};
        int result = recv(context.socket, buffer, BUFFER_LEN, 0);
        while(result != SOCKET_ERROR)
        {
            // graceful disconnect
            if (result == 0) { break; }

            context.command = (enum Command)atoi(buffer);
            printf("Processing command \"%s\"\n", commandStr(context.command));
            executeCommand(&context);
            result = recv(context.socket, buffer, BUFFER_LEN, 0);
        }
        
        printf("client disconnect\n");
        closesocket(context.socket);
    }
}

// parse command line arguments
void parseArgs(int argc, char **argv, char(* address)[MAX_STRING_LEN], int* port)
{
    strcpy(*address, ADDRESS);
    *port = PORT;
    int c;
    while ((c = getopt(argc, argv, "a:p:h")) != -1)
    {
        switch(c)
        {
            case 'h':
                printf("\
usage: fsw.exe [-h] [-a ADDRESS] [-p PORT]\n\
optional arguments:\n\
-h, show this help message and exit\n\
-a ADDRESS, IP address to serve on\n\
-p PORT, Port to serve on\n\
                ");
                exit(0);
                break;
            case 'a':
                strcpy(*address, optarg);
                break;
            case 'p':
                *port = atoi(optarg);
                break;
        }
    }
}

int main(int argc, char **argv)
{
    char address[MAX_STRING_LEN] = {0};
    int port;
    parseArgs(argc, argv, &address, &port);

    struct sockaddr_in sock_address;
    sock_address.sin_family = AF_INET;
    sock_address.sin_addr.s_addr = inet_addr(address);
    sock_address.sin_port = htons(port);
    SOCKET sock;
    initSock(&sock, &sock_address);

    eventLoop(sock);

    closesocket(sock);
    WSACleanup();
    exit(0);
}
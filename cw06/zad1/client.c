#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<signal.h>

#include "safe_functions.h"
#include "messages.h"


typedef struct {
    int server_msqid;
    int client_msqid;
    client_t client_id;
} connection_t;


static const char* commands[4] = {
    "stop",
    "list",
    "2all",
    "2one"
};


int running;


void interrupt_handler(int _sig) {
    running = 0;
    printf("\nSigning out..\n");
}


connection_t connect(key_t server_key, int client_msqid, key_t client_key) {
    int server_msqid = safe_msgget(server_key, S_IWRITE);

    msgbuf msg = (msgbuf) {};
    msg.type = INIT;
    sprintf(msg.text, "0$%d", client_key);

    puts("Pinging server..");
    safe_msgsnd(server_msqid, &msg, MSG_MAX * sizeof(char), 0);

    puts("Waiting for response..");
    safe_msgrcv(client_msqid, &msg, MSG_MAX * sizeof(char), 0, MSG_NOERROR);

    return (connection_t) {
        .server_msqid = server_msqid,
        .client_msqid = client_msqid,
        .client_id = atol(msg.text)
    };
}


void check_for_messages(connection_t* connection) {
    msgbuf msg = {};
    struct msqid_ds info = {};

    safe_msgctl(connection->client_msqid, IPC_STAT, &info);

    printf("Pending messages: %ld\n", info.msg_qnum);
    
    if (info.msg_qnum == 0)
        return;

    for (long unsigned i = 0; i < info.msg_qnum; i++) {
        safe_msgrcv(connection->client_msqid, &msg, MSG_MAX * sizeof(char), 0, IPC_NOWAIT);
        
        if (!strcmp(msg.text, "shutdown")) {
            interrupt_handler(SIGINT);
            return;
        }

        printf("%s\n", msg.text);
    }
}


message_type parse(const char* command) {
    for (message_type i = first_cmd; i <= last_cmd; i++)
        if (!strcmp(command, commands[i - first_cmd]))
            return i;

    return invalid_cmd;
}


void send_request(const connection_t connection, message_type type, const char* text) {
    msgbuf msg = (msgbuf) {};
    msg.type = type;
    strcpy(msg.text, text);

    safe_msgsnd(connection.server_msqid, &msg, MSG_MAX * sizeof(char), 0);
}


message_type read_command(char* receiver_buffer, char* text_buffer) {
    char cmd_buffer[MSG_MAX];
    message_type type;

    printf(">");
    scanf("%s", cmd_buffer);

    switch (type = parse(cmd_buffer)) {
        case invalid_cmd:
            return invalid_cmd;

        case LIST:
        case STOP:
            return type;

        case TO_ONE:
            printf("To: ");
            scanf("%s", receiver_buffer);

        default:
            break;
    }

    printf("Message: ");
    scanf("%s", text_buffer);
    fflush(stdin);

    return type;
}


void combine(message_type type, const client_t id, const char* receiver_buffer, const char* text_buffer, char* msg_buffer) {
    if (type == TO_ONE)
        sprintf(msg_buffer, "%ld$%s;%s", id, receiver_buffer, text_buffer);
    else 
        sprintf(msg_buffer, "%ld$%s", id, text_buffer);
}


int main(const int argc, const char** argv) {
    if (argc != 2) {
        puts("Wrong arguments. Usage: ./client <server queue key>");
        exit(EXIT_FAILURE);
    }

    safe_sigaction(SIGINT, interrupt_handler);

    key_t client_key = safe_ftok("./client.c", 2);
    int client_msqid = safe_msgget(client_key, IPC_CREAT | S_IREAD | 0666);

    connection_t connection = connect(
        atoi(argv[1]),
        client_msqid,
        client_key
    );

    printf("Successfully connected! My server ID: %ld\n", connection.client_id);

    char receiver_buffer[MSG_MAX];
    char text_buffer[MSG_MAX];
    char msg_buffer[MSG_MAX];
    message_type type;

    running = 1;
    while (running) {
        type = read_command(receiver_buffer, text_buffer);
        if (type == invalid_cmd) continue;
        combine(type, connection.client_id, receiver_buffer, text_buffer, msg_buffer);
        send_request(connection, type, msg_buffer);
        check_for_messages(&connection);
    }

    msgbuf stop_msg = (msgbuf) {};
    stop_msg.type = STOP;
    sprintf(stop_msg.text, "%ld$", connection.client_id);
    safe_msgsnd(connection.server_msqid, &stop_msg, MSG_MAX * sizeof(char), 0);

    safe_msgctl(client_msqid, IPC_RMID, NULL);

    return 0;
}

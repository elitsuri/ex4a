/*
* ex4a3.c: message queue
*
*  Created on: Nov 29, 2017
*      Author: eliyho tsuri
*       Login: eliyhots
*		  Id: 201610672
*
*
*/
// -------------------- Include Section ----------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>  // for wait()
#include <signal.h>
// ----------------------- Define ----------------------------
#define MAX_TEXT 50
#define ARR_SIZE 5
// ---------------------- Struct -----------------------------
struct Data
{
	pid_t m_id;
	char m_command;
	int m_num;
	char m_text[MAX_TEXT];
};
struct My_Msg
{
	long m_type[ARR_SIZE];
	struct Data m_data;
};
// ------------------------ Prototype ------------------------
void get_command(struct Data *my_data);
// -------------------------- Main ---------------------------
int main(void)
{
	struct My_Msg my_msg;
	key_t key_server, key_apps;
	int msg_id_server, msg_id_apps;

	if ((key_server = ftok(".", 'a')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((msg_id_server = msgget(key_server, 0)) == -1)
	{
		perror("msg get failed");
		exit(EXIT_FAILURE);
	}


	if ((key_apps = ftok(".", 'b')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((msg_id_apps = msgget(key_apps, 0)) == -1)
	{
		perror("msg get failed");
		exit(EXIT_FAILURE);
	}


	my_msg.m_data.m_id = getpid();
	printf("%d Request to join the server\n", (int)getpid());
	get_command(&my_msg.m_data);



	while (my_msg.m_data.m_command != 'e')
	{
		if (msgsnd(msg_id_server, &my_msg, sizeof(struct Data), 0) == -1)
		{
			perror("msg snd failed");
			exit(EXIT_FAILURE);
		}
		if (msgrcv(msg_id_server, &my_msg, sizeof(struct Data), 1, 0) == -1)
		{
			perror("msg rcv failed");
			exit(EXIT_FAILURE);
		}
		if (my_msg.m_data.m_num == 1 || my_msg.m_data.m_num == 2)
		{
			perror("Can't able to join the registry server");
			exit(EXIT_FAILURE);
		}
		else
			printf("%d Got from server, user %d join\n", my_msg.m_data.m_num, getpid());


		if (my_msg.m_data.m_command == 'n' || my_msg.m_data.m_command == 's')
		{
			if (msgsnd(msg_id_apps, &my_msg, sizeof(struct Data), 0) == -1)
			{
				perror("msg snd failed");
				exit(EXIT_FAILURE);
			}
			if (msgrcv(msg_id_apps, &my_msg, sizeof(struct Data), 1, 0) == -1)
			{
				perror("msg rcv failed");
				exit(EXIT_FAILURE);
			}
			printf("%d \n", my_msg.m_data.m_num);
		}
		else
		{
			printf("User %d want's to end\n ", getpid());
			if (msgsnd(msg_id_server, &my_msg, sizeof(struct Data), 0) == -1)
			{
				perror("msg snd failed");
				exit(EXIT_FAILURE);
			}
		}
		get_command(&my_msg.m_data);
	}
	return(EXIT_SUCCESS);
}
//----------------------- get_command ------------------------
void get_command(struct Data *my_data)
{
	char text[MAX_TEXT];
	int number;
	puts("\nEnter Command: 'n', 's', 'e' to end \n");
	scanf("%c", &my_data->m_command);
	if (my_data->m_command == 'n')
	{
		puts("Enter number \n");
		scanf("%d", &number);
		my_data->m_num = number;
	}
	if (my_data->m_command == 's')
	{
		puts("Enter string \n");
		scanf("%s", &text[MAX_TEXT]);
	}
}
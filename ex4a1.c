/*
* ex4a1.c: message queue
*
*  Created on: Nov 29, 2017
*      Author: eliyho tsuri
*       Login: eliyhots
*		  Id: 201610672
*
*
*/
// ------------------- Include Section -----------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
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
// ---------------------- Prototype --------------------------
bool is_on_arr(long m_type[], pid_t pid);
void signal_handler(int signal);
// ------------------------ Main -----------------------------
int main(void)
{
	signal(SIGTTIN, signal_handler);

	struct My_Msg my_msg;
	key_t key_server, key_apps;
	int msg_id_server, msg_id_apps;
	int index = 0;

	if ((key_server = ftok(".", 'a')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	if ((msg_id_server = msgget(key_server, 0600 | IPC_CREAT | IPC_EXCL)) == -1)
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

	puts("Hallo from register server");
	while (1)
	{
		if (msgrcv(msg_id_server, &my_msg, sizeof(struct Data), 1, 0) == -1)
		{
			kill(getpid(), SIGTTIN);
			perror("msg rcv failed");
		}
		if (index < ARR_SIZE)
		{
			if (!is_on_arr(my_msg.m_type, my_msg.m_data.m_id))
			{
				my_msg.m_data.m_num = 0;
				my_msg.m_type[index] = my_msg.m_data.m_id;
				index++;
			}
			else
				my_msg.m_data.m_num = 1;
		}
		else
			my_msg.m_data.m_num = 2;

		if (msgsnd(msg_id_server, &my_msg, sizeof(struct Data), 0) == -1)
		{
			perror("msg snd failed");
			kill(getpid(), SIGTTIN);
		}
		if (msgrcv(msg_id_apps, &my_msg, sizeof(struct Data), 1, 0) == -1)
		{
			perror("msg rcv failed");
			kill(getpid(), SIGTTIN);
		}
		if (is_on_arr(my_msg.m_type, my_msg.m_data.m_id))
			my_msg.m_data.m_num = 1;
		else
			my_msg.m_data.m_num = 0;

		if (msgsnd(msg_id_apps, &my_msg, sizeof(struct Data), 0) == -1)
		{
			perror("msg snd failed");
			kill(getpid(), SIGTTIN);
		}
	}
	return (EXIT_SUCCESS);
}
// --------------------- is_on_arr ---------------------------
bool is_on_arr(long m_type[], pid_t pid)
{
	int index;
	bool on_arr = false;

	for (index = 0; index < ARR_SIZE; index++)
		if (m_type[index] == pid)
			on_arr = true;
	return on_arr;
}
// -------------------- signal_handler -----------------------
void signal_handler(int signal)
{
	msgctl(getpid(), IPC_RMID, NULL);
	exit(EXIT_FAILURE);
}
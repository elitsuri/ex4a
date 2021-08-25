/*
* ex4a2.c: message queue
*
*  Created on: Nov 29, 2017
*      Author: eliyho tsuri
*       Login: eliyhots
*		  Id: 201610672
*
*
*/
// --------------------- Include Section -----------------------
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
// ------------------------ Prototype --------------------------
bool is_on_arr(long m_type[], pid_t pid);
void do_command(struct My_Msg *my_msg);
bool check_prime(int num);
bool check_palindrome(char string[]);
void signal_handler(int signal);
// -------------------------- Main -----------------------------
int main(void)
{
	signal(SIGTTIN, signal_handler);

	struct My_Msg my_msg;
	key_t key_apps, key_server;
	int msg_id_apps, msg_id_server;

	if ((key_server = ftok(".", 'a')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	if ((msg_id_server = msgget(key_server, 0)) == -1)
	{
		perror("msg get failed");
		kill(getpid(), SIGTTIN);
	}

	if ((key_apps = ftok(".", 'b')) == -1)
	{
		perror("ftok failed");
		kill(getpid(), SIGTTIN);
	}
	if ((msg_id_apps = msgget(key_apps, 0600 | IPC_CREAT | IPC_EXCL)) == -1)
	{
		perror("msg get failed");
		kill(getpid(), SIGTTIN);
	}

	puts("Hallo from app server");


	while (1)
	{
		if (msgrcv(msg_id_apps, &my_msg, sizeof(struct Data), 1, 0) == -1)
		{
			perror("msg rcv failed");
			kill(getpid(), SIGTTIN);
		}
		if (msgsnd(msg_id_server, &my_msg, sizeof(struct Data), 0) == -1)
		{
			perror("msg snd failed");
			kill(getpid(), SIGTTIN);
		}

		if (my_msg.m_data.m_num == 1)
		{
			do_command(&my_msg);
			if (msgsnd(msg_id_server, &my_msg, sizeof(struct Data), 0) == -1)
			{
				perror("msg snd failed");
				kill(getpid(), SIGTTIN);
			}
		}
	}
	return(EXIT_SUCCESS);
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
//----------------------- do_command ---------------------------
void do_command(struct My_Msg *my_msg)
{
	if (my_msg->m_data.m_command == 'n')
	{
		if (check_prime(my_msg->m_data.m_num))
			my_msg->m_data.m_num = 0;
		else
			my_msg->m_data.m_num = 1;
	}
	else if (my_msg->m_data.m_command == 's')
	{
		if (check_palindrome(my_msg->m_data.m_text))
			my_msg->m_data.m_num = 0;
		else
			my_msg->m_data.m_num = 1;
	}
}
//----------------------- check_prime --------------------------
bool check_prime(int num)
{
	int div;

	for (div = 2; div <= num / 2; div++)
		if (num % div == 0)
			return false;
	return true;
}
//--------------------- check_palindrome -----------------------
bool check_palindrome(char string[])
{
	int start = 0;
	int end = strlen(string);

	while (start < end)
	{
		if (string[start] != string[end])
			return false;
		start++;
		end--;
	}
	return true;
}
// ---------------------- signal_handler -----------------------
void signal_handler(int signal)
{
	msgctl(getpid(), IPC_RMID, NULL);
	exit(EXIT_FAILURE);
}
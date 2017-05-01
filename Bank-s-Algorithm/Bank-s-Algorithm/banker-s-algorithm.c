#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "banker-s-algorithm.h"
#include "queue.h"

int g_avaliable[RESOURCENUM] = { 10, 15, 12 };
PCB *g_pcb_arr;
int g_length;
Queue *wq;
Queue *bq;
Queue *fq;

int input() 
{
	int num;
	int flag;

	printf("����������Ҫ�����Ľ�����Ŀ: \n");
	flag = scanf_s("%d", &num);
	getchar();//ȡ��scanf�����ڻ������Ļس���

	if (flag <= 0) {
		printf("ERROR: ������Ĳ���������");
		exit(-1);
	}

	return num;
}

void init_process(PCB *p, char *name)
{
	srand((unsigned)time(NULL));
	strcpy(p->name, name);
	for (int i = 0; i < RESOURCENUM; i++) {
		p->max[i] = rand() % 7;
		p->need[i] = p->max[i];
		p->allocation[i] = 0;
	}

	p->status = WAIT;//����״̬
	p->next = NULL;
	insert(wq, p);
}

void init_queue()
{
	wq = new_queue();
	bq = new_queue();
	fq = new_queue();
}

void generate_processes()
{
	char name_buff[10];
	g_length = input();
	
	init_queue();
	g_pcb_arr = (PCB*)malloc(g_length * sizeof(PCB));
	for (int i = 0; i < g_length; i++) {
		printf("�������%d�����̵�����:\n", i + 1);
		gets(name_buff);
		init_process(&g_pcb_arr[i], name_buff);
	}
}

bool safe()
{
	int num = queue_length(wq);
	int work[RESOURCENUM];
	int cnt = 0;
	PCB *tmp;

	tmp = wq->queue_head;
	while (tmp != NULL) {
		tmp->finish = false;
		tmp = tmp->next;
	}

	for (int i = 0; i < RESOURCENUM; i++) {
		work[i] = g_avaliable[i];
	}
nextsearch:
	tmp = wq->queue_head;
	for (int i = 0; i < num; i++) {
		if (tmp->finish == false && tmp->need[0] <= work[0] && tmp->need[1] <= work[1] && tmp->need[2] <= work[2]) {
			work[0] += tmp->allocation[0];
			work[1] += tmp->allocation[1];
			work[2] += tmp->allocation[2];
			tmp->finish = true;
			goto nextsearch;
		}
		tmp = tmp->next;
	}
	tmp = wq->queue_head;
	while (tmp != NULL) {
		if (tmp->finish == false) {
			break;
		}
		tmp = tmp->next;
	}
	if (tmp == NULL) {
		return true;
	}
	else {
		return false;
	}

}

void print(PCB *p)
{
	PCB *tmp;

	printf("** �������еĽ���:");
	printf("%s\n", p->name);
	printf("** �ȴ����еĽ���:");
	tmp = wq->queue_head;
	while (tmp != NULL) {
		printf("%s%c", tmp->name, (tmp->next == NULL ? '\n' : ' '));
		tmp = tmp->next;
	}
	printf("** �������еĽ���:");
	tmp = bq->queue_head;
	while (tmp != NULL) {
		printf("%s%c", tmp->name, (tmp->next == NULL ? '\n' : ' '));
		tmp = tmp->next;
	}
	printf("\n");
	printf("** ��ɶ��еĽ���:");
	tmp = fq->queue_head;
	while (tmp != NULL) {
		printf("%s%c", tmp->name, (tmp->next == NULL ? '\n' : ' '));
		tmp = tmp->next;
	}
	printf("\n");
	printf("-------------------------------------------- PCB��Ϣ -------------------------------------------\n");
	printf("------------------------------------------------------------------------------------------------\n");
	printf("������ MAX[A] MAX[B] MAX[C] ALLOCATION[A] ALLOCATION[B] ALLOCATON[C] NEED[A] NEED[B] NEED[C] ״̬\n");
	printf("------------------------------------------------------------------------------------------------\n");
	for (int i = 0; i < g_length; i++) {
		printf("%s %5d %6d %6d %7d %13d %13d %13d %7d %7d %7d\n", g_pcb_arr[i].name, g_pcb_arr[i].max[0], g_pcb_arr[i].max[1], g_pcb_arr[i].max[2], g_pcb_arr[i].allocation[0], g_pcb_arr[i].allocation[1], g_pcb_arr[i].allocation[2], g_pcb_arr[i].need[0], g_pcb_arr[i].need[1], g_pcb_arr[i].need[2], g_pcb_arr[i].status);
	}
	printf("------------------------------------------------------------------------------------------------\n");
}

void bankers_algo()
{
	srand((unsigned)time(NULL));
	int nowtime = 0;
	int timeslice = rand() % 4 + 1;
	PCB *run_pro = NULL;
	int request[RESOURCENUM];

next:
	while (!is_empty(wq)){
		run_pro = wq->queue_head;
		delete(wq, run_pro);
		run_pro->status = RUN;
		for (int i = 0; i < RESOURCENUM; i++) {
			request[i] = rand() % 3;
		}
		
		printf("************* ����%s�����������ԴRequest(%d, %d, %d) *************\n", run_pro->name, request[0], request[1], request[2]);
		printf("************* ϵͳ��ǰ������ԴAvailable(%d, %d, %d) *************\n", g_avaliable[0], g_avaliable[1], g_avaliable[2]);
		print(run_pro);
		for (int i = 0; i < RESOURCENUM; i++) {
			if (request[i] > run_pro->need[i]) {
				printf("ERROR: ����%s�������Դ���������������ֵ\n\n", run_pro->name);
				system("pause");
				run_pro->status = WAIT;
				insert(wq, run_pro);
				run_pro = NULL;
				goto next;
			}
			if (request[i] > g_avaliable[i]) {
				printf("ϵͳ��δ���㹻��Դ,����%s����ȴ�\n\n", run_pro->name);
				system("pause");
				run_pro->status = WAIT;
				insert(wq, run_pro);
				run_pro = NULL;
				goto next;
			}
		}
		//Ԥ����
		for (int i = 0; i < RESOURCENUM; i++) {
			g_avaliable[i] -= request[i];
			run_pro->allocation[i] += request[i];
			run_pro->need[i] -= request[i];
		}
		if (!safe()) {
			//ϵͳ����ȫ,����Ԥ����
			for (int i = 0; i < RESOURCENUM; i++) {
				run_pro->need[i] += request[i];
				run_pro->allocation[i] -= request[i];
				g_avaliable[i] += request[i];
			}
			printf("������Դ�����ϵͳ��ʧȥ��ȫ״̬, ϵͳ�ܾ�������Դ!\n\n");
			system("pause");
			run_pro->status = WAIT;
			insert(wq, run_pro);
			run_pro = NULL;
			goto next;//������һ����
		}
		else {
			printf("��Դ����ɹ���\n\n");
			system("pause");
		}
		for (int i = 0; i < RESOURCENUM; i++) {
			if (run_pro->need[i] > 0) {
				run_pro->status = WAIT;
				insert(wq, run_pro);
				run_pro = NULL;
				goto next;//������һ����
			}	
		}
		run_pro->status = FINISH;
		//���̽������ͷ���Դ���Ҳ��뵽��������
		for (int i = 0; i < RESOURCENUM; i++) {
			g_avaliable[i] += run_pro->allocation[i];
		}
		insert(fq, run_pro);
		run_pro = NULL;
	}
	printf("ϵͳ��ǰ������Դ(%d, %d, %d)\n", g_avaliable[0], g_avaliable[1], g_avaliable[2]);
	print(run_pro);
}


void destroy()
{
	free(fq);
	free(bq);
	free(wq);
	free(g_pcb_arr);
}

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

	printf("请输入您需要产生的进程数目: \n");
	flag = scanf_s("%d", &num);
	getchar();//取出scanf遗留在缓冲区的回车符

	if (flag <= 0) {
		printf("ERROR: 您输入的不是整数！");
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

	p->status = WAIT;//吸纳状态
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
		printf("请输入第%d个进程的名字:\n", i + 1);
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

	printf("** 正在运行的进程:");
	printf("%s\n", p->name);
	printf("** 等待队列的进程:");
	tmp = wq->queue_head;
	while (tmp != NULL) {
		printf("%s%c", tmp->name, (tmp->next == NULL ? '\n' : ' '));
		tmp = tmp->next;
	}
	printf("** 阻塞队列的进程:");
	tmp = bq->queue_head;
	while (tmp != NULL) {
		printf("%s%c", tmp->name, (tmp->next == NULL ? '\n' : ' '));
		tmp = tmp->next;
	}
	printf("\n");
	printf("** 完成队列的进程:");
	tmp = fq->queue_head;
	while (tmp != NULL) {
		printf("%s%c", tmp->name, (tmp->next == NULL ? '\n' : ' '));
		tmp = tmp->next;
	}
	printf("\n");
	printf("-------------------------------------------- PCB信息 -------------------------------------------\n");
	printf("------------------------------------------------------------------------------------------------\n");
	printf("进程名 MAX[A] MAX[B] MAX[C] ALLOCATION[A] ALLOCATION[B] ALLOCATON[C] NEED[A] NEED[B] NEED[C] 状态\n");
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
		
		printf("************* 进程%s正在申请的资源Request(%d, %d, %d) *************\n", run_pro->name, request[0], request[1], request[2]);
		printf("************* 系统当前可用资源Available(%d, %d, %d) *************\n", g_avaliable[0], g_avaliable[1], g_avaliable[2]);
		print(run_pro);
		for (int i = 0; i < RESOURCENUM; i++) {
			if (request[i] > run_pro->need[i]) {
				printf("ERROR: 进程%s请求的资源大于其宣布的最大值\n\n", run_pro->name);
				system("pause");
				run_pro->status = WAIT;
				insert(wq, run_pro);
				run_pro = NULL;
				goto next;
			}
			if (request[i] > g_avaliable[i]) {
				printf("系统尚未有足够资源,进程%s仍需等待\n\n", run_pro->name);
				system("pause");
				run_pro->status = WAIT;
				insert(wq, run_pro);
				run_pro = NULL;
				goto next;
			}
		}
		//预分配
		for (int i = 0; i < RESOURCENUM; i++) {
			g_avaliable[i] -= request[i];
			run_pro->allocation[i] += request[i];
			run_pro->need[i] -= request[i];
		}
		if (!safe()) {
			//系统不安全,撤销预分配
			for (int i = 0; i < RESOURCENUM; i++) {
				run_pro->need[i] += request[i];
				run_pro->allocation[i] -= request[i];
				g_avaliable[i] += request[i];
			}
			printf("本次资源分配后系统将失去安全状态, 系统拒绝分配资源!\n\n");
			system("pause");
			run_pro->status = WAIT;
			insert(wq, run_pro);
			run_pro = NULL;
			goto next;//调用下一进程
		}
		else {
			printf("资源分配成功！\n\n");
			system("pause");
		}
		for (int i = 0; i < RESOURCENUM; i++) {
			if (run_pro->need[i] > 0) {
				run_pro->status = WAIT;
				insert(wq, run_pro);
				run_pro = NULL;
				goto next;//调用下一进程
			}	
		}
		run_pro->status = FINISH;
		//进程结束后释放资源并且插入到结束队列
		for (int i = 0; i < RESOURCENUM; i++) {
			g_avaliable[i] += run_pro->allocation[i];
		}
		insert(fq, run_pro);
		run_pro = NULL;
	}
	printf("系统当前可用资源(%d, %d, %d)\n", g_avaliable[0], g_avaliable[1], g_avaliable[2]);
	print(run_pro);
}


void destroy()
{
	free(fq);
	free(bq);
	free(wq);
	free(g_pcb_arr);
}

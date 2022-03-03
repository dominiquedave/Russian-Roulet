/*Written by: Dave Dominique */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"

#define MAX_soldiers_per_group 10
#define MAX_LENGTH 50

typedef struct soldierNode
{   int sequenceNumber;
    struct soldierNode* next;
}soldierNode;

typedef struct queue 
{  //for the linked lists
    soldierNode* front;
    soldierNode* rear;
    int group_num;
    char group_name[MAX_LENGTH];
    int original_nodecount;
    int k; //killrate
    int th;//threshold
    //execution proccess
    int current_node_count;
}queue;

int isfull(queue *q)
{
    if(q->current_node_count == q->original_nodecount)
       return 1;
    return 0;
}

//gcc leak_detector_c.c main.c
//diff -y out.txt sample_out.txt

int empty(queue* q)
{
    if(q->front == NULL && q->rear == NULL)
    {
     return 1;
    }
    return 0;
}

void initialize(queue* q)
{
    q->original_nodecount = 0;
    q->front = NULL;
    q->rear = NULL;
}

void enqueue(queue *q, soldierNode* Soldier)
{   
    if(empty(q)) 
    { 
        q->front = Soldier;
        q->rear= Soldier;
        
        q->rear->next = q->front;
    }  

    else
    {
        q->rear->next = Soldier;
        q->rear = Soldier;
        q->rear->next = q->front; 
    }        
}


soldierNode* dequeue(queue* q, FILE* outfile)
{
    soldierNode* temp;
    if (q->front == NULL) 
    {
        return NULL;
    }
     
    temp = q->front;

    q->front = q->front->next;
    q->original_nodecount--;

    if (q->original_nodecount == 0)
    {
      q->front = NULL;
      q->rear = NULL;
    }
    
    printf("Executed Soldier %d from line %d\n", temp->sequenceNumber, q->group_num );
    fprintf(outfile, "Executed Soldier %d from line %d\n", temp->sequenceNumber, q->group_num );
    free(temp);
    return q->front;
}

//should get the information nessecary for each group (G<=10)
queue* file_inputs (int num_of_groups, FILE* infile)
{
    int Ground_number, N, k, th; 
    char Ground_name[MAX_LENGTH];
  
    queue *q = malloc(10 * sizeof(queue));

    for(int i = 0; i <10; i++)
    {
        initialize(&q[i]);
    }
    
    for(int G_index = 0; G_index < num_of_groups; G_index++)
    {
      fscanf(infile, "%d", &Ground_number);
      initialize(&q[Ground_number-1]);
     fscanf(infile, " %s %d %d %d", Ground_name, &N, &k, &th);
   
      //store your data at q[ground_number-1]
  
     q[Ground_number-1].group_num = Ground_number;

      //q[Ground_number-1].group_name = malloc(MAX_LENGTH * sizeof(char));
      
      strcpy(q[Ground_number-1].group_name, Ground_name);
      
      q[Ground_number-1].original_nodecount = N;
      q[Ground_number-1].k = k;
      q[Ground_number-1].th = th;
      
      
    } 
    return q;
}   

soldierNode* deletesoldierNode(soldierNode *t, soldierNode* temp, FILE* outfile )
{
  soldierNode* the_dead;

  t->next = temp->next;
  the_dead = temp;
  temp = temp->next;

  printf("Soldier# %d executed\n", the_dead->sequenceNumber);
  fprintf(outfile, "Soldier# %d executed\n", temp->sequenceNumber);
  free(the_dead);
  return temp;
}
    
void phase1(queue* q, FILE* outfile)
{
  //soldierNode* t_tracker
  // t->prev = t_tracker;
  //t_tracker->next = t->prev;

  soldierNode* t = q->front;
  soldierNode* temp;
  
  int front_flag, rear_flag;
  int ti = 1;

  if(empty(q))
  {
    printf("Cannot Execute.\n");
     fprintf(outfile,"Cannot Execute.\n");
  }

  while(q->original_nodecount > q->th) 
  {      
    front_flag = 0; rear_flag = 0;
    if(ti % q->k == 0)
    {
      if(t == q->front)
      {
        front_flag = 1;
      }

      if(t == q->rear)
      {
        rear_flag = 1;
      }
      
      t = deletesoldierNode(temp, t, outfile);

      if(front_flag == 1)
      {
        q->front = t;
      }

      if(rear_flag == 1)
      {
        q->rear = t;
      }

      q->original_nodecount--;
    }

    else
    {
      temp = t;
      t = t->next;
    
    }

    ti++;
  }   
}


 int getTotalNodes(queue* q)
{
  int total = 0;

  for(int i = 0; i < 10; i++)
  {
    total += q[i].original_nodecount;
  }
  return total;
}

int top(queue* q)
{
  if (empty(q))
  {
    return 0;
  }

  return q->front->sequenceNumber;
} 


void phase2(queue* q, FILE* outfile)
{
  int total_nodes = getTotalNodes(q);

  while(total_nodes > 1)
  {
    int max_solderNum = -1;
    int max_queue_i = -1;

    for(int i = 0; i < 10; i++)
    {
      if(!empty(&q[i]) )
      {
        if(top(&q[i]) > max_solderNum)
        {
          max_queue_i = i;
          max_solderNum = top(&q[i]);
        }
      }
    }
   
    soldierNode* dead;
    dead = dequeue(&q[max_queue_i], outfile);
    
    total_nodes--;
  }

  int survived;
  for(int i = 1; i < 10; i++)
  {
    if(!empty(&q[i]))
    {
      survived = i;
    }
  }
  
  printf("\n");
  fprintf(outfile,"\n");

  printf("Soldier %d from line %d will survive\n", q[survived].front->sequenceNumber,q[survived].group_num );
   fprintf(outfile,"Soldier %d from line %d will survive\n", q[survived].front->sequenceNumber,q[survived].group_num );
  
  

  free(q[survived].front);
 
  
  free(q);

}
//It takes an integer, dynamically allocate a soldierNode structure and returns a soldierNode node
soldierNode* createsoldierNode(int sequence)
{  
    soldierNode* Soldier = malloc(sizeof(struct soldierNode));
    Soldier->sequenceNumber = sequence;
    Soldier->next = NULL;
      
    return Soldier;
}

// It takes the reference of a queue, and creates a circular singly linked list for that queue 
//where the nodes contain sequence numbers in reverse order.
void createReverseCircle(queue *q)
{
  int counter  = q->original_nodecount; 
  for (int j = 0; j < q->original_nodecount; j++) 
    { 
      if (counter != 0 ) 
        {
          soldierNode*  Soldier = createsoldierNode(counter);
          enqueue(q, Soldier);
        }
      
      counter--;
    }  
}


//This function takes the reference of a queue and reverse the given circular singly linked list 
//where the first node of the linked list is pointed by the front of the queue
void rearrangeCircle(queue* q, FILE* outfile)
{
    // Cannot reverse empty list
    if (q->front == NULL)
    {
        printf("Cannot reverse empty list.\n");
        fprintf(outfile, "Cannot reverse empty list.\n");
        return;
    }

    soldierNode *prev, *cur, *next, *last;

    // the front of the queue is going to be our last node after reversing list
    last = q->front;

    prev  = q->front;
    cur = q->front->next;
    q->front = q->front->next;

    // Iterate till you reach the initial node in circular list
    while (q->front != last)
    {
        q->front = q->front->next;
        cur->next = prev;

        prev = cur;
        cur  = q->front;
    }

    cur->next = prev;
    q->front= prev;       // Make last node as the front of the queue
}


//This function displays a given queue
void display(queue q, FILE* outfile)
{
  
    if (q.front == NULL) 
        {
        printf("Queue is empty\n");
        fprintf(outfile,"Queue is empty\n");
        return;
    }
    
    soldierNode* t = q.front;
   
    printf("%d ", q.front->sequenceNumber);
    fprintf(outfile, "%d ", q.front->sequenceNumber);
    q.front = q.front->next;
    
    while (t != q.front) 
        {
        printf("%d ", q.front->sequenceNumber);
        fprintf(outfile,"%d ", q.front->sequenceNumber);
        q.front = q.front->next;
    }
    printf("\n");
    fprintf(outfile,"\n");
}


int main()
{
    atexit(report_mem_leak);
    FILE* infile = fopen("in.txt", "r");
    FILE* outfile = fopen("out.txt", "w");
   
    queue* q;
    int i; 
    int num_of_groups;
    int G_index;
 
  if (infile != NULL)
	{
    fscanf(infile, "%d", &num_of_groups);
    q = file_inputs(num_of_groups, infile);
    
    printf("Initial nonempty lists status\n");
    fprintf(outfile,"Initial nonempty lists status\n");
    for (int i = 0; i < 10; i++)
    {
      if (q[i].group_num > 0) 
      {
        printf("%d  %s ", q[i].group_num, q[i].group_name );
        fprintf(outfile,"%d  %s ", q[i].group_num, q[i].group_name );
        createReverseCircle(&q[i]);
        display(q[i],outfile);
        
       }
    } 

    printf("\n");
    fprintf(outfile,"\n");

    printf("After ordering nonempty lists status\n");
    fprintf(outfile,"After ordering nonempty lists status\n");
    for (int i = 0; i < 10; i++)
    {
      if (q[i].group_num > 0) 
      {
        printf("%d  %s ", q[i].group_num, q[i].group_name );
        fprintf(outfile, "%d  %s ", q[i].group_num, q[i].group_name );
        rearrangeCircle(&q[i], outfile);
        display(q[i], outfile);
       }
    } 

    printf("\n");
    fprintf(outfile,"\n");
    
 
    printf("Phase1 execution\n\n");
    fprintf(outfile,"Phase1 execution\n\n");
    for (int j = 0; j < 10; j++)
    {
      if (q[j].group_num > 0) 
      {
        printf("Line# %d %s\n", q[j].group_num, q[j].group_name );
        fprintf(outfile,"Line# %d %s\n", q[j].group_num, q[j].group_name );

        phase1(&q[j], outfile);
       
        printf("\n");
        fprintf(outfile,"\n");

       }
    }

   
    printf("Phase2 execution\n");
    fprintf(outfile,"Phase2 execution\n");
    
   
      
        phase2(q, outfile);
      
    for (int i = 0; i < num_of_groups; i++){
        if(!empty(&q[i]))
     {
         free(q[i].group_name);
        }
    }
    
    free(q);

    fclose(infile);
  }

  else
  {
      printf("File could not be read\n");
      fprintf(outfile,"File could not be read\n");
      exit(-1);
  }

    fclose(outfile);
    return 0;    
}



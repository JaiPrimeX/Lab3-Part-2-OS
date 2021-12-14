#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void  Student(int []);
void  Dad(int []);

int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;

     ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     printf("Process has received a shared memory of two integers...\n");

     ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (*ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     printf("Process has attached the shared memory...\n");

     ShmPTR[0] = 0;
     ShmPTR[1] = 0;
    
     printf("Process has filled %d %d in shared memory...\n",
            ShmPTR[0], ShmPTR[1]);

     printf("Process is about to fork a child process...\n");
     pid = fork();
     if (pid < 0) {
          printf("*** fork error (server) ***\n");
          exit(1);
     }
     else if (pid == 0) {
          Student(ShmPTR);
          exit(0);
     }
     else{
       Dad(ShmPTR);
     }

     wait(&status);
     printf("Process has detected the completion of its child...\n");
     shmdt((void *) ShmPTR);
     printf("Process has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Process has removed its shared memory...\n");
     printf("Process exits...\n");
     exit(0);
}

void Dad(int SharedMem[]){
  int i = 0;
  int account;
  int amount;
  
  for(i=0; i<25; i++){
    sleep(rand()% 6);
    account = SharedMem[0];
    while (SharedMem[1]!=0){};
    if(account <= 100){
      amount = rand() % 101;
      if(amount % 2==0){
        account = account + amount;
        printf("Dear old Dad: Deposits $%d / Balance -> $%d\n", amount, account);
      }
      else{
        printf("Dear old Dad: Doesn't have any money to give \n");
      }
      SharedMem[0] = account;
    }
    else{
      printf("Dear old Dad: Thinks Student has enough cash ($%d)\n",account);
    }
    
    SharedMem[1] = 1;
  }
}

void  Student(int  SharedMem[])
{
  int i = 0;
  int account = SharedMem[0];
  int amountNeed;
  for (i=0; i<25; i++){
    sleep(random() % 5);
    while(SharedMem[1]!=1){}
    account = SharedMem[0];
    amountNeed = random() % 51;
    printf("Poor Student needs $%d\n", amountNeed);
    if (amountNeed <= account){
      account = account - amountNeed;
      printf("Poor Student: Withdraws $%d Account Balance = %d\n", amountNeed, account);
    }
    else{
      printf("Poor Student: Not enough Cash ($%d)\n", account);
    }
    SharedMem[0] = account;
    SharedMem[1] = 0;
  }
}
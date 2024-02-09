/**
 * Código base (incompleto) para implementação de relógios vetoriais.
 * Meta: implementar a interação entre três processos ilustrada na figura
 * da URL: 
 * 
 * https://people.cs.rutgers.edu/~pxk/417/notes/images/clocks-vector.png
 * 
 * Compilação: mpicc -o rvet rvet.c
 * Execução:   mpiexec -n 3 ./rvet
 */
 
#include <stdio.h>
#include <string.h>  
#include <mpi.h>     


typedef struct Clock { 
   int p[3];
} Clock;

//Função auxiliar para printar o clock de um processo
void printClock(int pid, Clock *clock){
   printf("[ Process: %d, Clock: (%d, %d, %d) ]\n", pid, clock->p[0], clock->p[1], clock->p[2]);
}

void Event(int pid, Clock *clock){
   // Evento interno: Incremeta o próprio relógio
   clock->p[pid]++;
   printClock(pid, clock);
}


void Send(int pidSender, int pidReceiver, Clock *clock){
   //Enviando uma mensagem: imcrementa o próprio relógio
   clock->p[pidSender]++;
   // Envia mensagem para Reciever
   MPI_Send(clock, 3, MPI_INT, pidReceiver, 0, MPI_COMM_WORLD);
   printClock(pidSender, clock); 
}

void Receive(int pidReceiver, int pidSender, Clock *clock){
   //Recebendo um par mensagem-clock: incremeta o próprio relógio
   clock->p[pidReceiver]++;
   //Recebe a mensagem
   int clockSender[3]; //Array com o clock do sender
   MPI_Recv(clockSender, 3, MPI_INT, pidSender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   // Para cada index do relógio interno, atualiza o valor para o maior entre o 
   // valor atual e o valor do relógio do sender
   for(int i = 0; i < 3; i++){
      if(clockSender[i] > clock->p[i]){
         clock->p[i] = clockSender[i];
      }
   }
   printClock(pidReceiver, clock); 
}

// Representa o processo de rank 0
void process0(){
   Clock clock = {{0,0,0}};
   Event(0, &clock);
   Send(0, 1, &clock);
   Receive(0, 1, &clock);
   Send(0, 2, &clock);
   Receive(0, 2, &clock);
   Send(0, 1, &clock);
   Event(0, &clock);
}

// Representa o processo de rank 1
void process1(){
   Clock clock = {{0,0,0}};
   printClock(1, &clock);
   Send(1, 0, &clock);
   Receive(1, 0, &clock);
   Receive(1, 0, &clock);
}

// Representa o processo de rank 2
void process2(){
   Clock clock = {{0,0,0}};
   printClock(2, &clock);
   Event(2, &clock);
   Send(2, 0, &clock);
   Receive(2, 0, &clock);
}

int main(void) {
   int my_rank;               

   MPI_Init(NULL, NULL); 
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
   if (my_rank == 0) { 
      process0();
   } else if (my_rank == 1) {  
      process1();
   } else if (my_rank == 2) {  
      process2();
   }

   /* Finaliza MPI */
   MPI_Finalize(); 

   return 0;
}  /* main */

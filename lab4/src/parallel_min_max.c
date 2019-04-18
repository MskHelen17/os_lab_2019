#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

pid_t currentPID;

void KillChildProcess(int sig){
    int result = kill(currentPID, SIGKILL);
    if (result == 0)
        printf ("child process was successfully killed\n");
    else
        printf ("error while killing a child process\n");
}


//argc - кол-во параметров
//argv - параметры
int main(int argc, char **argv) {
  int timeout = -1;
  int seed = -1; //для srand
  int array_size = -1; //размер сгенерированного массива
  int pnum = -1; //кол-во процессов
  bool with_files = false; //для синхронизации процессов использовать файлы, в противном случае pipe



  while (true) {
    //optind - индекс на следующий указатель argv, который будет обработан при следующем вызове getopt().
    int current_optind = optind ? optind : 1; 

    /*
     struct option {
    	const char *name;
    	int has_arg;   //определяет нужно ли для этого параметра значение
    	int *flag; //указатель на флаг, в который помещается значение val, если найден данный параметр
    	int val;   //содержит значение, которое помещается в flag или возвращается в качестве результата работы функции
     };
     
     Последняя запись массива longopts должна содержать нулевые значения, 
     для того чтобы функция могла однозначно определить конец массива
     */
    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},  
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) {
                printf("seed must be a positive number\n");
                return 1;
              }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0) {
                printf("array_size must be a positive number\n");
                return 1;
              }
            break;
            
          case 2:
            pnum = atoi(optarg);
            if (pnum <= 0) {
                printf("pnum must be a positive number\n");
                return 1;
              }
            break;
            
          case 3:
            with_files = true;
            break;
            
          case 4: 
            timeout = atoi(optarg);
            if (timeout < 0){
                printf("timeout is a positive number\n");
                return 1;
            }
            break;
            
          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
     
        break;  
      case 'f':
        with_files = true;
        break;

      case '?':
        break;
        
      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }




  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  //генерируем случайный массив
  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;
  //засекаем время
  struct timeval start_time;
  gettimeofday(&start_time, NULL);


  int fd[2];  
  pipe(fd);   //создаем pipe
  int part = array_size/pnum;  //по сколько эл в массиве на поток
  
 //от нуля до кол-ва процессов
  for (int i = 0; i < pnum; i++) {
      
    pid_t child_pid = fork(); //порождаем дочерний процесс
    currentPID = child_pid;
    
    if (child_pid >= 0) {
      // successful fork
       
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process
        struct MinMax tmp;
        
        //находим минимум/максимум на данном кусочке массива
        if(i != pnum - 1){  //если это не последний процесс
            tmp = GetMinMax(array, i*part, (i+1)*part);
        }
        else tmp = GetMinMax(array, i*part, array_size);
        
       
         printf("Parent process PID: %d\t child process PID: %d\t\n", getppid(), getpid());
        
        if (with_files) {
          FILE* fp = fopen("numbers.txt", "a");
          fwrite(&tmp, sizeof(struct MinMax), 1, fp);
          fclose(fp);
        } else {
           write(fd[1], &tmp, sizeof(struct MinMax));
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  while (active_child_processes > 0) {
      
    if (timeout != -1){
        //ualarm приводит к отправке сигнала SIGALRM вызывающему процессу (не менее чем) через usecs микросекунд
        //Если аргумент interval не равен нулю, то дальнейшие сигналы SIGALRM будут отправляться каждые interval микросекунд после первого
        
        signal (SIGALRM, KillChildProcess); //по срабатыванию таймера вызываем функцию KillChildProcess
        //ualarm (timeout*1000, 0);   // --timeout ?ms
        alarm(timeout);
        sleep(2);
    }
    
    //WNOHANG - немедленный возврат если ни один потомок не завершил выполнение
     /*int status = 0;
     pid_t wpid = waitpid(-1,&status,WNOHANG);
     if(status){
        printf("Child PID %d was successfuly killed", wpid);
}*/


    close(fd[1]);
    wait(NULL);
    
    active_child_processes --;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
  struct MinMax min_max2;

    if (with_files) {
        FILE* fp = fopen("numbers.txt", "rb");
        fseek(fp, i*sizeof(struct MinMax), SEEK_SET);
        fread(&min_max2, sizeof(struct MinMax), 1, fp);
        fclose(fp);
    } else {
        read(fd[0], &min_max2, sizeof(struct MinMax));
    }

    if ( min_max2.min < min_max.min) min_max.min =  min_max2.min;
    if ( min_max2.max > min_max.max) min_max.max =  min_max2.max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("\nMin: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %f ms\n\n", elapsed_time);
  fflush(NULL);
  remove("numbers.txt");
  return 0;
}

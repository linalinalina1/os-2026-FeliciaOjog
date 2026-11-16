#include "kernel/types.h"
#include "user/user.h"

// Print prime numbers using a chain of processes and pipes
int
main(int argc, char *argv[])
{
  int p[2];

  if(pipe(p) < 0){
    printf("pipe failed\n");
    exit(1);
  }

  int pid = fork();

  if(pid < 0){
    printf("fork failed\n");
    close(p[0]);
    close(p[1]);
    exit(1);
  }

  if(pid == 0){
    close(p[1]);

    int input = p[0];
    int prime;
    int number;

    while(read(input, &prime, sizeof(prime)) == sizeof(prime)){
      printf("prime %d\n", prime);

      int next[2];

      if(pipe(next) < 0){
        printf("pipe failed\n");
        close(input);
        exit(1);
      }

      pid = fork();

      if(pid < 0){
        printf("fork failed\n");
        close(input);
        close(next[0]);
        close(next[1]);
        exit(1);
      }

      if(pid == 0){
        close(next[1]);
        close(input);
        input = next[0];
        continue;
      }

      close(next[0]);

      while(read(input, &number, sizeof(number)) == sizeof(number)){
        if(number % prime != 0){
          write(next[1], &number, sizeof(number));
        }
      }

      close(input);
      close(next[1]);
      wait(0);
      exit(0);
    }

    close(input);
    exit(0);
  }

  close(p[0]);

  for(int i = 2; i <= 35; i++){
    write(p[1], &i, sizeof(i));
  }

  close(p[1]);
  wait(0);

  exit(0);
}

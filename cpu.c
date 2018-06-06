#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// machine info
#define NUM_REGISTERS 16
#define MEMORY_SIZE 0x10000
#define STATUS 0
#define SP 14
#define PC 15
#define OVERFLOW_BIT 0x00000001

// errors
#define ERR_INSTR_INVALID 1
#define ERR_INSTR_NOT_IMPLEMENTED 2
#define ERR_INSTR_HALT 3

int32_t memory[MEMORY_SIZE];
int32_t registers[NUM_REGISTERS];

#define MAX_ARGS 10
#define MAX_ARG_SIZE 80
#define BUF_SIZE 81

int32_t
cpu_register_get(int r)
{
  return registers[r];
}

int
cpu_register_set(int r, int32_t value)
{
  registers[r] = value;
  return 0;
}

void
cpu_register_dump(int reg)
{
  if (reg >= 0 && reg < NUM_REGISTERS) {
    printf("r%x = %08x\n", reg, cpu_register_get(reg));
  } else {
    printf("Illegal register\n");
  }
}

void
cpu_register_dump_all()
{
  for(int i = 0; i < NUM_REGISTERS; i++) {
    cpu_register_dump(i);
  }
  return;
}

int
cpu_register_scan()
{
  int reg;
  char buf[BUF_SIZE];
  while(1) {
    printf("R#> ");
    fgets(buf, BUF_SIZE, stdin);
    sscanf(buf, "%x", &reg);

    if ((reg >= 0) && reg < NUM_REGISTERS) {
      return reg;
    } else {
      printf("Illegal register\n");
    }
  }
}

int
cpu_argument_count(char *buffer, char argv[][MAX_ARG_SIZE])
{
  int argc = 0;
  char *str = strtok(buffer, " \n");

  while (str != NULL) {
    strcpy(&argv[argc][0], str);
    argc++;
    str = strtok(NULL, " \t\n");
  }

  return argc;
}

void
cpu_usage()
{
  printf("Commands:\n");
  printf("h   - list available commands\n");
  printf("r   - Print register\n");
  printf("ra  - Print all registers\n");
  printf("ld  - Load value into register\n");
  printf("m   - Print memory address content\n");
  printf("s   - step into next instruction\n");
  printf("t   - execute test\n");
}

void
cpu_init()
{
  char buf[BUF_SIZE];
  char argv[MAX_ARGS][MAX_ARG_SIZE];
  int argc = 0;
  int running = 1;

  do {
    printf("> ");
    fgets(buf, BUF_SIZE, stdin);
    argc = cpu_argument_count(buf, argv);

    if (argc == 0) {
      continue;
    }

    if (strcmp(argv[0], "q") == 0) {
      running = 0;
      break;
    } else if (strcmp(argv[0], "r") == 0) {
      if (argc > 1) {
        int reg;
        sscanf(argv[1], "%x", &reg);
        cpu_register_dump(reg);
      } else {
        int reg = cpu_register_scan();
        cpu_register_dump(reg);
      }
    } else if (strcmp(argv[0], "ra") == 0) {
      cpu_register_dump_all();
    } else if (strcmp(argv[0], "h") == 0) {
      cpu_usage();
    } else if (strcmp(argv[0], "ld") == 0) {
      if (argc < 2) {
        printf("wrong number of arguments\n  example: ld 0 0x100000\n");
        continue;
      }

      int r;
      int32_t v;

      sscanf(argv[1], "%x", &r);
      sscanf(argv[2], "%x", &v);

      cpu_register_set(r, v);
    }

  } while (running);

  printf("bye!\n");
}

int
main(int argc, char *argv[])
{
  printf("Welcome to interactive CPU!\n");
  cpu_init();
  return EXIT_SUCCESS;
}

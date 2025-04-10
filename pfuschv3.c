#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>


typedef struct command_line_arguments
{
    int i;
    const char *s;
    bool b;
} cli_args;

/*
cli_args parse_command_line(int const argc, char *argv[])
{
    cli_args args = {0, NULL, false};

    int optgot = -1;

    do
    {
        optgot = getopt(argc, argv, "i:s:b");
        switch(optgot)
        {
            case 'i':
                args.i = atoi(optarg);
                break;
            case 's':
                args.s = optarg;
                break;
            case 'b':
                args.b = true;
                break;
            case '?':
                printf("Usage: %s -i <number> -s <string> -b\n", argv[0]);
                exit(EXIT_FAILURE);
            }
        } while (optgot != -1);

        if(args.i <= 0 || strlen(args.s) <5)
        {
            printf("\nusage: i: %d, s: %s, b: %d\n", args.i, args.s, args.b);
            exit(EXIT_FAILURE);
        }

        return args;
}
*/

int child_labour()
{
    mqd_t command_queue = mq_open("/mq_1234", O_WRONLY);
    printf("[%d] mq_open returned %d\n", getpid(), command_queue);
    if (command_queue == -1)
    {
        printf("failed to message queue");
        return EXIT_FAILURE;
    }

    printf("[%d] waiting for instructions\n", getpid());
    struct work_message instructions;
    int const received = mq_receive(command_queue, &instructions, sizeof(struct work_message));
    if (received == -1)
    {
        fprintf(stderr, "failed to receive instructions. running away\n");
        return EXIT_FAILURE;
    }
    printf("[%d] received message of size %d bytes: work to do %d\n", getpid(), )

    printf("[%d] doing some work for (%d)...\n", getpid(), getppid());
    sleep(instructions.work);
    printf("[%d] jobs done\n", getpid());
    printf("[%d] Bringing coal to %d...\n", getpid(), getppid());

    mq_close(command_queue);

    // *(int *)(17) = 0; // führt zu absturz

    struct work_message
    {
        int work;
    };
}

int main(int argc, char *argv[])
{
    //cli_args const args = parse_command_line(argc, argv);
    //printf("\ni: %d, s: %s, b: %d\n", args.i, args.s, args.b);

    struct mq_attr queue_options = {
        .mq_maxmsg = 1;
        .mq_msgsize = sizeof(struct work_message);
    };

    mqd_t command_queue = mq_open("/mq_r47587", O_WRONLY | O_CREAT, S_IRWXU, &queue_options);
    printf("[%d] mq_open returned %d\n", getpid(), command_queue);
    if (command_queue == -1)
    {
        fprintf(stderr, "Failed")
        return EXIT_FAILURE;
    }


    mqd_t command_queue = mq_open("/mq_r47587", O_RDONLY);
    printf("[%d] mq_open returned %d\n", getpid(), command_queue);
    if (command_queue == -1)
    {
        return EXIT_FAILURE;
    }

    printf("my PID is %d, I'm child of %d\n", getpid(), getppid());

    pid_t forked = fork();

    if (forked == 0)
    {
        return child_labour();
    }

    struct work_message instructions = { .work = 3 + rand() % 7 };
    int const sent = mq_send(command_queue, (void *)&instructions, sizeof)

    if (sent == 1)
    {
        
    }
    printf("[%d] enjoying some brandy...\n", getpid());
    printf("[%d], where the fudge is my coal?\n", getpid());

   for (int i = 0; i < 10; ++i)
   { 
        int wstatus = 0;
        pid_t const waited = wait(&wstatus);
    
        if (WIFEXITED(wstatus))
        {
            printf("[%d] child %d exited normally with return code %d\n", getpid(), waited, WEXITSTATUS(wstatus));
        }
        else if(WIFSIGNALED(wstatus))
        {
            printf("[%d] child %d terminated with signal %d\n", getpid(), waited, WTERMSIG(wstatus));
        }
        else
        {
            printf("[%d] child %d, status is %d\n", getpid(), waited, WTERMSIG(wstatus));
        }

        printf("[%d] wait returned %d, status is %d\n", getpid(), waited, wstatus);
    }

    mq_close(command_queue);
    mq_unlink("/mq_r47587");

    return 0;
}
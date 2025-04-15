#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <mqueue.h>
#include <fcntl.h>
#include <errno.h>

// Type of message (Task- and Resultmessage)
#define MSG_TYPE_TASK 1
#define MSG_TYPE_RESULT 2

// Structure for message
struct message
{
    int type; // 1 = Task, 2 = Result

    union
    {
        struct
        {
            int effort; // 1–10, or 0 as termination indicator
        } task;

        struct
        {
            int worker_id;
            pid_t pid;
            int tasks_done;
            int total_time;
        } result;
    };
};

// Worker-Function
void worker_function(int worker_id, const char* queue_name)
{
    mqd_t queue = mq_open(queue_name, O_RDWR);
    if (queue == -1)
    {
        perror("Worker mq_open");
        exit(EXIT_FAILURE);
    }

    int tasks_done = 0;
    int total_time = 0;

    while (1)
    {
        struct message msg = {0};
        ssize_t bytes = mq_receive(queue, (char*)&msg, sizeof(msg), NULL);
        if (bytes >= 0 && msg.type == MSG_TYPE_TASK)
        {
            if (msg.task.effort == 0)
            {
                printf("Worker #%02d | Received termination task\n", worker_id);

                // Send result
                struct message res = {0};
                res.type = MSG_TYPE_RESULT;
                res.result.worker_id = worker_id;
                res.result.pid = getpid();
                res.result.tasks_done = tasks_done;
                res.result.total_time = total_time;

                mq_send(queue, (const char*)&res, sizeof(res), 0);
                break;
            }
            
            else
            {
                printf("Worker #%02d | Received task with effort %d\n", worker_id, msg.task.effort);
                sleep(msg.task.effort);
                tasks_done++;
                total_time += msg.task.effort;
            }
        }
    }

    mq_close(queue);
}

int main(int argc, char *argv[])
{
    int workers = 0, tasks = 0, queue_size = 0;
    char queue_name[] = "/task_queue_ventilator";

    // getopt to parse Arguments
    int opt;
while ((opt = getopt(argc, argv, "w:t:s:")) != -1)
{
    switch (opt)
    {
        case 'w':
            workers = atoi(optarg);
            break;
        case 't':
            tasks = atoi(optarg);
            break;
        case 's':
            queue_size = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Usage: %s -w <workers> -t <tasks> -s <queue_size>\n", argv[0]);
            exit(EXIT_FAILURE);
    }
}

// Error Handling
if (workers <= 0 || tasks <= 0 || queue_size <= 0)
{
    fprintf(stderr, "Usage: %s -w <workers> -t <tasks> -s <queue_size>\n", argv[0]);
    exit(EXIT_FAILURE);
}

    // Queue-Attributes
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = queue_size,
        .mq_msgsize = sizeof(struct message),
        .mq_curmsgs = 0
    };


    // Create Message Queue
    mqd_t queue = mq_open(queue_name, O_CREAT | O_RDWR, 0644, &attr);
    if (queue == -1)
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    printf("Ventilator | Starting %d workers for %d tasks and a queue size of %d\n", workers, tasks, queue_size);

    pid_t worker_pids[workers + 1];

    // Start the workers
    for (int i = 1; i <= workers; ++i)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            worker_function(i, queue_name);
            exit(EXIT_SUCCESS);
        }
        else if (pid > 0)
        {
            worker_pids[i] = pid;
            printf("Worker #%02d | Started worker PID %d\n", i, pid);
        }
        else
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }


    // Distribute the tasks
    printf("Ventilator | Distributing tasks\n");
    srand(time(NULL));
    for (int i = 1; i <= tasks; ++i)
    {
        struct message msg = {0};
        msg.type = MSG_TYPE_TASK;
        msg.task.effort = (rand() % 10) + 1;

        printf("Ventilator | Queuing task #%d with effort %d\n", i, msg.task.effort);
        mq_send(queue, (const char*)&msg, sizeof(msg), 0);
    }

    // send Termination-Tasks
    printf("Ventilator | Sending termination tasks\n");
    for (int i = 0; i < workers; ++i)
    {
        struct message term = {0};
        term.type = MSG_TYPE_TASK;
        term.task.effort = 0;

        while (mq_send(queue, (const char*)&term, sizeof(term), 0) == -1)
        {
            if (errno == EAGAIN) // Error Again -> Try again after sleep
            {
                sleep(1);
            }

            else
            {
                perror("mq_send termination");
                exit(EXIT_FAILURE);
            }
        }
    }

    // Collect results
    for (int i = 0; i < workers; ++i)
    {
        struct message res = {0};
        ssize_t bytes = mq_receive(queue, (char*)&res, sizeof(res), NULL);
        if (bytes >= 0 && res.type == MSG_TYPE_RESULT)
        {
            printf("Ventilator | Worker %d processed %d tasks in %d seconds (PID %d)\n", res.result.worker_id, res.result.tasks_done,
                res.result.total_time,
                res.result.pid);
        }
    }

    // Wait for processes
    for (int i = 0; i < workers; ++i)
    {
        int status;
        pid_t pid = wait(&status);

        // Find correct WorkerID for the right PID
        int wid = -1;
        for (int j = 1; j <= workers; ++j)
        {
            if (worker_pids[j] == pid)
            {
                wid = j;
                break;
            }
        }

        if (wid != -1)
        {
            printf("Ventilator | Worker %d with PID %d exited with status %d\n", wid, pid, WEXITSTATUS(status));
        }
        else
        {
            printf("Ventilator | Unknown worker with PID %d exited with status %d\n", pid, WEXITSTATUS(status));
        }
    }

    // Clean up
    mq_close(queue);
    mq_unlink(queue_name);

    return 0;
}

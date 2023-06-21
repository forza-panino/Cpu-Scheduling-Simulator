#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_BURSTS 3
#define BURST_DATA_SIZE 3

void scan(int datas[][MAX_BURSTS][BURST_DATA_SIZE], int pids[], FILE* output_file) {

    int pid = -1;
    char *buffer=NULL;
    size_t line_length=0;

    while (getline(&buffer, &line_length, output_file) >0){

        if (sscanf(buffer, "\trunning pid: %d", &pid) == 1) continue;

        int old_prediction = -1;
        if (sscanf(buffer, "\t\told prediction: %d", &old_prediction) == 1) {

            assert(pid != -1);
            assert(old_prediction != -1);

            int real_duration = -1;
            getline(&buffer, &line_length, output_file);
            sscanf(buffer, "\t\treal duration: %d", &real_duration);
            assert(real_duration != -1);

            int new_prediction = -1;
            getline(&buffer, &line_length, output_file);
            sscanf(buffer, "\t\tnew prediction: %d", &new_prediction);
            assert(new_prediction != -1);

            assert(datas[pid-1][pids[pid-1]][0] == old_prediction);
            assert(datas[pid-1][pids[pid-1]][1] == real_duration);
            assert(datas[pid-1][pids[pid-1]][2] == new_prediction);

            pids[pid-1]++;
        }
    }
}

void test1() {

    int datas[4][3][3] = {
        {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}},
        {{1, 10, 4}, {4, 5, 4}, {4, 10, 6}},
        {{1, 3, 2}, {2, 3, 2}, {2, 3, 2}},
        {{1, 3, 2}, {2, 3, 2}, {2, 3, 2}}
    };

    int pids[4] = {0, 0, 0, 0};
    int pipefd[2];
    pipe(pipefd);

    int pid = fork();
    if (pid == 0) {
        
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        close(pipefd[1]);

        char* args[6] = {
            "./sched_sim.out", 
            "examples/example1/p1.txt", 
            "examples/example1/p2.txt", 
            "examples/example1/p3.txt", 
            "examples/example1/p4.txt", 
            NULL
        };

        execvp(args[0], args);
        
    } else {

        int status;
        waitpid(pid, &status, 0);
        assert(status == 0);

        close(pipefd[1]);
        FILE* output_file = fdopen(pipefd[0], "r");

        scan(datas, pids, output_file);
        close(pipefd[0]);
    }
}


void test2() {
    
    int datas[4][3][3] = {
        {{1, 3, 2}, {2, 5, 3}, {3, 1, 2}},
        {{1, 10, 4}, {4, 10, 6}, {6, 3, 5}},
        {{1, 3, 2}, {2, 3, 2}, {2, 1, 2}},
        {{1, 3, 2}, {2, 6, 3}, {3, 9, 5}}
    };

    int pids[4] = {0, 0, 0, 0};
    int pipefd[2];
    pipe(pipefd);

    int pid = fork();

    if (pid == 0) {
        
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        close(pipefd[1]);

        char* args[6] = {
            "./sched_sim.out", 
            "examples/example2/p1.txt", 
            "examples/example2/p2.txt", 
            "examples/example2/p3.txt", 
            "examples/example2/p4.txt", 
            NULL
        };

        execvp(args[0], args);
        
    } else {

        int status;
        waitpid(pid, &status, 0);
        assert(status == 0);

        close(pipefd[1]);
        FILE* output_file = fdopen(pipefd[0], "r");

        scan(datas, pids, output_file);
        close(pipefd[0]);
    }
}

int main(int argc, char** argv) {
    test1();
    test2();
}
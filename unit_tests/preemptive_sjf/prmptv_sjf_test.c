#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>

void test() {

    int pipefd[2];
    pipe(pipefd);

    int pid = fork();
    if (pid == 0) {

        close(pipefd[0]);
        dup2(pipefd[1], 1);
        close(pipefd[1]);

        char* args[6] = {
            "./sched_sim.out", 
            "example/p1.txt", 
            "example/p2.txt", 
            "example/p3.txt", 
            "example/p4.txt", 
            NULL
        };

        execvp(args[0], args);
    }
    else {
        int status;
        waitpid(pid, &status, 0);
        assert(status == 0);

        close(pipefd[1]);
        FILE* output_file = fdopen(pipefd[0], "r");
        FILE* expected_output_file = fopen("example/test.out.cmp", "r");
        
        char output_char;
        char expected_output_char;
        while (fscanf(output_file, "%c", &output_char) != EOF) {
            fscanf(expected_output_file, "%c", &expected_output_char);
            assert(output_char == expected_output_char);
        }

        fclose(output_file);
        fclose(expected_output_file);
        close(pipefd[0]);
    }
}

int main() { test(); }
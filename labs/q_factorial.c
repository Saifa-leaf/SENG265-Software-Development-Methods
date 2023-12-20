#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

        // variable to store the final answer
        double factorial = 1;

        // WRITE YOUR CODE TO DO COMMAND LINE INPUT CHECKING HERE
        if (argc < 2) {
                printf("Please input a number with ./q_factorial [number] command\n");
                exit(1);
        }


        // Takes the command line input and converts it into int.
        double num = atoi(argv[1]);


        // WRITE YOUR CODE TO DO THE FACTORIAL CALCULATIONS HERE
        for (num; 1<num ; num--) {
                factorial = factorial * num;
        }

        printf("%f\n", factorial);
}

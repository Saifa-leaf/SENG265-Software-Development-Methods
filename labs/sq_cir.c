# include <stdio.h>
# include <stdlib.h>
# include <time.h>

int main(int argc, char* argv[]) {

    double incir = 0;
    double outcir = 0;
	
	srand(time(NULL));
	for (int i = 1; i <= 10000; i++) {
		double n1 = ((rand() % 21)-10)/10.0;
        double n2 = ((rand() % 21)-10)/10.0;

        if ((n1*n1 + n2*n2) <= 1) {
            incir++;
        }
        outcir++;            

	}
    double result = (outcir/incir);

    printf("%f\n",result);


}


# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>


int get_date(char *st);
int get_date_sscanf(char *st);
int get_date_dtend(char *st);


int main(int argc, char *argv[]){

        char *st1 = "asdrtgkl knbvhgfk2021udfhg (blgfdl) fdgllk";
        char *st2 = "DTEND:20210102T123000";
        char *st3 = "RRULE:FREQ=WEEKLY;WKST=MO;UNTIL=20210301T235959;BYDAY=SA";

        int num1 = get_date(st1);
        printf("number found. %d\n", num1);
        int num2 = get_date_sscanf(st1);
        printf("number found. %d\n", num2);
        int num3 = get_date_dtend(st1);
        printf("number found. %d\n", num3);
        printf("-------\n");

        num1 = get_date(st2);
        printf("number found. %d\n", num1);
        num2 = get_date_sscanf(st2);
        printf("number found. %d\n", num2);
        num3 = get_date_dtend(st2);
        printf("number found. %d\n", num3);
        printf("-------\n");

        num1 = get_date(st3);
        printf("number found. %d\n", num1);
        num2 = get_date_sscanf(st3);
        printf("number found. %d\n", num2);
        num3 = get_date_dtend(st3);
        printf("number found. %d\n", num3);
}

// COMPLETE IMPLEMENTATION OF FUNCTIONS BELOW.

int get_date(char *st){
        // Indexes through st, char by char, until it
        // encounters the first digit, and proceeds from there.
        int i;
        while(i !=100) {
        if (isdigit(st[i])) {
            char * temp = &st[i];
            int num = strtol(temp, &temp, 10);
            return num;
        } else {
            i++;
        }
}
}

int get_date_sscanf(char *st){
        // Uses the sscanf function to find date in st.
        int temp;
        int success = sscanf(st, "%*[^0123456789]%d", &temp);
        if (success == 1) {
            return temp;
        }
}

int get_date_dtend(char *st) {
        // Use direct memory access and pointer arithmetic to find the date in st

}

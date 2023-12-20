#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LINE_LEN 132
#define MAX_EVENTS 500

typedef struct {
        char st_date[MAX_EVENTS][MAX_LINE_LEN];
        char en_date[MAX_EVENTS][MAX_LINE_LEN];
        char location[MAX_EVENTS][MAX_LINE_LEN];
        char summary[MAX_EVENTS][MAX_LINE_LEN];
        char rule[MAX_EVENTS][MAX_LINE_LEN];
        int eCount;
    } cal;

void print_event(cal data,int m, int line_date, int first);
void dt_format(char *formatted_time, const char *dt_time, const int len);
void dt_increment(char *after, const char *before, int const num_days);

void print_events(int from_yy, int from_mm, int from_dd, int to_yy, int to_mm, int to_dd)
{
    
}

void read_file(char *filename, char need[MAX_EVENTS][MAX_LINE_LEN]) {
    FILE* ics_file = fopen(filename,"r");
    if (ics_file == NULL) {
        printf("file can't be open");
        exit(0);
    }

    // read from file
    char line[100]; 
    int j = 0;
    while ((fgets(line, 100, ics_file)) != NULL) {
        strcpy(need[j], line);
        j++;
    }
    fclose(ics_file);
}

cal process_data(char need[MAX_EVENTS][MAX_LINE_LEN], cal data) {
    int k=0;
    char want[MAX_LINE_LEN];
    data.eCount = -1;

    char *ptr1 = strstr(need[k], "BEGIN:VCAL");
    char *ptr2 = strstr(need[k], "BEGIN:VEV");
    char *ptr3 = strstr(need[k], "DTSTART");
    char *ptr4 = strstr(need[k], "DTEND");
    char *ptr5 = strstr(need[k], "RRULE");
    char *ptr6 = strstr(need[k], "LOCATION");
    char *ptr7 = strstr(need[k], "SUMMARY");
    char *ptr8 = strstr(need[k], "END:VEV");
    char *ptr9 = strstr(need[k], "END:VCA");
    
    while (ptr9 == NULL) {
        if (ptr2 != NULL) {
            data.eCount++;
            strcpy(data.rule[data.eCount], "NULL");
        } else if (ptr3 != NULL) {
            sscanf(need[k], "%*[^:]:%s", want);
            strcpy(data.st_date[data.eCount], want);
        } else if (ptr4 != NULL) {
            sscanf(need[k], "%*[^:]:%s", want);
            strcpy(data.en_date[data.eCount], want);
        } else if (ptr5 != NULL) {
            sscanf(need[k], "%*[^:]:%s", want);
            strcpy(data.rule[data.eCount], want);
        } else if (ptr6 != NULL) {
            sscanf(need[k], "%*[^:]:%[^\n]", want);
            strcpy(data.location[data.eCount], want);
        } else if (ptr7 != NULL) {
            sscanf(need[k], "%*[^:]:%[^\n]", want);
            strcpy(data.summary[data.eCount], want);
        } else if (ptr8 != NULL) {
            NULL;
        } else if (ptr1 != NULL) {
            NULL;
        }
        strcpy(want, "");
        k++;
        ptr1 = strstr(need[k], "BEGIN:VCAL");
        ptr2 = strstr(need[k], "BEGIN:VEV");
        ptr3 = strstr(need[k], "DTSTART");
        ptr4 = strstr(need[k], "DTEND");
        ptr5 = strstr(need[k], "RRULE");
        ptr6 = strstr(need[k], "LOCATION");
        ptr7 = strstr(need[k], "SUMMARY");
        ptr8 = strstr(need[k], "END:VEV");
        ptr9 = strstr(need[k], "END:VCA");
        }
    return data;
}

void sort_date(int *date_of_day, int size, cal data) {
    int time[size];
    char temp[30];
    int hold;
    int wait;

    for (int n=0; n<size; n++) {
        strcpy(temp, data.st_date[date_of_day[n]]);
        sscanf(temp, "%*[^T]T%d", &time[n]);
    }
    

    for (int o=0; o<=size-2; o++) {
        for (int p=0; p<size-1; p++) {
            if (time[p+1] < time[p]) {
                wait = time[p];
                time[p] = time[p+1];
                time[p+1] = wait;
                hold = date_of_day[p];
                date_of_day[p] = date_of_day[p+1];
                date_of_day[p+1] = hold;
            }
        }

    }


}

void print_data(cal data, int from_y, int from_m, int from_d, int to_y, int to_m, int to_d) {
    int cur_y, cur_m, cur_d, end_y, end_m, end_d, st_y, st_m, st_d;
    char cur_date_full[MAX_LINE_LEN], cur_date[MAX_LINE_LEN];
    int line_date = 0;
    int l = data.eCount;
    char st_date_full[MAX_LINE_LEN];
    int date_of_day[MAX_LINE_LEN];
    int size = 0;
    int first = 0;

    strcpy(cur_date_full, data.st_date[0]);
    sscanf(cur_date_full, "%4d%2d%2d%*[^T]T", &cur_y, &cur_m, &cur_d);
    

    if ((cur_m == to_m && cur_d > to_d) || cur_m > to_m) {
        exit(0);
    } else if ((cur_m == from_m && cur_d < from_d) || cur_m < from_m) {
        while (cur_m != from_m && cur_d != from_d) {
            dt_increment(cur_date_full, cur_date_full, 1);
            sscanf(cur_date_full, "%4d%2d%2d%*[^T]T", &cur_y, &cur_m, &cur_d);
        }
    } else {
        sscanf(cur_date_full, "%4d%2d%2d%*[^T]T", &cur_y, &cur_m, &cur_d);
    }

    while ((cur_d <= to_d && cur_m == to_m) || cur_m < to_m) {
        for (int m=0; m<=l; m++) {
            
            sscanf(data.st_date[m], "%4d%2d%2d%*[^T]T", &st_y, &st_m, &st_d);
            if ((strcmp(data.rule[m], "NULL") != 0)) {
                sscanf(data.rule[m], "%*[^U]UNTIL=%4d%2d%2d", &end_y, &end_m, &end_d);
            } else {
                sscanf(data.en_date[m], "%4d%2d%2d%*[^T]T", &end_y, &end_m, &end_d);
            }
            
            
            while ((st_m == cur_m && st_d < cur_d) || st_m < cur_m) {
                    dt_increment(st_date_full, data.st_date[m], 7);
                    strcpy(data.st_date[m], st_date_full);
                    sscanf(data.st_date[m], "%4d%2d%2d%*[^T]T", &st_y, &st_m, &st_d);    
            }
            if (st_m == cur_m && st_d == cur_d && (cur_m < end_m || (cur_m == end_m && cur_d <= end_d))) {
                date_of_day[size] = m;
                size++;
            }

        }
        if (size != 0) {
            sort_date(date_of_day, size, data);
            int index;

            for (int q=0; q<size; q++) {
                index = date_of_day[q];
                if ((strcmp(data.rule[index], "NULL") != 0)) {
                    print_event(data, index, line_date, first);
                    line_date++;
                    dt_increment(st_date_full, data.st_date[index], 7);
                    strcpy(data.st_date[index], st_date_full);
                } else if ((strcmp(data.rule[index], "NULL") == 0)) {
                    print_event(data, index, line_date, first);
                    line_date++;
                    dt_increment(st_date_full, data.st_date[index], 500);
                    strcpy(data.st_date[index], st_date_full);
                }
            }
            first++;
            
        }
        size=0;
        line_date = 0;
        dt_increment(cur_date_full, cur_date_full, 1);
        sscanf(cur_date_full, "%4d%2d%2d%*[^T]T", &cur_y, &cur_m, &cur_d);
    }
}



int main(int argc, char *argv[])
{
    int from_y = 0, from_m = 0, from_d = 0;
    int to_y = 0, to_m = 0, to_d = 0;
    char *filename = NULL;
    int i; 

    for (i = 0; i < argc; i++) {
        if (strncmp(argv[i], "--start=", 8) == 0) {
            sscanf(argv[i], "--start=%d/%d/%d", &from_y, &from_m, &from_d);
        } else if (strncmp(argv[i], "--end=", 6) == 0) {
            sscanf(argv[i], "--end=%d/%d/%d", &to_y, &to_m, &to_d);
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
            filename = argv[i]+7;
        }
    }

    if (from_y == 0 || to_y == 0 || filename == NULL) {
        fprintf(stderr, 
            "usage: %s --start=yyyy/mm/dd --end=yyyy/mm/dd --file=icsfile\n",
            argv[0]);
        exit(1);
    }

    /* Starting calling your own code from this point. */
    print_events(from_y, from_m, from_d, to_y, to_m, to_d);

    char need[MAX_EVENTS][MAX_LINE_LEN];
    read_file(filename, need);

    // process info
    cal data;
    data = process_data(need, data);


    // print data
    print_data(data, from_y, from_m, from_d, to_y, to_m, to_d);


    exit(0);
}
   


void print_event(cal data, int m, int line_date, int first) {
    int len;
    char am[3] = "AM";
    char pm[3] = "PM";
    char output[MAX_LINE_LEN];
    char st_time_hour[MAX_LINE_LEN];
    int st_hour;
    char st_time_min[MAX_LINE_LEN];
    char en_time_hour[MAX_LINE_LEN];
    int en_hour;
    char en_time_min[MAX_LINE_LEN];

    if (line_date == 0) {
        if (first != 0) {
            printf("\n");
        }
        dt_format(output, data.st_date[m], MAX_LINE_LEN);
        printf("%s\n", output);
        len = strlen(output);
        for (int n=0; n<len; n++) {
            printf("-");
        }
        printf("\n");
    }
    sscanf(data.st_date[m],"%*[^T]T%2s%2s", st_time_hour, st_time_min);
    sscanf(data.en_date[m],"%*[^T]T%2s%2s", en_time_hour, en_time_min);
    st_hour = atoi(st_time_hour);
    en_hour = atoi(en_time_hour);

    if (st_hour > 12 && en_hour > 12) {
        st_hour -= 12;
        en_hour -= 12;
        printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, pm, en_hour, en_time_min,
         pm, data.summary[m], data.location[m]);
    } else if (st_hour > 12 && en_hour < 12) {
        st_hour -= 12;
        printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, pm, en_hour, en_time_min,
          am, data.summary[m], data.location[m]);
    } else if (st_hour < 12 && en_hour > 12) {
        en_hour -= 12;
        printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, am, en_hour, en_time_min,
         pm, data.summary[m], data.location[m]);
    } else if (st_hour == 12 && en_hour > 12) {
        en_hour -= 12;
        printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, pm, en_hour, en_time_min,
         pm, data.summary[m], data.location[m]);
    } else if (st_hour > 12 && en_hour == 12) {
        st_hour -= 12;
        printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, pm, en_hour, en_time_min,
         pm, data.summary[m], data.location[m]);
    } else if (st_hour == 12 && en_hour < 12) {
        printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, pm, en_hour, en_time_min,
         am, data.summary[m], data.location[m]);
    } else if (st_hour < 12 && en_hour == 12) {
        printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, am, en_hour, en_time_min,
         pm, data.summary[m], data.location[m]);
    } else {
        printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, am, en_hour, en_time_min,
         am, data.summary[m], data.location[m]);
    }
}

void dt_format(char *formatted_time, const char *dt_time, const int len) {
    struct tm temp_time;
    time_t    full_time;
    char      temp[5];

    /*  
     * Ignore for now everything other than the year, month and date.
     * For conversion to work, months must be numbered from 0, and the 
     * year from 1900.
     */  
    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(dt_time, "%4d%2d%2d",
        &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    full_time = mktime(&temp_time);
    strftime(formatted_time, len, "%B %d, %Y (%a)", 
        localtime(&full_time));
}

/*
 * Function dt_increment:
 *
 * Given a date-time, it adds the number of days in a way that
 * results in the correct year, month, and day. For example,
 * if the string in "before" corresponds to:
 *
 *   20190520T111500
 *
 * then the datetime string stored in "after", assuming that
 * "num_days" is 100, will be:
 *
 *   20190828T111500
 *
 * which is 100 days after May 20, 2019 (i.e., August 28, 2019).
 *
 */

void dt_increment(char *after, const char *before, int const num_days)
{
    struct tm temp_time, *p_temp_time;
    time_t    full_time;
    char      temp[5];

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(before, "%4d%2d%2d", &temp_time.tm_year,
        &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    temp_time.tm_mday += num_days;

    full_time = mktime(&temp_time);
    after[0] = '\0';
    strftime(after, 9, "%Y%m%d", localtime(&full_time));
    strncpy(after + 8, before + 8, MAX_LINE_LEN - 8); 
    after[MAX_LINE_LEN - 1] = '\0';
}


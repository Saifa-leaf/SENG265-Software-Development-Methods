/*
 * process_cal3.c
 *
 * Starter file provided to students for Assignment #3, SENG 265,
 * Fall 2021.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emalloc.h"
#include "ics.h"
#include "listy.h"

void read_file(char *filename, char need[1000][250]);
void free_mem(node_t* list);
node_t* process_data(char need[1000][250]);
void print_data(node_t* list, int from_y, int from_m, int from_d, int to_y, int to_m, int to_d);
void sort_date(node_t* list2);
void print_event(node_t* list2, int line_date, int first);
void dt_format(char *formatted_time, const char *dt_time, const int len);
void dt_increment(char *after, const char *before, int const num_days);


int main(int argc, char *argv[]) {
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

    // read from ics file
    char need[1000][250];
    read_file(filename, need);
    // process info
    node_t *list = process_data(need);
    // print data
    print_data(list, from_y, from_m, from_d, to_y, to_m, to_d);
    // free memory
    free_mem(list);
    exit(0);
}


/*
This function take linked list pointer and free memory from the list including 
the value in the list.
*/

void free_mem(node_t* list) {
    node_t* temp = NULL;
    for (; list != NULL; list = temp) {
        temp = list->next;
        free(list->val);
        free(list);
    }
}


/*
take linked list pointer, line_date(to see if we need to print the line after the date) and
first (prevent \n for the first date tha need to be print).
This function format and print event in linked list pointer.
*/

void print_event(node_t* list2, int line_date, int first) {
    int len;
    char am[3] = "AM";
    char pm[3] = "PM";
    char output[130];
    char st_time_hour[130];
    int st_hour;
    char st_time_min[130];
    char en_time_hour[130];
    int en_hour;
    char en_time_min[130];

    if (line_date == 0) {
        if (first != 0) {
            printf("\n");
        }
        dt_format(output, list2->val->dtstart, 130);
        printf("%s\n", output);
        len = strlen(output);
        for (int n=0; n<len; n++) {
            printf("-");
        }
        printf("\n");
    }
    while (list2 != NULL) {
        sscanf(list2->val->dtstart,"%*[^T]T%2s%2s", st_time_hour, st_time_min);
        sscanf(list2->val->dtend,"%*[^T]T%2s%2s", en_time_hour, en_time_min);
        st_hour = atoi(st_time_hour);
        en_hour = atoi(en_time_hour);

        if (st_hour > 12 && en_hour > 12) {
            st_hour -= 12;
            en_hour -= 12;
            printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, pm, en_hour, en_time_min,
            pm, list2->val->summary, list2->val->location);
        } else if (st_hour > 12 && en_hour < 12) {
            st_hour -= 12;
            printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, pm, en_hour, en_time_min,
            am, list2->val->summary, list2->val->location);
        } else if (st_hour < 12 && en_hour > 12) {
            en_hour -= 12;
            printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, am, en_hour, en_time_min,
            pm, list2->val->summary, list2->val->location);
        } else if (st_hour == 12 && en_hour > 12) {
            en_hour -= 12;
            printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, pm, en_hour, en_time_min,
            pm, list2->val->summary, list2->val->location);
        } else if (st_hour > 12 && en_hour == 12) {
            st_hour -= 12;
            printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, pm, en_hour, en_time_min,
            pm, list2->val->summary, list2->val->location);
        } else if (st_hour == 12 && en_hour < 12) {
            printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, pm, en_hour, en_time_min,
            am, list2->val->summary, list2->val->location);
        } else if (st_hour < 12 && en_hour == 12) {
            printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, am, en_hour, en_time_min,
            pm, list2->val->summary, list2->val->location);
        } else {
            printf("%2d:%s %s to %2d:%s %s: %s {{%s}}\n", st_hour, st_time_min, am, en_hour, en_time_min,
            am, list2->val->summary, list2->val->location);
        }
        list2 = list2->next;
    }
}

/*
take pointer to linked list containing event_t (event_t has been memcpy so it a seperate linked list)
and sort them in order (event that happen earlier in the day come first) 
*/

void sort_date(node_t* list2) {
    node_t* prev = list2;
    node_t* curr = list2->next;
    event_t* temp;

    while (curr != NULL) {
        if (strcmp(curr->val->dtstart, prev->val->dtstart) < 0) {
            temp = curr->val;
            curr->val = prev->val;
            prev->val = temp;
            sort_date(list2);
        }
        curr = curr->next;
        prev = prev->next;
    }

}

/*
This fuction take linked list pointer containing event_t and from/to year, month and day from cmd input and
print out event between those date.
This function call sort_date and print_event function to help sort the date
that need to be print in chronological order and to actually print it out.
Parameter: linked list pointer, start and end date specify in cmd prompt.
*/

void print_data(node_t* list, int from_y, int from_m, int from_d, int to_y, int to_m, int to_d) {

    int cur_y, cur_m, cur_d, end_y, end_m, end_d, st_y, st_m, st_d;
    char cur_date_full[130];
    int line_date = 0;
    int m = 0;
    char st_date_full[130];
    int first = 0;
    char temp[10];
    event_t *cur2;
    node_t *head = list;
    node_t *list2 = NULL;
    node_t *head2 = NULL;
    node_t *node;

    strcpy(cur_date_full, list->val->dtstart);
    sscanf(cur_date_full, "%4d%2d%2d%*[^T]T", &cur_y, &cur_m, &cur_d);

    if ((cur_m == to_m && cur_d > to_d) || cur_m > to_m) {
        exit(0);
    } else if ((cur_m == from_m && cur_d < from_d) || (cur_m < from_m && cur_y == from_y)
        || cur_y < from_y) {
        while (cur_m != from_m || cur_d != from_d || cur_y != from_y) {
            dt_increment(cur_date_full, cur_date_full, 1);
            sscanf(cur_date_full, "%4d%2d%2d%*[^T]T", &cur_y, &cur_m, &cur_d);
        }
    } else {
        sprintf(temp, "%4d%02d%02d", from_y, from_m, from_d);
        strcpy(cur_date_full, temp);
        sscanf(cur_date_full, "%4d%2d%2d%*[^T]T", &cur_y, &cur_m, &cur_d);
    }

    while ((cur_d <= to_d && cur_m == to_m && cur_y == to_y) || (cur_m < to_m && cur_y <= to_y) 
        || cur_y < to_y) {
        m=0;
        while (list != NULL) {
            
            sscanf(list->val->dtstart, "%4d%2d%2d%*[^T]T", &st_y, &st_m, &st_d);
            if ((strcmp(list->val->rrule, "NULL") != 0)) {
                sscanf(list->val->rrule, "%*[^U]UNTIL=%4d%2d%2d", &end_y, &end_m, &end_d);
            } else {
                sscanf(list->val->dtend, "%4d%2d%2d%*[^T]T", &end_y, &end_m, &end_d);
            }
            
            
            while ((st_y == cur_y && st_m == cur_m && st_d < cur_d) || (st_y == cur_y && st_m < cur_m)
                || st_y < cur_y) {
                    dt_increment(st_date_full, list->val->dtstart, 7);
                    strcpy(list->val->dtstart, st_date_full);
                    sscanf(list->val->dtstart, "%4d%2d%2d%*[^T]T", &st_y, &st_m, &st_d);    
            }
            if (st_m == cur_m && st_d == cur_d && st_y == cur_y &&
                 (cur_m < end_m || (cur_m == end_m && cur_d <= end_d))) {
                cur2 = (event_t *)emalloc(sizeof(event_t));
                memcpy(cur2, list->val, sizeof(event_t));
                node = new_node(cur2);
                list2 = add_end(list2, node);

            }
            
            m++;
            list = list->next;
        }
    
    
        if (list2 != NULL) {
            head2 = list2;
            if (list2->next != NULL) {
                sort_date(list2);
            }
            
            print_event(list2, line_date, first);
            
            first++;
            
        }
        while (head2 != NULL) {
            if ((strcmp(head2->val->rrule, "NULL") != 0)) {
                    dt_increment(st_date_full, head2->val->dtstart, 7);
                    strcpy(head2->val->dtstart, st_date_full);
            }
            head2 = head2->next;
        }
        list = head;
        free_mem(list2);
        list2 = NULL;
        head2 = NULL;
        line_date = 0;
        dt_increment(cur_date_full, cur_date_full, 1);
        sscanf(cur_date_full, "%4d%2d%2d%*[^T]T", &cur_y, &cur_m, &cur_d);

    }
    
    list = head;
}


/*
Process_data take a 2D array containing each line in the file in each index of the array
and turn them into event variable (event_t) before putting them into linked list (node_t)
return linked list pointer.
*/

node_t* process_data(char need[1000][250]) {
    int k=0;
    char want[130];
    event_t *cur = (event_t *)emalloc(sizeof(event_t));
    node_t *list = NULL;
    node_t *node;

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
            strcpy(cur->rrule, "NULL");
        } else if (ptr3 != NULL) {
            sscanf(need[k], "%*[^:]:%s", want);
            strcpy(cur->dtstart, want);
        } else if (ptr4 != NULL) {
            sscanf(need[k], "%*[^:]:%s", want);
            strcpy(cur->dtend, want);
        } else if (ptr5 != NULL) {
            sscanf(need[k], "%*[^:]:%s", want);
            strcpy(cur->rrule, want);
        } else if (ptr6 != NULL) {
            sscanf(need[k], "%*[^:]:%[^\n]", want);
            strcpy(cur->location, want);
        } else if (ptr7 != NULL) {
            sscanf(need[k], "%*[^:]:%[^\n]", want);
            strcpy(cur->summary, want);
        } else if (ptr8 != NULL) {
            node = new_node(cur);
            list = add_end(list, node);
            cur = (event_t *)emalloc(sizeof(event_t));
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
    free(cur);
    return list;

}



/*
    This function read from ics file and put each line of data into a 
    2D array. If the file is null print "file can't be open" and exit.
    Parameter:ics file name, the 2D array to store data
    */
void read_file(char *filename, char need[1000][250]) {
    FILE* ics_file = fopen(filename,"r");
    if (ics_file == NULL) {
        printf("file can't be open");
        exit(0);
    }
    
    char line[100]; 
    int j = 0;
    while ((fgets(line, 100, ics_file)) != NULL) {
        strcpy(need[j], line);
        j++;
    }
    fclose(ics_file);
}

/*
 * Function dt_format.
 *
 * Given a date-time, creates a more readable version of the
 * calendar date by using some C-library routines. For example,
 * if the string in "dt_time" corresponds to:
 *
 *   20190520T111500
 *
 * then the string stored at "formatted_time" is:
 *
 *   May 20, 2019 (Mon).
 *
 */

void dt_format(char *formatted_time, const char *dt_time, const int len)
{
    struct tm temp_time;
    time_t    full_time;

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
    struct tm temp_time;
    time_t    full_time;

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(before, "%4d%2d%2d", &temp_time.tm_year,
        &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    temp_time.tm_mday += num_days;

    full_time = mktime(&temp_time);
    after[0] = '\0';
    strftime(after, 9, "%Y%m%d", localtime(&full_time));
    strncpy(after + 8, before + 8, 130 - 8); 
    after[130 - 1] = '\0';
}

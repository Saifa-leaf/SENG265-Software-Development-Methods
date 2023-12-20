#!/usr/bin/env python3


from typing import List, Dict, TextIO, Tuple
import datetime
import sys
import typing

"""
This python file take given icalender file and print out all events from given start
date to given end date (need to pass in start date, end date and file to read in calling)
"""

def main():

    if len(sys.argv) == 4:
        num = 1
        st_date = sys.argv[num]
        thrown, st_date = st_date.split("=")
        st_d = convertor_for_start(st_date, 0)
        num = 2
        end_date = sys.argv[num]
        thrown, end_date = end_date.split("=")
        end_d = convertor_for_start(end_date, 1)
        num = 3
        file_name = sys.argv[num]
        thrown, file_name = file_name.split("=")
    else:
        print("usage: ./filename.py --start=yyyy/mm/dd --end=yyyy/mm/dd --file=icsfile\n")
        exit()


    list_line = read_file(file_name)
    all_events = process_data(list_line)
    print_event(all_events, st_d, end_d)

"""
loop through each day between start date and end date then loop 
through list of dictionary containing all events for that day
to get event that happening today and passing then to sort_and_print
st_d -> given start date
en_d -> given end date
all_events -> list of dict of all events
"""
def print_event(all_events: List[dict], st_d: datetime, en_d: datetime) -> None:
    cur_d = st_d
    add_day = datetime.timedelta(1)
    add_week = datetime.timedelta(7)
    happening_today = []
    newline = 0

    while (cur_d < en_d):
        for dict in all_events:
            while (dict["STDATE"] < cur_d and "RRULE" in dict):
                dict["STDATE"] += add_week

            if ("RRULE" in dict and dict["RUDATE"] >= cur_d and dict["STDATE"] <= dict["RUDATE"] and cur_d == dict["STDATE"]):
                happening_today.append(dict)
                dict["STDATE"] += add_week
            elif ((not "RRULE" in dict) and dict["STDATE"] == cur_d):
                happening_today.append(dict)
        
        if (len(happening_today) != 0):
            sort_and_print(happening_today, cur_d, newline)
            newline += 1

        cur_d += add_day
        happening_today = []

"""
take a list of dictionary containing event that happening today, sort them and then print
happening_today -> list of dictionary containing event that happening today
cur_date -> to print current date
new_line -> to prevent printing new line when calling this function for the first time
"""
def sort_and_print(happening_today: List[dict], cur_date: datetime, new_line: int) -> None:
    date_check = 0

    happening_today = sorted(happening_today,  key=lambda a: a['STTIME']) 
    if (new_line != 0):
        print()

    for dict in happening_today:
        st_date = dict["DTSTART"]
        en_date = dict["DTEND"]
        st_time = (st_date.strftime("X%I:%M %p")).replace("X0"," ").replace("X","")
        en_time = (en_date.strftime("X%I:%M %p")).replace("X0"," ").replace("X","")
        
        if (date_check == 0):
            today = cur_date.strftime("%B %d, %Y (%a)")
            print(today)
            for num in range(len(today)):
                print("-", end="")
            print()
            date_check += 1

        print(st_time + " to " + en_time + ": " + dict["SUMMARY"] + " {{" + dict["LOCATION"] + "}}")


"""
convert the passing start date and end date from command propmt into datetime variable
num -> check if it is a start date or end date
date -> string with format yyyy/mm/dd
return datetime
"""
def convertor_for_start(date: str, num: int) -> datetime:
    y, m ,d = date.split("/")
    y, m, d = int(y), int(m), int(d)
    if (num == 1):
        d1 = datetime.datetime(y ,m ,d , 23, 59, 59)
    else:
        d1 = datetime.datetime(y ,m ,d)
    return d1


"""
take list_line from read_file function and process each line for needed data
then store them in dictionary. 
return list of dictionary

"""
def process_data(list_line: List[str]) -> List[dict]:
    all_events = []
    event = {}

    for line in list_line:
        if ("END:VEVENT" in line):
            all_events.append(event)
            event = {}
        elif ("LOCATION" in line):
            k,v = line.split(":")
            event[k] = v
        elif ("SUMMARY" in line):
            k,v = line.split(":")
            event[k] = v
        elif ("DTSTART" in line):
            k,v = line.split(":")
            time = list(v)
            v, date, t = convertor(time)
            event[k] = v
            event["STDATE"] = date
            event["STTIME"] = t
        elif ("DTEND" in line):
            k,v = line.split(":")
            time = list(v)
            v, thrown, thrown = convertor(time)
            event[k] = v
        elif ("RRULE" in line):
            k,v = line.split(":")
            thrown, v = line.split("L=")
            v, thrown = v.split(";")
            time = list(v)
            v, date, t = convertor(time)
            event[k] = v
            event["RUDATE"] = date
            event["RUTIME"] = t

    return all_events

            

        
"""
take a list of character and convert them into 3 diffeerent datetime variable
return d1(datetime with year, month, day, hour, min, sec), 
d2(datetime that contain only year, month, day), 
t(datetime that contain only hour, min, sec)
"""
def convertor(time: List[str]) -> datetime:
    count = 0
    y ,mo ,d ,h ,mi ,s = "","","","","",""
    for char in time:
        if count < 4:
            y = y + time[count]
        elif count < 6:
            mo = mo + time[count]
        elif count < 8:
            d = d + time[count]
        elif count >= 9 and count < 11:
            h = h + time[count]
        elif count >= 11 and count < 13:
            mi = mi + time[count]
        elif count >= 13 and count < 15:
            s = s + time[count]
        count = count + 1

    y ,mo ,d ,h ,mi ,s = int(y) ,int(mo) ,int(d) ,int(h) ,int(mi) ,int(s) 
    d1 = datetime.datetime(y ,mo ,d ,h ,mi ,s)
    d2 = datetime.datetime(y, mo, d)
    t = datetime.time(h ,mi ,s)

    return d1, d2, t


"""
purpose: return a list of line from a file
Precondition: file name 
"""

def read_file(file_name: str) -> List[str]:
    list_line = []
    
    file = open(file_name, "r")
    for line in file:
        line = line.rstrip()
        list_line.append(line)

    file.close()
    return list_line


if __name__ == "__main__":
    main()

#!/usr/bin/env python3

'''
This python program is use to create process_cal class.
The process_cal class is a class that contain list of dictionary of events from ics file
This file use code from process_cal2.py to help with the methods.
Aside from the constructor this file has 1 main method which is get_events_for_day every other
method is a helper to either the constructor or get_events_for_day
'''

from typing import List, Dict, TextIO, Tuple
import datetime
import re


class process_cal:


    def __init__(self, file_name):
        '''
        constructor for process_cal class.
        take file name as a string and convert data in it into list of dictionary

        '''
        list_line = self.read_file(file_name)
        self.eventList = list_line


    def get_events_for_day(self, cur_date: datetime) -> str:
        '''
        purpose: takes the datetime object and checks to see if there are any events for that 
            date in the eventList from process_cal class and if found, pass all event that match
            the datetime to function sort_and_return as list of dict before return.
        return: if there is event that match datetime object from eventList
            , returns a string with the day’s events. else return None
        '''
        add_week = datetime.timedelta(7)
        happening_today = []

        for dict in self.eventList:
            while (dict["STDATE"] < cur_date and "RULEDATE" in dict):
                dict["STDATE"] += add_week

            if ("RULEDATE" in dict and dict["RULEDATE"] >= cur_date and dict["STDATE"] <= dict["RULEDATE"] and cur_date == dict["STDATE"]):
                happening_today.append(dict)
                dict["STDATE"] += add_week
            elif ((not "RULEDATE" in dict) and dict["STDATE"] == cur_date):
                happening_today.append(dict)
        
        if (len(happening_today) != 0):
            return self.sort_and_return(happening_today, cur_date)
        else:
            return None



    def sort_and_return(self, happening_today: List[dict], cur_date: datetime) -> str:
        '''
        purpose: help function get_events_for_day by sorting event in order of what event will 
            happen first.
        return: sorted list of dict containing event that happen at cur_date
        '''
        date_check = 0
        new_line = 0
        stringToReturn = ""

        happening_today = sorted(happening_today,  key=lambda a: a['STTIME'])

        for dict in happening_today:
            st_date = dict["STDATEFULL"]
            en_date = dict["ENDDATEFULL"]
            st_time = (st_date.strftime("X%I:%M %p")).replace("X0"," ").replace("X","")
            en_time = (en_date.strftime("X%I:%M %p")).replace("X0"," ").replace("X","")
            
            if (date_check == 0):
                today = cur_date.strftime("%B %d, %Y (%a)")
                stringToReturn += today
                stringToReturn += "\n"
                for num in range(len(today)):
                    stringToReturn += "-"
                stringToReturn += "\n"
                date_check += 1

            if (new_line != 0):
                stringToReturn += "\n"
            
            new_line += 1
            if ("LOCATION" in dict):
                stringToReturn += (st_time + " to " + en_time + ": " + dict["SUMMARY"] + " {{" + dict["LOCATION"] + "}}")
            else:
                stringToReturn += (st_time + " to " + en_time + ": " + dict["SUMMARY"] + " {{}}")
        
        return stringToReturn


    def process_data(self, list_line: List[str]) -> List[dict]:
        """
        Precondition: list_line (List of string) each value is 1 line from file

        purpose: take list_line from read_file function and process each line for needed data
            then store them in dictionary. 
        return: list of dictionary with each dictinary containing start date, end date, location
            summary, and rrule for 1 event if it has one of the mention parameter.
        """
        all_events = []
        event = {}
        
        st_time = re.compile("(DTSTART):(\d\d\d\d)(\d\d)(\d\d)T(\d\d)(\d\d)(\d\d)")
        end_time = re.compile("(DTEND):(\d\d\d\d)(\d\d)(\d\d)T(\d\d)(\d\d)(\d\d)")
        location = re.compile("(LOCATION):(.+)")
        summary = re.compile("(SUMMARY):(.+)")
        rrule = re.compile("UNTIL=(\d\d\d\d)(\d\d)(\d\d)T(\d\d)(\d\d)(\d\d)")
        event_end = re.compile("(END:VEVENT)")

        for line in list_line:
            st = st_time.search(line)
            en = end_time.search(line)
            lo = location.search(line)
            su = summary.search(line)
            rr = rrule.search(line)
            ev = event_end.search(line)

            if (ev != None):
                all_events.append(event)
                event = {}
            elif (lo != None):
                event[lo.group(1)] = lo.group(2)
            elif (su != None):
                event[su.group(1)] = su.group(2)
            elif (st != None):
                y ,mo ,d = int(st.group(2)) ,int(st.group(3)) ,int(st.group(4))
                h ,mi ,s = int(st.group(5)) ,int(st.group(6)) ,int(st.group(7))
                dt = datetime.datetime(y ,mo ,d ,h ,mi ,s)
                d = datetime.datetime(y, mo, d)
                t = datetime.time(h ,mi ,s)
                event["STDATEFULL"] = dt
                event["STDATE"] = d
                event["STTIME"] = t
            elif (en != None):
                y ,mo ,d = int(en.group(2)) ,int(en.group(3)) ,int(en.group(4))
                h ,mi ,s = int(en.group(5)) ,int(en.group(6)) ,int(en.group(7))
                dt = datetime.datetime(y ,mo ,d ,h ,mi ,s)
                d = datetime.datetime(y, mo, d)
                event["ENDDATEFULL"] = dt
                event["ENDDATE"] = d
            elif (rr != None):
                y ,mo ,d = int(rr.group(1)) ,int(rr.group(2)) ,int(rr.group(3))
                h ,mi ,s = int(rr.group(4)) ,int(rr.group(5)) ,int(rr.group(6))
                dt = datetime.datetime(y ,mo ,d ,h ,mi ,s)
                d = datetime.datetime(y, mo, d)
                event["RULEDATEFULL"] = dt
                event["RULEDATE"] = d

        return all_events    


    def read_file(self, file_name: str) -> List[dict]:
        """
        purpose: read each line in file and send it to process_data to get list of dictionary
            then retrun it.
        Precondition: file name (string) 
        return: list of dictionary from process_data
        """
        list_line = []
        
        file = open(file_name, "r")
        for line in file:
            line = line.rstrip()
            list_line.append(line)

        file.close()
        all_event = self.process_data(list_line)
        return all_event


#!/bin/python


from datetime import date

now = date.today()
full = '-' + str(now.month) + "-" + str(now.day) + "-" + str(now.year)
print full

full = "testFile" + full
print full

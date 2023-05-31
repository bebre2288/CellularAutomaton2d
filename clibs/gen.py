#!/bin/python3
from random import randint as rint
from sys import argv
size = int(argv[1])

print("int pos[]={", end="")
for i in range(int(10**6/50)):
    for j in range(50):
        print(rint(0, size), ",", sep="", end="")
    print()
print("};")

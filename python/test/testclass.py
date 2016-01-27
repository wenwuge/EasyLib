#!/bin/env python

class Person:
    def __init__(self, first, middle, last, age):
        self.first = first;
        self.middle = middle;
        self.last = last;
        self.age = age;
        self.cnt = 0;
        self.jobs = {};
    def __str__(self):
        return self.first + ' ' + self.middle + ' ' + self.last + \
               ' ' + str(self.age)
    def initials(self):
        return self.first[0] + self.middle[0] + self.last[0]
    def changeAge(self, val):
        self.age += val
    def changeJobs(self, val):
        self.jobs[(str)(self.cnt)] = val
        self.cnt+=1

myPerson = Person('Raja', 'I', 'Kumar', 21)
print(myPerson)

myPerson.changeAge(5)
myPerson.changeJobs("mtk")
myPerson.changeJobs("hanbai")
myPerson.changeJobs("venus")
myPerson.changeJobs("topsec")
myPerson.changeJobs("360")
print(myPerson)
print(myPerson.initials())

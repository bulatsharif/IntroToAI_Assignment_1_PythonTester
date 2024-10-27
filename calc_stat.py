from statistics import mean, mode, stdev

print("########### A* Search ########### ")
file1 = open("statistics.txt", "r")

list = []


for i in range(1001):
    list.append(float(file1.readline()))

for i in range(2):
    print(file1.readline(), end="")

print()
print(f"Median: {mean(list)}")
print(f"Mode: {mode(list)}")
print(f"Standard Deviation: {stdev(list)}")

print()
print("########### Backtracking ########### ")
file1 = open("statistics_backtrack.txt", "r")

list = []


for i in range(1000):
    list.append(float(file1.readline()))

for i in range(2):
    print(file1.readline(), end="")

print()
print(f"Median: {mean(list)}")
print(f"Mode: {mode(list)}")
print(f"Standard Deviation: {stdev(list)}")


"""
Number of wins for A* search: 894
Number of losses for A* search: 106
Median: 0.010898561239242554
Mode: 0.009165048599243164
Standard Deviation: 0.0020412393828589627
"""


"""
Number of wins for backtrack search: 761
Number of losses for backtrack search: 229
Median: 0.013794861793518066
Mode: 0.009557008743286133
Standard Deviation: 0.004212036456138466

"""
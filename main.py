import random
import re
import time
from subprocess import Popen, PIPE

def print_grid(grid):
    print("\t", end=" ")
    for i in range(len(grid)):
        print(f' ({i})', end = "  ")
    print()
    for i, row in enumerate(grid):
        print(f'({i})\t ', end= " ")
        for cell in row:
            print(f'|{cell}|', end="   ")
        print()

def handle_node(map_grid, val, l, r, response):
    if val[0] + l < 9 and val[1]  + r < 9 and val[0] + l >= 0 and val[1] + r >=  0:
        if map_grid[val[0] + l][val[1] + r] != 0:
            if map_grid[val[0] + l][val[1] + r] == 'Z':
                object = str(val[0] + l) + " " + str(val[1] + r) + " " + 'K'
                response.append(object)
                object = str(val[0] + l) +  " " + str(val[1] + r) + " " + 'P'
                response.append(object)
            elif map_grid[val[0] + l][val[1] + r] != 0:
                object = str(val[0] + l) +  " " + str(val[1] + r) + " " + str(map_grid[val[0] + l][val[1] + r])
                response.append(object)

def handle_move(move,grid):
    val = move[1: 3]
    val = [int(obj) for obj in val]

    response = []


    handle_node(grid, val, 0, 1, response)
    handle_node(grid, val, 1, 0, response)
    handle_node(grid, val, 0, -1, response)
    handle_node(grid, val, -1, 0, response)

    handle_node(grid, val, 1, -1, response)
    handle_node(grid, val, 1, 1, response)
    handle_node(grid, val, -1, 1, response)
    handle_node(grid, val, -1, -1, response)
    return response


time_result_astar = []
time_result_backtrack = []
number_of_wins = 0
number_of_losses = 0

for i in range(50):
    keymaker = (random.randint(0, 8), random.randint(0, 8))
    agents = [(random.randint(0,8), random.randint(0,8)) for i in range(random.randint(0, 3))]
    sentinel =[(random.randint(0, 8), random.randint(0,8)) for i in range(random.randint(0, 1))]
    # Remove duplicates in agents
    for i, val in enumerate(agents):
        while agents.count(val) > 1:
            new_tuple = (random.randint(0,8), random.randint(0,8))
            if new_tuple not in agents:
                agents[i] = new_tuple

    # Remove collision between agents and keymaker
    for i, val in enumerate(agents):
        while agents[i] == keymaker:
            new_tuple = (random.randint(0,8), random.randint(0,8))
            if new_tuple not in agents:
                agents[i] = new_tuple

    while len(sentinel) > 0 and ([0] in agents or sentinel[0] == keymaker):
        new_tuple = (random.randint(0, 8), random.randint(0, 8))


    # Now create a grid and fill it with agents and key maker
    map_grid = [[0 for x in range(9)] for y in range(9)]

    map_grid[keymaker[0]][keymaker[1]] = 'K'

    for i, val in enumerate(agents):
        map_grid[val[0]][val[1]] = "A"

        if val[0] < 9 and val[1]  + 1 < 9: map_grid[val[0]][val[1] + 1] = "P"
        if val[0] < 9 and val[1]  - 1 >= 0: map_grid[val[0]][val[1] - 1] = "P"
        if val[0] + 1 < 9 and val[1] < 9: map_grid[val[0] + 1][val[1]] = "P"
        if val[0] - 1 >= 0 and val[1] < 9: map_grid[val[0] - 1][val[1]] = "P"

        if val[0] - 1 >= 0 and val[1]  + 1 < 9: map_grid[val[0] - 1][val[1] + 1] = "P"
        if val[0] - 1 >= 0 and val[1]  - 1 >= 0: map_grid[val[0] - 1][val[1] - 1] = "P"
        if val[0] + 1 < 9 and val[1]  + 1 < 9: map_grid[val[0] + 1][val[1] + 1] = "P"
        if val[0] + 1 < 9 and val[1]  - 1 >= 0: map_grid[val[0] + 1][val[1] - 1] = "P"

    if len(sentinel) > 0:
        map_grid[sentinel[0][0]][sentinel[0][1]] = 'S'

        if sentinel[0][0] < 9 and sentinel[0][1]  + 1 < 9: map_grid[sentinel[0][0]][sentinel[0][1] + 1] = "P"
        if sentinel[0][0] < 9 and sentinel[0][1]  - 1 >= 0: map_grid[sentinel[0][0]][sentinel[0][1] - 1] = "P"
        if sentinel[0][0] + 1 < 9 and sentinel[0][1] < 9: map_grid[sentinel[0][0] + 1][sentinel[0][1]] = "P"
        if sentinel[0][0] - 1 >= 0 and sentinel[0][1] < 9: map_grid[sentinel[0][0] - 1][sentinel[0][1]] = "P"


    for i, val in enumerate(agents):
        map_grid[val[0]][val[1]] = "A"

    if len(sentinel) > 0:
        map_grid[sentinel[0][0]][sentinel[0][1]] = 'S'

    if map_grid[keymaker[0]][keymaker[1]] != 'K':
        map_grid[keymaker[0]][keymaker[1]] = 'Z'

    # 2) Run the executable file, give queries to my program
    start = time.time()
    p = Popen(['./astar.out'], shell=True, stdout=PIPE, stdin=PIPE)
    perception_variant = bytes('1\n', 'UTF-8')
    p.stdin.write(perception_variant)
    p.stdin.flush() # maybe remove?
    keymaker_string = str(keymaker[0]) + " " + str(keymaker[1]) + "\n"
    p.stdin.write(bytes(keymaker_string, 'UTF-8'))
    p.stdin.flush()
    result = p.stdout.readline().decode('UTF-8')
    match = re.search(r'^e', result)
    while not match:
        response = handle_move(result.split(),map_grid)
        length_of_response = str(len(response)) + "\n"
        p.stdin.write(bytes(length_of_response, 'UTF-8'))
        p.stdin.flush()
        for key in response:
            single_response = str(key) + '\n'
            p.stdin.write(bytes(single_response, 'UTF-8'))
            p.stdin.flush()
        result = p.stdout.readline().decode('UTF-8')
        if result.find('e') != -1:
            break


    if (result.split()[1] == "-1"):
        number_of_losses += 1
        print_grid(map_grid)
    else:
        number_of_wins += 1


    end = time.time()

    time_result_astar.append(end - start)


    # backtrack
    start = time.time()
    p = Popen(['./backtrack.out'], shell=True, stdout=PIPE, stdin=PIPE)
    perception_variant = bytes('1\n', 'UTF-8')
    p.stdin.write(perception_variant)
    p.stdin.flush() # maybe remove?
    keymaker_string = str(keymaker[0]) + " " + str(keymaker[1]) + "\n"
    p.stdin.write(bytes(keymaker_string, 'UTF-8'))
    p.stdin.flush()
    result = p.stdout.readline().decode('UTF-8')
    match = re.search(r'^e', result)
    while not match:
        response = handle_move(result.split(),map_grid)
        length_of_response = str(len(response)) + "\n"
        p.stdin.write(bytes(length_of_response, 'UTF-8'))
        p.stdin.flush()
        for key in response:
            single_response = str(key) + '\n'
            p.stdin.write(bytes(single_response, 'UTF-8'))
            p.stdin.flush()
        result = p.stdout.readline().decode('UTF-8')
        if result.find('e') != -1:
            break


    end = time.time()

    time_result_backtrack.append(end - start)
    print(f'Result of the A* search in {end - start} seconds: {result}')


file1 = open("statistics.txt", "a")
[file1.write(str(val) + "\n") for val in time_result_astar]
file1.write("Number of wins for A* search: " + str(number_of_wins) + '\n')
file1.write("Number of losses for A* search: " + str(number_of_losses) + '\n')
file1.close()
file2 = open("statistics_backtrack.txt", "a")
[file2.write(str(val) + "\n") for val in time_result_backtrack]
file2.write("Number of wins for backtrack search: " + str(number_of_wins) + '\n')
file2.write("Number of losses for backtrack search: " + str(number_of_losses) + '\n')
file2.close()


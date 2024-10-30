#include <iostream>
#include <set>
#include <algorithm>
#include <vector>
#include <stack>
#include <climits>
using namespace std;


pair<int, int> keymaker;

struct Node {
    // This class represents the Node, which is the single cell in the task.
    // It has itself coordinates (too easy access), coordinates of the parent (to safely backtrack)
    // Also have some value, such as f, g, h used for A* algorithm. For the backtracking algorithm
    // Only f field used. Visited means that we already visited this node, and death means
    // That at this cell we have perception zone of the enemy or enemy itself
    // Overriden operators needed for STL algorithms.
    int self_i, self_j;
    int parent_i, parent_j;
    int f, g, h;
    bool visited = false;
    bool death = false;
    bool operator<(const Node& a) const {
        return this->f < a.f;
    }
    bool operator==(const Node& a) const {
        return (this->self_i == a.self_i) && (this->self_j == a.self_j);
    }
    Node(int self_x, int self_y, int f, int g, int h, int parent_x, int parent_y, bool death): self_i(self_x), self_j(self_y),f(f),
    g(g), h(h), parent_i(parent_x), parent_j(parent_y), death(death) {}
    Node() = default;
};

// The game grid.
vector<vector<Node> > nodes(9, vector<Node>(9, Node()));

void initializeNodes () {
    // This method used to initialize all nodes to make them ready for algorithm.
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            nodes[i][j] = Node(i , j , INT_MAX, INT_MAX, INT_MAX, -1, -1, nodes[i][j].death);
        }
    }

    nodes[0][0].f = 0;
    nodes[0][0].g = 0;
    nodes[0][0].h = 0;
    nodes[0][0].parent_i = 0;
    nodes[0][0].parent_j = 0;
}


bool isPositionValid(int i, int j) {
    return (i >= 0) && (i < 9) && (j>= 0) && (j < 9);
}


bool isDestination(int i, int j) {
    if (i == keymaker.first && j == keymaker.second) {
        return true;
    }
    return false;
}

void handle_input() {
    // This function is needed for handling inputs from interactor.
    // It checks the received input, and assigns cells death value if needed.
    int number_of_things;
    cin >> number_of_things;
    for(int i = 0; i < number_of_things; i++) {
        int x, y;
        cin >> x >> y;
        string s;
        cin >> s;
        if(s == "P" || s == "A" || s == "S") {
            if (s == "P") {
                nodes[y][x].death = true;
            }
            else if (s == "A") {
                nodes[y][x].death = true;
                if(isPositionValid(y+1, x)) nodes[y+1][x].death = true;
                if(isPositionValid(y-1, x)) nodes[y-1][x].death = true;
                if(isPositionValid(y, x + 1)) nodes[y][x + 1].death = true;
                if(isPositionValid(y+1, x + 1)) nodes[y+1][x + 1].death = true;
                if(isPositionValid(y-1, x + 1)) nodes[y-1][x + 1].death = true;
                if(isPositionValid(y, x - 1)) nodes[y][x - 1].death = true;
                if(isPositionValid(y+1, x - 1)) nodes[y+1][x - 1].death = true;
                if(isPositionValid(y-1, x - 1)) nodes[y-1][x- 1].death = true;
            }
            else if (s == "S") {
                nodes[y][x].death = true;
                if(isPositionValid(y+1, x)) nodes[y+1][x].death = true;
                if(isPositionValid(y-1, x)) nodes[y-1][x].death = true;
                if(isPositionValid(y, x + 1)) nodes[y][x + 1].death = true;
                if(isPositionValid(y, x - 1)) nodes[y][x - 1].death = true;
            }
        }

    }
}

void backtrack(Node& last,const Node& currentNode) {
    // This function is used when Actor encounters deadlock, and he has to teleport
    // However, teleport are prohibited in this algorithm, therefore, I simulate sequential moves
    // So, the algorithm uses parents of the previous node and currentNode, to go to the initial position (0, 0)
    // And from that, go to the currentNode, through inverse path. Also, along the way it accepts the inputs
    // from the interactor

    while(!(last.self_i == 0 && last.self_j == 0)) {
        last = nodes[last.parent_i][last.parent_j];
        cout << "m "<< last.self_j << " " << last.self_i << endl;
        cout.flush();
        handle_input();
    }

    auto copyCurrent = currentNode;
    vector<string> path;
    while(!(copyCurrent.self_i == 0 && copyCurrent.self_j == 0)) {
        copyCurrent = nodes[copyCurrent.parent_i][copyCurrent.parent_j];
        path.emplace_back("m " + to_string(copyCurrent.self_j) + " " + to_string(copyCurrent.self_i));
    }

    reverse(path.begin(), path.end());
    path.emplace_back("m " + to_string(currentNode.self_j) + " " + to_string(currentNode.self_i));

    for(auto it = path.begin() + 1; it != path.end(); it++) {
        cout << *it << endl;
        cout.flush();
        handle_input();
    }
}


bool isMapSolvable(Node& currentNode) {
    // This function is used to check whether the map has solution
    // It is done by finding any path to the destination
    // By applying iterative DFS (The same DFS, but no in recursion format), I get some path to the keymaker.
    // If after traversing all possible nodes, I did not encounter keymaker, then the map is unsolvable, returns false.
    stack<Node> depthFirstSearch;

    depthFirstSearch.push(currentNode);

    Node last;

    while(!depthFirstSearch.empty()) {
        Node currentNode = depthFirstSearch.top();
        depthFirstSearch.pop();


        // When I have teleportation
        if(abs(last.self_i - currentNode.self_i) + abs(last.self_j - currentNode.self_j) > 1) {
            backtrack(last, currentNode);
        } else {
            cout << "m " << currentNode.self_j << " " << currentNode.self_i << endl;
            cout.flush();
            handle_input();
        }

        if (isDestination(currentNode.self_i, currentNode.self_j)) {
            last = currentNode;
            while(!(last.self_i == 0 && last.self_j == 0)) {
                last = nodes[last.parent_i][last.parent_j];
                cout << "m "<< last.self_j << " " << last.self_i << endl;
                cout.flush();
                handle_input();
            }
            return true;
        }

        last = currentNode;
        nodes[currentNode.self_i][currentNode.self_j].visited = true;


        // Add neighbouring nodes to the traversal
        if(isPositionValid(currentNode.self_i + 1,currentNode.self_j + 0) && !nodes[currentNode.self_i + 1][currentNode.self_j + 0].visited && !nodes[currentNode.self_i + 1][currentNode.self_j + 0].death) {
            nodes[currentNode.self_i + 1][currentNode.self_j + 0].parent_i = currentNode.self_i;
            nodes[currentNode.self_i + 1][currentNode.self_j + 0].parent_j = currentNode.self_j;
            depthFirstSearch.push(nodes[currentNode.self_i + 1][currentNode.self_j + 0]);

        }

        if(isPositionValid(currentNode.self_i - 1,currentNode.self_j + 0) && !nodes[currentNode.self_i - 1][currentNode.self_j + 0].visited && !nodes[currentNode.self_i - 1][currentNode.self_j + 0].death) {
            nodes[currentNode.self_i - 1][currentNode.self_j + 0].parent_i = currentNode.self_i;
            nodes[currentNode.self_i - 1][currentNode.self_j + 0].parent_j = currentNode.self_j;
            depthFirstSearch.push(nodes[currentNode.self_i - 1][currentNode.self_j + 0]);

        }

        if(isPositionValid(currentNode.self_i + 0,currentNode.self_j + 1) && !nodes[currentNode.self_i + 0][currentNode.self_j + 1].visited && !nodes[currentNode.self_i + 0][currentNode.self_j + 1].death) {
            nodes[currentNode.self_i + 0][currentNode.self_j + 1].parent_i = currentNode.self_i;
            nodes[currentNode.self_i + 0][currentNode.self_j + 1].parent_j = currentNode.self_j;
            depthFirstSearch.push(nodes[currentNode.self_i + 0][currentNode.self_j + 1]);

        }

        if(isPositionValid(currentNode.self_i + 0,currentNode.self_j - 1) && !nodes[currentNode.self_i + 0][currentNode.self_j - 1].visited && !nodes[currentNode.self_i + 0][currentNode.self_j - 1].death) {
            nodes[currentNode.self_i + 0][currentNode.self_j - 1].parent_i = currentNode.self_i;
            nodes[currentNode.self_i + 0][currentNode.self_j - 1].parent_j = currentNode.self_j;
            depthFirstSearch.push(nodes[currentNode.self_i + 0][currentNode.self_j - 1]);

        }
    }
    return false;
}

void printPath() {
    // This function used when I already have path to the keymaker, by using the backtracking function
    //  So, I use the function to trace the path through the parents to the keymaker
    // At the end, I output the answer
    auto currentNode = nodes[keymaker.first][keymaker.second];
    auto copyCurrent = nodes[keymaker.first][keymaker.second];
    vector<string> path;
    while(!(copyCurrent.self_i == 0 && copyCurrent.self_j == 0)) {
        copyCurrent = nodes[copyCurrent.parent_i][copyCurrent.parent_j];
        path.emplace_back("m " + to_string(copyCurrent.self_j) + " " + to_string(copyCurrent.self_i));
    }

    reverse(path.begin(), path.end());
    path.emplace_back("m " + to_string(currentNode.self_j) + " " + to_string(currentNode.self_i));

    for(auto it = path.begin() + 1; it != path.end(); it++) {
        cout << *it << endl;
        cout.flush();
        handle_input();
    }

    cout << "e " << currentNode.f << endl;
    cout.flush();
    exit(0);
}

void backtracking(Node& currentNode) {

    // This is the main backtracking algorithm that finds the shortest path to the keymaker
    // It uses DFS, as in the lecture slides. In the slides, the check if performed whether the path is too costly
    // or incorrect. Here, I have the same, so I check, if the neighbouring cell already have better path, so I don't
    // consider it (move back up to the three). This can be seen as pruning, I throw out non-effective branches.

    if (isDestination(currentNode.self_i, currentNode.self_j)) {
        return;
    }

    if (currentNode.death) return;

    // Update neighbouring cells costs

    if(isPositionValid(currentNode.self_i + 1, currentNode.self_j) && currentNode.f + 1 <= nodes[currentNode.self_i + 1][currentNode.self_j].f) nodes[currentNode.self_i + 1][currentNode.self_j].f = currentNode.f + 1;
    if(isPositionValid(currentNode.self_i - 1, currentNode.self_j) && currentNode.f + 1 <= nodes[currentNode.self_i - 1][currentNode.self_j].f) nodes[currentNode.self_i - 1][currentNode.self_j].f = currentNode.f + 1;
    if(isPositionValid(currentNode.self_i + 0, currentNode.self_j + 1) && currentNode.f + 1 <= nodes[currentNode.self_i + 0][currentNode.self_j + 1].f) nodes[currentNode.self_i + 0][currentNode.self_j + 1].f = currentNode.f + 1;
    if(isPositionValid(currentNode.self_i + 0, currentNode.self_j - 1) && currentNode.f + 1 <= nodes[currentNode.self_i + 0][currentNode.self_j - 1].f) nodes[currentNode.self_i + 0][currentNode.self_j - 1].f = currentNode.f + 1;

    // Analyse neighbouring cells, if can be improved, recursively call this function with child nodes.
    if(isPositionValid(currentNode.self_i + 1, currentNode.self_j + 0) &&
        !nodes[currentNode.self_i + 1][currentNode.self_j + 0].death && !(nodes[currentNode.self_i + 1][currentNode.self_j + 0].f  < currentNode.f))
    {
        nodes[currentNode.self_i + 1][currentNode.self_j + 0].f = currentNode.f + 1;
        nodes[currentNode.self_i + 1][currentNode.self_j + 0].parent_i = currentNode.self_i;
        nodes[currentNode.self_i + 1][currentNode.self_j + 0].parent_j = currentNode.self_j;
        backtracking(nodes[currentNode.self_i + 1][currentNode.self_j + 0]);
    }

    if(isPositionValid(currentNode.self_i - 1, currentNode.self_j + 0) &&
    !nodes[currentNode.self_i - 1][currentNode.self_j + 0].death && !(nodes[currentNode.self_i - 1][currentNode.self_j + 0].f < currentNode.f))
    {
        nodes[currentNode.self_i - 1][currentNode.self_j + 0].f = currentNode.f + 1;
        nodes[currentNode.self_i - 1][currentNode.self_j + 0].parent_i = currentNode.self_i;
        nodes[currentNode.self_i - 1][currentNode.self_j + 0].parent_j = currentNode.self_j;
        backtracking(nodes[currentNode.self_i - 1][currentNode.self_j + 0]);

    }

    if(isPositionValid(currentNode.self_i + 0, currentNode.self_j + 1) &&
    !nodes[currentNode.self_i + 0][currentNode.self_j + 1].death && !(nodes[currentNode.self_i + 0][currentNode.self_j + 1].f < currentNode.f))
    {
        nodes[currentNode.self_i + 0][currentNode.self_j + 1].f = currentNode.f + 1;
        nodes[currentNode.self_i + 0][currentNode.self_j + 1].parent_i = currentNode.self_i;
        nodes[currentNode.self_i + 0][currentNode.self_j + 1].parent_j = currentNode.self_j;
        backtracking(nodes[currentNode.self_i + 0][currentNode.self_j + 1]);

    }

    if(isPositionValid(currentNode.self_i + 0, currentNode.self_j - 1) &&
    !nodes[currentNode.self_i + 0][currentNode.self_j - 1].death && !(nodes[currentNode.self_i + 0][currentNode.self_j - 1].f < currentNode.f))
    {
        nodes[currentNode.self_i + 0][currentNode.self_j - 1].f = currentNode.f + 1;
        nodes[currentNode.self_i + 0][currentNode.self_j - 1].parent_i = currentNode.self_i;
        nodes[currentNode.self_i + 0][currentNode.self_j - 1].parent_j = currentNode.self_j;
        backtracking(nodes[currentNode.self_i + 0][currentNode.self_j - 1]);

    }
}

void analyseTheMap(Node& currentNode) {
    // This function is used to traverse through all the map, so I get  the all information about
    // enemies perception zones, to make my other algorithm ready for finding the shortest path.
    // It also implements iterative DFS, handles teleportations, and process neighbouring cells.

    stack<Node> depthFirstSearch;

    depthFirstSearch.push(currentNode);

    Node last;

    while(!depthFirstSearch.empty()) {
        Node currentNode = depthFirstSearch.top();
        depthFirstSearch.pop();

        if(abs(last.self_i - currentNode.self_i) + abs(last.self_j - currentNode.self_j) > 1) {
            backtrack(last, currentNode);
        } else {
            cout << "m " << currentNode.self_j << " " << currentNode.self_i << endl;
            cout.flush();
            handle_input();
        }
        last = currentNode;
        nodes[currentNode.self_i][currentNode.self_j].visited = true;


        if(isPositionValid(currentNode.self_i + 1,currentNode.self_j + 0) && !nodes[currentNode.self_i + 1][currentNode.self_j + 0].visited && !nodes[currentNode.self_i + 1][currentNode.self_j + 0].death) {
            nodes[currentNode.self_i + 1][currentNode.self_j + 0].parent_i = currentNode.self_i;
            nodes[currentNode.self_i + 1][currentNode.self_j + 0].parent_j = currentNode.self_j;
            depthFirstSearch.push(nodes[currentNode.self_i + 1][currentNode.self_j + 0]);

        }

        if(isPositionValid(currentNode.self_i - 1,currentNode.self_j + 0) && !nodes[currentNode.self_i - 1][currentNode.self_j + 0].visited && !nodes[currentNode.self_i - 1][currentNode.self_j + 0].death) {
            nodes[currentNode.self_i - 1][currentNode.self_j + 0].parent_i = currentNode.self_i;
            nodes[currentNode.self_i - 1][currentNode.self_j + 0].parent_j = currentNode.self_j;
            depthFirstSearch.push(nodes[currentNode.self_i - 1][currentNode.self_j + 0]);

        }

        if(isPositionValid(currentNode.self_i + 0,currentNode.self_j + 1) && !nodes[currentNode.self_i + 0][currentNode.self_j + 1].visited && !nodes[currentNode.self_i + 0][currentNode.self_j + 1].death) {
            nodes[currentNode.self_i + 0][currentNode.self_j + 1].parent_i = currentNode.self_i;
            nodes[currentNode.self_i + 0][currentNode.self_j + 1].parent_j = currentNode.self_j;
            depthFirstSearch.push(nodes[currentNode.self_i + 0][currentNode.self_j + 1]);

        }

        if(isPositionValid(currentNode.self_i + 0,currentNode.self_j - 1) && !nodes[currentNode.self_i + 0][currentNode.self_j - 1].visited && !nodes[currentNode.self_i + 0][currentNode.self_j - 1].death) {
            nodes[currentNode.self_i + 0][currentNode.self_j - 1].parent_i = currentNode.self_i;
            nodes[currentNode.self_i + 0][currentNode.self_j - 1].parent_j = currentNode.self_j;
            depthFirstSearch.push(nodes[currentNode.self_i + 0][currentNode.self_j - 1]);

        }
    }


    while(!(last.self_i == 0 && last.self_j == 0)) {
        last = nodes[last.parent_i][last.parent_j];
        cout << "m "<< last.self_j << " " << last.self_i << endl;
        cout.flush();
        handle_input();
    }

    cout << "m 0 0" << endl;
    cout.flush();
    handle_input();
}



int main() {
    initializeNodes();
    int perceptionVariant;
    cin >> perceptionVariant;
    int x_keymaker, y_keymaker;
    cin >> x_keymaker >> y_keymaker;
    keymaker.first = y_keymaker;
    keymaker.second = x_keymaker;
    // I check whether the map is solvable
    auto flag = isMapSolvable(nodes[0][0]);
    if (flag == true) {
        // If yes, then I again initialize nodes to get rid of isMapSolvable function remainders.
        initializeNodes();
        // Then I analyze the map for enemies.
        analyseTheMap(nodes[0][0]);
        // Then I perform main backtracking algorithm to find the shortest path to the keymaker.
        backtracking(nodes[0][0]);
        // After I found it, print the path.
        printPath();
    }

    // If I got here, then the map is not solvable, so output corresponding answer.
    cout << "e -1" << endl;
    cout.flush();
    return 0;
}

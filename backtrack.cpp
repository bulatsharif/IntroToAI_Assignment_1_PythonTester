#include <iostream>
#include <set>
#include <queue>
#include <algorithm>
#include <vector>
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
vector<vector<Node>> nodes(9, vector<Node>(9, Node()));

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

int calculateHeuristics(const Node& node) {
    return abs(node.self_i - keymaker.first) + abs(node.self_j- keymaker.second);
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


void handle_node(const Node& currentNode, queue<Node>& bfsTraversal, int l, int r) {
    // This function used for handling the neighbouring cells of the node
    // It firstly checks if the given node is valid (meaning inside the game grid)
    // And that this node is not node deadly (enemies' perception zone or the enemy itself)
    // If no, then updates the value of the node, parents, and mark as visited.
    // Then pushes the node to the queue
    // The r and l parameters are responsible for understanding to which neighbour we go

    if(isPositionValid(currentNode.self_i + l,currentNode.self_j + r) && !nodes[currentNode.self_i + l][currentNode.self_j + r].visited && !nodes[currentNode.self_i + l][currentNode.self_j + r].death) {
        nodes[currentNode.self_i + l][currentNode.self_j + r].f = currentNode.f + 1;
        nodes[currentNode.self_i + l][currentNode.self_j + r].parent_i = currentNode.self_i;
        nodes[currentNode.self_i + l][currentNode.self_j + r].parent_j = currentNode.self_j;
        nodes[currentNode.self_i + l][currentNode.self_j + r].visited = true;
        bfsTraversal.push(nodes[currentNode.self_i + l][currentNode.self_j + r]);
    }
}

void backtrack(Node& last,const Node& currentNode, queue<Node>& bfsTraversal) {
    // This function is used when, for example, Neo encountered deadlock
    // So then, the inner algorithm - BFS, will choose the next node to go with.
    // However, it will see as the teleportation, for example, we were in the node (2,0), and imagine here we have
    // deadlock, therefore, the algorithm will choose (0, 1). To "simulate" backtracking, the function is used
    // It uses parents to back to the initial position safely (as the parents path is surely safe, because
    // it was chosen by this algorithm). From initial position, it goes also using parents to the
    // "teleportation" position, simulating the movements.

    while(!(last.self_i == 0 && last.self_j == 0)) {
        last = nodes[last.parent_i][last.parent_j];
        cout << "m "<< last.self_j << " " << last.self_i << endl;
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
        handle_input();
    }
}


void BFS() {
    // The function for the "Backtracking" algorithm itself, so it is seen just as A* without heuristics,
    // or as Dijkstra on unweighted graph. So, we use the queue to make the traversal BFS style.
    // I used BFS instead of DFS, since DFS is very inefficient in shortest-path finding. (was tested)
    // I add the initial node, iterate until I get zero queue. I use aforementioned method along the way.
    // So, If I reached the goal, I print the path and send answer. If not, I proceed with algorithm, handling
    // inputs in parallel, as moves goes on. If the queue is empty, but we didn't output anything at this moment
    // Then the doesn't the solution.
    queue<Node> bfsTraversal;
    bfsTraversal.push(nodes[0][0]);
    int count = 0;

    Node last;
    while(!bfsTraversal.empty()) {
        Node currentNode = bfsTraversal.front();
        bfsTraversal.pop();
        while(currentNode.death == true) {
            currentNode = bfsTraversal.front();
            bfsTraversal.pop();
        }

        if(isDestination(currentNode.self_i,currentNode.self_j)) {
            if(abs(last.self_i - currentNode.self_i) + abs(last.self_j - currentNode.self_j) > 1) {
                backtrack(last, currentNode, bfsTraversal);
            } else {
                cout << "m " << currentNode.self_j << " " << currentNode.self_i << endl;
                handle_input();
            }
            cout << "e " << currentNode.f << endl;

            break;
        }
        nodes[currentNode.self_i][currentNode.self_j].visited = true;
        if(count != 0) {
            if(abs(last.self_i - currentNode.self_i) + abs(last.self_j - currentNode.self_j) > 1) {
                backtrack(last, currentNode, bfsTraversal);
            } else {
                cout << "m " << currentNode.self_j << " " << currentNode.self_i << endl;
                handle_input();
            }

        } else {
            cout << "m " << currentNode.self_i << " " << currentNode.self_j << endl;
            handle_input();
        }

        last = currentNode;
        handle_node(currentNode, bfsTraversal, 0, 1);
        handle_node(currentNode, bfsTraversal, 1, 0);
        handle_node(currentNode, bfsTraversal, 0, -1);
        handle_node(currentNode, bfsTraversal, -1, 0);
        count++;
    }
    cout << "e -1" << endl;
}


int main() {
    // main function that initializes nodes, takes very first parameters, and calls the main algorithm.
    initializeNodes();
    int perceptionVariant;
    cin >> perceptionVariant;
    int x_keymaker, y_keymaker;
    cin >> x_keymaker >> y_keymaker;
    keymaker.first = y_keymaker;
    keymaker.second = x_keymaker;
    BFS();
    return 0;
}

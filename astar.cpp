#include <iostream>
#include <set>
#include <algorithm>
#include <vector>
#include <climits>
using namespace std;


pair<int, int> keymaker;

struct Node {
    // This class represents the Node, which is the single cell in the task.
    // It has itself coordinates (too easy access), coordinates of the parent (to safely backtrack)
    // Also have some value, such as f, g, h used for A* algorithm.
    // That at this cell we have perception zone of the enemy or enemy itself
    // Overriden operators needed for STL algorithms.
    int self_i, self_j;
    int parent_i, parent_j;
    int f, g, h;
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

// The game grid
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

int calculateHeuristics(Node& node) {
    return abs(node.self_i - keymaker.first) + abs(node.self_j- keymaker.second);
}

bool handle_nodes(Node& current, vector<Node>& open, vector<Node>& closed,int l, int r) {
        // This function used for handling the neighbouring cells of the node
        // It firstly checks if the given node is valid (meaning inside the game grid)
        // And that this node is destination, outputting the answer.
        // If no, then updates the values of the node and parents.
        // The r and l parameters are responsible for understanding to which neighbour we go(coordinates)

        int i, j;
        if(isPositionValid(current.self_i + l, current.self_j + r)) {
            i = current.self_i + l, j = current.self_j + r;

            if(isDestination(i, j)) {
                nodes[i][j].parent_i = current.self_i;
                nodes[i][j].parent_j = current.self_j;
                cout << "e " << current.g + 1 << endl;
                cout.flush();
                return true;
            }

            if(find(closed.begin(), closed.end(), nodes[i][j]) == closed.end()) {
                auto successor = find(open.begin(), open.end(), nodes[i][j]);
                if (successor == open.end()) {
                    nodes[i][j].g = current.g + 1;
                    nodes[i][j].h = calculateHeuristics(nodes[i][j]);
                    nodes[i][j].f = nodes[i][j].g + nodes[i][j].h;
                    nodes[i][j].parent_i = current.self_i;
                    nodes[i][j].parent_j = current.self_j;

                    open.push_back(nodes[i][j]);
                }
                else {
                    if((current.g + 1) + calculateHeuristics(nodes[i][j]) < nodes[i][j].f) {
                        nodes[i][j].g = current.g + 1;
                        nodes[i][j].h = calculateHeuristics(nodes[i][j]);
                        nodes[i][j].f = nodes[i][j].g + nodes[i][j].h;
                        nodes[i][j].parent_i = current.self_i;
                        nodes[i][j].parent_j = current.self_j;

                        open.erase(successor);
                        open.push_back(nodes[i][j]);
                    }
                }
            }
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


void a_star_algorithm() {
    // The A* algorithm itself
    // Firstly checks if the keymaker position is valid
    // Then checks if the keymaker in the initial position
    // Then we initialize two vectors - closed and open, according to the A* algorithm.
    // For the next comments follow down the function.

    if (isPositionValid(keymaker.first, keymaker.second) == false) {
        cout << "e -1" << endl;
        cout.flush();
        return;
    }

    if(isDestination(0, 0) == true) {
        cout << "e 0"<< endl;
        cout.flush();
        return;
    }

    vector<Node> closed;
    vector<Node> open;

    open.push_back(nodes[0][0]);

    while(!open.empty()) {
        Node current;

        // Ignore list is used to avoid agents and sentinels cells, and their perception zones
        vector<Node> ignoreList = open;
        // found a next move without being deadly
        bool found = false;
        while(!found) {
            auto current_it = min_element(ignoreList.begin(), ignoreList.end());
            // get the minimum element, according to the A* algorithm
            current = *current_it;
            if(current.death == true) {
                ignoreList.erase(current_it);
                if(ignoreList.empty()) {
                    // All possible moves have been observed - no one leading to non death. Unsolvable
                    cout << "e -1" << endl;
                    cout.flush();
                    return;
                }

            } else {
                found = true;
                // We found a safe move, stop iterating
            }
        }

        if(closed.size() != 0) {
            // Get the last move, to trace the path
            // In the original A* algorithm, there is a "teleportation", when we observe a better move at some
            // cell, that is not neighbouring. However, according to task description, we cannot have teleportations
            // Therefore, I will "simulate" it. As usual algorithm, I will choose the "teleported" point,
            // and simulate "backtracking" and going for this new more preferable node.
            auto last = closed.back();
            if(abs(last.self_i - current.self_i) + abs(last.self_j - current.self_j) > 1) {
                // If this conditions holds, then we have a "teleportation"

                // We go to the initial node, using the parent, to safely come to the initial position.
                while(!(last.self_i == 0 && last.self_j == 0)) {
                    last = nodes[last.parent_i][last.parent_j];
                    cout << "m "<< last.self_j << " " << last.self_i << endl;
                    cout.flush();
                    handle_input();
                }

                // Here we already go back to the preferred node, also using the parents,
                // And since we go forth, we reverse the path, and print it.
                auto copyCurrent = current;
                vector<string> path;
                while(!(copyCurrent.self_i == 0 && copyCurrent.self_j == 0)) {
                    copyCurrent = nodes[copyCurrent.parent_i][copyCurrent.parent_j];
                    path.emplace_back("m " + to_string(copyCurrent.self_j) + " " + to_string(copyCurrent.self_i));
                }

                reverse(path.begin(), path.end());
                for(auto it = path.begin() + 1; it != path.end(); it++) {
                    cout << *it << endl;
                    cout.flush();
                    handle_input();
                }
            }
        }
        if(current.death == true) {
            // This could happen if we have a closed set size equal to zero.
            cout << "e -1" << endl;
            cout.flush();
        }

        cout << "m " << current.self_j << " " << current.self_i << endl;
        cout.flush();
        handle_input();
        open.erase(find(open.begin(), open.end(), current));
        closed.push_back(current);

        // Here, we call the function to handle nodes to process the neighbouring cells.
        if(handle_nodes(current, open, closed, 0, 1) ||
            handle_nodes(current, open, closed, 1, 0)||
            handle_nodes(current, open, closed, 0, -1) ||
            handle_nodes(current, open, closed, -1, 0)) {
            return;
        };

    }

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
    a_star_algorithm();
    return 0;
}

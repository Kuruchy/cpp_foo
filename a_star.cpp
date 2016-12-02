/* 
 * A* Search algorithm implementation
 * @date 2016
 * @author Kuruchy
 */

#include <vector>
#include <array>
#include <string> 
#include <iostream>
#include <algorithm> //find in vector or array
#include <random>
#include <unistd.h> //usleep
#include <time.h>

const int SIDE = 12;
const int LEN = SIDE*SIDE;
const int NEN = LEN/3;
const int MAX = 0xFFFF;
const char S = '-';
const char P = '*';
const char G = 'x';
const char E = '0';


using namespace std;

/* 
 * getRandomNumber()
 * Creates random numbers
 *
 * @input int min
 * @input int max
 * @output int [min, max)
 */
int getRandomNumber(int min, int max){
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(min, max);
    return dist(mt);
}

/*
 * Board Class
 *
 * Defines the board and the methods to do with it
 */
class Board{
    protected:
        string dist = "";
        int start;
        int pos;
        int goal;
        vector<int> enV;
    public:
        Board(){}
        Board(int s, int g){
            start=s;
            pos=s;
            goal=g;
            // Generates the enemy array randomly
            for (int iii=0; iii<NEN; iii++){
                int en = getRandomNumber(0, LEN-1);
                while (en == start || en == goal)
                    en = getRandomNumber(0, LEN-1);
                enV.push_back(en);
            }
            Config();
        }
        int getStart(){
            return this->start;
        }
        int getPos(){
            return this->pos;
        }
        int getGoal(){
            return this->goal;
        }
        vector<int> getEnV(){
            return this->enV;
        }
        void Config() {
            this->dist = "";
            
            // First fill all with slashes
            for (int iii=0; iii<LEN; iii++){
                this->dist += S;
            }

            // Fill with enemies
            for (int iii : this->enV){
                this->dist[iii] = E;
            }

            // Put origin and destination
            //this->dist[pos] = P;
            this->dist[this->goal] = G;
        }
        void Display(int pos){
            system("clear");
            cout << "A* Search" << endl;
            cout << "From " << start << " to " << goal << endl;
            cout << "Pos: " << pos << endl;
            int iii = 0;
            for (char ccc : this->dist){
                if (iii == pos)
                    cout << P << " ";
                else
                    cout << ccc << " ";
                iii++;
                if (iii%SIDE==0)
                cout << endl;
            }
            cout << endl;
        }
};

/*
 * getNeighbors()
 *
 * Evaluates the neighbors for an integer, returning just
 * the valid neighbors. Removes the out of matrix, and the
 * wall neighbors.
 *
 * @input int index
 * @input vector<int> enemies (Walls)
 * @output vector<int> neighbors
 */
vector<int> getNeighbors(int index, vector<int> enV){
    int u = index-SIDE;
    int d = index+SIDE;
    int l = index-1;
    int r = index+1;
    vector<int> neighbors;

    if (index == 0)
        // No up, no left
        neighbors = {d,r};
    else if (index == SIDE-1)
        // No up, no right
        neighbors = {d,l};
    else if (index == LEN-SIDE)
        // No down, no left
        neighbors = {u,r};
    else if (index == LEN-1)
        // No down, no right
        neighbors = {u,l};
    else if (index < SIDE)
        // No up
        neighbors = {d, l, r};
    else if (index > LEN-SIDE)
        // No down
        neighbors = {u, l, r};
    else if (index%SIDE == 0)
        // No left
        neighbors = {u, d, r};
    else if (index%SIDE == SIDE-1)
        // No right
        neighbors = {u, d, l};
    else
        // Normal
        neighbors = {u, d, l, r};

    // Delete neighbor if is a wall
    for (int iii=neighbors.size()-1; iii>=0; iii--){
        if(find(enV.begin(), enV.end(), neighbors.at(iii)) != enV.end()){
            neighbors.erase(neighbors.begin()+iii);
        }
    }

    return neighbors;
}

/*
 * reconstruct_path()
 *
 * Reconstructs the path from the came_from array.
 *
 * @input array<int, LEN> came_from
 * @input int current_index
 * @input Board board 
 */
void reconstruct_path(array<int, LEN> came_from, int current_index, Board board){

    int pos = board.getPos();
    int goal = board.getGoal();

    int curretn = goal;
    vector<int> total_path = {goal};
    int iii = 0;
    while (curretn != pos){
        if (iii > LEN*2){
            cout << "Problem with current = pos" << endl;
            exit(0);
        }
        curretn = came_from.at(curretn);
        total_path.push_back(curretn);
        iii++;
    }

    reverse(total_path.begin(), total_path.end());
    
    for (int iii : total_path){
        board.Display(iii);
        usleep(200000);
    }
}

int  heuristic_cost(int start, int goal){
    return abs((goal-start)%SIDE) * 0.001;
}

int dist_between(int start, int goal){
    if (abs(goal - start > SIDE))
        return abs(goal-start)%SIDE;
    else
        return abs(goal-start);
}

/*
 * A* algorithm implementation
 * 
 * @input Board board
 */
clock_t a_star(Board board){

    int start = board.getPos();
    int goal = board.getGoal();

    // Set of nodes already Evaluated
    vector<int> closed_set {};

    // Set of currently discovered nodes, still to be evaluated
    // Initially, only the start node is known
    vector<int> open_set {start};

    // For each node, which node it can most efficiently be reached from
    // If a node can be reached from many nodes, cameFrom will eventually
    // contain the most efficient previous step
    array<int, LEN> came_from {0};
    
    // The cost of getting from the start node to that node
    array<int, LEN> g_score;
    
    // The cost of going from start node to start is zero
    g_score.at(start) = 0;

    // The total cost of getting from the start node to the goal, passing
    // by that node. That value is partly known, partly heuristic
    array<int, LEN> f_score;

    // Fill the f_score with maximun values
    for (int iii=0; iii<LEN; iii++){
        f_score.at(iii) = MAX;
    }

    // For the first node, that value is completely heuristic
    f_score[start] = heuristic_cost(start, goal);

    while (open_set.size() > 0){
        // Is the node in open_set having the lowest f_score
        int current_index;
        int lowest_f_score = MAX;
        for (int iii=0; iii<open_set.size(); iii++){
            if (f_score[open_set[iii]] < lowest_f_score){
                lowest_f_score = f_score[open_set[iii]];
                current_index = iii;
            }
        }

        // Get current node
        int curretn = open_set.at(current_index);

        if (curretn == goal){
            clock_t end_time = clock();
            reconstruct_path(came_from, current_index, board);
            return end_time;
            break;
        }

        open_set.erase(open_set.begin()+current_index);
        closed_set.push_back(curretn);

        // Loop through all the neighbors of current_index
        vector<int> current_neighbors = getNeighbors(curretn, board.getEnV());

        for (int neighbor : current_neighbors){
            // Ignore neighbo already evaluated
            if(find(closed_set.begin(), closed_set.end(), neighbor) != closed_set.end())
                continue;
            // The distance from start to a neighbor
            int tent_g_score = g_score[curretn] + dist_between(curretn, neighbor);
            if(find(open_set.begin(), open_set.end(), neighbor) == open_set.end()){
                // Discover a new node
                open_set.push_back(neighbor);
            }else if (tent_g_score >= g_score.at(neighbor)){
                // This is not a better path
                continue;
            }
            // This is the best path until now
            came_from.at(neighbor) = curretn;
            g_score.at(neighbor) = tent_g_score;
            f_score.at(neighbor) = g_score.at(neighbor) + heuristic_cost(neighbor, goal);
        }
    }
}

int main() {  

    // Init pos randomly generated
    int pos = getRandomNumber(0, LEN);
    // Goal pos randomly generated
    int goal = pos;
    while (goal == pos){
        goal = getRandomNumber(0, LEN);
    }
    // Create the Board
    Board board(pos, goal);

    clock_t begin_time = clock();
    clock_t end_time = a_star(board);
    
    // Print time taken to complete
    printf("Time: %.2fs\n", (double)((end_time - begin_time)*1000)/CLOCKS_PER_SEC);

    return 0;
}

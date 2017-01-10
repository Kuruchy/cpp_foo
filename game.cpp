/* 
 * Small persecution game C++ Code
 * @date 2016
 * @author Kuruchy
 */

#include <vector>
#include <array>
#include <string> 
#include <iostream>
#include <algorithm> //find in vector or array
#include <random>
#include <unistd.h>  //usleep
#include <time.h>


const short int SIDE = 8;
const short int LEN = SIDE*SIDE;
const int MAX = 0xFFFF;
const char SP = '.';
const char LRW = '|';
const char DW = '-';

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
 * getNeighbors()
 *
 * Evaluates the neighbors for an integer, returning just
 * the valid neighbors. Removes the out of matrix, and the
 * non-valid neighbors.
 *
 * @input int index
 * @input vector<int> enemies (Walls)
 * @output vector<int> neighbors
 */
vector<int> getNeighbors(int index, vector<int> enemy_vect){
    int u = index-SIDE;
    int d = index+SIDE;
    int l = index-1;
    int r = index+1;
    vector<int> neighbors;

    if (index == 0){
        // No up, no left
        neighbors = {d,r};
    }else if (index == SIDE-1){
        // No up, no right
        neighbors = {d,l};
    }else if (index == LEN-SIDE){
        // No down, no left
        neighbors = {u,r};
    }else if (index == LEN-1){
        // No down, no right
        neighbors = {u,l};
    }else if (index < SIDE){
        // No up
        neighbors = {d, l, r};
    }else if (index > LEN-SIDE){
        // No down
        neighbors = {u, l, r};
    }else if (index%SIDE == 0){
        // No left
        neighbors = {u, d, r};
    }else if (index%SIDE == SIDE-1){
        // No right
        neighbors = {u, d, l};
    }else{
        // Normal
        neighbors = {u, d, l, r};
    }

    // Delete neighbor if is a wall
    for (int iii=neighbors.size()-1; iii>=0; iii--){
        if(find(enemy_vect.begin(), enemy_vect.end(), neighbors.at(iii)) != enemy_vect.end()){
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
 * @input int goal
 * @output shortest path
 */
vector<int> reconstruct_path(array<int, LEN> came_from, int pos, int goal){

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
    return total_path;
}

/*
 * heuristic_cost()
 * 
 * Calculate the heuristic cost
 * @input int start
 * @input int goal
 * @output cost
 */
int heuristic_cost(int start, int goal){
    return abs((goal-start)%SIDE) * 0.001;
}

/*
 * dist_between()
 * 
 * Calculate the distance
 * @input int start
 * @input int goal
 * @output dist
 */
int dist_between(int start, int goal){
    if (abs(goal - start > SIDE)){
        return abs(goal-start)%SIDE;
    }else{
        return abs(goal-start);
    }
}

/*
 * A* algorithm implementation
 * 
 * @input Board board
 * @input int pos
 * @input vector<int> enemy_vect
 * @output shortest path
 */
vector<int> a_star(int start, int goal, vector<int> enemy_vect){

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
            return reconstruct_path(came_from, start, goal);
        }

        open_set.erase(open_set.begin()+current_index);
        closed_set.push_back(curretn);

        // Loop through all the neighbors of current_index
        vector<int> current_neighbors = getNeighbors(curretn, enemy_vect);

        for (int neighbor : current_neighbors){
            // Ignore neighbo already evaluated
            if(find(closed_set.begin(), closed_set.end(), neighbor) != closed_set.end()){
                continue;
            }
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

/*
 * PJ Class
 * Defines the PJ and the methods to do with it.
 */
class PJ{
    protected:
        char representation;
        short int faction;
        short int initial_actions;
        short int actions;
    public:
        vector<int> destination;
        PJ(){}
        PJ(char r, int f){
            representation=r;
            faction=f;
            initial_actions=3;
            actions=0;
        }
        char getRepresentation(){
            return this->representation;
        }
        short int getFaction(){
            return this->faction;
        }
        short int getActionsRemaining(){
            return this->initial_actions - this->actions;
        }
        void updateActions(){
            this->actions++;
        }
        void resetActions(){
            this->actions = 0;
        }
};

/*
 * NPJ Class
 * Defines the NPJ and the methods to do with it.
 */
class NPJ{
    protected:
        char representation;
        short int faction;
        short int initial_actions;
        short int actions;
    public:
        vector<int> destination;
        NPJ(){}
        NPJ(char r, int f){
            representation=r;
            faction=f;
            initial_actions=3;
            actions=0;
        }
        char getRepresentation(){
            return this->representation;
        }
        short int getFaction(){
            return this->faction;
        }
        short int getActionsRemaining(){
            return this->initial_actions - this->actions;
        }
        void updateActions(){
            this->actions++;
        }
        void resetActions(){
            this->actions = 0;
        }
};

/*
 * Tile Class
 * Defines the Tile and the methods to do with it.
 */
class Tile{
    protected:
        char representation;
        // 0-street 1-interior etc
        short int tile_class;
    public:
        vector<PJ> pjs_vector;
        vector<NPJ> npjs_vector;
        Tile(){}
        Tile(char r, int t){
            representation=r;
            tile_class=t;
        }
        char getRepresentation(){
            return this->representation;
        }
        short int getTileClass(){
            return this->tile_class;
        }
        void setTileClass(short int tclas)
        {
            this->tile_class = tclas;
        }
};

/*
 * Board Class
 * Defines the board and the methods to do with it.
 */
class Board{
    protected:
        short int turn;
    public:
        array<Tile, LEN> tile_array;
        vector<int> enemy_vect;
        vector<int> wall_vect;
        Board(){}
        Board(short int t){
            turn=t;
            Config();
        }
        void Config(){

            for (int iii=0; iii<LEN; iii++){
                Tile T(SP, 0);
                tile_array[iii] = T;               
            }

            // Hero
            PJ P('H', 1);
            tile_array[44].pjs_vector.push_back(P); 

            // Zombie
            NPJ Z('Z', 2);
            tile_array[24].npjs_vector.push_back(Z);
            enemy_vect.push_back(24);

            // Walls
            wall_vect.push_back(25);
            tile_array[25].setTileClass(1);
            wall_vect.push_back(17);
            tile_array[17].setTileClass(1);
            wall_vect.push_back(33);
            tile_array[33].setTileClass(1);
        }
        void Display(){
            system("clear");
            int iii = 0;
            char ccc;
            for (Tile tile : this->tile_array){
                // If hero print 'em
                if (tile.pjs_vector.begin()!=tile.pjs_vector.end() &&
                    tile.npjs_vector.begin()!=tile.npjs_vector.end()){
                    ccc='X';
                }else if(tile.pjs_vector.begin()!=tile.pjs_vector.end()){
                    ccc=tile.pjs_vector[0].getRepresentation();
                }else if(tile.npjs_vector.begin()!=tile.npjs_vector.end()){
                    ccc=tile.npjs_vector[0].getRepresentation();
                }else{
                    ccc=tile.getRepresentation();
                }

                // Represent the enemies
                if (tile.getTileClass() == 1){
                    cout << ccc << "|";
                }else{
                    cout << ccc << " ";
                }
                
                iii++;
                if (iii%SIDE==0)
                    cout << endl;
            }
            cout << endl;    
        }

        short int moveNPJ(int npj_pos, int pj_pos){

            short int npj_interm;
            vector<int> v = a_star(npj_pos, pj_pos, this->wall_vect);

            for (int iii : v){
                if (iii != npj_pos){
                    npj_interm = iii;
                    break;
                }
            }

            if (npj_pos != pj_pos){
                this->tile_array[npj_interm].npjs_vector.push_back(this->tile_array[npj_pos].npjs_vector[0]);
                this->tile_array[npj_pos].npjs_vector.clear();
            }
            return npj_interm;
        }

        short int movePJ(int pj_pos, char dir){
            
            short int pj_dest = pj_pos;

            switch (dir){
                case 'U':
                case 'u':
                    if (pj_pos >= SIDE)
                        pj_dest-=SIDE;
                break;
                case 'd':
                case 'D':
                    if (LEN - pj_pos > SIDE)
                        pj_dest+=SIDE;
                break;
                case 'r':
                case 'R':
                    if (pj_pos < LEN-1 && pj_pos%SIDE != SIDE-1)
                        pj_dest+=1;
                  break;
                case 'l':
                case 'L':
                    if (pj_pos > 0 && pj_pos%SIDE != 0)
                        pj_dest-=1;
                break;
            }
            if (pj_pos != pj_dest){
                this->tile_array[pj_dest].pjs_vector.push_back(this->tile_array[pj_pos].pjs_vector[0]);
                this->tile_array[pj_dest].pjs_vector[0].updateActions();
                this->tile_array[pj_pos].pjs_vector.clear();
            }
            return pj_dest;
        }

};

int main() {

    short int player_pos = 44;
    short int zombie_pos = 24;
    char movement;
    Board board(0);
    board.Display();
    clock_t begin_time = clock();

    while (true){
        board.tile_array[player_pos].pjs_vector[0].resetActions();
        while (board.tile_array[player_pos].pjs_vector[0].getActionsRemaining() > 0){
            cout << "Player's turn!" << endl;
            cout << "PJ in pos " << player_pos << ", you move. Act(" << board.tile_array[player_pos].pjs_vector[0].getActionsRemaining() << ")" << endl;
            cin >> movement;

            player_pos = board.movePJ(player_pos, movement);

            usleep(200000);
            board.Display();
        }
            cout << "Zombie's turn!" << endl;
            cout << "NPJ at " << zombie_pos << " move!" << endl;
            zombie_pos = board.moveNPJ(zombie_pos, player_pos);

            usleep(200000);
            board.Display();
    }

    // Print time taken to complete
    printf("Time taken: %.2fs\n", (double)((clock() - begin_time)*1000)/CLOCKS_PER_SEC);
    return 0;
}

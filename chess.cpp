/* 
 * Chess
 * @date 2016
 * @author Kuruchy
 */

#include <vector>
#include <array>
#include <string> 
#include <iostream>
#include <algorithm> //find in vector or array
#include <unistd.h>  //usleep
#include <time.h>


const short int SIDE = 8;
const short int LEN = SIDE*SIDE;
const char SP = '-';

using namespace std;

/*
 * Piece Class
 * Defines the piece and the methods to do with it.
 */
class Piece{
    protected:
        char ch;
        short int owner;
        short int first_move;
    public:
        vector<int> v_dest;
        Piece(){}
        Piece(char c, int o){
            ch=c;
            owner=o;
            first_move=1;
        }
        char getCh(){
            return this->ch;
        }
        short int getOwner(){
            return this->owner;
        }
        short int getFirsMove(){
            return this->first_move;
        }
        void setFirstMove(short int first_move){
            this->first_move = first_move;
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
        array<Piece, LEN> board_arr;
        Board(){}
        Board(short int t){
            turn=t;
            Config();
        }
        void Config(){

            short int ttt = 0;
            for (char ccc : "thbkqbht"){
                Piece P(ccc, 2);
                board_arr[ttt] = P;
                ttt++;
            }

            for (int iii=SIDE; iii<2*SIDE; iii++){
                Piece P('p', 2);
                board_arr[iii] = P;               
            }

            for (int iii=2*SIDE; iii<6*SIDE; iii++){
                Piece P(SP, 0);
                board_arr[iii] = P;               
            }

            for (int iii=6*SIDE; iii<7*SIDE; iii++){
                Piece P('P', 1);
                board_arr[iii] = P;               
            }

            ttt = 7*SIDE;
            for (char ccc : "THBKQBHT"){
                Piece P(ccc, 1);
                board_arr[ttt] = P;
                ttt++;
            }

        }
        void Display(){
            system("clear");
            cout << "  a b c d e f g h" << endl;
            int iii = 0;
            for (Piece piece : this->board_arr){
                if (iii%SIDE==0)
                    cout << iii/SIDE + 1 << " ";
                cout << piece.getCh() << " ";
                iii++;
                if (iii%SIDE==0)
                    cout << endl;
            }
            cout << endl;    
        }
        void UpdateMovList(){

            for (int i=0; i<LEN; i++){
                this->board_arr.at(i).v_dest.clear();
                // Add the possible moves to all pieces
                int move;
                int oponent;
                int dir;
                int depth_arr[] = {1,1,1,1};
                int depth_arrQ[] = {1,1,1,1,1,1,1,1};

                if (this->board_arr.at(i).getCh()>=97){
                    oponent = 1;
                    dir = SIDE;
                }
                else if (this->board_arr.at(i).getCh()>=65){
                    oponent = 2;
                    dir = -SIDE;
                }
                else{
                    oponent = 0;
                }

                switch(this->board_arr.at(i).getCh()){
                    // Pawn movements
                    case 'P':
                    case 'p':
                        // First movement +2
                        if (this->board_arr.at(i).getFirsMove()==1){
                            move = i + 2*dir;
                            if (this->board_arr[move].getOwner()==0)
                                this->board_arr.at(i).v_dest.push_back(move);
                        }
                        // Normal movement
                        move = i + dir;
                        if (this->board_arr[move].getOwner()==0)
                            this->board_arr.at(i).v_dest.push_back(move);
                        // Diagonal kill
                        move = i + dir - 1;
                        if (this->board_arr[move].getOwner()==oponent){
                            this->board_arr.at(i).v_dest.push_back(move);
                        }
                        // Diagonal kill
                        move = i + dir + 1;
                        if (this->board_arr[move].getOwner()==oponent){
                            this->board_arr.at(i).v_dest.push_back(move);
                        }
                    break;
                    // Tower movements
                    case 'T':
                    case 't':
                        // Normal movement
                       for (int depth=1; depth<SIDE; depth++){
                            int moves[] = {i + depth*SIDE*depth_arr[0], i + depth*1*depth_arr[1], i - depth*1*depth_arr[2], i - depth*SIDE*depth_arr[3]};
                            int move_index=0;
                            for (int move : moves){
                                if (move >= 0 && move < LEN){
                                    if (this->board_arr[move].getOwner()==0){
                                        this->board_arr.at(i).v_dest.push_back(move);
                                    }else if(this->board_arr[move].getOwner()==oponent){
                                        this->board_arr.at(i).v_dest.push_back(move);
                                        depth_arr[move_index]=0;
                                    }else{
                                        depth_arr[move_index]=0;
                                    }
                                }
                                move_index++;
                            }
                        }
                    break;
                    // Horse movements
                    case 'H':
                    case 'h':
                        // Normal movement
                        for (int depth=1; depth<3; depth++){
                            int moves[] = {i + depth*SIDE + (3 - depth), i - depth*SIDE + (3 - depth), i + depth*SIDE - (3 - depth), i - depth*SIDE - (3 - depth)};

                            for (int move : moves){
                                if (move >= 0 && move < LEN){
                                    if (this->board_arr[move].getOwner()==0 ||
                                        this->board_arr[move].getOwner()==oponent){
                                        this->board_arr.at(i).v_dest.push_back(move);
                                    }
                                }
                            }
                        }
                    break;
                    // Bishop movements
                    case 'B':
                    case 'b':
                        // Normal movement
                        for (int depth=1; depth<SIDE; depth++){    
                            int moves[] = {i + depth*(SIDE + 1)*depth_arr[0], i + depth*(SIDE - 1)*depth_arr[1], i + depth*(-SIDE + 1)*depth_arr[2], i + depth*(-SIDE - 1)*depth_arr[3]};
                            int move_index=0;
                            for (int move : moves){
                                if (move >= 0 && move < LEN){
                                    if (this->board_arr[move].getOwner()==0){
                                        this->board_arr.at(i).v_dest.push_back(move);
                                    }else if(this->board_arr[move].getOwner()==oponent){
                                        this->board_arr.at(i).v_dest.push_back(move);
                                        depth_arr[move_index]=0;
                                    }else{
                                        depth_arr[move_index]=0;
                                    }
                                }
                                move_index++;
                            }
                        }
                    break;
                    // Queen movements
                    case 'Q':
                    case 'q':
                        // Normal movements
                        for (int depth=1; depth<SIDE; depth++){    
                            int moves[] = {i + depth*SIDE*depth_arrQ[0], i + depth*1*depth_arrQ[1], i - depth*1*depth_arrQ[2], i - depth*SIDE*depth_arrQ[3],
                                           i + depth*(SIDE + 1)*depth_arrQ[4], i + depth*(SIDE - 1)*depth_arrQ[5], i + depth*(-SIDE + 1)*depth_arrQ[6], i + depth*(-SIDE - 1)*depth_arrQ[7]};
                            int move_index=0;
                            for (int move : moves){
                                if (move >= 0 && move < LEN){
                                    if (this->board_arr[move].getOwner()==0){
                                        this->board_arr.at(i).v_dest.push_back(move);
                                    }else if(this->board_arr[move].getOwner()==oponent){
                                        this->board_arr.at(i).v_dest.push_back(move);
                                        depth_arrQ[move_index]=0;
                                    }else{
                                        depth_arrQ[move_index]=0;
                                    }
                                }
                                move_index++;
                            }
                        }
                    break;
                    // King movements
                    case 'K':
                    case 'k':
                        // Normal movements  
                        int moves[] = {i + SIDE, i + 1, i - 1, i - SIDE,
                                       i + SIDE + 1, i + SIDE - 1, i - SIDE + 1, i - SIDE - 1};

                        for (int move : moves){
                            if (move >= 0 && move < LEN){
                                if (this->board_arr[move].getOwner()==0 ||
                                    this->board_arr[move].getOwner()==oponent){
                                    this->board_arr.at(i).v_dest.push_back(move);
                                }
                            }
                        }
                    break;
                }
            }
        }

        int Move(int p1, int p2, int active_player){
            int mod = 1;

            if (this->board_arr[p1].getOwner() == active_player &&
                this->board_arr[p2].getOwner() != active_player &&
                this->board_arr[p2].getOwner() != 0){

                if (this->board_arr[p2].getCh() == 'K' ||
                    this->board_arr[p2].getCh() == 'k')
                    mod = -1;

                // Perform Killing
                this->board_arr[p2] = this->board_arr[p1];

                Piece S(SP, 0);
                this->board_arr[p1] = S;

            }else{
                // Perform Movement
                Piece temp = board_arr[p2];

                this->board_arr[p2] = this->board_arr[p1];

                this->board_arr[p1] = temp;
            }

            // Set first move to cero
            this->board_arr[p2].setFirstMove(0);

            return (active_player==1)?active_player=2*mod:active_player=1*mod;          
        }
};

int main() {

    short int active_player = 1;
    string movement;
    Board board(0);
    board.Display();
    clock_t begin_time = clock();

    while (active_player > 0){
        cout << "Player " << active_player << " you move: " << endl;
        cin >> movement;
        usleep(200000);

        // Calculate the positions p1 and p2
        int p1 = movement[0] - 97 + (movement[1] - 49)*SIDE; //ascii a y 1
        int p2 = movement[2] - 97 + (movement[3] - 49)*SIDE; //ascii a y 1

        board.UpdateMovList();

        // Check for ilegal movements
        if (movement.size() > 4 || 
            movement[0] > 104 || movement[0] < 97 ||
            movement[2] > 104 || movement[2] < 97 ||
            movement[1] > 56 || movement[1] < 49 ||
            movement[3] > 56 || movement[3] < 49 ||
            board.board_arr[p1].getOwner() != active_player ||
            find(board.board_arr[p1].v_dest.begin(), board.board_arr[p1].v_dest.end(), p2) == board.board_arr[p1].v_dest.end()){

            cout << "Error: Movement not valid!" << endl;
            usleep(200000);
            continue;
        }
        active_player = board.Move(p1, p2, active_player);
        board.Display();
    }

    // Print time taken to complete
    printf("Time taken: %.2fs\n", (double)((clock() - begin_time)*1000)/CLOCKS_PER_SEC);
    return 0;
}


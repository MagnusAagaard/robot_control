#include <random>
#include <vector>
#include <iostream>
using namespace std;

// Dimensions of the environment
#define COLUMNS 4
#define ROWS 3
#define ACTIONS 4
#define STATES 11

// Environment -- spaces: agent can move, "+": reward, "-": punishment.
char environment[ROWS][COLUMNS] = { { ' ', ' ', ' ', ' ' },
                                    { ' ', ' ', ' ', ' ' },
                                    { ' ', ' ', '+', ' ' } };

vector<vector<float>> qTable;

struct state {
    int row;
    int col;
    bool terminal;
};

const state terminalState = {-1, -1, true};

float discountRate = 0.9f;

float theta = 0.01f;

float learningRate = 0.4f;

int episodes = 0;
int maxEpisodes = 100000;

enum action {UP, DOWN, LEFT, RIGHT};

action actions[ACTIONS] = {UP, DOWN, LEFT, RIGHT};

void initQtable(){
    string maxStateString = to_string(ROWS) + to_string(COLUMNS);
    int maxState = stoi(maxStateString) + 1;

    vector<float> temp;

    for (int k = 0; k < ACTIONS; k++) {
        temp.push_back(0.0f);
    }

    cout << "Init qTable" << endl;
    for (int i = 0; i < maxState; i++) {
        qTable.push_back(temp);
    }

    /*
    for (int _state = 0; _state < STATES; _state++) {
        for (int _action = 0; _action < ACTIONS; _action++)
            qTable[_state][_action] = 0.0f;

    }
    */
}

state getNextState(state _state, action _action){
    if (environment[_state.row][_state.col] != ' ')
        return terminalState;

    switch (_action){
        case UP:
            _state.row -= 1;
            break;
        case DOWN:
            _state.row += 1;
            break;
        case LEFT:
            _state.col -= 1;
            break;
        case RIGHT:
            _state.col += 1;
            break;
    }

    if (_state.row < 0 || _state.col < 0 || _state.row >= ROWS || _state.col >= COLUMNS)
        return terminalState;

    _state.terminal = false;
    return _state;
}

float getReward(state _state, action _action){
    state nextState = getNextState(_state, _action);

    if (nextState.row < 0 || nextState.col < 0 || nextState.row >= ROWS || nextState.col >= COLUMNS)
        return -1.0f;

    if (environment[nextState.row][nextState.col] == '+')
        return 10;
    return 0;

    //return rewardTable[nextState.row][nextState.col];
}

int getStateID(state _state) {
    string stateIDString = to_string(_state.row) + to_string(_state.col);
    return stoi(stateIDString);

    //return COLUMNS * _state.row + _state.col;
}

action bestAction(state _state){
    action bestAction = UP;
    float qValue, maxQvalue = -100;
    int stateID = getStateID(_state);

    for (int i = 0; i < ACTIONS; i++) {
        qValue = qTable[stateID][i];
        if (maxQvalue < qValue){
            maxQvalue = qValue;
            bestAction = actions[i];
        }
    }
    return bestAction;
}

action getAction(state _state){
    random_device rd;
    uniform_int_distribution<int> dist(0,maxEpisodes);
    uniform_int_distribution<int> distAction(0, ACTIONS - 1);

    if (episodes * 0.9 < dist(rd)){
        return actions[distAction(rd)];
    }

    return bestAction(_state);
}

float getMaxQalueAtState(state _state){
    int stateID = getStateID(_state);
    float qValue, maxQvalue = -100.f;

    for (int i = 0; i < ACTIONS; i++) {
        qValue = qTable[stateID][i];
        if (maxQvalue < qValue){
            maxQvalue = qValue;
        }
    }
    return maxQvalue;
}

void printQtable(){
    string stateIDString;
    int stateID;

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLUMNS; col++) {
            if (environment[row][col] == ' ') {
                stateIDString = to_string(row) + to_string(col);
                stateID = stoi(stateIDString);
                cout << stateIDString;
                for (int _action = 0; _action < ACTIONS; _action++)
                    printf(" %5.2f ", qTable[stateID][_action]);

                printf("\n");
            }
        }
    }

    /*
    for (int _state = 0; _state < STATES; _state++) {
        for (int _action = 0; _action < ACTIONS; _action++)
            printf(" %5.2f ", qTable[_state][_action]);

        printf("\n");
    }
    */
}

// Print the environment with border around:
void printEnvironment() {
    for (int y = -1; y <= ROWS; y++)
    {
        for (int x = -1; x <= COLUMNS; x++)
            if (y < 0 || y >= ROWS || x < 0 || x >= COLUMNS)
                std::cout << "#";
            else
                std::cout << environment[y][x];

        std::cout << std::endl;
    }
}

string actionName(action _action) {
        switch (_action){
            case UP:
                return "UP";

            case DOWN:
                return "DOWN";

            case LEFT:
                return "LEFT";

            case RIGHT:
                return "RIGHT";

        }
};

int main() {

    initQtable();
    printQtable();
    cout << endl;


    state _state, nextState;
    action _action;
    int stateID;
    float reward, maxQvalueAtState;

    while (episodes < maxEpisodes){

        _state = {0, 0, false};

        ++episodes;

        while(!_state.terminal){
            _action = getAction(_state);
            //cout << "action: " << _action << endl;

            nextState = getNextState(_state, _action);
            //cout << "NextState: " << nextState.row << nextState.col << nextState.terminal << endl;

            cout << "Taking action " << actionName(_action) <<" Going from " << getStateID(_state) << " to " << getStateID(nextState) << endl;
            stateID = getStateID(_state);
            //cout << "StateID: " << stateID << endl;

            if (nextState.terminal)
                maxQvalueAtState = 0;
            else
                maxQvalueAtState = getMaxQalueAtState(nextState);
            //cout << "maxQvalueAtState: " << maxQvalueAtState << endl;

            reward = getReward(_state, _action);
            //cout << "Reward: " << reward << endl;

            //cout << "Updating qTable.." << endl;
            qTable[stateID][_action] = (1-learningRate) * qTable[stateID][_action]
                    + learningRate * (reward + discountRate * maxQvalueAtState);

            if (environment[nextState.row][nextState.col] == '+')
                break;

            _state = nextState;
        }

        cout << "Episodes: " << episodes << endl;
    }

    printEnvironment();
    printQtable();


    std::cout << "Hello, World!" << std::endl;
    return 0;
}
//
// Created by August Mader on 29/11/2019.
//

#include <math.h>
#include <random>
#include <vector>
#include <iostream>
using namespace std;

// Dimensions of the environment

#define ACTIONS 4

/*
#define ROWS 5
#define COLUMNS 5
char environment[ROWS][COLUMNS] = { {' ',' ',' ',' ',' '},
                                    {' ','+',' ','#','+'},
                                    {' ','#',' ','#',' '},
                                    {' ','#',' ',' ',' '},
                                    {'+','#',' ','+',' '},
                                    };
                                    */
/*
#define ROWS 21
#define COLUMNS 24

char environment[ROWS][COLUMNS] = {
        {' ', ' ', ' ', '#', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#', '#', '#', '#', ' ', ' '},
        {' ', '+', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#', ' ', ' '},
        {' ', ' ', ' ', '#', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '+', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' '},
        {'#', '#', '#', '#', ' ', '+', '#', ' ', ' ', '+', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', '#', '#', '#', '#', '#', '#', '#', ' ', ' ', '#', '+', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#', '+', ' ', '#', ' ', ' '},
        {' ', '+', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '+', ' ', ' ', '#', ' ', ' ', '#', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#', ' ', ' '},
        {'#', '#', '#', '#', '#', ' ', '#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', ' ', '#', '#', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', '#', '#', ' ', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', '+', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', '+', ' ', '#', ' ', '+', ' ', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', '+', ' ', ' ', ' ', '#', ' ', ' ', '+', ' ', '#', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' '}
};*/

/*
char environment[ROWS][COLUMNS] = {{ ' ',' ','#',' ','#',' ',' '},
                                   { ' ',' ',' ',' ','#',' ',' '},
                                   { ' ','+','#',' ','#',' ',' '},
                                   { ' ','#','#',' ','#',' ',' '},
                                   { ' ',' ',' ',' ','#',' ','#'},
                                   { '#','#','#',' ',' ',' ',' '},
                                   { ' ','+','#',' ','#','#','#'},
                                   { ' ','#','#',' ',' ',' ',' '},
                                   { ' ',' ',' ',' ','#',' ',' '}};*/

#define ROWS 11
#define COLUMNS 10

char environment[ROWS][COLUMNS] = {
        {'+',' ',' ','#',' ','#','#',' ','#',' '},
        {'#','#',' ','#',' ','+','#',' ',' ',' '},
        {'+',' ',' ','#',' ','#','#',' ','#','+'},
        {'#','#',' ',' ',' ','+','#',' ','#',' '},
        {' ',' ',' ','#',' ','#','#',' ','#',' '},
        {' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
        {' ','#',' ','#','#','#','#','#','#',' '},
        {' ','#',' ','#','+',' ',' ',' ',' ',' '},
        {' ','#',' ','#','#','#','#','#','#',' '},
        {' ','#',' ','#','+','#','+',' ','#',' '},
        {'+','#','+','#',' ',' ',' ',' ',' ',' '}
};

/*
#define ROWS 5
#define COLUMNS 6

char environment[ROWS][COLUMNS] = {
        {' ',' ',' ',' ',' ','+'},
        {' ',' ',' ',' ','#',' '},
        {' ','#',' ',' ','#',' '},
        {' ','#','+',' ','#',' '},
        {'+','#',' ',' ',' ',' '}
};
 */

char environmentDefualt[ROWS][COLUMNS];

struct state {
    int row;
    int col;
};

float discountRate = 0.99f;

float theta = 0.0001f;
float epsilon = 0.05f;
float learningRate = 0.90f;

int episodes;
int maxNumberOfQtables;

int marblesPickedUp = 0;
int numberOfMarbles = 0;

enum action {UP, DOWN, LEFT, RIGHT};
action actions[ACTIONS] = {UP, DOWN, LEFT, RIGHT};

void numberOfMarblesInEnv(){
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if (environment[i][j] == '+')
                numberOfMarbles++;
        }
    }
}

vector<float> temp;

string maxStateString;
int maxState;

struct qtable {
    vector<vector<float>> qT;
    vector<state> goals;
    string goalsString;
    float sum;
};

vector<qtable> qtables;

void initTempVector(){
    maxStateString = to_string(ROWS - 1) + to_string(COLUMNS - 1);
    maxState = stoi(maxStateString) + 1;

    string a = maxStateString;
    int b = maxState;

    for (int k = 0; k < ACTIONS; k++) {
        temp.push_back(0.0f);
    }
}

vector<state> findGoals(){
    state goalState;
    vector<state> goals;

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if (environmentDefualt[i][j] == '+'){
                goalState = {i, j};
                goals.push_back(goalState);
            }
        }
    }
    return goals;
}

int getStateID(state _state) {
    string stateIDString = to_string(_state.row) + to_string(_state.col);
    return stoi(stateIDString);
}

string getGoalString(vector<state> _goals){
    string goalString;
    for (int i = 0; i < numberOfMarbles; i++) {
        if (_goals[i].row != -1 && _goals[i].col != -1)
            goalString += to_string(_goals[i].row) + to_string(_goals[i].col);
    }
    return goalString;
}

qtable initQtable(vector<state> _goals){
    vector<vector<float>> emptyQtable;

    //cout << "Init qTable" << endl;
    for (int i = 0; i < maxState; i++) {
        emptyQtable.push_back(temp);
    }

    string goalString = getGoalString(_goals);

    qtable newQtable = {emptyQtable, _goals, goalString, 0};

    // Add to qTable to vector
    qtables.push_back(newQtable);

    return newQtable;
}

state getNextState(state _state, action _action){
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

    return _state;
}

float getReward(state _state){
    if (_state.row < 0 || _state.col < 0 || _state.row >= ROWS || _state.col >= COLUMNS)
        return -1.0f;

    if (environment[_state.row][_state.col] == '+')
        return 10.0f;

    if (environment[_state.row][_state.col] == '#')
        return -1.0f;

    return 0.0f;
}

action bestAction(state _state, qtable _qTable){
    action bestAction = UP;
    float qValue, maxQvalue = -100;
    int stateID = getStateID(_state);

    for (int i = 0; i < ACTIONS; i++) {
        qValue = _qTable.qT[stateID][i];
        if (maxQvalue < qValue){
            maxQvalue = qValue;
            bestAction = actions[i];
        }
    }
    return bestAction;
}

int randomActions = 0;
int bestActions = 0;
action getAction(state _state, qtable _qTable){
    random_device rd;
    uniform_real_distribution<> dist(0, 1);
    uniform_int_distribution<int> distAction(0, ACTIONS - 1);

    if (epsilon < dist(rd)){
        randomActions++;
        return actions[distAction(rd)];
    }

    bestActions++;
    return bestAction(_state, _qTable);
}

// Bottleneck
void saveQtable(qtable _qTable){
    // Save qtable
    for (int k = 0; k < qtables.size(); k++) {
        if (_qTable.goalsString == qtables[k].goalsString)
            qtables[k] = _qTable;
    }
}

qtable changeQtable(state _state, qtable _qTable){
    state removeGoal = {-1, -1};
    vector<state> newGoals;

    string goalString;

    saveQtable(_qTable);

    goalString = getGoalString(_qTable.goals);
    string goal1 = to_string(_state.row) + to_string(_state.col);

    // Find position af marble som er samlet op.
    for (int i = 0; i < numberOfMarbles; i++) {
        if(_state.row == _qTable.goals[i].row && _state.col == _qTable.goals[i].col){

            // Lave en vector med de resterende marblesPickedUp
            newGoals = _qTable.goals;
            newGoals[i] = removeGoal;

            goalString = getGoalString(newGoals);

            // Led efter det corresponding qTable - Bottleneck
            for (int j = 0; j < qtables.size(); j++) {
                if (goalString == getGoalString(qtables[j].goals))
                    return qtables[j];
            }
            return initQtable(newGoals);
        }
    }
}

float getMaxQalueAtState(state _state, qtable _qTable){
    if (_state.row < 0 || _state.col < 0 || _state.row >= ROWS || _state.col >= COLUMNS)
        return 0;

    if(environment[_state.row][_state.col] == '#')
        return 0;

    // if nextState is goal change qTable
    if(environment[_state.row][_state.col] == '+'){
        marblesPickedUp++;
        if (marblesPickedUp < numberOfMarbles){
            _qTable = changeQtable(_state, _qTable);
        }
    }

    int stateID = getStateID(_state);
    float qValue, maxQvalue = -100.f;

    for (int i = 0; i < ACTIONS; i++) {
        qValue = _qTable.qT[stateID][i];
        if (maxQvalue < qValue){
            maxQvalue = qValue;
        }
    }
    return maxQvalue;
}

bool insideEnvironment(state _state){
    if (environment[_state.row][_state.col] == '#')
        return false;

    if (_state.row < 0 || _state.col < 0 || _state.row >= ROWS || _state.col >= COLUMNS)
        return false;

    return true;
}

void resetEnvironment(){
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            environment[i][j] = environmentDefualt[i][j];
        }
    }
}

void printQtable(qtable _qTable){
    string stateIDString;
    int stateID;

    cout << "qTable with goals in " << endl;
    for (int i = 0; i < numberOfMarbles; i++) {
        if (_qTable.goals[i].row != -1 && _qTable.goals[i].col != -1)
            cout << to_string(_qTable.goals[i].row) << to_string(_qTable.goals[i].col) << " ";
    }
    cout << endl;

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLUMNS; col++) {
            if (environment[row][col] == ' ') {
                stateIDString = to_string(row) + to_string(col);
                stateID = stoi(stateIDString);
                cout << stateIDString;
                for (int _action = 0; _action < ACTIONS; _action++)
                    printf(" %5.2f ", _qTable.qT[stateID][_action]);

                printf("\n");
            }
        }
    }
    cout << endl;
}

void printQtables(){
    for (int i = 0; i < qtables.size(); ++i) {
        printQtable(qtables[i]);
        cout << endl;
    }
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
}

state randomStartingPosition(){
    state stateStart;

    random_device rd;
    uniform_int_distribution<int> distROW (0,ROWS - 1);
    uniform_int_distribution<int> distCOL (0,COLUMNS - 1);
    int row = distROW(rd);
    int col = distCOL(rd);

    while (environment[row][col] != ' '){
        row = distROW(rd);
        col = distCOL(rd);
    }
    stateStart = {row, col};

    return stateStart;
}

float sumOfQtables(){
    float sums = 0;
    for (int i = 0; i < qtables.size(); i++) {
        sums += qtables[i].sum;
    }
    return sums;
}

void train(){

    episodes = 0;
    int counter = 0;

    maxNumberOfQtables = (int) pow(2,numberOfMarbles) - 1;

    float preSum, newSum = 0.0f;

    state _state, nextState;
    action _action;
    int stateID;
    float reward, maxQvalueAtState, qValue, delta = 1, preQvalue;

    qtable qTable, qtStart, newQtable;

    initQtable(findGoals());

    cout << "numberOfMarbles " << numberOfMarbles << endl;

    do {
        marblesPickedUp = 0;

        preSum = newSum;

        resetEnvironment();

        qtStart = qtables[0];
        qTable = qtStart;

        _state = randomStartingPosition();

        ++episodes;

        while (marblesPickedUp < numberOfMarbles){

            _action = getAction(_state, qTable);

            nextState = getNextState(_state, _action);

            stateID = getStateID(_state);

            maxQvalueAtState = getMaxQalueAtState(nextState, qTable);

            reward = getReward(nextState);

            preQvalue = qTable.qT[stateID][_action];

            qValue = (1 - learningRate) * preQvalue + learningRate * (reward + discountRate * maxQvalueAtState);

            qTable.qT[stateID][_action] = qValue;

            qTable.sum = preQvalue - qValue;

            // Check if a goal is found
            if (0 <= reward && environment[nextState.row][nextState.col] == '+'){

                //cout << "Goal found!" << endl;
                //printQtable(qTable);

                environment[nextState.row][nextState.col] = ' ';

                // check if all marblesPickedUp is found
                if(marblesPickedUp == numberOfMarbles){
                    saveQtable(qTable);
                    //cout << "All marblesPickedUp found" << endl;
                    break;
                }

                //Change qTable
                qTable = changeQtable(nextState, qTable);
            }

            if (insideEnvironment(nextState))
                _state = nextState;
            else
                _state = randomStartingPosition();
        }

        if (qtables.size() == maxNumberOfQtables){
            newSum = sumOfQtables();
            delta = fabs(preSum - newSum);
        }


        cout << "Episodes: " << episodes << " delta: " << delta << " number of qtables " << qtables.size() << endl;

        if (delta < theta)
            counter++;
        else
            counter = 0;

    } while (counter < 10);

    cout << "All qtables" << endl;
    printQtables();
    cout << "number of qtables " << qtables.size() << endl;
    //printQtable(qTable);
}

void printActionSym(action a) {
    switch (a) {
        case UP: std::cout << " ↑ ";
            break;
        case DOWN: std::cout << " ↓ ";
            break;
        case LEFT: std::cout << " ← ";
            break;
        case RIGHT: std::cout << " → ";
            break;
    }
}

void getBestPath(state _state){
    vector<state> bestPath;
    qtable _qTable = qtables[0];
    action _action;
    string goalString;

    string b = to_string(getStateID(_state));

    marblesPickedUp = 0;

    bestPath.push_back(_state);

    resetEnvironment();

    while(marblesPickedUp < numberOfMarbles){
        _action = bestAction(_state, _qTable);
        _state = getNextState(_state, _action);
        b = to_string(getStateID(_state));
        cout << "Taking ";
        printActionSym(_action);
        cout << " and going to state " << b << endl;
        bestPath.push_back(_state);
        if(environment[_state.row][_state.col] == '+'){
            cout << "goal found" << endl;
            marblesPickedUp++;
            environment[_state.row][_state.col] = ' ';
            if(marblesPickedUp < numberOfMarbles)
                _qTable = changeQtable(_state, _qTable);
        }
    }

    for (int i = 0; i < bestPath.size(); i++) {
        cout << to_string(getStateID(bestPath[i])) << " ";
    }
    cout << endl;
    cout << "Number of steps " << bestPath.size() - 1 << endl;
}

void initEnv(){
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            environmentDefualt[i][j] = environment[i][j];
        }
    }
}

void init(){
    initEnv();
    initTempVector();
    numberOfMarblesInEnv();
}

int main() {
    init();

    train();

    cout << "Random Actions " << randomActions << endl;
    cout << "BestActions " << bestActions << endl;

    state startState = {5, 5};
    getBestPath(startState);
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
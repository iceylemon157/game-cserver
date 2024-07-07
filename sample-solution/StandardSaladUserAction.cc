#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "GameController.h"
#include "UserAction.h"

using namespace wfrest;
using namespace ctl;
using namespace std;

GameController& controller = GameController::getInstance("default");

// -- DON'T MODIFY ANY CODE ABOVE THIS LINE -- //
// -- YOU ARE ALLOW TO ADD HEADER FILES UNDER THIS LINE -- //

// TODO: Announce the following grading policy
// -- Grading Policy --
// Final Score of this code: 12250

// Will get 0/15 if you get 0 score
// Will get 12/15 if you can get 12250 score (baseline)
// Will get 15/15 if you can get 17000 score (perfect score)

// Will get score proportionally if you are between 0 and 12250
// Formula: (score - 0) / (12250 - 0) * 12

// Will get score proportionally if you are between 12250 and 17000
// Formula: (score - 12250) / (17000 - 12250) * 3 + 12
// -- Grading Policy --

// TODO: Choose your recipe mode (use salad for SaladUserAction)
const string RecipeMode = "salad";
// const string RecipeMode = "salad_cheeseburger";
// const string RecipeMode = "all";

// A template GLOBAL VARIABLE vector to store operations
// Feel free to modify this data structure! (or create your own to use)
// vector<string> operations;
// TODO: Change operations from vector to queue
#include <queue>
queue<char> operations;

// A template map to store the position of each counter
// Question: How do you extend this map to store more than one position for a counter?
// Question2: cutting / normal counter have more than one position, how will you handle it?
const map<Counter, pair<int, int>> counterPosition = {
    { BreadCounter, {0, 1} },
    { CabbageCounter, {8, 4} }, // TODO: Used to be {8, 2}
    { CheeseBlockCounter, {0, 9} },
    { TomatoCounter, {8, 7} },  // TODO: Used to be {8, 5}
    { RawPattyCounter, {0, 20} },
    { StoveCounter, {0, 17} },  // TODO: Used to be {0, 19}
    { PlatesCounter, {3, 20} },
    { TrashCounter, {6, 20} },
    { DeliveryCounter, {1, 20} },

    // There are 2 cutting counters, this is only one of it
    { CuttingCounter, {8, 15} },
    // There are so many normal counters, this is only one of it
    // { NormalCounter, {8, 20} }
    { NormalCounter, {8, 12} } // TODO: Used to be {8, 20}
};

const map<Counter, char> counterDirection = {
    { BreadCounter, 'w' },
    { CabbageCounter, 's' },
    { CheeseBlockCounter, 'w' },
    { TomatoCounter, 's' },
    { RawPattyCounter, 'w' },
    { StoveCounter, 'w' },
    { PlatesCounter, 'd' },
    { TrashCounter, 'd' },
    { DeliveryCounter, 'd' },

    // There are 2 cutting counters, this is only one of it
    { CuttingCounter, 's' },
    // There are so many normal counters, this is only one of it
    { NormalCounter, 's' }
};

// TODO: Function Prototypes need to be changed to Salad... functions
void SaladInitialize();
void SaladSendOperation();

// Init the game (DO NOT MODIFY THIS FUNCTION)
void UserAction::InitGame() {
    Initialize();
    // Set the response to "ok" when finished initialization
    controller.SetResponse("ok");
}

// Just a cute Initializing function
void UserAction::Initialize() {
    // TODO: Use SaladInitialize instead of DefaultInitialize
    cout << "Initializing the game..." << endl;
    SaladInitialize();
}

// Main Function of you game logic
void UserAction::SendOperation() {
    // TODO: Use SaladSendOperation instead of DefaultSendOperation
    SaladSendOperation();
}

// -- Moving series functions Below -- //

void MovePointToPoint(pair<int, int> from, pair<int, int> to) {
    // TODO: Move from one point to another point
    char xdir = (from.first < to.first) ? 's' : 'w';
    char ydir = (from.second < to.second) ? 'd' : 'a';

    while (from.first != to.first) {
        operations.push(xdir);
        from.first += (from.first < to.first) ? 1 : -1;
    }

    while (from.second != to.second) {
        operations.push(ydir);
        from.second += (from.second < to.second) ? 1 : -1;
    }
}

// TODO: Name has been changed. Doesn't have Interact before!
void MovePointToCounterAndInteract(pair<int, int> fromPoint, Counter toCounter) {
    // TODO: Move from one point to a counter, and then interact with the counter
    pair<int, int> targetPosition = counterPosition.at(toCounter);

    // Move from fromPoint to targetPosition
    MovePointToPoint(fromPoint, targetPosition);

    // Change the direction to counter direction
    operations.push(counterDirection.at(toCounter));

    // With interact is because, each time we move to a counter, we are likely to interact with it
    operations.push('e');
}

// TODO: Name has been changed. Doesn't have Interact before!
void MoveCounterToCounterAndInteract(Counter from, Counter to) {
    // TODO: Move from one counter to another counter
    pair<int, int> fromPosition = counterPosition.at(from);
    MovePointToCounterAndInteract(fromPosition, to);
}

// -- Moving series functions Above -- //

// -- Strategy for different Recipe Mode Below -- //

Recipe SaladModeStrategy() {
    // Only salad will be in order in Salad Mode
    return Salad;
}

Recipe SaladCheeseburgerModeStrategy() {}

Recipe AllRecipeModeStrategy() {}

Recipe GetNextOrder() {
    // Return the next order based on the Recipe Mode
    if (RecipeMode == "salad") {
        return SaladModeStrategy();
    } else if (RecipeMode == "salad_cheeseburger") {
        return SaladCheeseburgerModeStrategy();
    } else {
        return AllRecipeModeStrategy();
    }
}

// -- Strategy for different Recipe Mode Above -- //

// -- Miscallaneous functions Below -- //

void CutIngredient(int times) {
    // TODO: Cut the Ingredient for times
    for (int i = 0; i < times; i ++) {
        operations.push('f');
    }
}

// -- Miscallaneous functions Above -- //

// -- Pipeline Funtions Below -- //

// You are welcome to change the function prototype
// Like changing the return type or adding more parameters
void MakeSalad() {
    // TODO: Salad Making Pipeline

    // clear queue
    while (!operations.empty()) {
        operations.pop();
    }
    pair<int, int> playerPosition = controller.GetPlayerPosition();
    // Grab Cabbage
    MovePointToCounterAndInteract(playerPosition, CabbageCounter);
    // Move to Cutting Counter
    MoveCounterToCounterAndInteract(CabbageCounter, CuttingCounter);
    // Cut Cabbage
    CutIngredient(5);
    // Grab Cabbage Slices
    operations.push('e');
    // Put Down Cabbage Slices On Normal Counter
    MoveCounterToCounterAndInteract(CuttingCounter, NormalCounter);
    // Grab Tomato at Tomato Counter
    MoveCounterToCounterAndInteract(NormalCounter, TomatoCounter);
    // Put Down Tomato To A Cutting Counter
    MoveCounterToCounterAndInteract(TomatoCounter, CuttingCounter);
    // Cut Tomato
    CutIngredient(3);
    // Grab Plate At Plate Counter
    MoveCounterToCounterAndInteract(CuttingCounter, PlatesCounter);
    // Grab Tomato Slices At Cutting Counter
    MoveCounterToCounterAndInteract(PlatesCounter, CuttingCounter);
    // Grab Cabbage Slices At Normal Counter
    MoveCounterToCounterAndInteract(CuttingCounter, NormalCounter);
    // Deliver the order
    MoveCounterToCounterAndInteract(NormalCounter, DeliveryCounter);
}

void MakeBurger() {}
void MakeCheeseBurger() {}
void MakeMegaBurger() {}

// -- Pipeline Funtions Below -- //

// -- Default Series Function Below -- //

// TODO: Implement SaladSendOperation from DefaultSendOperation
void SaladSendOperation() {

    if (operations.empty()) {
        // No operation left, means order is finished or not started!

        // Get the next order
        Recipe orderRecipe = GetNextOrder();
        if (orderRecipe == Salad) MakeSalad();
        if (orderRecipe == Burger) MakeBurger();
        if (orderRecipe == CheeseBurger) MakeCheeseBurger();
        if (orderRecipe == MegaBurger) MakeMegaBurger();
    }

    // Perform next operation in the queue
    char s;
    s = operations.front();
    operations.pop();
    if (s == 'w') controller.MoveUp();
    if (s == 's') controller.MoveDown();
    if (s == 'a') controller.MoveLeft();
    if (s == 'd') controller.MoveRight();
    if (s == 'e') controller.Interact();
    if (s == 'f') controller.InteractSpecial();
}

// TODO: Implement SaladInitialize from DefaultInitialize
void SaladInitialize() {
    while (!operations.empty()) {
        operations.pop();
    }
}

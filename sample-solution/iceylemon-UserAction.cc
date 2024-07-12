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

#include <queue>
queue<char> operations;

bool isSpareSalad = false;
bool isSaladPlate = false;
bool firstHalfSalad = false;
bool isFrying = false;
pair<int, int> playerPosition = {4, 10};

const pair<int, int> playerStartPosition = {4, 10};
const vector<pair<int, int>> UpperNormalCounter = {{0, 11}, {0, 12}, {0, 13}};
const vector<pair<int, int>> UpperCuttingCounter = {{0, 14}, {0, 15}, {0, 16}};

const map<Counter, pair<int, int>> counterPosition = {
    { BreadCounter, {0, 1} },
    { CabbageCounter, {8, 4} },
    { CheeseBlockCounter, {0, 10} },
    { TomatoCounter, {8, 7} },
    { RawPattyCounter, {0, 20} },
    { StoveCounter, {0, 17} },
    { PlatesCounter, {2, 20} },
    { TrashCounter, {6, 20} },
    { DeliveryCounter, {1, 20} },
    { CuttingCounter, {8, 14} },
    { NormalCounter, {8, 13} }
};

void DefaultInitialize();
void SaladSendOperation();
void SaladCheeseburgerSendOperation();
void AllRecipeSendOperation();
void MovePointToPointAndInteract(pair<int, int>, pair<int, int>);

void UserAction::InitGame() {
    Initialize();
    controller.SetResponse("ok");
}

void UserAction::Initialize() {
    cout << "Initializing the game..." << endl;
    DefaultInitialize();
}

void UserAction::SendOperation() {
    string recipeMode = controller.GetRecipeMode();
    cout << "Recipe Mode: " << recipeMode << endl;
    if (recipeMode == "Salad") {
        SaladSendOperation();
    } else if (recipeMode == "SaladAndCheeseBurger") {
        SaladCheeseburgerSendOperation();
    } else {
        AllRecipeSendOperation();
    }
}

// -- Moving series functions Below -- //

class MovingPipeline {
public:
    MovingPipeline() { Clear(); }

    void Clear() {
        points.clear();
    }

    void AddToPipeline(const pair<int, int> point) {
        if (!points.empty()) {
            MovePointToPointAndInteract(points.back(), point);
        }
        points.emplace_back(point);
    }

    void AddToPipeline(const Counter counter) {
        AddToPipeline(counterPosition.at(counter));
    }

    void AddToPipeline(const vector<pair<int, int>> pointList) {
        // If there are multiple points, choose the closest one
        if (pointList.empty()) return;
        pair<int, int> resultPoint = pointList.front();
        int minDistance = Distance(points.back(), pointList[0]);
        for (pair<int, int> p : pointList) {
            int distance = Distance(points.back(), p);
            if (distance < minDistance) {
                minDistance = distance;
                resultPoint = p;
            }
        }
        AddToPipeline(resultPoint);
    }

    void AddOperationToPipeline(char oper, int times = 1) {
        for (int i = 0; i < times; i ++) {
            operations.push(oper);
        }
    }

    void PrintPipeline() {
        for (pair<int, int> p : points) {
            cout << "(" << p.first << ", " << p.second << ") ";
        }
        cout << endl;
    }

private:
    vector<pair<int, int>> points;
    int Distance(pair<int, int> from, pair<int, int> to) {
        return abs(from.first - to.first) + abs(from.second - to.second);
    }
};

char GetFacingDirection(pair<int, int> point) {
    int x = point.first, y = point.second;
    if (x == 0) {
        return 'w';
    } else if (x == 8) {
        return 's';
    } else if (y == 0) {
        return 'a';
    } else if (y == 20) {
        return 'd';
    } else {
        return '-';
    }
}

void MovePointToPoint(pair<int, int> from, pair<int, int> to) {
    char xdir = (from.first < to.first) ? 's' : 'w';
    char ydir = (from.second < to.second) ? 'd' : 'a';
    char direction = GetFacingDirection(to);

    vector<char> xpath, ypath;
    while (from.first != to.first) {
        xpath.emplace_back(xdir);
        from.first += (from.first < to.first) ? 1 : -1;
    }
    while (from.second != to.second) {
        ypath.emplace_back(ydir);
        from.second += (from.second < to.second) ? 1 : -1;
    }

    // If xdir is the same as the direction, move it after ypath
    if (direction != xdir) {
        for (char c : xpath) {
            operations.push(c);
        }
    }
    for (char c : ypath) {
        operations.push(c);
    }
    if (direction == xdir) {
        for (char c : xpath) {
            operations.push(c);
        }
    }

    // Use a more general way to get facing direction
    if (!operations.empty() and operations.back() == direction) {
        // If the last operation is the same as the direction, we don't need to move
        return;
    }
    operations.push(direction);
}

void MovePointToPointAndInteract(pair<int, int> from, pair<int, int> to) {
    MovePointToPoint(from, to);
    operations.push('e');
}

// -- Miscallaneous functions Below -- //

void CutIngredient(int times) {
    for (int i = 0; i < times; i ++) {
        operations.push('f');
    }
}

// -- Strategy for different Recipe Mode Below -- //

Recipe SaladModeStrategy() {
    // Only salad will be in order in Salad Mode
    return Salad;
}

Recipe SaladCheeseburgerModeStrategy() {
    Recipe orderRecipe = Salad;
    vector<Order> orderList = controller.GetOrderList();
    if (!orderList.empty()) {
        orderRecipe = orderList[0].recipe;
    }
    return orderRecipe;
}

Recipe AllRecipeModeStrategy() {
    Recipe orderRecipe = Salad;
    vector<Order> orderList = controller.GetOrderList();
    if (!orderList.empty()) {
        orderRecipe = orderList[0].recipe;
    }
    for (int i = 0; i < (int)orderList.size(); i ++) {
        if (orderList[i].score >= 700) {
            if (orderList[i].recipe != MegaBurger or isSpareSalad) {
                orderRecipe = orderList[i].recipe;
            }
            break;
        }
    }
    return orderRecipe;
}

Recipe GetNextOrder() {
    // Return the next order based on the Recipe Mode
    string recipeMode = controller.GetRecipeMode();
    if (recipeMode == "Salad") {
        return SaladModeStrategy();
    } else if (recipeMode == "SaladAndCheeseBurger") {
        return SaladCheeseburgerModeStrategy();
    } else {
        return AllRecipeModeStrategy();
    }
}

// -- Pipeline Funtions Below -- //

void MakeFirstHalfSalad() {
    MovingPipeline pipeline;
    pipeline.AddToPipeline(playerPosition);
    if (!isSaladPlate) {
        pipeline.AddToPipeline(PlatesCounter);
    }
    pipeline.AddToPipeline(NormalCounter);
    isSaladPlate = false;
    pipeline.AddToPipeline(TomatoCounter);
    pipeline.AddToPipeline(CuttingCounter);
    pipeline.AddOperationToPipeline('f', 3);
    pipeline.AddOperationToPipeline('e');
    pipeline.AddToPipeline(NormalCounter);
}

void MakeLastHalfSalad() {
    MovingPipeline pipeline;
    pipeline.AddToPipeline(playerPosition);
    pipeline.AddToPipeline(CabbageCounter);
    pipeline.AddToPipeline(CuttingCounter);
    pipeline.AddOperationToPipeline('f', 5);
    pipeline.AddOperationToPipeline('e');
    pipeline.AddToPipeline(NormalCounter);
    pipeline.AddOperationToPipeline('e');

    if (controller.GetRecipeMode() == "Salad") {
        pipeline.AddToPipeline(DeliveryCounter);
    }
}

void MakeSalad() {
    if (!firstHalfSalad) {
        MakeFirstHalfSalad();
        playerPosition = counterPosition.at(NormalCounter);
    }
    MakeLastHalfSalad();
    firstHalfSalad = false;
    isSpareSalad = true;
}

void MakeBurgerGeneral(bool cheese, bool mega) {
    MovingPipeline pipeline;
    pipeline.AddToPipeline(playerPosition);
    if (!isFrying) {
        pipeline.AddToPipeline(RawPattyCounter);
        pipeline.AddToPipeline(StoveCounter);
    }
    unsigned int startFryingSize = operations.size();
    if (!mega) {
        pipeline.AddToPipeline(PlatesCounter);
        pipeline.AddToPipeline(UpperNormalCounter);
    }
    pipeline.AddToPipeline(BreadCounter);
    pipeline.AddToPipeline(UpperNormalCounter);

    if (cheese) {
        pipeline.AddToPipeline(CheeseBlockCounter);
        pipeline.AddToPipeline(UpperCuttingCounter);
        pipeline.AddOperationToPipeline('f', 3);
        pipeline.AddOperationToPipeline('e');
        pipeline.AddToPipeline(UpperNormalCounter);
    }
    pipeline.AddOperationToPipeline('e');

    // 52 is a magic number to wait for the patty to be ready
    while (!isFrying and operations.size() < startFryingSize + 52u) {
        pipeline.AddOperationToPipeline('f');
    }
    // (0, 19) is closer to DeliveryCounter, move to (0, 19) first
    pipeline.AddToPipeline({0, 19});
    isFrying = false;
}

void MakeBurger() {
    MakeBurgerGeneral(false, false);
}

void MakeCheeseBurger() {
    MakeBurgerGeneral(true, false);
}

void MakeMegaBurger() {
    MovingPipeline pipeline;
    pipeline.AddToPipeline(playerPosition);
    if (playerPosition == counterPosition.at(DeliveryCounter)) {
        pipeline.AddToPipeline(RawPattyCounter);
        pipeline.AddToPipeline(StoveCounter);
        playerPosition = counterPosition.at(StoveCounter);
        isFrying = true;
    }
    if (!isSpareSalad) {
        MakeSalad();
        playerPosition = counterPosition.at(NormalCounter);
        pipeline.Clear();
    }
    pipeline.AddToPipeline(NormalCounter);
    isSpareSalad = false;
    pipeline.AddToPipeline(UpperNormalCounter[0]);
    playerPosition = UpperNormalCounter[0];
    MakeBurgerGeneral(true, true);
}

void MakeSaladSCMode(bool move = true) {
    playerPosition = controller.GetPlayerPosition();
    isSaladPlate = true;
    MakeSalad();
    MovingPipeline pipeline;
    pipeline.AddToPipeline(NormalCounter);
    pipeline.AddToPipeline(DeliveryCounter);
    if (move) {
        pipeline.AddToPipeline(PlatesCounter);
        // Move to (2, 13), special case for SC mode
        pipeline.AddOperationToPipeline('a', 7);
    }
}

void MakeCheeseBurgerSCMode(bool move = true) {
    playerPosition = controller.GetPlayerPosition();

    MovingPipeline pipeline;
    pipeline.AddToPipeline(playerPosition);
    pipeline.AddToPipeline(UpperNormalCounter);
    pipeline.AddToPipeline(RawPattyCounter);
    pipeline.AddToPipeline(StoveCounter);
    unsigned int startFryingSize = operations.size();

    pipeline.AddToPipeline(BreadCounter);
    pipeline.AddToPipeline(UpperNormalCounter);
    pipeline.AddToPipeline(CheeseBlockCounter);
    pipeline.AddToPipeline(UpperCuttingCounter);
    pipeline.AddOperationToPipeline('f', 3);
    pipeline.AddOperationToPipeline('e');
    pipeline.AddToPipeline(UpperNormalCounter);
    pipeline.AddOperationToPipeline('e');

    // MeatPatty is not ready yet, so we need to wait
    // (0, 19) is closer to DeliveryCounter, move to (0, 19) first
    pipeline.AddToPipeline({0, 19});

    while (operations.size() < startFryingSize + 60u) {
        pipeline.AddOperationToPipeline('f');
    }
    pipeline.AddOperationToPipeline('e');
    pipeline.AddToPipeline(DeliveryCounter);

    if (move) {
        pipeline.AddToPipeline(PlatesCounter);
        // Move to (2, 13), special case for SC mode
        pipeline.AddOperationToPipeline('a', 7);
    }
}

void MakeNextOrder() {
    Recipe orderRecipe = GetNextOrder();
    playerPosition = controller.GetPlayerPosition();
    if (orderRecipe == Salad) MakeSalad();
    if (orderRecipe == Burger) MakeBurger();
    if (orderRecipe == CheeseBurger) MakeCheeseBurger();
    if (orderRecipe == MegaBurger) MakeMegaBurger();
}

// -- Initialize functions Below -- //

void DefaultInitialize() {
    playerPosition = playerStartPosition;
    while (!operations.empty()) {
        operations.pop();
    }
}

void SaladInitialize() {
    // waiting for the first plate
    for (int i = 0; i < 9; i ++) {
        operations.push('f');
    }
}

void SaladCheeseburgerInitialize() {
    isSpareSalad = false;
    isSaladPlate = false;
    firstHalfSalad = false;
    isFrying = false;
}

void AllRecipeInitialize() {
    SaladInitialize();
    SaladCheeseburgerInitialize();
}

// -- Send Operation functions Below -- //

void SendOperationFromQueue() {
    // Perform next operation in the queue
    char s = operations.front();
    operations.pop();
    if (s == 'w') controller.MoveUp();
    if (s == 's') controller.MoveDown();
    if (s == 'a') controller.MoveLeft();
    if (s == 'd') controller.MoveRight();
    if (s == 'e') controller.Interact();
    if (s == 'f') controller.InteractSpecial();
}

void SaladSendOperation() {
    if (controller.GetRound() == 1) {
        SaladInitialize();
    }
    if (operations.empty()) {
        // Player finished the salad, ready to deliver
        if (controller.GetPlayerHoldItems().size() > 0) {
            // Wait for next order
            if (controller.GetOrderList().empty()) {
                controller.InteractSpecial();
            } else {
                controller.Interact();
            }
            return;
        }

        // Make next order
        MakeNextOrder();
    }
    SendOperationFromQueue();
}

void SaladCheeseburgerSendOperation() {
    pair<int, int> ExtraNormalCounter = {8, 20};
    pair<int, int> CenterWaitingPoint = {2, 13};

    if (controller.GetRound() == 1) {
        SaladCheeseburgerInitialize();
        MakeCheeseBurger();

        // Move to CenterWaitingPoint after finish each order for SC mode
        // Place the extra cheeseburger at (8, 20)
        MovingPipeline pipeline;
        pipeline.AddToPipeline(StoveCounter);
        pipeline.AddToPipeline(ExtraNormalCounter);
        pipeline.AddToPipeline(PlatesCounter);
        pipeline.AddToPipeline(CenterWaitingPoint);
    }
    if (controller.GetRound() == 1800) {
        // Last round, deliver the last order
        controller.Interact();
        return;
    }
    if (operations.empty()) {
        if (controller.GetTotalScore() == 17000 and controller.GetPlayerHoldItems().empty()) {
            // Deliver the last order
            MovingPipeline pipeline;
            pipeline.AddToPipeline(DeliveryCounter);
            pipeline.AddToPipeline(ExtraNormalCounter);
            MovePointToPoint(ExtraNormalCounter, counterPosition.at(DeliveryCounter));
            SendOperationFromQueue();
            return;
        }
        if (controller.GetOrderList().empty()) {
            // Do nothing, wait for next order
            controller.InteractSpecial();
            return;
        }
        Recipe nextRecipe = GetNextOrder();
        if (controller.GetTotalScore() < 16000) {
            // Move to the center for normal orders
            if (nextRecipe == Salad) MakeSaladSCMode();
            else MakeCheeseBurgerSCMode();
        } else if (controller.GetTotalScore() < 17000) {
            // Don't move to the center for the second last order
            if (nextRecipe == Salad) MakeSaladSCMode(false);
            else MakeCheeseBurgerSCMode(false);
        }
    }
    SendOperationFromQueue();
}

void AllRecipeSendOperation() {
    if (controller.GetRound() == 1) {
        AllRecipeInitialize();
        MakeSalad();
        operations.push('e');
        isSpareSalad = true;
    }
    if (operations.empty()) {
        if (controller.GetPlayerHoldItems().size() > 0) {
            // Player finished the order, ready to deliver
            if (controller.GetOrderList().empty()) {
                // Do nothing, wait for next order
                controller.InteractSpecial();
            } else {
                // Deliver the order
                Recipe orderRecipe = controller.GetOrderList()[0].recipe;
                playerPosition = controller.GetPlayerPosition();

                if (orderRecipe == Salad) {
                    // special case for salad
                    isSpareSalad = false;
                }

                MovingPipeline pipeline;
                pipeline.AddToPipeline(playerPosition);
                pipeline.AddToPipeline(DeliveryCounter);

                SendOperationFromQueue();
            }
            return;
        }

        if (controller.GetOrderList().empty()) {
            // If there is no order, wait for the next order

            int currentRound = controller.GetRound() % 100;
            if (currentRound < 75 and !isSpareSalad) {
                // If there is some time left, make salad
                playerPosition = controller.GetPlayerPosition();
                if (!firstHalfSalad) {
                    MakeFirstHalfSalad();
                    firstHalfSalad = true;
                } else {
                    MakeSalad();
                    operations.push('e');
                }
                SendOperationFromQueue();
            } else {
                controller.InteractSpecial();
            }

            return;
        }

        Recipe nextOrder = GetNextOrder();
        if (isSpareSalad && nextOrder == Salad) {
            // If the next order is salad and we have a spare salad
            // Deliver the salad
            MovingPipeline pipeline;
            pipeline.AddToPipeline(controller.GetPlayerPosition());
            pipeline.AddToPipeline(NormalCounter);
            pipeline.AddToPipeline(DeliveryCounter);
            playerPosition = counterPosition.at(DeliveryCounter);

            MakeSalad();
            operations.push('e');
            isSpareSalad = true;
        } else {
            // If the player is not holding any item
            // Make next order
            MakeNextOrder();
        }
    }
    SendOperationFromQueue();
}
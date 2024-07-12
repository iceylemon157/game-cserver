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

#include <queue>
queue<char> operations;
void PrintOperations();

bool isSpareSalad = false;
bool isSaladPlate = false;
bool firstHalfSalad = false;
bool isFrying = false;
pair<int, int> playerPosition = {4, 10};

const pair<int, int> playerStartPosition = {4, 10};

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
void MakeCheeseBurger(bool, bool);
void MakeFirstHalfSalad();

void UserAction::InitGame() {
    Initialize();
    controller.SetResponse("ok");
}

// Just a cute Initializing function
void UserAction::Initialize() {
    cout << "Initializing the game..." << endl;
    // MakeCheeseBurger();
    DefaultInitialize();
}

// Main Function of you game logic
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

char GetFacingDirection(pair<int, int> point) {
    int x = point.first, y = point.second;
    if (x == 0) {
        return 'w';
    } else if (x == 8) {
        return 's';
    } else if (y == 0) {
        return 'a';
    } else {
        return 'd';
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

void MovePointToCounter(pair<int, int> fromPoint, Counter toCounter) {
    pair<int, int> targetPosition = counterPosition.at(toCounter);
    MovePointToPoint(fromPoint, targetPosition);
}

void MoveCounterToPoint(Counter from, pair<int, int> toPoint) {
    pair<int, int> fromPosition = counterPosition.at(from);
    MovePointToPoint(fromPosition, toPoint);
}

void MoveCounterToCounter(Counter from, Counter to) {
    pair<int, int> fromPosition = counterPosition.at(from);
    MovePointToCounter(fromPosition, to);
}

void MovePointToCounterAndInteract(pair<int, int> fromPoint, Counter toCounter) {
    pair<int, int> targetPosition = counterPosition.at(toCounter);
    MovePointToPointAndInteract(fromPoint, targetPosition);
}

void MoveCounterToPointAndInteract(Counter from, pair<int, int> toPoint) {
    pair<int, int> fromPosition = counterPosition.at(from);
    MovePointToPointAndInteract(fromPosition, toPoint);
}

void MoveCounterToCounterAndInteract(Counter from, Counter to) {
    pair<int, int> fromPosition = counterPosition.at(from);
    MovePointToCounterAndInteract(fromPosition, to);
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

void PrintOperations() {
    queue<char> temp = operations;
    cout << "*** Print Operations ***\n";
    cout << "Operation size: " << temp.size() << "\n";
    while (!temp.empty()) {
        cout << temp.front() << " ";
        temp.pop();
    }
    cout << endl;
}

void MakeFirstHalfSalad() {
    if (!isSaladPlate) {
        // Grab Plate
        MovePointToCounterAndInteract(playerPosition, PlatesCounter);
        // Move to Normal Counter
        MoveCounterToCounterAndInteract(PlatesCounter, NormalCounter);
        // Move to Cabbage Counter
    } else {
        // Move to Cabbage Counter
        MovePointToCounterAndInteract(playerPosition, NormalCounter);
    }
    isSaladPlate = false;
    // Grab Tomato at Tomato Counter
    MoveCounterToCounterAndInteract(NormalCounter, TomatoCounter);
    // Move to Cutting Counter
    MoveCounterToCounterAndInteract(TomatoCounter, CuttingCounter);
    CutIngredient(3);
    operations.push('e');
    // Put Down Tomato Slices To The Plate on Normal Counter
    MoveCounterToCounterAndInteract(CuttingCounter, NormalCounter);
}

void MakeLastHalfSalad() {
    MovePointToCounterAndInteract(playerPosition, CabbageCounter);
    MoveCounterToCounterAndInteract(CabbageCounter, CuttingCounter);
    CutIngredient(5);
    operations.push('e');
    MoveCounterToCounterAndInteract(CuttingCounter, NormalCounter);
    // Grab Plate
    operations.push('e');
    if (controller.GetRecipeMode() == "Salad") {
        // Move to Delivery Counter without delivering
        MoveCounterToCounterAndInteract(NormalCounter, DeliveryCounter);
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

void MakeBurger() {
    MakeCheeseBurger(false, false);
}

void MakeCheeseBurger(bool cheese = true, bool mega = false) {
    // pair<int, int> playerPosition = controller.GetPlayerPosition();
    if (!isFrying) {
        MovePointToCounterAndInteract(playerPosition, RawPattyCounter);
        MoveCounterToCounterAndInteract(RawPattyCounter, StoveCounter);
        playerPosition = counterPosition.at(StoveCounter);
    }
    unsigned int startFryingSize = operations.size();
    if (!mega) {
        MovePointToCounterAndInteract(playerPosition, PlatesCounter);
        MoveCounterToPointAndInteract(PlatesCounter, {0, 11});
        MovePointToCounterAndInteract({0, 11}, BreadCounter);
    } else {
        MovePointToCounterAndInteract(playerPosition, BreadCounter);
    }
    MoveCounterToPointAndInteract(BreadCounter, {0, 11});
    if (cheese) {
        MovePointToCounterAndInteract({0, 11}, CheeseBlockCounter);
        MoveCounterToPointAndInteract(CheeseBlockCounter, {0, 14});
        // MoveCounterToCounterAndInteract(CheeseBlockCounter, CuttingCounter);
        CutIngredient(3);
        operations.push('e');
        MovePointToPointAndInteract({0, 14}, {0, 13});
        operations.push('e');
    } else {
        // MovePointToPoint({0, 11}, {0, 13});
        operations.push('e');
        operations.push('d');
        operations.push('d');
    }
    // magic number
    while (!isFrying and operations.size() < startFryingSize + 54u) {
        operations.push('f');
    }
    MovePointToCounterAndInteract({0, 13}, StoveCounter);
    isFrying = false;
    // MoveCounterToCounter(StoveCounter, DeliveryCounter);
}

void MakeSaladSCMode(bool move = true) {
    playerPosition = controller.GetPlayerPosition();
    isSaladPlate = true;
    MakeSalad();
    MoveCounterToCounterAndInteract(NormalCounter, DeliveryCounter);
    if (move) {
        MoveCounterToCounterAndInteract(DeliveryCounter, PlatesCounter);
        for (int i = 0; i < 7; i ++) {
            operations.push('a');
        }
    }
}

void MakeCheeseBurgerSCMode(bool move = true) {
    playerPosition = controller.GetPlayerPosition();
    MovePointToPointAndInteract(playerPosition, {0, 13});
    MovePointToCounterAndInteract({0, 13}, RawPattyCounter);
    MoveCounterToCounterAndInteract(RawPattyCounter, StoveCounter);
    unsigned int startFryingSize = operations.size();
    MoveCounterToCounterAndInteract(StoveCounter, BreadCounter);
    MoveCounterToPointAndInteract(BreadCounter, {0, 11});
    MovePointToCounterAndInteract({0, 11}, CheeseBlockCounter);
    MoveCounterToPointAndInteract(CheeseBlockCounter, {0, 14});
    CutIngredient(3);
    operations.push('e');
    MovePointToPointAndInteract({0, 14}, {0, 13});
    operations.push('e');
    MovePointToPoint({0, 13}, {0, 19});
    while (operations.size() < startFryingSize + 60u) {
        operations.push('f');
    }
    operations.push('e');
    MovePointToCounterAndInteract({0, 19}, DeliveryCounter);
    if (move) {
        MoveCounterToCounterAndInteract(DeliveryCounter, PlatesCounter);
        for (int i = 0; i < 7; i ++) {
            operations.push('a');
        }
    }
}

void MakeMegaBurger() {
    if (playerPosition == counterPosition.at(DeliveryCounter)) {
        MoveCounterToCounterAndInteract(DeliveryCounter, RawPattyCounter);
        MoveCounterToCounterAndInteract(RawPattyCounter, StoveCounter);
        playerPosition = counterPosition.at(StoveCounter);
        isFrying = true;
    }
    if (!isSpareSalad) {
        MakeSalad();
        playerPosition = counterPosition.at(NormalCounter);
    } else {
        MovePointToCounterAndInteract(playerPosition, NormalCounter);
    }
    isSpareSalad = false;
    MoveCounterToPointAndInteract(NormalCounter, {0, 11});
    playerPosition = {0, 11};
    MakeCheeseBurger(true, true);
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
    if (controller.GetRound() == 1) {
        SaladCheeseburgerInitialize();
        MakeCheeseBurger();
        MoveCounterToPointAndInteract(StoveCounter, {8, 20});
        MovePointToCounterAndInteract({8, 20}, PlatesCounter);
        MoveCounterToPoint(PlatesCounter, {2, 13});
    }
    if (controller.GetRound() == 1800) {
        // Last round, deliver the last order
        controller.Interact();
        return;
    }
    if (operations.empty()) {
        if (controller.GetTotalScore() == 17000 and controller.GetPlayerHoldItems().empty()) {
            // Deliver the last order
            MoveCounterToPointAndInteract(DeliveryCounter, {8, 20});
            MovePointToCounter({8, 20}, DeliveryCounter);
            SendOperationFromQueue();
            return;
        }
        if (controller.GetOrderList().empty()) {
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
            if (nextRecipe == CheeseBurger) MakeCheeseBurgerSCMode(false);
        }
    }
    SendOperationFromQueue();
}

void SaladCheeseburgerSendOperationBackup() {
    if (controller.GetRound() == 1) {
        SaladCheeseburgerInitialize();
        MakeCheeseBurger();
        MoveCounterToPointAndInteract(StoveCounter, {8, 20});
        MovePointToCounter({8, 20}, PlatesCounter);
    }
    if (controller.GetRound() == 1800) {
        // Last round, deliver the last order
        cout << "Last Round\n";
        cout << "size: " << operations.size() << endl;
        controller.Interact();
        return;
    }
    if (operations.empty()) {
        // Player finished the salad, ready to deliver
        if (controller.GetPlayerHoldItems().size() > 0) {
            // Wait for next order
            if (controller.GetOrderList().empty()) {

                
                controller.InteractSpecial();
                // MovePointToPointAndInteract(controller.GetPlayerPosition(), {0, 7});
                // isSpareSalad = true;
                // SendOperationFromQueue();
            } else {


                Recipe orderRecipe = controller.GetOrderList()[0].recipe;

                MovePointToCounterAndInteract(controller.GetPlayerPosition(), DeliveryCounter);
                SendOperationFromQueue();
                return;

                controller.Interact();
                return;

                // Check if the player is holding the correct item
                vector<Items> playerHoldItems = controller.GetPlayerHoldItems();
                // remove the plate
                for (int i = 0; i < playerHoldItems.size(); i ++) {
                    if (playerHoldItems[i] == Plate) {
                        playerHoldItems.erase(playerHoldItems.begin() + i);
                        break;
                    }
                }
                sort(playerHoldItems.begin(), playerHoldItems.end());

                vector<Items> correctItems = RecipeIngredients.at(orderRecipe);
                sort(correctItems.begin(), correctItems.end());

                if (playerHoldItems != correctItems) {
                    // if the player is not holding the correct item, put the plate down
                    // MovePointToPointAndInteract(controller.GetPlayerPosition(), {8, 13});
                    // MovePointToPointAndInteract(controller.GetPlayerPosition(), {0, 13});
                    MovePointToPointAndInteract(controller.GetPlayerPosition(), {0, 7});
                    isSpareSalad = true;
                    SendOperationFromQueue();
                    return;
                } else {
                    MovePointToCounterAndInteract(controller.GetPlayerPosition(), DeliveryCounter);
                    SendOperationFromQueue();
                    return;
                }

                // controller.Interact();
            }
            return;
        }

        if (controller.GetRound() == 1800) {
            // Last round, deliver the last order
            cout << "Last Round\n";
            controller.Interact();
            return;
        }

        if (controller.GetTotalScore() == 17000) {
            // If the player has delivered predictable orders
            MoveCounterToPointAndInteract(DeliveryCounter, {8, 20});
            MovePointToCounter({8, 20}, DeliveryCounter);
            SendOperationFromQueue();
            return;
        }

        // int currentRound = controller.GetRound() % 100;
        // if (currentRound > 60 and currentRound + 17 < 100) {
        //     if (isSaladPlate) {
        //         // Try to move to center
        //         if (controller.GetPlayerPosition().second > 10) {
        //             controller.MoveLeft();
        //         } else {
        //             controller.InteractSpecial();
        //         }
        //         return;
        //     } else {
        //         MovePointToCounterAndInteract(controller.GetPlayerPosition(), PlatesCounter);
        //         MoveCounterToCounterAndInteract(PlatesCounter, NormalCounter);
        //         isSaladPlate = true;
        //     }
        //     SendOperationFromQueue();
        //     return;
        // }


        if (controller.GetOrderList().empty()) {
            // If there is no order, wait for the next order

            controller.InteractSpecial();
            return;

            if (isSaladPlate and controller.GetPlayerPosition().second > 10) {
                controller.MoveLeft();
            } else if (isSaladPlate and controller.GetPlayerPosition().first < 4) {
                controller.MoveDown();
            } else {
                controller.InteractSpecial();
            }
            return;
        }

        MakeNextOrder();
        cout << "Operation size: " << operations.size() << endl;
        cout << "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n";
        return;
        Recipe nextOrder = GetNextOrder();
        if (isSpareSalad && nextOrder == Salad) {
            // If the next order is salad and we have a spare salad
            // Deliver the salad
            MovePointToPointAndInteract(controller.GetPlayerPosition(), {0, 7});
            MovePointToCounterAndInteract({0, 7}, DeliveryCounter);
            isSpareSalad = false;
        } else {
            // If the player is not holding any item
            // Make next order
            MakeNextOrder();
        }
        // Make next order
        // MakeNextOrder();
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
    PrintOperations();
    if (operations.empty()) {
        if (controller.GetPlayerHoldItems().size() > 0) {
            // Player finished the order, ready to deliver
            if (controller.GetOrderList().empty()) {
                // Wait for next order
                controller.InteractSpecial();
            } else {
                // Deliver the order
                Recipe orderRecipe = controller.GetOrderList()[0].recipe;
                playerPosition = controller.GetPlayerPosition();

                if (orderRecipe == Salad) {
                    // special case for salad
                    isSpareSalad = false;
                }

                MovePointToCounterAndInteract(playerPosition, DeliveryCounter);
                SendOperationFromQueue();
            }
            return;
        }

        if (controller.GetOrderList().empty()) {
            // If there is no order, wait for the next order

            int currentRound = controller.GetRound() % 100;
            if (currentRound < 75 and !isSpareSalad) {
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
            MovePointToPointAndInteract(controller.GetPlayerPosition(), {8, 13});
            MovePointToCounterAndInteract({8, 13}, DeliveryCounter);
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
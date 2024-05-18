#include "GameController.h"

using namespace std;
using namespace ctl;

void GameController::ReceiveEvents(const wfrest::Json &json) {
    // Parse the JSON and update the game state

    GameController::SetRound(int(json["Round"]));
    GameController::SetTotalScore(int(json["TotalScore"]));

    GameController::SetPlayerPosition(pair<int, int>(int(json["PlayerPosition"]["x"]), int(json["PlayerPosition"]["y"])));

    // TODO: Should playerHoldItems be sorted?
    vector<Items> playerHoldItems;
    for (auto item : json["PlayerHoldItems"]) {
        Items itemEnum = (Items)int(item);
        playerHoldItems.emplace_back(itemEnum);
    }
    GameController::SetPlayerHoldItems(playerHoldItems);

    // TODO: RecipeDelivered not considered yet
    // GameController::SetRecipeDelivered()

    // New Recipe
    Order order = Order(
        int(json["NewRecipe"]["RecipeID"]), 
        (Recipe)int(json["NewRecipe"]["Recipe"]), 
        int(json["NewRecipe"]["Score"])
    );
    GameController::SetNewOrder(order);

    // Recipe List
    vector<Order> orderList;
    for (auto orderInfo : json["RecipeList"]) {
        Order order = Order(
            int(orderInfo["id"]), 
            // (Recipe)int(orderInfo["Recipe"]), 
            Burger, // TODO: Recipe enum not considered yet
            int(orderInfo["recipeScore"])
        );
        orderList.emplace_back(order);
    }

    // Frying Timer and Frying State
    GameController::SetFryingTimer(int(json["FryingTimer"]));
    GameController::SetFryingState((FryingState)int(json["FryingState"]));

    PrintEvents();

    // Update the response string
    resp = "ok";
}

void GameController::PrintEvents() {
    cout << "--- Current Game State ---" << endl;

    cout << "Round: " << GameController::GetRound() << endl;
    cout << "Total Score: " << GameController::GetTotalScore() << endl;

    cout << "Player Position: (" << GameController::GetPlayerPosition().first << ", " << GameController::GetPlayerPosition().second << ")" << endl;
    cout << "Player Hold Items: ";
    GameController::PrintItems(GameController::GetPlayerHoldItems());

    cout << "New Order: ";
    GameController::PrintOrderInfo(GameController::GetNewOrder());

    cout << "Order List: " << endl;
    for (auto order : GameController::GetOrderList()) {
        GameController::PrintOrderInfo(order);
    }

    cout << "Frying Timer: " << GameController::GetFryingTimer() << endl;
    cout << "Frying State: " << FryingStateMap.at(GameController::GetFryingState()) << endl;
    

    cout << "--- End of Game State ---" << endl;
}
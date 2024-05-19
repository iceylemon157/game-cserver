#ifndef CONTROL_H
#define CONTROL_H

#include <utility>
#include <string>
#include <vector>
#include "kitchen.h"
#include "wfrest/Json.h"

using namespace std;

namespace ctl {
    class GameController {
    public:

        // Singleton
        static GameController& getInstance(string _resp) {
            static GameController instance(_resp);
            return instance;
        }

        // Operation functions
        void MoveLeft() { resp = "a"; };
        void MoveRight() { resp = "d"; };
        void MoveUp() { resp = "w"; };
        void MoveDown() { resp = "s"; }
        void Interact() { resp = "e"; }
        void InteractSpecial() { resp = "f"; }

        void SetResponse(string _resp) { resp = _resp; };
        string GetResponse() { return resp; }

        int GetRound() const { return round; };
        int GetTotalScore() const { return totalScore; };

        pair<int, int> GetPlayerPosition() const { return playerPosition; };
        vector<Items> GetPlayerHoldItems() { return playerHoldItems; };

        pair<int, int> GetOrderDelivered() const { return orderDelivered; };
        
        Order GetNewOrder() const { return newOrder; };
        vector<Order> GetOrderList() const { return orderList; };

        int GetFryingTimer() const { return fryingTimer; };
        FryingState GetFryingState() const { return fryingState; };

        void ReceiveEvents(const wfrest::Json &json);
        void PrintEvents();

    private:
        // By chatGPT, I don't understand it, but it implements the singleton pattern
        GameController() {}
        GameController(std::string _resp) {
            resp = _resp;
        }
        ~GameController() {}

        // Private copy constructor and assignment operator to prevent copying
        GameController(const GameController&) = delete;
        GameController& operator=(const GameController&) = delete;

        string resp;

        int round;
        int totalScore;
        int timeLeft; // Deprecated

        pair<int, int> playerPosition;
        vector<Items> playerHoldItems;

        pair<int, int> orderDelivered;

        Order newOrder;
        vector<Order> orderList;

        int fryingTimer;
        FryingState fryingState;

        void SetRound(int _round) { round = _round; };
        void SetTotalScore(int _score) { totalScore = _score; };
        void SetTimeLeft(int _time) { timeLeft = _time; };

        void SetPlayerPosition(pair<int, int> _position) { playerPosition = _position; };
        void SetPlayerHoldItems(vector<Items> _items) { playerHoldItems = _items; };

        void SetOrderDelivered(pair<int, int> _data) { orderDelivered = _data; };

        void SetNewOrder(Order _order) { newOrder = _order; };
        void SetOrderList(vector<Order> _orderList) { orderList = _orderList; };

        void SetFryingTimer(int _time) { fryingTimer = _time; };
        void SetFryingState(enum FryingState _state) { fryingState = _state; };

        void PrintItems(vector<Items> items) {
            bool first = true;
            cout << "[ ";
            for (auto item : items) {
                if (first) first = false;
                else cout << ", ";
                cout << ItemsMap.at(item);
            }
            cout << " ]" << endl;
        }

        void PrintOrderInfo(Order order) {
            cout << "> ";
            cout << "Order ID: " << order.OrderID << ", ";
            cout << "Recipe: " << RecipeMap.at(order.recipe) << ", ";
            cout << "Score: " << order.Score << ", ";
            cout << "Arrial Time: " << order.ExistedTime << endl;
        }

    };
}

#endif /* CONTROL_H */

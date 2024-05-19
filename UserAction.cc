#include "workflow/WFFacilities.h"
#include <csignal>
#include <random>
#include <vector>
#include <iostream>
#include <unistd.h>
#include "wfrest/HttpServer.h"
#include "GameController.h"
#include "UserAction.h"

using namespace wfrest;
using namespace ctl;
using namespace std;

// A template vector to store operations
// Feel free to modify this vector (or not to use it at all)!
vector<string> operations;

void DefaultInitOperation();
void DefaultSendOperation(GameController& controller);


// Init the game
void UserAction::InitGame(GameController& controller) {
    cout << "InitGame" << endl;
    Initialize();
    // Set the response to "ok" when finished initialization
    controller.SetResponse("ok");
}

// Just a cute init function
void UserAction::Initialize() {
    /// TODO: You can do some initialization in this function.
    // Feel free to modify this function.
    // DefaultInitOperation() will make you a MEGA burger!
    cout << "Initialize" << endl;
    DefaultInitOperation();
}

// Gaming operations
void UserAction::SendOperation(GameController& controller) {
    DefaultSendOperation(controller);
}

// SendOperation function template, free MEGA burger for you!
void DefaultSendOperation(GameController& controller) {
    string s = "";
    if (!operations.empty()) {
        s = operations.back();
        operations.pop_back();
        cout << "Operation: " << s << endl;
    }
    if (s == "w") controller.MoveUp();
    if (s == "s") controller.MoveDown();
    if (s == "a") controller.MoveLeft();
    if (s == "d") controller.MoveRight();
    if (s == "e") controller.Interact();
    if (s == "f") controller.InteractSpecial();
}

void DefaultInitOperation() {
    cout << "You should only call me once ..." << endl;

    // map-based
    operations = {
        "w", "w", "w", "w", "e", "d", "d", "d", "d", "w", "e", "f", "f", "f", // grab cheese and cut it
        "d", "d", "d", "d", "d", "d", "w", "e", "a", "w", "e", "s", "s", "d", "e", // fry meat and grab plate
        "a", "a", "a", "a", "w", "w", "e", "d", "w", "e", // grab cheese slices and cooked meat
        "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", // wait for the meat to be cooked
        "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", // wait for the meat to be cooked
        "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", // wait for the meat to be cooked
        "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", // wait for the meat to be cooked
        "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", // wait for the meat to be cooked
        "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", // wait for the meat to be cooked
        "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", // wait for the meat to be cooked
        "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", // wait for the meat to be cooked
        "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", // wait for the meat to be cooked
        "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", // wait for the meat to be cooked
        "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", // wait for the meat to be cooked
        "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", // wait for the meat to be cooked
        "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", // wait for the meat to be cooked
        "a", "a", "a", "a", "w", "e", // put the plate down
        "s", "s", "s", "s", "s", "s", "s", "s", // move down
        "a", "a", "a", "a", "a", "a", "s", "e",  // grab tomato
        "d", "d", "d", "d", "d", "d", "d", "s", "e", "f", "f", "f", // cut tomato
        "e", "a", "s", "e", "a", "a", "a", "a", "a", "a", "a", "a", "a", "s", "e", // grab lettuce
        "d", "d", "d", "d", "d", "d", "d", "d", "d", "d", // move to cutting counter
        "s", "e", "f", "f", "f", "f", "f", // cut lettuce
        "e", "a", "w", "w", "w", "w", "w", "w", "w", "w", "e", "e", // grab plate
        "s", "s", "s", "s", "s", "s", "s", "s", // move down
        "e", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", // move to bottom left
        "w", "w", "w", "w", "w", "w", "w", "w", "w", "w", // move to bread counter
        "d", "w", "e", "a", "w", "e", "d", "w", "e", // grab bread
        "e", "d", "d", "d", "d", "d", "d", "d", "d", "d", "d", "d", "d", "d", "d",
        "d", "d", "d", "d", "d", "d", "d", "d", "d", "d", "d", "d", "d", "d", "e" // finish
    };
    reverse(operations.begin(), operations.end());
}
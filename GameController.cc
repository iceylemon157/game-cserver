#include "GameController.h"

using namespace ctl;

std::string GameController::resp;

void GameController::moveRight() {
    resp = "d";
}

void GameController::moveLeft() {
    resp = "a";
}

void GameController::moveUp() {
    resp = "w";
}

void GameController::moveDown() {
    resp = "s";
}

void GameController::interact() {
    resp = "e";
}

void GameController::interactSpecial() {
    resp = "f";
}

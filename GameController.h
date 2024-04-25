#ifndef CONTROL_H
#define CONTROL_H

#include <string>
#include "wfrest/HttpServer.h"

namespace ctl {
    class GameController {
    public:
        static std::string resp;
        GameController() {}
        GameController(std::string _resp) {
            resp = _resp;
        }
        static void moveLeft();
        static void moveRight();
        static void moveUp();
        static void moveDown();
        static void interact();
        static void interactSpecial();
    private:
    };
}

#endif /* CONTROL_H */

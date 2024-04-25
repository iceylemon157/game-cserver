#include <vector>

class UserAction {
public:
    UserAction() {
        InitOperation();
    }
    void SendOperation(ctl::GameController);
    void GetEvents(std::vector<std::string> events);
    void ReceiveEvents(ctl::GameController, std::vector<std::string>);

    void InitGame(ctl::GameController);

private:
    void InitOperation();
};

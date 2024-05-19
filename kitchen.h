#include <map>
using namespace std;

enum Items {
    // Non-edible
    Plate,

    // Raw Ingredients
    Cabbage,
    CheeseBlock,
    RawMeatPatty,
    Tomato,

    // Processed Ingredients
    Bread,
    CabbageSlices,
    CheeseSlices,
    CookedPatty,
    TomatoSlices,

    // Overcooked Ingredients
    BurntPatty,
};

enum Recipe {
    Salad,
    Burger,
    CheeseBurger,
    MegaBurger,
};

enum FryingState {
    Idle,
    Frying,
    Fried,
    Burnt,
};

enum DeliveryState {
    // Is this necessary?
    // Idle,
    Delivered,
    Failed,
};

enum Counter {
    // This is probably not necessary now

    // Ingredient counters
    BreadCounter,
    CabbageCounter,
    CheeseCounter,
    MeatPattyCounter,
    TomatoCounter,

    // Tool counters
    CuttingCounter,
    StoveCounter,
    NormalCounter,
    PlatesCounter,
    TrashCounter,
    DeliveryCounter,
};

// enum Event {
//     // Global events
//     Round,
//     TotalScore,
//     TimeLeft,

//     // Recipe events
//     NewRecipe,
//     RecipeList, // with score change and time left

//     // Should the following two be replaced with DeliveryState?
//     OrderDelivered,
//     RecipeTimeout,
    
//     // Player events
//     PlayerPosition,
//     PlayerHoldItems,

//     // Ingredient events
//     FryingTimer,
//     FryingState,

// };

struct Order {
    // TODO: Separate Order from Recipe in Unity
    // Properties
    int OrderID;
    Recipe recipe;
    int Score;
    int ExistedTime;
    // int RoundLeft; // Not implemented, not sure if necessary

    // Constructors
    Order() {}
    Order(int _OrderID, Recipe _recipe, int _Score, int _ExistedTime) {
        OrderID = _OrderID,
        recipe = _recipe,
        Score = _Score,
        ExistedTime = _ExistedTime;
     }
};

const map<Items, string> ItemsMap = {
    { Plate, "Plate" },
    { Cabbage, "Cabbage" },
    { CheeseBlock, "CheeseBlock" },
    { RawMeatPatty, "RawMeatPatty" },
    { Tomato, "Tomato" },
    { Bread, "Bread" },
    { CabbageSlices, "CabbageSlices" },
    { CheeseSlices, "CheeseSlices" },
    { CookedPatty, "CookedPatty" },
    { TomatoSlices, "TomatoSlices" },
    { BurntPatty, "BurntPatty" },
};

const map<Recipe, string> RecipeMap = {
    { Salad, "Salad" },
    { Burger, "Burger" },
    { CheeseBurger, "CheeseBurger" },
    { MegaBurger, "MegaBurger" },
};

const map<Recipe, vector<Items>> RecipeIngredients = {
    { Salad, { CabbageSlices, TomatoSlices } },
    { Burger, { Bread, CookedPatty } },
    { CheeseBurger, { Bread, CheeseSlices, CookedPatty } },
    { MegaBurger, { Bread, CabbageSlices, CheeseSlices, CookedPatty, TomatoSlices } },
};

const map<FryingState, string> FryingStateMap = {
    { Idle, "Idle" },
    { Frying, "Frying" },
    { Fried, "Fried" },
    { Burnt, "Burnt" },
};

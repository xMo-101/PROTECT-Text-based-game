/**
 * @brief       Main file of PROTECT
 * @author      xMo (DISCORD: alexanderaugustus, GITHUB: xMo)
 * @date        07.06.2024
 */

/*
NOTE TO SELF:
Last time i worked on battle function
do the TODO functions and implement the battle, add a loop in the game function

*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "items.h"
#include "locations_actions_enemies.h"

#define INVENTORY_CAPACITY      6
#define ESC                     (27 - '0')
#define LOAD_ERROR              "Error: Could not load data file.\n"
#define gotoxy(x, y)            printf("\033[%d;%dH", (y), (x))
#define barrier() (             printf("========================================\n") )
#define SAVECURSORPOS()         printf("\x1b[s")
#define RESTORECURSORPOS()      printf("\x1b[u")
/*
        SLEN ---> locations_actions.h
*/

typedef struct item
{
    char name[SLEN];
    float heal;
    float damage_multiplier;
    int uses;
    int uses_max;
}   item;



typedef struct player
{
    int level;
    int health;
    char location[SLEN];
    item inventory[INVENTORY_CAPACITY]; // Inventory array
    int locationsUNLOCKED;
}   player;

player plr;

// Function prototypes
// game functions:
void main_menu();
int getInput();
int isFirstRun();
void displayUI();
void deleteUsedItems(FILE* file);
void printPossibleActions();
void printInventory();
int getPlayerLocationID();
int rolldie(int sides);
int game();
int findenemy();
int goBack();

// TODO:
int evaluateAction(int action);
int Battle(enemy foe);
void printBattleUI(enemy foe);
enemy createEnemy();

// system essential functions:
void exitGame();
void getUses(char item_name[], int Index);
void first_run_code();
int save();
int load();

// Global variables
FILE* dataFile = NULL;

// Main function
int main() {
    system("CHCP 65001"); // Set console to UTF-8
    printf("\033c");
    int first_run = isFirstRun(); // is this the first run of the program?
    int plrPlayed = 0; // did player play game or direclty exit?
    
    // Open data file
    dataFile = fopen("files/data.txt", "r+");
    if ( NULL == dataFile ) {
        printf("%s", LOAD_ERROR);
        return EXIT_FAILURE;
    }

    // Code to manage a first run of the program (game)
    if (first_run) {
        first_run_code();
    }
    if (!first_run)
    {
        load(dataFile);
        for (int i = 0; i < 3; i++)
        {
            printf("Loading game data in %d seconds %s", 3 - i, (i == 0) ? "." : (i == 1) ? ".." : "...");
            Sleep(1000);
            printf("\033c");
        }
        Sleep(500);
    }
    
    int input = 0;
    int UIInput = 0;
    // - [ MAIN MAIN CODE ] -
    printf("\033c");

    main_menu();
    do
    {
        input = 0;
        // Get and check input from user
        while ((input < 1 || input > 2) && ESC != input)
        {
            input = getInput();
        }

        printf("\033c");
        
        switch (input)
        { // MAIN GAME:
        case 1:
            // TODO: Implement game code
            game();

        case 2:
        case ESC:
            break;
        }

    } while (input != 2 && input != ESC);

    
    exitGame(dataFile);
}

int game()
{
    int action = 0;
    displayUI();

    action = getInput();
    evaluateAction(action);

    return 1;
}

void main_menu() {
    printf("- - - - - - - - - - -\n");
    printf("┌─┐┬─┐┌─┐┌┬┐┌─┐┌─┐┌┬┐\n");
    printf("├─┘├┬┘│ │ │ ├┤ │   │ \n");
    printf("┴  ┴└─└─┘ ┴ └─┘└─┘ ┴ \n");
    printf("- - - - - - - - - - -\n");
    printf("[1]  Start\n");
    printf("[2]  Exit\n\n");
    printf("Input: ");
}

void exitGame()
{
    printf("\033c");
    printf("Exiting...\n");
    save();
    if (save)
    {
        printf("Saved progress.\n");
    }
    else
    {
        printf("Could not save progress.\n");
    }

    // End of Program (Exit)
    fclose(dataFile);
    printf("\n");
    system("PAUSE");
    exit(EXIT_SUCCESS);
}

void first_run_code() {
    // TODO: Implement first run code (Play tutorial, give Info, etc.)
    // Set player data to default values:
    plr.level = 1;
    plr.health = 100;
    plr.locationsUNLOCKED = 2;
    strcpy(plr.location, locs_acs[0].location);
    for (int i = 0; i < INVENTORY_CAPACITY; i++)
    {
        if (i == 0)
        {
            strcpy(plr.inventory[i].name, SWORD);
            plr.inventory[i].heal = SWORD_HEAL;
            plr.inventory[i].damage_multiplier = SWORD_DAMAGE;
        }
        else
        {
            strcpy(plr.inventory[i].name, EMPTY);
            plr.inventory[i].heal = EMPTY_HEAL;
            plr.inventory[i].damage_multiplier = EMPTY_DAMAGE;
        }
    }
    // Tutorial:
    printf("Hello, welcome to PROTECT!\n");
    printf("This is your first time playing the game, so let's get you started.\n\n");
    system("PAUSE");
    printf("\033c");
    printf("Please make sure to exit the game properly to save your progress.\n");
    printf("If you want to learn about the stats of items, please read the included README file.\n\n");
    system("PAUSE");
    printf("\033c");
    printf("Good luck and have fun!\n");
    printf("\033c");
    return;
}

int save() {
    fseek(dataFile, 0, SEEK_SET);

    // Save player data to file:
    fprintf(dataFile, "%d\n", plr.level);
    fprintf(dataFile, "%d\n", plr.health);
    fprintf(dataFile, "%s\n", plr.location);
    for (int i = 0; i < INVENTORY_CAPACITY; i++) {
        fprintf(dataFile, "%s\n", plr.inventory[i].name);
    }
    fprintf(dataFile, "%d\n", plr.locationsUNLOCKED);

    // Save inventory stats to file:
    FILE* usesFile = NULL;
    usesFile = fopen("files/uses.txt", "r+");
    if ( NULL == usesFile ) {
        printf("%s", LOAD_ERROR);
        return 0;
    }
    for (int i = 0; i < INVENTORY_CAPACITY; i++)
    {
        fprintf(usesFile, "%s\n%d\n", plr.inventory[i].name, plr.inventory[i].uses);
    }
    
    return 1;
}

int getInput() {
    int input = getch();
    return input - '0';
}

int load() {
    // Get player data from file:
    fseek(dataFile, 0, SEEK_SET);
    fscanf(dataFile, "%d", &plr.level);
    fscanf(dataFile, "%d", &plr.health);
    fscanf(dataFile, "%s", plr.location);

    // Load inventory from file:
    for (int i = 0; i < INVENTORY_CAPACITY; i++) {
        fscanf(dataFile, "%s", plr.inventory[i].name);
    }
    fscanf(dataFile, "%d", &plr.locationsUNLOCKED);
    
    // Load inventory stats from file:
    // Open items file
    FILE* itemsFile = NULL;
    itemsFile = fopen("files/uses.txt", "r");
    if ( NULL == itemsFile ) {
        printf("%s", LOAD_ERROR);
        return 0;
    }
    // Load item stats from file:
    for (int i = 0; i < INVENTORY_CAPACITY; i++)
    {
        if ( !(strcmp(plr.inventory[i].name, SWORD)) )
        {
            plr.inventory[i].heal = SWORD_HEAL;
            plr.inventory[i].damage_multiplier = SWORD_DAMAGE;
            plr.inventory[i].uses_max = SWORD_USES_MAX;
            // Load Uses
            getUses(SWORD, i);
            
        }
        else if (strcmp(plr.inventory[i].name, JUICE) == 0)
        {
            plr.inventory[i].heal = JUICE_HEAL;
            plr.inventory[i].damage_multiplier = JUICE_DAMAGE;
            plr.inventory[i].uses_max = JUICE_USES_MAX;
            // Load Uses
            getUses(JUICE, i);
        }
        else if (strcmp(plr.inventory[i].name, BREAD) == 0)
        {
            plr.inventory[i].heal = BREAD_HEAL;
            plr.inventory[i].damage_multiplier = BREAD_DAMAGE;
            plr.inventory[i].uses_max = BREAD_USES_MAX;
            // Load Uses
            getUses(BREAD, i);
        }
        else if (strcmp(plr.inventory[i].name, EMPTY) == 0)
        {
            plr.inventory[i].heal = EMPTY_HEAL;
            plr.inventory[i].damage_multiplier = EMPTY_DAMAGE;
        }
        
    }
    return 1;
}

void getUses(char item_name[], int Index) {
    // Open uses file
    FILE* usesFile = NULL;
    usesFile = fopen("files/uses.txt", "r");
    if ( NULL == usesFile ) {
        printf("%s", LOAD_ERROR);
        return;
    }
    // Load item uses from file:
    char tmp[SLEN];
    int tmpINT;
    fseek(usesFile, 0, SEEK_SET);

    for (int i = 0; i < INVENTORY_CAPACITY; i++)
    {
        fscanf(usesFile, "%s", tmp);
        if ( !(strcmp(tmp, item_name)) )
        {
            fscanf(usesFile, "%d", &plr.inventory[i].uses);
            break;
        }
        else
        {
            fscanf(usesFile, "%d", &tmpINT);
        }
    }
    fclose(usesFile);
}

/**
 * @brief   Checks if it is the first run of the program.
 * @details if flag file found -> not first run | if flag file not found -> first run
 * @return 1 = First run, 0 = Not first run
 */
int isFirstRun() {
    FILE* flagFile = NULL;
    flagFile = fopen("files/first_run.flag", "r"); // Open file in read mode

    if (NULL == flagFile) { // if file not found, create it, = first run
        // printf("Flag-File not found\n");
        flagFile = fopen("files/first_run.flag", "w+");
        fclose(flagFile);
        return 1;
    }

    fclose(flagFile);
    return 0;
}

void printInventory()
{
    printf("[Level: %d]     [Health: %d]        [Location: %s]\n", plr.level, plr.health, plr.location);
    printf("Inventory: ");
    for (int i = 0; i < INVENTORY_CAPACITY/2; i++)
    {
        printf("%01i[%5s: %03d/%03d] ", i+1, plr.inventory[i].name, plr.inventory[i].uses, plr.inventory[i].uses_max);
    }
    printf("\n           ");
    for (int i = INVENTORY_CAPACITY/2; i < INVENTORY_CAPACITY; i++)
    {
        printf("%01i[%5s: %03d/%03d] ", i+1, plr.inventory[i].name, plr.inventory[i].uses, plr.inventory[i].uses_max);
    }
}

void displayUI() {
    printInventory();
    printPossibleActions();
}

void deleteUsedItems(FILE* file) {
    for (int i = 0; i < INVENTORY_CAPACITY; i++)
    {
        if ( plr.inventory[i].uses == plr.inventory[i].uses_max )
        {
            plr.inventory[i].heal = EMPTY_HEAL;
            plr.inventory[i].damage_multiplier = EMPTY_DAMAGE;
            plr.inventory[i].uses = 0;
            plr.inventory[i].uses_max = 0;
            save(file);
            strcpy(plr.inventory[i].name, EMPTY);
        }
    }
    displayUI();
}

void printPossibleActions()
{
    int index = getPlayerLocationID();
    printf("\n\nActions:\n");
    for (int i = 0; i < MAX_POSSIBLE_ACTIONS; i++)
    {
        printf("%i. [ %-20s ]\n", i + 1, locs_acs[index].actions[i]);
    }
}

int evaluateAction(int action)
{
    // What locationID is the player at?
    int location = getPlayerLocationID();
    // Handle go back action:
    switch (action)
    {
    case 6:
        if (!goBack()) exitGame();
        break;
    case 1:
        if (!location && plr.health >= 100) // Player tries to rest with 100HP
        {
            printf("You good fam\n");
        }
        else if (!location  && plr.health < 100) // Player rests
        {
            plr.health = 100;
            printf("You have been healed to full health.\n");
        }
        else if (location > 0 && location < 10) // Player tries to find enemy
        {
            if (findenemy())
            {
                Battle( createEnemy() );
            }
        }
        
    }
}

int Battle(enemy foe)
{
    printf("\033c");
    int attack = 0;
    float FDM = 1.0; // Foe Damage Multiplier
    do
    {
        gotoxy(0, 0); // Set cursor to top left
        // print battle UI
        printBattleUI(foe);
        // Player Attack:
        // get Player attack
        printf("\nUse item [1-6]: ");
        SAVECURSORPOS();
        do
        {
            RESTORECURSORPOS();
            printf("\x1b[u");
            attack = getInput();
        } while (attack < 1 && attack > 6);
        // use player attack / item
        attack -= 1;
        if (plr.inventory[attack].uses > 0)
        {
            foe.health -= ((int)(((float)FISTDAMAGE) * (plr.inventory[attack].damage_multiplier)));
            if (plr.health < 100)
            {
                plr.health += ((int)(((float)plr.health) * (plr.inventory[attack].heal)));
            }
            plr.inventory[attack].uses += 1;
            // Generate random foe dmg multiplier when player uses multiplier
        }
        // print battle UI
        printBattleUI(foe);
        // Delete used items
        deleteUsedItems(dataFile);
        
        // Enemy attack:
        FDM = ((float)(rolldie(6) / 6)) + 0.5; // Generate random foe dmg multiplier  (in 2 out of 6 cases, the multiplier is <1)
        plr.health -= ((int)(((float)foe.damage) * FDM));

    } while (foe.health > 0 && plr.health > 0);
    
    if (foe.health <= 0)
    {
        printf("You have defeated the %s!\n", foe.name);
        return 1;
    }
    else
    {
        printf("You have been defeated by the %s!\n", foe.name);
        return 0;
    }
}

int findenemy()
{
    printf("Searching . . .\n");
    Beep(523, 500);
    int roll = rolldie(6);
    if ( roll <= 3 ) // Enemy found
    {
        printf("Enemy found!\n");
        return 1;
    }
    else
    {
        printf("No enemy found. :(\n");
        return 0;
    }
}

void printBattleUI(enemy foe)
{
    printInventory();
    printf("\n\n");
    barrier();
    printf("%-40s\n", foe.name);
    barrier();
    printf("\n\n");
    printf("\x1b[4m%-20s%10s%-20s\x1b[0m\n", "You", " ", foe.name);
    printf("%03i%27s%03i\n", plr.health, " ", foe.health);
    printf("\n\n");
}

/**
 * @brief   Generates a random number between 1 and the specified number of sides.
 * @param   sides The number of sides on the die.
 * @return  The randomly generated number.
 */
int rolldie(int sides)
{
    time_t t;
    time(&t);
    srand((unsigned int) t);
    return rand() % sides + 1;
}

int getPlayerLocationID()
{
    // What location is the player at?
    for (int i = 0; i < AMT_LOCATIONS; i++)
    {
        if ( !(strcmp(plr.location, locs_acs[i].location)) )
        {
            return i;
        }
    }
}

int goBack()
{
    int locationID = getPlayerLocationID();
    if (locationID == 0) exitGame();
    strcpy(plr.location, locs_acs[locationID-1].location);
    return 1;
}

enemy createEnemy()
{
    enemy Foe;
    int location = getPlayerLocationID();
    switch (location)
    {
        case 1:
            strcpy(Foe.name, JA_BANDIT_NAME);
            Foe.health = HP_FULL;
            Foe.damage = JA_BANDIT_BASEDMG;
            Foe.XPDrop = JA_BANDIT_XPDROP;
            break;
        default:
            break;
    }

    return Foe;
}
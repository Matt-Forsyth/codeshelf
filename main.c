//gcc main.c -o main -I/usr/include/postgresql -lpq

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <libpq-fe.h>

#define qTerm true
#define tDebug false

typedef struct {
    int id;
    char name[64];
    float version;
    char path[256];
    char exectution[256];
    int menuID;
    char created_at[20];
    int langid;
    char fileName[256];
} Project;

typedef struct {
    int id;
    char name[64];
    char contents[4096];
    int bannerID;
    int parentID;
    char path[257];
} Menu;

typedef struct {
    int id;
    char name[64];
    char contents[4096];
    char created_at[20];
} Banner;

typedef struct {
    int id;
    char name[256];
    char prefix[256];
    char path[256];
    char command[256];
    double version;
} Language;

    int currentBanner = 0;
    int currentMenu = 0;
    #define clear() printf("\033[H\033[J")

PGconn* connect_db() {
    const char *conninfo = "host=localhost port=5432 dbname=codeshelf user=postgres password=roadscout";
    PGconn *conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    return conn;
}

// Function to fetch projects from database.
Project* get_projects(PGconn *conn, int *count) {
    PGresult *res = PQexec(conn, "SELECT id, name, version, path, exectution, menuID, langid, fileName FROM projects;");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "get_projects query failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    int n = PQntuples(res);
    *count = n;
    Project *projects = malloc(n * sizeof(Project));
    for (int i = 0; i < n; i++) {
        projects[i].id = atoi(PQgetvalue(res, i, 0));
        strncpy(projects[i].name, PQgetvalue(res, i, 1), sizeof(projects[i].name)-1);
        projects[i].version = atof(PQgetvalue(res, i, 2));
        strncpy(projects[i].path, PQgetvalue(res, i, 3), sizeof(projects[i].path)-1);
        strncpy(projects[i].exectution, PQgetvalue(res, i, 4), sizeof(projects[i].exectution)-1);
        projects[i].menuID = atoi(PQgetvalue(res, i, 5));
        projects[i].langid = atoi(PQgetvalue(res, i, 6)); // Updated to load langid.
        strncpy(projects[i].fileName, PQgetvalue(res, i, 7), sizeof(projects[i].fileName)-1); // Assuming you have a filename field.
    }
    PQclear(res);
    return projects;
}

// Function to fetch menus from database.
Menu* get_menus(PGconn *conn, int *count) {
    PGresult *res = PQexec(conn, "SELECT id, name, contents, bannerID, parentID, path FROM menus;");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    int n = PQntuples(res);
    *count = n;
    Menu *menus = malloc(n * sizeof(Menu));
    for (int i = 0; i < n; i++) {
        menus[i].id = atoi(PQgetvalue(res, i, 0));
        strncpy(menus[i].name, PQgetvalue(res, i, 1), sizeof(menus[i].name)-1);
        strncpy(menus[i].contents, PQgetvalue(res, i, 2), sizeof(menus[i].contents)-1);
        menus[i].bannerID = atoi(PQgetvalue(res, i, 3));
        menus[i].parentID = atoi(PQgetvalue(res, i, 4));
        strncpy(menus[i].path, PQgetvalue(res, i, 5), sizeof(menus[i].path)-1);
    }
    PQclear(res);
    return menus;
}

// Function to fetch banners from database.
Banner* get_banners(PGconn *conn, int *count) {
    PGresult *res = PQexec(conn, "SELECT id, name, contents, created_at FROM banners;");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    int n = PQntuples(res);
    *count = n;
    Banner *banners = malloc(n * sizeof(Banner));
    for (int i = 0; i < n; i++) {
        banners[i].id = atoi(PQgetvalue(res, i, 0));
        strncpy(banners[i].name, PQgetvalue(res, i, 1), sizeof(banners[i].name)-1);
        strncpy(banners[i].contents, PQgetvalue(res, i, 2), sizeof(banners[i].contents)-1);
    }
    PQclear(res);
    return banners;
}
Language* get_languages(PGconn *conn, int *count) {
    PGresult *res = PQexec(conn, "SELECT id, name, prefix, path, command, version FROM languages;");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "get_languages query failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    int n = PQntuples(res);
    *count = n;
    Language *languages = malloc(n * sizeof(Language));
    for (int i = 0; i < n; i++) {
        languages[i].id = atoi(PQgetvalue(res, i, 0));
        strncpy(languages[i].name, PQgetvalue(res, i, 1), sizeof(languages[i].name)-1);
        strncpy(languages[i].prefix, PQgetvalue(res, i, 2), sizeof(languages[i].prefix)-1);
        strncpy(languages[i].path, PQgetvalue(res, i, 3), sizeof(languages[i].path)-1);
        strncpy(languages[i].command, PQgetvalue(res, i, 4), sizeof(languages[i].command)-1);
        languages[i].version = strtod(PQgetvalue(res, i, 5), NULL);
    }
    PQclear(res);
    return languages;
}
void execute_query(PGconn *conn, const char *query) {
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Query failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    PQclear(res);
}


void activateDB() {
    PGconn *conn = connect_db();
    if(tDebug == true){
        printf("DEBUG: Database Connection Made\n");
    }
    PQfinish(conn);
}

void deactivateDB() {
    PGconn *conn = connect_db();
    if(tDebug == true){
        printf("DEBUG: Database Connection Closed\n");
    }
    PQfinish(conn);
}

int tQuerryTerminal(){
    PGconn *conn = connect_db();

    int projects_count = 0;
    int menus_count = 0;
    int banners_count = 0;

    int selectionInt;
    int option;

    bool requestSolved = false;

    printf("1. Projects\n2. Menus\n3. Banners\n");
    scanf("%d", &option);

    Project *projects = get_projects(conn, &projects_count);
    Menu *menus = get_menus(conn, &menus_count);
    Banner *banners = get_banners(conn, &banners_count);

    clear();
    printf("Input Requested ID: \n");
    scanf("%d", &selectionInt);

    switch (option) {
        case 1:
            for (int i = 0; i < projects_count; i++) {
                if (projects[i].id == selectionInt) {
                    clear();
                    printf("Requested Project: \nID: %d \nName: %s \nVersion: %.2f \nPath: %s \nExecution: %s \nMenuID: %d\n",
                           projects[i].id, projects[i].name, projects[i].version, projects[i].path, projects[i].exectution, projects[i].menuID);
                    requestSolved = true;
                    break;
                }
            }
            if (!requestSolved) {
                clear();
                printf("Project with ID %d not found.\n", selectionInt);
            }
            break;
        case 2:
            for (int i = 0; i < menus_count; i++) {
                if (menus[i].id == selectionInt) {
                    clear();
                    printf("Requested Menu: \nID: %d \nName: %s \nContents: %s \nBannerID: %d \nParentID: %d \nPath: %s\n",
                           menus[i].id, menus[i].name, menus[i].contents, menus[i].bannerID, menus[i].parentID, menus[i].path);
                    requestSolved = true;
                    break;
                }
            }
            if (!requestSolved) {
                clear();
                printf("Menu with ID %d not found.\n", selectionInt);
            }
            break;
        case 3:
            for (int i = 0; i < banners_count; i++) {
                if (banners[i].id == selectionInt) {
                    clear();
                    printf("Requested Banner: \nID: %d \nName: %s \nContents: %s\n",
                           banners[i].id, banners[i].name, banners[i].contents);
                    requestSolved = true;
                    break;
                }
            }
            if (!requestSolved) {
                clear();
                printf("Banner with ID %d not found.\n", selectionInt);
            }
            break;
        default:
            clear();
            printf("Invalid option selected.\n");
            break;
    }

    free(projects);
    free(menus);
    free(banners);
}

void tBanner(PGconn *conn){
    int banners_count = 0;
    Banner *banners = get_banners(conn, &banners_count);
    Banner *all_banners = banners; // Save the original pointer
    int found = -1;
    for (int i = 0; i < banners_count; i++) {
        if (banners[i].id == currentBanner) {
            found = i;
            break;
        }
    }
    if (found >= 0) {
        for (char *p = banners[found].contents; *p; p++) {
            if (*p == '\\' && *(p + 1) == 'n') {
                putchar('\n');
                p++;
            } else {
                putchar(*p);
            }
        }
    } else {
        printf("Invalid banner index.\n");
    }
    free(banners);
}
void tGetBanner(PGconn *conn, int menuID) {
    int menus_count = 0;
    Menu *menus = get_menus(conn, &menus_count);
    int found = -1;
    for (int i = 0; i < menus_count; i++) {
        if (menus[i].id == menuID) {
            found = i;
            break;
        }
    }
    if (found >= 0) {
        int menuBannerID = menus[found].bannerID;
        int banners_count = 0;
        Banner *banners = get_banners(conn, &banners_count);
        
        // First, print banner with id 2 if found.
        int banner2_found = -1;
        for (int i = 0; i < banners_count; i++) {
            if (banners[i].id == 2) {
                banner2_found = i;
                break;
            }
        }
        if (banner2_found >= 0) {
            for (char *p = banners[banner2_found].contents; *p; p++) {
                if (*p == '\\' && *(p + 1) == 'n') {
                    putchar('\n');
                    p++;
                } else {
                    putchar(*p);
                }
            }
        } else {
            printf("Code Shelf Banner Not Found. ");
        }
        if (menuBannerID == 2) {
        } else {
            int banner_found = -1;
            for (int i = 0; i < banners_count; i++) {
                if (banners[i].id == menuBannerID) {
                    banner_found = i;
                    break;
                }
            }
            if (banner_found >= 0) {
                for (char *p = banners[banner_found].contents; *p; p++) {
                    if (*p == '\\' && *(p + 1) == 'n') {
                        putchar('\n');
                        p++;
                    } else {
                        putchar(*p);
                    }
                }
            } else {
                for (int i = 0; i < banners_count; i++) {
                    printf(" %d", banners[i].id);
                }
            }
        }
        free(banners);
    } else {
        printf("Menu with ID %d not found.\n", menuID);
    }
    free(menus);
}

int tGetIntInput(){
    int input;
    scanf("%d", &input);
    return input;
}

int tMenuMMSel(int selection){
    switch (selection){
        case 1:
            clear();
            return 3;
            break;
        case 2:
            clear();
            return 2;
            break;
        case 3:
            clear();
            return 4;
            break;
        default:
            clear();
            printf("Invalid Selection: %d\n", selection);
            break;
    }
}

int tSwitchMenu(int reqMenu){
    PGconn *conn = connect_db();
    int menus_count = 0;
    Menu *menus = get_menus(conn, &menus_count);

    switch (reqMenu){
        case 1: // Main Menu
            clear();
            currentMenu = 1;
            tGetBanner(conn, currentMenu);
            break;
        case 2: // Tools
            clear();
            currentMenu = 2;
            tGetBanner(conn, currentMenu);
            break;
        case 3: // C Projects
            clear();
            currentMenu = 3;
            tGetBanner(conn, currentMenu);
            break;
        case 4: // Settings
            clear();
            currentMenu = 4;
            tGetBanner(conn, currentMenu);
            break;
        default:
            clear();
            currentMenu = 1;
            printf("Invalid Menu Selection: %d\n", currentMenu);
            tBanner(conn);
            break;
    }
    for (char *p = menus[currentMenu].contents; *p; p++) {
        if (*p == '\\' && *(p + 1) == 'n') {
            putchar('\n');
            p++;
        } else {
            putchar(*p);
        }
    }
}
void launch_project(PGconn *conn, int project_id) {
    clear();
    int projects_count = 0;
    Project *projects = get_projects(conn, &projects_count);
    int found = -1;
    for (int i = 0; i < projects_count; i++) {
        if (projects[i].id == project_id) {
            found = i;
            break;
        }
    }
    if (found != -1) {
        char command[1024];
        // Build command based on langid:
        switch (projects[found].langid) {
            case 1:
                // For compiled programs: assume filename is an executable in the current directory.
                snprintf(command, sizeof(command), "sudo ./%s", projects[found].fileName);
                break;
            case 2:
                // For Python scripts:
                snprintf(command, sizeof(command), "python3 %s", projects[found].fileName);
                break;
            case 3:
                // For Node.js applications:
                snprintf(command, sizeof(command), "node %s", projects[found].fileName);
                break;
            default:
                printf("Unsupported language ID %d for Project ID %d\n", projects[found].langid, project_id);
                free(projects);
                return;
        }
        // Execute the command in the terminal.
        printf("Executing command: %s\n", command);
        system(command);
    } else {
        printf("Project with ID %d not found.\n", project_id);
    }
    free(projects);
}
int main() {
    PGconn *conn = connect_db();
    deactivateDB();
    activateDB();

    tSwitchMenu(1);
    int sel = tMenuMMSel(tGetIntInput());
    tSwitchMenu(sel);
    launch_project(conn, tGetIntInput());
    //if(qTerm == true){
    //    querryTerminal(); Put this somewhere else to be called when needed
    //}
    
}
#include <stdio.h>
#include <stdbool.h> 
#include <string.h>

#define MAX_PROJECTS 128
#define MAX_LANGUAGE 16
#define MAX_MENUS 32

#define DEBUG false

#define DEFAULT_LANGUAGE 127

struct project_t {
    int projectID;
    int projectMenuID;
    int projectLanguage;

    char projectName[16];
    char projectAuthor[16];
    char projectVersion[16];
    char projectLocation[64];

    bool projectOccupied;
};
struct cMenu_t {
    int cmenuID;
    char cmenuName[16];
    char cmenuLocation[16];
    bool cmenuIsActive;
};
struct langIdentifer_t {
    int languageID;
    char languageName[16];
    char languagePath[16];
    char languagePrefix[4];
};

struct cMenu_t cMenus[MAX_MENUS];
struct project_t ProjectLibrary[MAX_PROJECTS];
struct langIdentifer_t Languages[MAX_LANGUAGE];

bool NewProjectLibrary(){
    bool projectsCreated = false;
    bool languagesCreated = false;
    bool cmenusCreated =false;

    if(projectsCreated == false){
        int i = 0;
        for(i = 0; i < MAX_PROJECTS; i++){

            ProjectLibrary[i].projectID = i;
            ProjectLibrary[i].projectMenuID = 0;
            ProjectLibrary[i].projectLanguage = 127;

            strcpy(ProjectLibrary[i].projectName, "\0)");
            strcpy(ProjectLibrary[i].projectAuthor, "\0)");
            strcpy(ProjectLibrary[i].projectLocation, "\0)");
            strcpy(ProjectLibrary[i].projectVersion, "\0");

            ProjectLibrary[i].projectOccupied = false;
            if(DEBUG == true){
                printf("Project ID: %d Creaated \n", ProjectLibrary[i].projectID);
            }
            if(i == MAX_PROJECTS){
                projectsCreated = true;
                break;
            }
        }
    }

    if(languagesCreated == false){
        int i = 0;
        for(i = 0; i < MAX_LANGUAGE; i++){
            Languages[i].languageID = i;
            if (DEBUG == true){
                printf("Language ID: %d Created \n", Languages[i].languageID);
            }
            if(i == MAX_LANGUAGE){
                languagesCreated = true;
                break;
            }
        }
    }

    if(cmenusCreated == false){
        int i = 0;
        for(i = 0; i < MAX_MENUS; i++){
            cMenus[i].cmenuID = i;
            if(DEBUG == true){
            printf("Menu ID: %d Created \n", cMenus[i].cmenuID);
            }
            if(i == MAX_MENUS){
                cmenusCreated = true;
                break;
            }
        }
    }

    if(cmenusCreated == true && languagesCreated == true && projectsCreated == true){
        return true;
    }else{
        return false;
    }

}

bool intitalizeProjectLibrary(){

    strcpy(Languages[DEFAULT_LANGUAGE].languageName, "Unknown");
    strcpy(Languages[DEFAULT_LANGUAGE].languagePrefix, "UNKN");
    strcpy(Languages[DEFAULT_LANGUAGE].languagePath, "\0");
    if (DEBUG == true)
    {
        printf("Defualt Language: %s \n", Languages[DEFAULT_LANGUAGE].languageName);
    }

    strcpy(Languages[0].languageName, "C++");
    strcpy(Languages[0].languagePrefix, "C++");
    strcpy(Languages[0].languagePath, "\0");
    if(DEBUG == true){
        printf("Language Initalized: %s \n", Languages[0].languageName);
    }

    strcpy(Languages[1].languageName, "Python 3.12");
    strcpy(Languages[1].languagePrefix, "PYTH");
    strcpy(Languages[1].languagePath, "\0");
    if (DEBUG == true){
        printf("Language Initalized: %s \n", Languages[1].languageName);
    }

    int i = 0;
    int l = 0;

    for(i = 0; i < MAX_PROJECTS; i++){
        ProjectLibrary[i].projectID = DEFAULT_LANGUAGE;
        if(i = MAX_PROJECTS){
            
        }
    }

    if(DEBUG == true){
        printf("DEBUG MODE ENABLED\n");

        int i = ProjectLibrary[0].projectLanguage;
        printf("Project 1 Language: %s \n", Languages[i].languageName);
    }

    printf("Project Library Initalized \n");

}

int main() {
    int menuID[MAX_MENUS];

    NewProjectLibrary();
    intitalizeProjectLibrary();
}
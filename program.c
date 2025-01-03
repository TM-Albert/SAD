#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct User {
    int UserID;                  // Unikalny identyfikator użytkownika
    struct User *parent;         // Wskazuje na rodzica danego użytkownika
    struct User **children;      // Wskaźnik na tablicę wskaźników, gdzie każdy wskaźnik wskazuje na dziecko (inny obiekt User)
    int childrenCount;           // Liczba dzieci aktualnie przypisanych do tego użytkownika
    int childrenCapacity;        // Pojemność tablicy dzieci (ile dzieci maksymalnie może być przechowywanych w obecnej alokacji)
    unsigned int *movies;        // Tablica identyfikatorów filmów, które dany użytkownik ocenił
    unsigned int moviesCount;    // Liczba filmów, które dany użytkownik ocenił
    unsigned int moviesCapacity; // Pojemność tablicy filmów (ile filmów maksymalnie może być przechowywanych w obecnej alokacji)
} User;

User *initializeRoot() {
    User *root = malloc(sizeof(User));
    root->UserID = 0;
    root->parent = NULL;
    root->children = NULL;
    root->childrenCount = 0;
    root->childrenCapacity = 0;
    root->movies = NULL;
    root->moviesCount = 0;
    root->moviesCapacity = 0;

    return root;
}

void freeUser(User *user) {
    if (user == NULL) return;

    for (int i = 0; i < user->childrenCount; i++) {
        freeUser(user->children[i]);
    }

    free(user->children);
    free(user->movies);
    free(user);
}

User *findUser(User *node, unsigned int userID) {
    if (node == NULL) return NULL;
    if (node->UserID == userID) return node;

    for (unsigned int i = 0; i < node->childrenCount; i++) {
        User *result = findUser(node->children[i], userID);
        if (result != NULL) return result;
    }

    return NULL;
}

void addUser(User *root, unsigned int parentUserID, unsigned int userID) {
    if (findUser(root, userID) != NULL) {
        printf("ERROR\n");
        return;
    }

    User *parent = findUser(root, parentUserID);
    if (parent == NULL) {
        printf("ERROR\n");
        return;
    }

    User *newUser = malloc(sizeof(User));
    newUser->UserID = userID;
    newUser->parent = parent;
    newUser->children = NULL;
    newUser->childrenCount = 0;
    newUser->childrenCapacity = 0;
    newUser->movies = NULL;
    newUser->moviesCount = 0;
    newUser->moviesCapacity = 0;

    if (parent->childrenCount == parent->childrenCapacity) {
        parent->childrenCapacity = parent->childrenCapacity == 0 ? 1 : parent->childrenCapacity * 2;
        parent->children = realloc(parent->children, parent->childrenCapacity * sizeof(User *)); 
    }

    parent->children[parent->childrenCount++] = newUser;

    printf("OK\n");
}

void delUser(User *root, unsigned int userID) {
    if (userID == 0) {
        printf("ERROR\n");
        return;
    }

    User *user = findUser(root, userID);
    if (user == NULL) {
        printf("ERROR\n");
        return;
    }

    if (user->childrenCount > 0) {
        User *parent = user->parent;
        for (unsigned int i = 0; i < user->childrenCount; i++) {
            User *child = user->children[i];
            child->parent = parent;

            if (parent->childrenCount == parent->childrenCapacity) {
                parent->childrenCapacity = (parent->childrenCapacity == 0) ? 2 : parent->childrenCapacity * 2;
                parent->children = realloc(parent->children, parent->childrenCapacity * sizeof(User *));
            }
            parent->children[parent->childrenCount++] = child;
        }
    }

    User *parent = user->parent;
    for (unsigned int i = 0; i < parent->childrenCount; i++) {
        if (parent->children[i] == user) {
            for (unsigned int j = i; j < parent->childrenCount - 1; j++) {
                parent->children[j] = parent->children[j + 1];
            }
            parent->childrenCount--;
            break;
        }
    }

    free(user->children);
    free(user->movies);
    free(user);

    printf("OK\n");
}

void addMovie(User *user, unsigned int movieID) {
    for (unsigned int i = 0; i < user->moviesCount; i++) {
        if (user->movies[i] == movieID) {
            printf("ERROR\n");
            return;
        }
    }

    if (user->moviesCount == user->moviesCapacity) {
        user->moviesCapacity = (user->moviesCapacity == 0) ? 2 : user->moviesCapacity * 2;
        user->movies = realloc(user->movies, user->moviesCapacity * sizeof(unsigned int));
    }

    user->movies[user->moviesCount++] = movieID;
    printf("OK\n");
}

void delMovie(User *user, unsigned int movieID) {
    int index = -1;
    for (unsigned int i = 0; i < user->moviesCount; i++) {
        if (user->movies[i] == movieID) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("ERROR\n");
        return;
    }

    for (unsigned int i = index; i < user->moviesCount - 1; i++) {
        user->movies[i] = user->movies[i + 1];
    }

    user->moviesCount--;
    printf("OK\n");
}

void bubbleSort(unsigned int *arr, unsigned int n) {
    for (unsigned int i = 0; i < n - 1; i++) {
        for (unsigned int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                unsigned int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

unsigned int removeDuplicates(unsigned int *arr, unsigned int n) {
    if (n == 0) return 0;

    unsigned int uniqueIndex = 0;
    for (unsigned int i = 1; i < n; i++) {
        if (arr[i] != arr[uniqueIndex]) {
            uniqueIndex++;
            arr[uniqueIndex] = arr[i];
        }
    }
    return uniqueIndex + 1;
}

void printMovies(User *user, unsigned int movieID) {
    User *parent = user->parent;

    unsigned int *userMovies = user->movies;
    unsigned int *parentMovies = parent ? parent->movies : NULL;

    unsigned int userMoviesCount = user->moviesCount;
    unsigned int parentMoviesCount = parent ? parent->moviesCount : 0;
    unsigned int totalMoviesCount = userMoviesCount + parentMoviesCount;

    if (totalMoviesCount == 0) {
        printf("\n");
        return;
    }

    unsigned int *allMovies = malloc(totalMoviesCount * sizeof(unsigned int));
    unsigned int index = 0;
    for (unsigned int i = 0; i < userMoviesCount; i++) {
        allMovies[index++] = userMovies[i];
    }
    for (unsigned int i = 0; i < parentMoviesCount; i++) {
        allMovies[index++] = parentMovies[i];
    }

    bubbleSort(allMovies, totalMoviesCount);
    totalMoviesCount = removeDuplicates(allMovies, totalMoviesCount);

    for (unsigned int i = 0; i < totalMoviesCount; i++) {
        printf("%u ", allMovies[i]);
    }

    printf("\n");
    free(allMovies);
}

int main() {
    User *root = initializeRoot();  

    char command[256];
    while (fgets(command, sizeof(command), stdin) != NULL) {
        char operation[20];
        unsigned int arg1, arg2;

        if (sscanf(command, "%s %u %u", operation, &arg1, &arg2) == 3) {
            if (strcmp(operation, "addUser") == 0) {
                addUser(root, arg1, arg2);
            } else if (strcmp(operation, "addMovie") == 0) {
                User *user = findUser(root, arg1);
                if (user != NULL) {
                    addMovie(user, arg2);
                } else {
                    printf("ERROR\n");
                }
            } else if (strcmp(operation, "delMovie") == 0) {
                User *user = findUser(root, arg1);
                if (user != NULL) {
                    delMovie(user, arg2);
                } else {
                    printf("ERROR\n");
                }
            } else {
                printf("ERROR\n");
            }
        } else if (sscanf(command, "%s %u", operation, &arg1) == 2) {
            if (strcmp(operation, "printUserMovies") == 0) {
                User *user = findUser(root, arg1);
                if (user != NULL) {
                    printMovies(user, arg1);
                } else {
                    printf("ERROR\n");
                }
            } else if (strcmp(operation, "delUser") == 0) {
                delUser(root, arg1);
            } else {
                printf("ERROR\n");
            }
        } else {
            printf("ERROR\n");
        }
    }

    freeUser(root);
    return 0;
}

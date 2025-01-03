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
    unsigned int i = 0;
    while (i < user->moviesCount && user->movies[i] < movieID) {
        i++;
    }

    if (i < user->moviesCount && user->movies[i] == movieID) {
        printf("ERROR\n");
        return;
    }

    if (user->moviesCount == user->moviesCapacity) {
        user->moviesCapacity = (user->moviesCapacity == 0) ? 2 : user->moviesCapacity * 2;
        user->movies = realloc(user->movies, user->moviesCapacity * sizeof(unsigned int));
    }

    for (unsigned int j = user->moviesCount; j > i; j--) {
        user->movies[j] = user->movies[j - 1];
    }

    user->movies[i] = movieID;
    user->moviesCount++;
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

void printMovies(User *user, unsigned int movieID) {
    if (user == NULL) {
        printf("ERROR\n");
        return;
    }

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

    unsigned int *result = malloc(totalMoviesCount * sizeof(unsigned int));
    unsigned int i = 0, j = 0, k = 0;

    while (i < userMoviesCount && j < parentMoviesCount) {
        if (userMovies[i] < parentMovies[j]) {
            result[k++] = userMovies[i++];
        } else if (userMovies[i] > parentMovies[j]) {
            result[k++] = parentMovies[j++];
        } else {
            result[k++] = userMovies[i++];
            j++;
        }
    }

    while (i < userMoviesCount) {
        result[k++] = userMovies[i++];
    }

    while (j < parentMoviesCount) {
        result[k++] = parentMovies[j++];
    }

    for (unsigned int x = 0; x < k; x++) {
        printf("%u ", result[x]);
    }

    printf("\n");
    free(result);
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

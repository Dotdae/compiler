#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

// Declaración de un token tipo enum.

typedef enum tokenType{

    // Index 0...7

    palabraReservada,
    id,
    number,
    symbol,
    dataType,
    string,
    errorType,
    boolean

} token;

struct Token{

    char nombre[255];
    enum tokenType type;
    char lexema[255];
    float value;
    int lineNumbers;
    int columnNumber;

};

struct Nodo{

    struct Token info;
    struct Nodo *izq;
    struct Nodo *der;

};

struct Nodo *root = NULL;
struct Nodo *actual = NULL;



FILE *fileData;

// Obtener el tamaño del archivo.

int getDataSize(FILE *file){

    size_t position = ftell(file);
    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    fseek(file, position, SEEK_SET);

    return length;

}

/* FUNCIONES QUE MANEJAN LOS TOKENS */

// Insertar tokens en un árbol binario.

void insertToken(struct Token token){

    struct Nodo *new;

    new = malloc(sizeof(struct Nodo));

    // Al nodo info se le añade el token y después de limpian los nodos izquierdo y derecho.

    new->info = token;
    new->izq = NULL;
    new->der = NULL;

    // Si la raíz está vacia se le agregará el nuevo token como raíz y pasa a ser el nodo actual.

    if(root == NULL){

        root = new;
        actual = new;

    }

    else{

        // El nodo de la izquierda pasa a ser el que era actual y el nodo derecho pasa a ser el nuevo token.
        // Después solo se actualiza el puntero actual apuntando al último token añadido.

        new->izq = actual;
        actual->der = new;
        actual = new;

    }


}


void cleanTokenList(struct Token token){

    token.value = 0;
    memset(token.lexema, 0, sizeof(token.lexema));
    memset(token.nombre, 0, sizeof(token.nombre));
    token.type = 0;
    token.columnNumber = 0;
    token.lineNumbers = 0;

}

/* FUNCIONES QUE COMPRUEBAN LAS PALABRAS */


// Verificar si el input es una cadena.

bool isString(char* str, struct Token *token){

    char character = str[0];
    char z = str[strlen(str)-1];

    if(str[0] == '"' && (str[strlen(str)-1] == '"' || str[strlen(str)-1 == ' '])){

        strcpy(token->lexema, str);
        token->type = string;
        strcpy(token->nombre, "Cadena");
        token->value = 0;

        return true;

    }
    else{
        return false;
    }

}

// Verificar el tipo de dato el input.

bool isDataType(char* str, struct Token *token){

    if(strcmp(str, "ent") == 0 || strcmp(str, "flot") == 0 || strcmp(str, "cad") == 0 || strcmp(str, "bool") == 0){

        strcpy(token->lexema, str);
        token->type = dataType;
        token->value = 0;
        strcpy(token->nombre, "Tipo de Dato");
        
        return true;

    }
    
    return false;

}

//

bool isReservedWord(char* str, struct Token *token){

    if(strcmp(str, "paquete") == 0 || strcmp(str, "funct") == 0 || strcmp(str, "inicio") == 0 || strcmp(str, "var") == 0 || strcmp(str, "const") == 0 || strcmp(str, "si") == 0 || strcmp(str, "o") == 0 || strcmp(str, "entonces") == 0 || strcmp(str, "para") == 0 || strcmp(str, "rango") == 0 || strcmp(str, "mostrart") == 0 || strcmp(str, "introducir")){

        strcpy(token->lexema, str);
        token->type = palabraReservada;
        strcpy(token->nombre, "Instruccion");
        token->value = 0;

        return true;
    }

    return false;

}


// Recorrer el archivo y leer cada token existente.

void readTokens(char* source){

    int lines = 1, columna = 0;
    char aux[255];
    memset(aux, 0, sizeof(aux));

    int x = 0, y = 0;
    struct Token token;
    bool cad = false;
    int cadAuxiliar = 0;

    // Mientras no se llegue al final del archivo.

    while(source[x] != EOF){

        // Se cuentan los saltos de línea.

        if(source[x] == '\n'){
            lines++;
            columna = 0;
        }

        char character = source[x];

        // Se verifica si existen comilla simples.

        if(cad == true && cadAuxiliar != 2){

            if(source[x] == '"'){
                cad = true;
                cadAuxiliar++;
            }

            aux[y] = source[x];
            y++;

        }

        // Se verifica si es una cadena.

        else if(cadAuxiliar == 2){

            if(isString(aux, &token)){

                cad = false;
                cadAuxiliar = 0;
                token.columnNumber = columna;
                token.lineNumbers = lines;
                insertToken(token);
                cleanTokenList(token);

            }
            else if(aux[0] == '"'){

                token.type = errorType;
                strcpy(token.nombre, "Error de comilla");
                token.value = 0;
                strcpy(token.lexema, "Error de comilla");
                token.columnNumber = columna;
                token.lineNumbers = lines;
                insertToken(token);
                limpiarToken(token);

            }

            memset(aux, 0, sizeof(aux));
            y = 0;
            continue;

        }
        else{

            if(esLimitador(source[x])){

                if(y != 0){

                    if(isDataType(aux, &token)){

                        token.columnNumber = columna;
                        token.lineNumbers = lines;
                        insertToken(token);
                        limpiarToken(token);

                    }
                    else if(isReservedWord(aux, &token)){
                        
                        token.columnNumber = columna;
                        token.lineNumbers = lines;
                        insertToken(token);
                        limpiarToken(token);

                    }

                }


            }

        }

    }

}


int main(int argc, char *argv[]){
    
    // Verifica si en la ejecución se envía un archivo como parámetro.

    if(argc >= 2){

        // Se abre el archivo de ejemplo.

        fileData = fopen("./example.txt", "r");


        if(fileData == NULL){
            printf("Hubo un error al abrir el archivo.");
        }
        else{

            int dataSize = getDataSize(fileData);  

            printf("Tamaño del archivo %d\n", dataSize);

            // Variable donde se guardan los datos.

            char fileContent[dataSize];
            fflush(stdin);

            // Se recorre el archvio para obtener los caracteres.

            for(int i = 0; i<=dataSize-2; i++){

                char character = fgetc(fileData);
                fileContent[i] = character;

            }

            fclose(fileData);

            fileContent[dataSize-1] = EOF;

            // Mostrar lo que hay en el archivo.

            printf("%s\n", fileContent);

            // Lectura de tokens.

            readTokens(fileContent);

        }


    }
    else{

        printf("No se ha cargado ningún archivo.");

    }

    
    return 0;
}
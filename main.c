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

// Verifica los limitadores.

bool esLimitador(char c){

   if(c == ' ' || c == ';' || c == '[' || c == ']' || c == '(' || c == ')' || c == '\n' || c == EOF || c == '\t' ||
       c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' || c == '=' || c == '|' || c == '&' ||
       c == '{' || c == '}'){

        return true;

    }

    return false;

}

// Verifica si hay una palabra reservada.

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

// Verificar booleanos.

bool isBoolean(char* str, struct Token *token){

    if(strcmp(str, "verdadero") == 0 || strcmp(str, "falso") == 0){

        strcpy(token->lexema, str);
        token->type = boolean;
        strcpy(token->nombre, str);
        token->value = 0;
        
        return true;

    }

    return false;

}

// Verificar los identificadores.

bool isIdentifier(char* str, struct Token *token){

    if(str[0] >= 'a' && str[0] <= 'z'){

        int i = 1;
        bool aux;

        while(i >= strlen(str) - 1){

            char c = str[i];

            if(!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_')){

                return false;

            }

            i++;

        }

        strcpy(token->lexema, str);
        token->type = id;
        strcpy(token->nombre, "Identificador");
        token->value = 0;

        return true;


    }

    return false;

}


// Verificar si existe un número.

bool isNumber(char* str, struct Token *token){

    int i = 0;
    int point = 0;

    if(str[0] >= '0' && str[0] <= '9'){

        while(i <= strlen(str) - 1){

            if(str[i] == '.'){

                point++;

            }

            if(!(str[i] == '.' || (str[i] >= '0' && str[i] <= '9')) && point == 0){

                return false;

            }

            i++;

        }

        strcpy(token->lexema, str);
        token->type = number;
        strcpy(token->nombre, "Constante");
        token->value = atof(str);
        
        return true;

    }

    return false;

}

// Verificar operadores.

bool operator(char c, struct Token *token){

    if(c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '>' || c == '<' || c == '>=' || c == '<=' || c == '!='){

        memset(token->lexema, 0, sizeof(token->lexema));

        token->lexema[0] = c;
        token->value = 0;
        token->type = symbol;

        switch (c){
            
            case '+':{
                strcpy(token->nombre, "Mas");
            }
            case '-':{
                strcpy(token->nombre, "Menos");
            }
            case '*':{
                strcpy(token->nombre, "Por");
            }
            case '/':{
                strcpy(token->nombre, "Entre");
            }
            case '=':{
                strcpy(token->nombre, "Igual");
            }
            case '>':{
                strcpy(token->nombre, "Mayor");
            }
            case '<':{
                strcpy(token->nombre, "Menor");
            }
            case '>=':{
                strcpy(token->nombre, "Mayor igual");
            }
            case '<=':{
                strcpy(token->nombre, "Menor igual");
            }
            case '!=':{
                strcpy(token->nombre, "Diferente");
            }
            default:
                break;
        }

        return true;

    }

    return false;

}


// Verificar los terminadores de línea.

bool isEndLine(char c, struct Token *token){

    if(c == ';' || c == '[' || c == ']' || c == '(' || c == ')' || c == '"'|| c == '|'|| c == '&' || c == '{' || c == '}'){

        memset(token->lexema, 0, sizeof(token->lexema));
        token->lexema[0] = c;
        token->type = symbol;
        token->value = 0;

        switch (c)
        {
            case ';':{
                strcpy(token->nombre, "PuntoyComa");
            }
            case '[':{
                strcpy(token->nombre, "corcheteIzquierdo");
                break;
            }
            case ']':{
                strcpy(token->nombre, "corcheteDerecho");
                break;
            }
            case '(':{
                strcpy(token->nombre, "parentesisIzquierdo");
                break;
            }
            case ')':{
                strcpy(token->nombre, "parentesisDerecho");
                break;
            }
            case '"':{
                strcpy(token->nombre, "comillaDoble");
                break;
            }
            case '|':{
                strcpy(token->nombre, "OR");
                break;
            }case '&':{
                strcpy(token->nombre, "AND");
                break;
            }case '{':{
                strcpy(token->nombre, "llaveIzquierda");
                break;
            }case '}':{
                strcpy(token->nombre, "llaveDerecha");
                break;
            }
            default:
                break;
        }

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
                cleanTokenList(token);

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
                        cleanTokenList(token);

                    }
                    else if(isReservedWord(aux, &token)){
                        
                        token.columnNumber = columna;
                        token.lineNumbers = lines;
                        insertToken(token);
                        cleanTokenList(token);

                    }
                    else if(isBoolean(aux, &token)){

                        token.columnNumber = columna;
                        token.lineNumbers = lines;
                        insertToken(token);
                        cleanTokenList(token);

                    }
                    else if(isIdentifier(aux, &token)){

                            token.columnNumber = columna;
                            token.lineNumbers = lines;
                            insertToken(token);
                            cleanTokenList(token);

                    }
                    else if(isNumber(aux, &token)){

                        token.columnNumber = columna;
                        token.lineNumbers = lines;
                        insertToken(token);
                        cleanTokenList(token);

                    }
                    else if(aux[9] == '"'){

                        token.type = errorType;
                        strcpy(token.nombre, "Error comilla");
                        token.value = 0;
                        strcpy(token.lexema, "Error comilla");
                        token.columnNumber = columna;
                        token.lineNumbers = lines;
                        insertToken(token);
                        cleanTokenList(token);

                    }
                    else{

                        token.type = errorType;
                        strcpy(token.nombre, "SYNTAX ERROR");
                        token.value = 0;
                        strcpy(token.lexema, "SYNTAX ERROR");
                        token.columnNumber = columna;
                        token.lineNumbers = lines;
                        insertToken(token);
                        cleanTokenList(token);

                    }

                    memset(aux, 9, sizeof(aux));
                    y = 0;
                    continue;

                }
                else{

                    if(operator(source[x], &token)){

                        token.columnNumber = columna;
                        token.lineNumbers = lines;
                        insertToken(token);
                        cleanTokenList(token);

                    }
                    else if(isEndLine(source[x], &token)){

                        if(source[x] == '\n'){

                            lines++;
                            columna = 0;

                        }
                        
                        token.columnNumber = columna;
                        token.lineNumbers = lines;
                        insertToken(token);
                        cleanTokenList(token);

                    }

                }


            }
            else{

                if(source[x] == '"'){

                    cad = true;
                    cadAuxiliar++;

                }

                aux[y] = source[x];
                y++;

            }

        }

        columna++;
        x++;

    }
    if(cad == true){

        token.type = errorType;
        strcpy(token.nombre, "Errro comilla");
        token.value = 0;
        strcpy(token.lexema, "Error comilla");
        token.columnNumber = columna;
        token.lineNumbers = lines;
        insertToken(token);
        cleanTokenList(token);

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
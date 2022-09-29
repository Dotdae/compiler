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

struct Nodo *raiz = NULL;
struct Nodo *act = NULL;

// Limpiar tokens.  


#define MAX 650 // Constante de tokens.
#define MAX_RESWORDS 18 // Constante de palabras reservadas.

FILE *fileData;
char * palabrasReservadas[MAX_RESWORDS] = {"paquete", "funct", "inicio", "var", "ent", "cad", "flot", "bool", "verdadero",
"falso", "const", "si", "o", "entonces", "para", "en rango", "mostrar", "introducir"};

// Obtener el tamaño del archivo.

int getDataSize(FILE *file){

    size_t position = ftell(file);
    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    fseek(file, position, SEEK_SET);

    return length;

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

        }


    }
    else{

        printf("No se ha cargado ningún archivo.");

    }

    
    return 0;
}
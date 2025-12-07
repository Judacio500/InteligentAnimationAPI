#ifndef ANI_H
#define ANI_H

#include<stdio.h>
#include "list.h" 
#include "graph.h" 
#include "hash.h"  

/*
    Para el desarrollo de esta API se tiene el siguiente planteamiento:

    La inspiracion de esta libreria vino a partir del modulo de animacion de matplotlib

    Una animacion es un tipo de dato interpretado por la API, tipo de dato que guarda
    todas las escenas que se deben mostrar, el contexto en el que se muestran
    las acomoda en profundidad, como, donde y cuando se dibujan
    en otras palabras, es el corazon de la libreria y lo que la hace utilizable y generica
    para imprimir algo en esta libreria se debe generar un tipo ANI (struct animation)
    pero de eso hablaremos mas tarde

    La jerarquia de las estructuras

    El tipo "struct animation" es la punta del ice berg, es el contenedor que recibe
    el usuario al generar una animacion y lo que reciben las funciones de impresion 
    de la libreria para mostrar las animaciones y tal vez generar gifs con ellas ()

    Animation
    |->Panel
       |->Layer
          |->Objects
          |->Scene

    El panel es la unidad de impresion de la libreria y de manera secuencial (COLAS)
    recibe la nueva posicion o contexto que debe dibujar conforme pasan los paneles
    como cada cosa en un panel se comporta distinto su dibujado tambien implica
    distintos parametros por eso...

    Tenemos la estructura layer, que define el comportamiento de un grupo de objetos en el panel
    puede o no definir una profundidad global para los objetos asi se podria definir un background por ejemplo

    Cada capa contiene:
    
    Scene: 
    Que simplemente es el cuadro a dibujar es decir "En este panel, donde esta mi camara y a donde apunta?"

    Lista de...
    Object:
    Un elemento a dibujar en el panel formado por figuras geometricas fundamentales

*/

typedef int (*Draw)(struct list *figures);  // El usuario debe definir sus draw para cada estado de su automata
typedef int (*Check)(struct object *self, void *environment);
typedef void (*Behavior)(struct object *self, int step, void *params, void *env);

typedef enum figures
{
    TRIANGLE,
    SQUARE,
    POLYGON,
    LINE,
    CIRCLE,
    OVAL
}FIG;

typedef enum animationStatus
{
    PAUSE,      // Animacion en pausa
    PLAYING,    // Animacion corriendo
    STEPS       // Animacion por pasos (permite explorarla con las flechas del teclado)
}aniStat;

typedef enum animationCycle 
{
    ALIVE,      // Sigue animándose normalmente
    FROZEN,     // Se queda quieto en su último frame dibujado
    HIDDEN,     // Deja de dibujarse 
    LOOP        // Reinicia su ciclo, para acciones rutinarias, como 4 frames de caminata/correr/saltar
} LIFE;

typedef struct animation
{
    enum animationStatus AS;
    float speed;            // Velocidad de reproduccion de la animación
    struct queue *panels;   // Cola de paneles a dibujar 
}ANI;

typedef struct panel
{
    struct hash *layers;    // Capas en este panel
}PANEL;

typedef struct layer
{
    struct scene *currentScene; // Cuadro a dibujar
    struct hash *objects;       // Lo que dibujamos
}LAYER;

typedef struct scene
{
    float width;                // lo que vemos en x
    float height;               // lo que vemos en y
                                // al definirlo como width y height
                                // nos ahorramos definir de una forma para 2D y otra para 3D
                                // ya que el 3D es calculable considerando un cono de vision
                                // Aunque lo mas probable es que no implemente 3D para esta entrega
}SCENE;

typedef struct object
{   
    char key[30];                    // clave hash del objeto
    char layerKey[30];               // Capa en la que se debe dibujar 
    struct transform *t;             // calculo de posicion del objeto
    struct list *figures;            // Componentes a dibujar para formar el objeto 
    struct graph *bluePrint;         // Conciencia de nuestro objeto, todas las posibilidades del objeto en el mundo
    struct node *currentStatus;      // donde estoy en el mundo? como me veo respecto a mi posicion?
                                     // el estado de nuestro automata
    Behavior brain;                  // Es la forma en la que se comporta nuestro automata
    enum animationCycle status;      // Auxiliar para la generacion de animaciones
}OBJECT;

typedef struct transform
{
    struct coordinates  *globalPos;  // La posicion global del objeto en el mundo
    float scaleX,scaleY,scaleZ;     // La escala de z solo es relevante cuando se anima en 3D, al animar en 2D
                                    // la funcion la ignorara porque puede romper la animación
    float rotX,rotY,rotZ;           // Las transformaciones rotacionales de nuestro objeto son posibles
                                    // esto nos permitiria mostrar en la animacion el perfil de algo
                                    // sin la necesidad de definir como se dibuja, aunque puede no verse bien
                                    // dependiendo del formato de dibujado
    struct fig *colissionBox;       // Colision del objeto por si hay interaccion en la animacion
                                    // vive aqui para que las cajas de colision tambien escalen con el objeto
                                    // como ambos vienen de figuras que se calculan con offset sera un escalado perfecto
}TRANSFORM;

typedef struct trigger 
{
    Check check;                // El automata no sabe que acciones se deben tomar en un prefabricado
                                // pero se le puede decir de que estar pendiente
    char *targetStatusKey;      // Como nuestro grafo tiene un hash un trigger puede estar siempre ligado
                                // a un estado distinto de dibujado
} TRIGGER;

typedef struct fig
{
    struct list *offset; // Puntos a dibujar de la figura calculados mediante un offset
                         // definido por el struct coordenada     
    enum figures f;      // Figura a dibujar;
}F;

typedef struct coordinates
{
    float x, y, z;
}CORD;

#endif
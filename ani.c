#include "ani.h"

// Creacion de estructuras

/*

Esta seccion no esta comentada en su mayoria porque las decisiones del diseño ya estan en el .h
y la logica de estas funciones es sencilla, agarramos una bolsa del supermercado del tamaño que quieramos
metemos lo que vamos a comprar para esa bolsa y cerramos la bolsa

Basicamente...

Damos memoria a la estructura
Inicializamos con valores base y aprueba de crash

Esta seccion aunque simple se asegura de que si algo sale mal, nuestra libreria tenga las herramientas para hacer
el handling correcto

*/

COORD *initCoord(float x, float y, float z)
{
    COORD *newC = (COORD*)malloc(sizeof(COORD));

    if(!newC)
        return NULL;
    
    newC->x = x;
    newC->y = y;
    newC->z = z;

    return newC;
}

F *initFigure(LIST *pointOffSet, COORD *localPosition, COORD *localRotation, enum figures f)
{
    F *newF = (F*)malloc(sizeof(F));
    
    if(!newF)
        return NULL;

    newF->offSet = pointOffSet;
    newF->f = f;

    if (localPosition)
    {
        newF->relPos = localPosition;
    }
    else
    {
        newF->relPos = initCoord(0, 0, 0);
        if(!newF->relPos)
        {
            free(newF);
            return NULL;
        }
    }

    if (localRotation)
    {
        newF->relRot = localRotation;
    }
    else
    {
        newF->relRot = initCoord(0, 0, 0);
        if(!newF->relRot)
        {
            free(newF->relPos);
            free(newF);
            return NULL;
        }
    }

    return newF;
}

TRIGGER *initTrigger(Check check, char *targetStatusKey)
{
    TRIGGER *newT = (TRIGGER*)malloc(sizeof(TRIGGER));
    
    if(!newT)
        return NULL;

    newT->check = check;
    
    if(targetStatusKey)
        newT->targetStatusKey = strdup(targetStatusKey);
    else
        newT->targetStatusKey = NULL;

    return newT;
}

TRANSFORM *initPhysics(F *colision, COORD *pos, COORD *scale, COORD *rotation)
{
    TRANSFORM *newT = (TRANSFORM*)malloc(sizeof(TRANSFORM));
    
    if(!newT)
        return NULL;

    newT->globalPos = pos;
    newT->scale = scale;
    newT->rotation = rotation;
    
    newT->colissionBox = colision;

    return newT;
}

OBJECT *initObject(char *objectName, char *objectLayer, TRANSFORM *initial, LIST *figures, GRAPH *bluePrint, Behavior brain)
{
    OBJECT *newO = (OBJECT*)malloc(sizeof(OBJECT));

    if(!newO)
        return NULL;

    if(objectName)
        strncpy(newO->key, objectName, 29);
    else
        strncpy(newO->key, "unnamed", 29);
    newO->key[29] = '\0'; 

    if(objectLayer)
        strncpy(newO->layerKey, objectLayer, 29);
    else
        strncpy(newO->layerKey, "default", 29);
    newO->layerKey[29] = '\0';

    if(!initial)
    {
        COORD *defPos   = initCoord(0, 0, 0);
        COORD *defScale = initCoord(1, 1, 1);
        COORD *defRot   = initCoord(0, 0, 0);

        if(!defPos || !defScale || !defRot)
        {
            free(newO);
            if(defPos) 
                free(defPos);
            if(defScale) 
                free(defScale);
            if(defRot)
                free(defRot);
            return NULL; 
        }

        newO->t = initPhysics(NULL, defPos, defScale, defRot); 

        if(!newO->t)
        {
            free(newO);
            free(defPos);
            free(defScale);
            free(defRot);
            return NULL;
        }
    }
    else
        newO->t = initial;
    
    if(!newO->t)
    {
        free(newO);
        return NULL;
    }

    newO->figures = figures;
    newO->bluePrint = bluePrint;
    newO->currentStatus = NULL; 
    newO->brain = brain;
    newO->status = ALIVE; 

    return newO;
}

SCENE *initScene(float width, float height)
{
    SCENE *newS = (SCENE*)malloc(sizeof(SCENE));
    if(!newS)
        return NULL;

    newS->width = width;
    newS->height = height;

    return newS;
}

LAYER *initLayer(char *layerName)
{
    LAYER *newL = (LAYER*)malloc(sizeof(LAYER));
    if(!newL)
        return NULL;

    if(layerName)
        strncpy(newL->layerName, layerName, 29);
    else
        strncpy(newL->layerName, "unnamed_layer", 29);
    newL->layerName[29] = '\0';

    newL->objects = initHash(0); 

    return newL;
}

PANEL *initPanel(SCENE *camera)
{
    PANEL *newP = (PANEL*)malloc(sizeof(PANEL));
    if(!newP)
        return NULL;

    newP->currentScene = camera;
    newP->layers = initHash(0);
    LAYER *backGround = initLayer("BACKGROUND");
    
    if(backGround)
        saveKey(&newP->layers, backGround->layerName, backGround);
    else
    {
        printf("Error initializing background layer");
        return NULL; // No se puede inicializar un panel sin capas es como querer caminar sobre nubes
    }

    return newP;
}

ANI *initAnimation()
{
    ANI *newA = (ANI*)malloc(sizeof(ANI));
    if(!newA)
        return NULL;

    newA->AS = PAUSE;   
    newA->speed = 1.0f; 
    newA->panels = createWrap(); 

    return newA;
}


// Insercion de estructuras
/*
    Estos serian los dummys de insercion para usuario/funciones "inteligentes" AKA Automatas

    Sirven para ensamblar la animacion manualmente especificando cada lista de objetos en un panel
    Cada capa y cada comportamiento 

    O para recibir las instrucciones de insercion de nuestras funciones inteligentes

*/

int addPanel(ANI *animation, PANEL *p)  // Esta funcion corresponde a la punta del iceberg
{                                       // Cuando un panel esta terminado lo añade a la cola de la animacion 
                                        // permitiendo siempre insertar al final de la animacion
    if(!animation || !p)
        return -1; // Panel no añadido

    return handleAppend(&animation->panels,p,1,DOUBLE); // Insercion en lista doblemente enlazada
}

// Añadir una capa a un panel existente
int addLayer(PANEL *p, LAYER *l)
{
    if(!p || !l) 
        return -1;
    
    return saveKey(&p->layers, l->layerName, l); 
}

// Añadir un objeto a una capa específica
int addObject(PANEL *l, OBJECT *o)
{
    if(!l || !o) 
        return -1;

    return saveKey(&l->objects, o->key, o);
}

// Añadir figura a un objeto
int addFigure(OBJECT *o, F *f)
{
    if(!o || !f)
        return -1;

    return handleInsert(o->figures, f, 0, SIMPLE);
}

/*

    Estas dos funciones son similares porque a diferencia de los paneles
    que son secuenciales, son muchos y son mas dificiles de describir

    Los objetos y las capas de un panel son contados, son especificos del panel
    y son unicos y relevantes para el contexto de la animacion

    por eso su implementacion esta hecha mediante hash, de esta forma
    se pueden buscar y modificar por como fueron insertados

*/

int addColission(OBJECT *o, F *colissionBox)
{
    if(!o || !colissionBox)
        return -1;
    
    o->t->colissionBox = colissionBox;

    return 0;
}

LIST *rectangleOffSet(float width, float length)
{
    LIST *offSet = NULL;
    float halfX = length / 2.0f;
    float halfY = width / 2.0f;

    // Vertice 1: Esquina superior izquierda
    handleInsert(&offSet, initCoord(-halfX, halfY, 0), 0, SIMPLE);

    // Vertice 2: Esquina superior derecha
    handleInsert(&offSet, initCoord(halfX, halfY, 0), 0, SIMPLE);
    
    // Vertice 3: Esquina inferior derecha
    handleInsert(&offSet, initCoord(halfX, -halfY, 0), 0, SIMPLE);
    
    // Vertice 4: Esquina inferior izquierda
    handleInsert(&offSet, initCoord(-halfX, -halfY, 0), 0, SIMPLE);

    return offSet;
}

LIST *polygonOffSet(int segments, float radius)
{
    LIST *offSet = NULL;

    if (segments < 3) segments = 3;

    float angleStep = (2.0f * M_PI) / segments;

    for(int i = 0; i < segments; i++)
    {
        float theta = i * angleStep;
        // Calculamos offSet (eliminado localX/Y para centrar en 0,0)
        float x = cosf(theta) * radius; 
        float y = sinf(theta) * radius;
        
        handleInsert(&offSet, initCoord(x, y, 0), 0, SIMPLE);
    }

    return offSet;
}

LIST *circleOffSet(int smoothness, float radius)
{
    LIST *offSet = NULL;

    if(smoothness <= 0)
        return NULL;

    int segments = smoothness * 70; // Tu logica de suavizado

    float angleStep = (2.0f * M_PI) / segments;

    for(int i = 0; i < segments; i++)
    {
        float theta = i * angleStep;
        // Calculamos offSet (eliminado localX/Y para centrar en 0,0)
        float x = cosf(theta) * radius; 
        float y = sinf(theta) * radius;
        
        handleInsert(&offSet, initCoord(x, y, 0), 0, SIMPLE);
    }

    return offSet;
}

LIST *lineOffSet(float length)
{
    LIST *offSet = NULL;
    float half = length / 2.0f;

    // Linea horizontal unitaria (Inicio)
    handleInsert(&offSet, initCoord(-half, 0, 0), 0, SIMPLE);

    // Linea horizontal unitaria (Fin)
    handleInsert(&offSet, initCoord(half, 0, 0), 0, SIMPLE);

    return offSet;
}

LIST *triangleOffSet(float base, float height)
{
    LIST *offSet = NULL;
    
    float halfBase = base / 2.0f;
    float halfHeight = height / 2.0f;

    // Vertice 1: Punta superior (Centrada)
    handleInsert(&offSet, initCoord(0, halfHeight, 0), 0, SIMPLE);

    // Vertice 2: Esquina inferior izquierda
    handleInsert(&offSet, initCoord(-halfBase, -halfHeight, 0), 0, SIMPLE);
    
    // Vertice 3: Esquina inferior derecha
    handleInsert(&offSet, initCoord(halfBase, -halfHeight, 0), 0, SIMPLE);

    return offSet;
}

F *generateFigure(enum figures figType, float arg1, float arg2, float localX, float localY, float zPriority, float rotX, float rotY, float rotZ)
{
    LIST *offSet = getOffSet(figType, arg1, arg2);
    
    if(!offSet)
        return NULL;

    COORD *pos = initCoord(localX, localY, zPriority);

    if(!pos)
        return NULL;

    COORD *rot = initCoord(rotX, rotY, rotZ);

    if(!rot)
    {
        free(pos);
        freeList(&offSet,free);
        return NULL;
    }

    F *newF = initFigure(offSet,pos,rot,f);

    return newF;
}

F *generateColission(enum figures figType, arg1,  arg2)
{
    LIST *offSet = getOffSet(figType, arg1, arg2);

    if(!offSet)
        return NULL;

    COORD *pos = initCoord(0, 0, 0);

    if(!pos)
        return NULL;

    COORD *rot = initCoord(0, 0, 0);

    if(!rot)
    {
        free(pos);
        freeList(&offSet,free);
        return NULL;
    }

    F *newF = initFigure(offSet,pos,rot,f);

    return newF;
}

LIST *getOffSet(enum figures figType, float arg1, float arg2)
{
    switch(figType)
    {
        case TRIANGLE:
            return triangleOffSet(arg1, arg2);
        case RECTANGLE:
            return rectangleOffSet(arg1, arg2);
        case POLYGON:
            return polygonOffSet(arg1, arg2);
        case LINE:
            return lineOffSet(arg1);
        case CIRCLE:
            return circleOffSet(arg1, arg2);
        case OVAL:
            // Not yet implemented
            return NULL;
            //break;
        default:
            return NULL;
    }
}

/*

Funciones de construccion de secuencias de animacion para un objeto, construccion de objetos 

*/


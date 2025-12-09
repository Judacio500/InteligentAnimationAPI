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

OBJECT *initObject(char *objectName, char *layerName, TRANSFORM *initial, LIST *figures)
{
    OBJECT *newO = (OBJECT*)malloc(sizeof(OBJECT));

    if(!newO)
        return NULL;

    if(objectName)
        strncpy(newO->key, objectName, 29);
    else
        strncpy(newO->key, "unnamed", 29);
    newO->key[29] = '\0'; 

    if(objectName)
        strncpy(newO->layerKey, layerName, 29);
    else
        strncpy(newO->layerKey, "unnamed", 29);
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
    newO->statusStack = NULL;  // La pila de estados esta vacia 
    newO->activeStatus = NULL;
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

LAYER *initLayer(char *layerName, Behavior initialBehavior)
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

    newL->initialBehavior =  !initialBehavior ? getIdle() : initialBehavior;    // Si se define un estado inicial se aplica
                                                                                // si no se le da el estado Idle

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

int addObject(LAYER *l, OBJECT *o)
{
    if(!l || !o) 
        return -1;

    return saveKey(&l->objects, o->key, o); 
}

int addLayer(PANEL *p, LAYER *l)
{
    if(!p || !l) 
        return -1;
    
        return saveKey(&p->layers, l->layerName, l);
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
    if(!o || !o->t) 
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



STATUS *generateStatus(Behavior func, struct graph *animationSequence, void *params)
{
    STATUS *newStatus = (STATUS*)malloc(sizeof(STATUS));

    if(!newStatus)
        return NULL;

    newStatus->func = func;
    newStatus->animSequence = animationSequence;
    newStatus->params = params;

    return newStatus;
}

int pushFrame(QUEUE *sequence, OBJECT *frameObj)
{
    if(!sequence || !frameObj)
        return -1;
    
    return handleAppend(&sequence, frameObj, 1.0f, SIMPLE);
}

int pushFigure(LIST **figureList, F *fig)
{
    if(!fig)
        return -1;

    return handleInsert(figureList, fig, 0, SIMPLE);
}

PANEL *generatePanelFromObjects(SCENE *camera, LIST *objects)
{
    PANEL *newP = initPanel(camera);
    if(!newP) return NULL;

    LIST *current = objects;
    while(current)
    {
        OBJECT *obj = (OBJECT*)current->data;
        
        LAYER *targetLayer = NULL;
        EHASH *found = hashing(newP->layers, obj->layerKey);

        if(found)
        {
            targetLayer = (LAYER*)found->pair;
        }
        else
        {
            targetLayer = initLayer(obj->layerKey);
            if(addLayer(newP, targetLayer) < 0)
            {
                return NULL;
            }
        }

        addObject(targetLayer, obj);

        current = current->next;
    }

    return newP;
}

GRAPH *generateBluePrint(char *sequenceName, QUEUE *objectSequence, int type)
{
    if(!objectSequence || !objectSequence->first)
        return NULL;

    GRAPH *newSequence = createGraph(sequenceName, NULL);
    if(!newSequence) 
        return NULL;

    LIST *iter = objectSequence->first;
    int index = 0;
    char currentKey[50];

    NODE *prevNode = NULL, *firstNode = NULL;

    while(iter)
    {
        sprintf(currentKey, "%s_%d", sequenceName, index);
        
        void *currentFrameObj = iter->data;

        NODE *newNode = addNode(newSequence, currentKey, currentFrameObj); 

        if(!newNode)
            return NULL;

        if(index == 0)
            firstNode = newNode;

        if(prevNode)
        {
            addEdgeThrough(prevNode, newNode, 1.0f, 1); //
        }

        prevNode = newNode;
        iter = iter->next;
        index++;
    }

    if(type == 1 && prevNode && firstNode)
    {
        addEdgeThrough(prevNode, firstNode, 1.0f, 1);
    }

    return newSequence;
}

OBJECT *instanceObject(char *objectName, char *layerName, TRANSFORM *initial, LIST *figures, GRAPH *bluePrint)
{
    /*
        Asi es, el bluePrint va aqui

        que nada te detenga de ponerle la animacion de un pollo asado bailando a una persona :D
    
        AKA Libertad creativa

    */

    OBJECT *newObj = initObject(objectName, layerName, initial, figures);
    
    if(!newObj) return NULL;

    // Cargar el estado base 
    STATUS *idleSt = getIdle();
    if(idleSt)
    {
        // Insertamos el IDLE en la pila (Bottom)
        handleInsert(&newObj->statusStack, idleSt, 0, SIMPLE);
        newObj->activeStatus = idleSt;

        if(bluePrint)
            newObj->avtiveStatus->animSequence = bluePrint;

    }

    return newObj;
}

// Instanciador del comportamiento inicial IDLE
// sin secuencia de dibujo ni parametros extra
// (el usuario puede cambiar el IDLE de un objeto especifico si asi lo quiere)
STATUS *getIdle()
{
    return generateStatus(idle,NULL,NULL);
}

void idle(struct object *self, int step, void *params, void *env); // Solo declarada no definida

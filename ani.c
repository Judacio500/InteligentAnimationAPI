#include "ani.h"

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

F *initFigure(LIST *pointOffSet, enum figures f)
{
    F *newF = (F*)malloc(sizeof(F));
    
    if(!newF)
        return NULL;

    newF->offset = pointOffSet;
    newF->f = f;

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
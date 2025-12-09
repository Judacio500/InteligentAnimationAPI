## 🟦 ANI — Motor de Animación 2D (C)

#### Proyecto unificado con Introduccion a la Inteligencia Artificial
#### Por lo siguiente se garantiza que el proyecto actual constituye el +70% del proyecto:

ANI es un motor de animación 2D modular inspirado en `matplotlib.animation`, diseñado desde cero en C.  
El sistema se organiza en **Paneles → Capas → Objetos → Figuras**, con una arquitectura totalmente extensible.

### 🔹 Componentes principales
- **Figuras primitivas:** triángulos, rectángulos, líneas, círculos, polígonos.  
- **Transformaciones:** posición global, escala, rotación, área de efecto, hitbox.  
- **Objetos:** múltiples figuras, estados, comportamientos, colisiones y parámetros.  
- **Capas:** colecciones ordenadas de objetos con lógica compartida.  
- **Paneles:** agrupan capas y la escena visible (cámara).  
- **Animación:** control de reproducción (play/pause/step) y cola de paneles.

### 🔹 Automatización y estados
- Máquina de estados integrada (`Idle`, `Static`, `Walk`, `Jump`, `Fall`).  
- **Triggers** para transiciones basadas en condiciones.  
- **Física básica:** gravedad, fricción, detección de suelo, actualización de transformaciones.

### 🔹 Enfoque
- API genérica y extensible.  
- Separación clara entre lógica, física, animación y estructura visual.  
- Diseñada para crecer hacia animación más avanzada o integración con motores gráficos.

Este módulo ya permite definir objetos, aplicar física, gestionar estados, y construir paneles animados por pasos o reproducción continua.

## Lo que falta

### Animaciones inteligentes
- El modulo actual esta casi listo para implementar triggers especificos a un cerebro solo falta implementar la funcion

### Cambio de animacion
- Se necesita implementar el pareado de Grafos de animacion con la pila de comportamientos

sentinel-dispatcher - Orquestador del Enjambre de Centinelas (The Matrix)

sentinel-dispatcher es una implementación en lenguaje C bajo estándares de Linux (POSIX) de un motor de 
planificación por tics de reloj discreto que administra el flujo y la ejecución de tareas concurrentes 

Características Principales

Esquema de Colas Multinivel Apropiativas: Tres niveles de prioridad estricta:

Cola 0 (Comando): Alta prioridad con Round Robin ($Q_0 = 2$ tics) y prioridad absoluta.   
Cola 1 (Perforadores): Prioridad media con Round Robin ($Q_1 = 4$ tics).   
Cola 2 (Desgaste): Baja prioridad operando bajo un esquema FCFS puro.   

Arquitectura Multihilo Síncrona: Separación física de responsabilidades entre un Hilo Maestro (controlador del reloj y admisiones de hardware) y un Hilo Trabajador (núcleo físico CPU_0).   

Expropiación y Rotación Determinista: Control automático de desalojos ante llegadas a colas superiores, expiración de quantums y gestión de estados de ejecución (NEW, READY, RUNNING, INTERRUPTED, FINISHED). 

Métricas de Rendimiento Globales: Cálculo automatizado de Throughput (centinelas por tic) y Tiempo de Espera Promedio al finalizar la simulación.

Modos de Ejecución Flexibles: Soporte para Modo Estándar (hitos operativos y métricas de rendimiento) y Modo Depuración / Verbose (-v) para trazas detalladas del estado interno de las colas en cada tic.   

Arquitectura del Sistema

main.c: Punto de entrada del programa. Administra el análisis de argumentos por consola (CLI) e inicia el ciclo de vida de la simulación.   
parser.c / parser.h: Lector y analizador sintáctico del archivo de entrada. Descarta comentarios, procesa las líneas del escenario de ataque y construye los bloques TCB iniciales.   
tcb.c / tcb.h: Definición e inicialización del Bloque de Control de Tarea (TCB), la estructura de datos que almacena el estado y las métricas de cada centinela.   
queue.c / queue.h: Implementación de la estructura de datos de Cola Enlazada Dinámica y sus operaciones (enqueue, enqueueFront, dequeue, printQueue).   
scheduler.c / scheduler.h: Motor principal del planificador. Maneja las tres colas de prioridad, evalúa vencimientos de quantum, expropiaciones por prioridad y despacha centinelas a la CPU.   
threads.c / threads.h: Administrador de hilos y rutinas independientes para el Hilo Maestro (admisión de llegadas) y el Hilo Trabajador (procesamiento de ráfagas).   
sync.c / sync.h: Motor de sincronización basado en un cerrojo (pthread_mutex_t) y variables de condición (pthread_cond_t) para mantener la barrera de fase síncrona tic a tic.   
logger.c / logger.h: Gestor de salidas del sistema. Encargado de imprimir la configuración de parámetros y dirigir reportes de errores hacia la salida estándar de errores (stderr).  
metrics.c / metrics.h: Módulo de procesamiento estadístico. Calcula e imprime el rendimiento global del sistema al terminar la ejecución.

Requisitos Previos

Sistema Operativo GNU/Linux (o WSL compatible con estándares POSIX).   
Compilador GCC (compatible con C99 o superior).   
GNU Make (Makefile obligatorio para la compilación del proyecto).   
Paquetes de depuración recomendados: libc6-dbg y valgrind.  

Compilación y Ejecución

make
make clean
./sentinel-dispatcher --input inputs/sion_attack.txt (Estándar)
./sentinel-dispatcher -v --input inputs/sion_attack.txt (Verbose)
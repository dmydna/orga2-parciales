
// sched.c


static sched_entry_t sched_tasks[MAX_TASKS] = {0};
    
int8_t i;
int8_t current_task = 0;
int8_t last_task_priority = 0;
int8_t last_task_no_priority = 0;


uint16_t sched_next_task(void) {


    // Buscamos la próxima tarea viva con prioridad
    for ( i = (last_task_priority + 1);(i % MAX_TASKS) != last_task_priority; i++) {
        if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE && es_prioritaria(i)) {
            break;
        }
    }
    
    // A la salida:
    // - i != last_task_priority: más de una tarea prioritaria
    // - i == last_task_priority == current_task, no quiero repetir
    // - i == last_task_priority != current_task,
    // última no fue prioritaria
    // Ajustamos i para que esté entre 0 y MAX_TASKS-1
    
    i = i % MAX_TASKS;
    if (i != current_task && es_prioritaria(i)) {
        // Hay más de una tarea prioritaria + viva
        // o la última tarea ejecutada fue sin prioridad
        last_task_priority = i;
        current_task = i;
        return sched_tasks[i].selector;
    }

    // Si llegué acá es porque
    // - La última tarea ejecutada fue
    // con prioridad (y hay solo una con prioridad)
    // - o no hay tareas con prioridad
    // - o no hay más tareas vivas
    
    for ( i = (last_task_no_priority + 1);(i % MAX_TASKS) != last_task_no_priority; i++) {
        // Si esta tarea está disponible la ejecutamos
        if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE) {
            break;
        }
    }

    // Si la tarea que encontramos es ejecutable ,
    // entonces la corremos.
    if (sched_tasks[i].state == TASK_RUNNABLE) {
        // Si llegamos acá, la tarea que encontramos
        // no es prioritaria
        last_task_no_priority = i;
        current_task = i;
        return sched_tasks[i].selector;
    }
    // En el peor de los casos no hay ninguna tarea viva.
    // Usemos la idle como selector.
    
    return GDT_IDX_TASK_IDLE << 3;
}



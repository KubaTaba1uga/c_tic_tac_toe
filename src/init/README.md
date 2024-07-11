# Init subsystem

Goal of init subsystem is to perform initialization of each registered file. 
In C I'm often using static data available only in C src file, this subsystem is generic way to initialize such static data.

# Reprioritization

Some subsystems like logging or config should be initialized prior to other systems. For example config during it's initialization
would like to use logging to inform about config's status. I belive it's better to have one subsystem for init with priorities 
than to have two subsystems like pre_init and init. This way mechanism is simpler and create less ambiquity if managed properly.
When repriotizng you need to keep clear indexes of tasks. Do not repriotize multiple tasks to beginning of the queue, instead
use index to hardcode order of operations. For example:

1. This is bad:
```
// logging_utils.c

INIT_REGISTER_SUBSYSTEM_PRIORITY(logging_utils_ops.init_loggers,
                        logging_utils_ops.destroy_loggers, module_id, 0);

// config_utils.c

INIT_REGISTER_SUBSYSTEM_PRIORITY(config_utils_ops.init_loggers,
                        config_utils_ops.destroy_loggers, module_id, 0);
```

2. This is good:
```
// logging_utils.c
INIT_REGISTER_SUBSYSTEM_PRIORITY(logging_utils_ops.init_loggers,
                        logging_utils_ops.destroy_loggers, module_id, 0);

// config_utils.c
INIT_REGISTER_SUBSYSTEM_PRIORITY(config_utils_ops.init_loggers,
                        config_utils_ops.destroy_loggers, module_id, 1);
```

The goal is to keep clean and easy to understand chain of operations.

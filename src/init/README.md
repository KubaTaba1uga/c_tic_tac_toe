# Init subsystem

Goal of init subsystem is to perform initialization of each registered file. 
In C I'm often using static data available only in C src file, this subsystem is generic way to initialize such static data.

# Prioritization

Some subsystems like logging or config should be initialized prior to other systems. For example config during it's initialization
would like to use logging to inform about config's status. I belive it's better to have one subsystem for init with priorities 
than to have two subsystems like pre_init and init. This way mechanism is simpler and create less ambiquity if managed properly.

That's why to register new module in init subsystem you always need to add element to `enum init_module_order_number`. This way 
whole prioritization can be modified easily by modifying enum's order.

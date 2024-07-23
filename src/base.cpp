#define globalVar static
#define localPersist static

#define FOR_RANGE(type, variable_name, max_value_exclusive) \
    for (type variable_name = 0; (variable_name) < (max_value_exclusive); variable_name++)

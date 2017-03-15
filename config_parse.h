#ifndef CONFIG_PARSE_H
#define CONFIG_PARSE_H

#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Config string buffer
 * 
 */
typedef struct config_buffer
{
    char * data;        /** < Pointer to the string buffer containing the */
    size_t capacity;    /** < Length of the buffer */
    size_t length;      /** < Length of the string in the buffer */
} config_buffer_t;


/**
 * @brief Config parse object structure
 *  
 */
typedef struct confg_parse
{
    config_buffer_t variable_name;  /** < Name of the current cariable */
    config_buffer_t value;          /** < Value of the current variable */
    FILE * source;                  /** < File containing the configuration */
    config_buffer_t buffer;         /** < Config buffer */
    size_t buffer_pos;              /** < Position in the buffer */    
} config_parse_t;


/**
 * @brief Initialize configuration parser and load configuration from a file
 * 
 * @param[out] cp       Configuration object to be initialized
 * @param      filename Configuration file
 * @return 0 on success
 */
int config_parse_file_init(
    config_parse_t * cp,
    const char * filename
);

/**
 * @brief Initialize configuration parser and load configuration from a string
 * 
 * @param[out] cp            Configuration object to be initialized
 * @param[in]  config        The configuration string
 * @param      config_length Length of the configuration string
 * @return 0 on success
 */
int config_parse_string_init(
    config_parse_t * cp,
    const char * config,
    size_t config_length
);

/**
 * @brief Destroy config parse object
 * 
 * @param[in,out] cp Config parse object to be destroyed
 */
void config_parse_deinit(
    config_parse_t * cp
);

#endif /* CONFIG_PARSE_H */


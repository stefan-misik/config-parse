#ifndef CONFIG_PARSE_H
#define CONFIG_PARSE_H

#include <stdio.h>

/**
 * @brief Config parse return values
 */
typedef enum config_parse_res
{
    CONFIG_PARSE_OK = 0,        /** < Configuration parsing successfull */
    CONFIG_PARSE_MEMORY_ERROR,  /** < Memory error while processing file */
    CONFIG_PARSE_SYNTAX,        /** < Config file syntax error */
    CONFIG_PARSE_SETENV_ERROR,  /** < Error during setting a environment
                                 *    variable */
    CONFIG_PARSE_FILE_ERROR     /** < Reserved */
} config_parse_res_t;

/**
 * @brief Begin parsing of the passed file
 * 
 * Function parses bash-like variables definitions inside configuration file
 * 
 * @param[in,out] fd File to psrse
 * @param overwrite Overwrite existing environment variables
 * 
 * @return See config_parse_res_t
 */
config_parse_res_t config_parse(
    FILE * fd,
    int overwrite
);


#endif /* CONFIG_PARSE_H */
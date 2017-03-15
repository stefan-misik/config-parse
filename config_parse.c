#include <stdio.h>
#include "config_parse.h"


/******************************************************************************/
/*                               Private                                      */
/******************************************************************************/

/* Buffer capacity of a static buffer */
#define STATIC_BUFFER_CAPACITY ((size_t)-1)

/* Minimal capacity of buffers */
#define CONFIG_BUFFER_MIN_CAPACITY 128

/**
 * @brief Initialize config string buffer
 * 
 * @param[out] buff Buffer object to be initialized
 */
static void config_buffer_init(
    config_buffer_t * buff
)
{
    buff->capacity = 0;
    buff->length = 0;
    buff->data = NULL;
}

/**
 * @brief De-initialize config buffer
 * 
 * @param[in,out] buff Buffer object to be de-initialized
 */
static void config_buffer_deinit(
    config_buffer_t * buff
)
{
    /* Free buffer */
    if(NULL != buff->data)
    {
        free(buff->data);
    }
    
    /* Zero the fields */
    buff->capacity = 0;
    buff->length = 0;
    buff->data = NULL;
}

/**
 * @brief Get the next character of a configuration
 * 
 * @param[in,out] cp Config parse object
 * @return The next character from configuration, or EOF
 */
static int config_parse_getc(
    config_parse_t * cp
)
{
    /* If at the end of the buffer */
    if(cp->buffer_pos >= cp->buffer.length)
    {
        /* If possible read another data from the file */
        if(NULL != cp->source)
        {
            cp->buffer.length = fread(cp->buffer.data, sizeof(char),
                cp->buffer.capacity, cp->source);
            cp->buffer_pos = 0;
            
            /* If no new data were read, return EOF */
            if(0 == cp->buffer.length)
                return EOF;
        }
        else
            return EOF;
    }
    
    /* Get the next character */       
    return cp->buffer.data[cp->buffer_pos ++];
}

/**
 * @brief Initialize config parse object
 * 
 * @param[out] cp Config parse object to be initialized
 */
static void config_parse_init(
    config_parse_t * cp
)
{
    /* Initialize buffers */
    config_buffer_init(&(cp->variable_name));
    config_buffer_init(&(cp->value));
    config_buffer_init(&(cp->buffer));
    
    /* Initialize other fields */
    cp->buffer_pos = 0;
    cp->source = NULL;
}

/******************************************************************************/
/*                                Public                                      */
/******************************************************************************/

/******************************************************************************/
int config_parse_file_init(
    config_parse_t * cp,
    const char * filename
)
{
    /* Do common initialization */
    config_parse_init(cp);

    return -1;
}

/******************************************************************************/
int config_parse_string_init(
    config_parse_t * cp,
    const char * config,
    size_t config_length
)
{
    /* Do common initialization */
    config_parse_init(cp);

    /* Indicate the configuration the buffer is static */
    cp->buffer.capacity = STATIC_BUFFER_CAPACITY;
    /* Load passed string into configuration buffer */
    cp->buffer.length = config_length;
    cp->buffer.data = (char *)config;

    return -1;
}

/******************************************************************************/
void config_parse_deinit(
    config_parse_t * cp
)
{
    /* Destroy buffers */
    config_buffer_deinit(&(cp->variable_name));
    config_buffer_deinit(&(cp->value));

    if(STATIC_BUFFER_CAPACITY != cp->buffer.capacity)
        config_buffer_deinit(&(cp->buffer));

    /* Close config file */
    if(NULL != cp->source)
    {
        fclose(cp->source);
        cp->source = NULL;
    }
}
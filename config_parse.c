#include <stdlib.h>
#include <ctype.h>
#include "config_parse.h"


/******************************************************************************/
/*                               Private                                      */
/******************************************************************************/

/* Minimal length of the string buffer */
#define STRING_BUFFER_MIN_LENGTH 64

/* Configuration format definitions */
#define CONF_COMMENT_CHAR '#'
#define CONF_NEWLINE_CHAR '\n'
#define CONF_EQUALS_CHAR  '='
#define CONF_QUOTE1_CHAR  '"'
#define CONF_QUOTE2_CHAR  '\''
#define CONF_ESCAPE_CHAR  '\\'

/* Get the length of the string */
#define string_buffer_strlen(sb)    \
    ((size_t)((sb)->length))

/**
 * @brief State of the parses
 */
typedef enum parser_state
{
    PARSER_DIVIDER, /** < Inside token divider */
    PARSER_COMMENT, /** < Inside comment */
    PARSER_VAR_NAME,
    PARSER_VAR_VALUE_BASE,
    PARSER_VAR_VALUE_ESCAPE,
    PARSER_VAR_VALUE_QUOTES1,
    PARSER_VAR_VALUE_QUOTES2            
} parser_state_t;

/**
 * @brief Structure containing information on the buffer
 * 
 */
typedef struct string_buffer
{
    size_t capacity;    /** < allocated capacity of the buffer */
    size_t length;      /** < Length of the string */
    char * buffer;      /** < Buffer pointer */
} string_buffer_t;


/**
 * @brief Initialize the string buffer 
 * 
 * @param[out] sb String buffer object to initialize
 * @return 0 on success
 */
int string_buffer_init(
    string_buffer_t * sb
)
{
    /* Allocate the buffer */
    sb->buffer = (char *)malloc(sizeof(char) * STRING_BUFFER_MIN_LENGTH);
    sb->length = 0;
    
    /* Set the capacity end return accordingly */
    if(NULL != sb->buffer)
    {
        sb->capacity = STRING_BUFFER_MIN_LENGTH;
        return 0;
    }
    else
    {
        sb->capacity = 0;
        return -1;
    }
}

/**
 * @brief De-initialize the string buffer
 * 
 * @param[in] sb String buffer object to de-initialize
 */
void string_buffer_deinit(
    string_buffer_t * sb
)
{
    /* Free the buffer */
    if(NULL != sb->buffer)
    {
        free(sb->buffer);
        sb->buffer = NULL;
    }
}

/**
 * @brief Ensure there is enough space in the buffer for the string of specified
 *        length
 * 
 * @param[in,out] sb String buffer object
 * @param length Length of the string
 * @return 0 on success
 */
int string_buffer_ensure(
    string_buffer_t * sb,
    size_t length
)
{
    size_t required;
    
    /* Calculate required size */
    required = sb->length + length + 1;
    /* Check the buffer capacity */
    if(required > sb->capacity)
    {
        size_t new_capacity;
        void * new_buffer;
        
        /* Calculate new capacity */
        new_capacity = sb->capacity * 2;
        if(new_capacity < required)
            new_capacity = required;
        
        /* Re-allocate buffer */
        new_buffer = realloc((void *)sb->buffer, new_capacity);
        if(NULL == new_buffer)
            return -1;
        /* Assign new buffer */
        sb->buffer = (char *)new_buffer;
        sb->capacity = new_capacity;
    }
    
    return 0;
}

/**
 * @brief Push character to the buffer
 * 
 * @param[in,out] sb String buffer object
 * @param c Character to push
 * @return 0 on success
 */
int string_buffer_pushc(
    string_buffer_t * sb,
    char c
)
{
    /* Ensure size of the buffer */
    if(0 != string_buffer_ensure(sb, sizeof(c)))
        return -1;
    
    /* Add character */
    sb->buffer[sb->length++] = c;
    return 0;
}

/**
 * @brief Get the string from the buffer and reset buffer object
 * 
 * @param[in,out] sb String buffer object
 * @return String contained in the buffer. Do not modify this string
 */
const char * string_buffer_flush(
    string_buffer_t * sb
)
{
    sb->buffer[sb->length] = '\0';
    sb->length = 0;
    return sb->buffer;
}

/**
 * 
 * @param fd
 * @param var_name
 * @param var_value
 * @return 
 */
config_parse_res_t config_parse_get(
    FILE * fd,
    string_buffer_t * var_name,
    string_buffer_t * var_value
)
{
    int c;
    parser_state_t state = PARSER_DIVIDER;
    
    while((c = fgetc(fd)) != EOF)
    {
        switch(state)
        {
            /* token divider */
            case PARSER_DIVIDER:
                if(isalpha(c))
                {
                    string_buffer_pushc(var_name, c);
                    state = PARSER_VAR_NAME;
                }
                else if(CONF_COMMENT_CHAR == c)
                {
                    state = PARSER_COMMENT;
                }
                else if(!isspace(c))
                {
                    /* Syntax error */
                    return CONFIG_PARSE_SYNTAX;
                }
                break;

            /* Comment */
            case PARSER_COMMENT:
                if(CONF_NEWLINE_CHAR == c)
                    state = PARSER_DIVIDER;
                break;

            /* Variable name */
            case PARSER_VAR_NAME:
                if(isalnum(c))
                {
                    string_buffer_pushc(var_name, c);
                }
                else if(CONF_EQUALS_CHAR == c)
                {
                    state = PARSER_VAR_VALUE_BASE;
                }
                else
                {
                    return CONFIG_PARSE_SYNTAX;
                }
                break;

            /* Variable value - base */
            case PARSER_VAR_VALUE_BASE:
                if(CONF_QUOTE1_CHAR == c)
                {
                    state = PARSER_VAR_VALUE_QUOTES1;
                }
                else if(CONF_QUOTE2_CHAR == c)
                {
                    state = PARSER_VAR_VALUE_QUOTES2;
                }
                else if(CONF_ESCAPE_CHAR == c)
                {
                    state = PARSER_VAR_VALUE_ESCAPE;
                }
                else if(isspace(c))
                {
                    state = PARSER_DIVIDER;
                    return CONFIG_PARSE_OK;
                }
                else
                {
                    string_buffer_pushc(var_value, c);
                }
                break;
                
            /* Variable value - quotes 1 */
            case PARSER_VAR_VALUE_QUOTES1:
                if(CONF_QUOTE1_CHAR == c)
                {
                    state = PARSER_VAR_VALUE_BASE;
                }
                else
                {
                    string_buffer_pushc(var_value, c);
                }
                break;

            /* Variable value - quotes 2 */
            case PARSER_VAR_VALUE_QUOTES2:
                if(CONF_QUOTE2_CHAR == c)
                {
                    state = PARSER_VAR_VALUE_BASE;
                }
                else
                {
                    string_buffer_pushc(var_value, c);
                }
                break;

            /* Escaped character */
            case PARSER_VAR_VALUE_ESCAPE:
                if(CONF_NEWLINE_CHAR != c)
                {
                    string_buffer_pushc(var_value, c);
                }
                state = PARSER_VAR_VALUE_BASE;
                break;
        }
        
    }
    
    return CONFIG_PARSE_FILE_ERROR;
}

/******************************************************************************/
/*                                Public                                      */
/******************************************************************************/

/******************************************************************************/
config_parse_res_t config_parse(
    FILE * fd,
    int overwrite
)
{
    string_buffer_t var_name, var_value;
    int cont;
    config_parse_res_t res = CONFIG_PARSE_OK;
    
    /* initialize the string buffers */
    if(0 == string_buffer_init(&var_name) &&
        0 == string_buffer_init(&var_value))
    {
    }
    else
        res = CONFIG_PARSE_MEMORY_ERROR;
    
    cont = 1;
    while(cont)
    {
        res = config_parse_get(fd, &var_name, &var_value);
        switch(res)
        {
            case CONFIG_PARSE_OK:
                if(0 != setenv(string_buffer_flush(&var_name),
                    string_buffer_flush(&var_value), overwrite))
                {
                    res = CONFIG_PARSE_SETENV_ERROR;
                    cont = 0;
                }
                break;
            
            case CONFIG_PARSE_FILE_ERROR:
                res = CONFIG_PARSE_OK;
            default:
                cont = 0;                
                break;                
        }
        
    }

    /* Free string buffers */
    string_buffer_deinit(&var_name);
    string_buffer_deinit(&var_value);

    return res;
}
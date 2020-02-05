#include "config_parse.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern char **environ;

/******************************************************************************/
int main(
    int argc,
    char *argv[]
)
{
    FILE * conf_fd;
    config_parse_res_t res;
    
    if(argc != 2)
    {
        printf("usage: config_parse file ('-' for stdin)\n");
        return EXIT_FAILURE;
    }
    
    if(0 == strcmp("-", argv[1]))
    {
        conf_fd = stdin;
    }
    else
    {
        conf_fd = fopen(argv[1], "r");
    }
    
    if(NULL != conf_fd)
    {
        res = config_parse(conf_fd, 1);
        if(CONFIG_PARSE_OK == res)
        {
            char **env;
            for (env = environ; *env; ++env)
                printf("%s\n", *env);
        }
        else
        {
            printf("Error no. %d while processing file \n", res);
        }
        
        fclose(conf_fd);
    }
    else
    {
        printf("Could not open the specified file.\n");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

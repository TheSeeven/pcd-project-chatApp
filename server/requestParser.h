
#include <string.h>
#include <math.h>

bool isNumberOrCharacter(char *ch)
{
    if (((*ch) >= 'a' && (*ch) <= 'z') || (*ch) >= '0' && (*ch) <= '9')
        return true;
    else
        return false;
}
int stringToInt(char string[10])
{
    int result = 0;
    int initialSize = strlen(string) - 1;
    for (int i = 0; string[i] != '\0'; i++)
        result += ((string[i] - 48) * (pow(10, initialSize--)));
    return result;
}

int getArgumentLength(char *string)
{
    int size = 0;
    for (int i = 0; string[i] != ',' && string[i] != '\0' && string[i] != ';'; i++)
    {
        char c = string[i];
        size++;
    }
    return ++size;
}

char *getArgumentByIndex(char *source, int index)
{
    char *result;
    bool startReading = false;
    int destCounter = 0;
    int argumentLength;
    int separators = 0;
    for (int i = 0; source[i] != '\0'; i++)
    {
        char c = source[i];
        if (source[i] == ';')
        {
            startReading = true;
        }
        else if (source[i] == ',')
        {
            separators++;
        }
        else if (startReading && (source[i] == '\0' || source[i] == ',') && separators == index)
        {
            break;
        }
        else if (startReading && (separators == index))
        {
            argumentLength = getArgumentLength(&source[i]);
            result = (char *)malloc(argumentLength + 1);
            memset(result, '\0', argumentLength + 1);
            strncpy(result, &source[i], argumentLength - 1);
            return result;
        }
    }
}

char *getRequestName(char *string)
{
    char *result;
    int requestNameLength;

    requestNameLength = getArgumentLength(string);
    result = malloc(requestNameLength);
    memset(result, '\0', requestNameLength);
    strncpy(result, string, requestNameLength - 1);
    return result;
}

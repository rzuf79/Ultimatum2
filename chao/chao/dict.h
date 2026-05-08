#ifndef CHAO_DICT_H
#define CHAO_DICT_H

/** 
    "hey, why not just use a string as a dictionary"
    why it's the best idea ever:
    - it works lol
    - no need to serialise it
    - it's hideously slow, forcing you to be careful with your data
    - makes people angry

    so it's just a string with \n-separated values, like
    pippo=1
    albowiem=zaprawde
*/

char* char_dict_create(int size) {
    char* dict = (char*)malloc(sizeof(char)*size);
    dict[0] = '\0';
    return dict;
}

void char_dict_erase(char* dict, const char* key) {
    char* start = strstr(dict, key);
    int keyn = strlen(key);
    if (start && start[keyn] == '=') {
        char* end = strchr(start, '\n');
        end = end ? end+1 : start + strlen(start);
        memmove(start, end, strlen(end) + 1);
    }
}

void char_dict_set(char* dict, const char* key, char* value) {
    char_dict_erase(dict, key);
    int n = strlen(dict);
    sprintf(dict, "%s\n%s=%s", dict, key, value);
}

void char_dict_seti(char* dict, const char* key, int value) {
    char_dict_erase(dict, key);
    int n = strlen(dict);
    sprintf(dict, "%s\n%s=%d", dict, key, value);
}

void char_dict_setf(char* dict, const char* key, double value) {
    char_dict_erase(dict, key);
    int n = strlen(dict);
    sprintf(dict, "%s\n%s=%.4f", dict, key, value);
}

void char_dict_get(char* dst, char* dict, const char* key, const char* default_value) {
    char* start = strstr(dict, key);
    int keyn = strlen(key);
    if (start && start[keyn] == '=') {
        char* end = strchr(start, '\n');
        if (end) {
            int len = end - (start + keyn + 1);
            strncpy(dst, start + keyn + 1, len);
            dst[len] = '\0';
        } else {
            strcpy(dst, start + keyn + 1);
        }
    } else {
        strcpy(dst, default_value);
    }
}

int char_dict_geti(char* dict, const char* key, int default_value) {
    char dst[64];
    char_dict_get(dst, dict, key, "");
    return strlen(dst) > 0 ? atoi(dst) : default_value;
}

double char_dict_getf(char* dict, const char* key, double default_value) {
    char dst[64];
    char_dict_get(dst, dict, key, "");
    return strlen(dst) > 0 ? atof(dst) : default_value;
}

#endif // CHAO_DICT_H

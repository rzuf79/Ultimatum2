#ifndef CHAO_JSON_H
#define CHAO_JSON_H

typedef struct {
    char* key;
    char* value;
} JSON;

typedef enum {
    JTYPE_OBJECT,
    JTYPE_ARRAY,
    JTYPE_STRING,
    JTYPE_PRIMITIVE
} JsonValueType;

JsonValueType json_get_type(char* value) {
    switch (value[0]) {
        case '{': return JTYPE_OBJECT;
        case '[': return JTYPE_ARRAY;
        case '"': return JTYPE_STRING;
    }
    return JTYPE_PRIMITIVE;
}

static void json_skip_whitespace(char** pos_ptr) {
    while (**pos_ptr && isspace(**pos_ptr)) {
        (*pos_ptr)++;
    }
}

static char* json_parse_string(char** pos_ptr) {
    char* pos = *pos_ptr;
    if (*pos != '"') return NULL;
    pos++;
    char* start = pos;
    while (*pos && *pos != '"') {
        if (*pos == '\\' && *(pos + 1)) {
            pos += 2;
        } else {
            pos++;
        }
    }
    size_t len = pos - start;
    char* result = malloc(len + 1);
    if (!result) return NULL;
    strncpy(result, start, len);
    result[len] = '\0';
    if (*pos == '"') pos++;
    *pos_ptr = pos;
    return result;
}

static char* json_parse_value(char** pos_ptr) {
    json_skip_whitespace(pos_ptr);
    char* pos = *pos_ptr;
    char* start = pos;
    if (*pos == '"') {
        return json_parse_string(pos_ptr);
    } else if (*pos == '{' || *pos == '[') {
        char open = *pos;
        char close = (open == '{') ? '}' : ']';
        int count = 0;
        while (*pos) {
            if (*pos == open)
                count++;
            else if (*pos == close)
                count--;
            pos++;
            if (count == 0)
                break;
        }
        size_t len = pos - start;
        char* result = malloc(len + 1);
        if (!result) return NULL;
        strncpy(result, start, len);
        result[len] = '\0';
        *pos_ptr = pos;
        return result;
    } else {
        while (*pos && *pos != ',' && *pos != '}' && *pos != ']') {
            pos++;
        }
        size_t len = pos - start;
        while (len > 0 && isspace(start[len - 1])) {
            len--;
        }
        char* result = malloc(len + 1);
        if (!result) return NULL;
        strncpy(result, start, len);
        result[len] = '\0';
        *pos_ptr = pos;
        return result;
    }
}

JSON** json_parse(char* json, int* jsons_count) {
    char* pos = json;
    json_skip_whitespace(&pos);
    if (*pos != '{') return NULL;
    pos++;
    
    int capacity = 16;
    int count = 0;
    JSON** items = malloc(capacity * sizeof(JSON*));
    if (!items) return NULL;
    
    while (*pos) {
        json_skip_whitespace(&pos);
        if (*pos == '}') {
            pos++;
            break;
        }
        if (*pos == ',') {
            pos++;
            continue;
        }
        if (*pos != '"') {
            break;
        }
        char* key = json_parse_string(&pos);
        json_skip_whitespace(&pos);
        if (*pos != ':') {
            free(key);
            break;
        }
        pos++;
        json_skip_whitespace(&pos);
        char* value = json_parse_value(&pos);
        
        JSON* item = malloc(sizeof(JSON));
        if (!item) break;
        item->key = key;
        item->value = value;
        
        if (count >= capacity) {
            capacity *= 2;
            JSON** temp = realloc(items, capacity * sizeof(JSON*));
            if (!temp) break;
            items = temp;
        }
        items[count++] = item;
    }
    
    *jsons_count = count;
    return items;
}

bool json_has(JSON** jsons, int jsons_count, const char* key) {
    for (int i = 0; i < jsons_count; i++) {
        if (strcmp(jsons[i]->key, key) == 0) {
            return true;
        }
    }
    return false;
}

char* json_get(JSON** jsons, int jsons_count, const char* key) {
    for (int i = 0; i < jsons_count; i++) {
        if (strcmp(jsons[i]->key, key) == 0) {
            return jsons[i]->value;
        }
    }
    return NULL;
}

int json_geti(JSON** jsons, int jsons_count, const char* key) {
    return atoi(json_get(jsons, jsons_count, key));
}

double json_getf(JSON** jsons, int jsons_count, const char* key) {
    return atof(json_get(jsons, jsons_count, key));
}

char** json_get_array(JSON** jsons, int jsons_count, const char* key, int* array_length) {
    *array_length = 0;
    char* array_str = json_get(jsons, jsons_count, key);
    if (!array_str || json_get_type(array_str) != JTYPE_ARRAY) {
        return NULL;
    }

    char* pos = array_str + 1; // skip the opening '['
    json_skip_whitespace(&pos);

    int count = 0;
    char* temp = pos;
    while (*temp && *temp != ']') {
        if (*temp == ',') count++;
        temp++;
    }
    if (pos != temp) count++;
    

    char** result = malloc((count + 1) * sizeof(char*));
    if (!result) return NULL;
    result[count] = NULL;

    int index = 0;
    while (*pos && *pos != ']') {
        json_skip_whitespace(&pos);
        char* element = json_parse_value(&pos);
        if (!element) {
            for (int i = 0; i < index; i++) free(result[i]);
            free(result);
            return NULL;
        }
        result[index++] = element;

        json_skip_whitespace(&pos);
        if (*pos == ',') pos++;
    }
    
    *array_length = index-1;
    return result;
}

void json_free_array(char** array, int length) {
    if (array) {
        for (int i = 0; i < length; ++i) {
            if (array[i]) free(array[i]);
        }
    }
}


void json_free(JSON** jsons, int jsons_count) {
    for (int i = 0; i < jsons_count; ++i) {
		if (jsons[i]) {
            if (jsons[i]->key) free(jsons[i]->key);
            if (jsons[i]->value) free(jsons[i]->value);
            
            free(jsons[i]);
        }
	}
	free(jsons);
}


#endif // CHAO_JSON_H
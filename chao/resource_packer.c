#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>

FILE *out;

void sanitize_name(const char *in, char *outname) {
    while (*in) {
        char c = *in++;
        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9'))
            *outname++ = c;
        else
            *outname++ = '_';
    }
    *outname = '\0';
}

static int is_text_file(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return 0;
    ext++;

    return (strcmp(ext, "txt") == 0 ||
            strcmp(ext, "json") == 0 ||
            strcmp(ext, "jevko") == 0 ||
            strcmp(ext, "obj") == 0);
}

typedef struct AssetEntry {
    char name[256];
    char path[1024];
    size_t size;
    int is_text;
} AssetEntry;

AssetEntry assets[2048];
int asset_count = 0;

void write_asset(const char *path, const char *name) {
    FILE *f = fopen(path, is_text_file(path) ? "r" : "rb");
    if (!f) return;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    unsigned char *data = malloc(size + 1);
    fread(data, 1, size, f);
    fclose(f);
    data[size] = '\0';

    int text = is_text_file(path);

    strcpy(assets[asset_count].name, name);
    strcpy(assets[asset_count].path, path);
    assets[asset_count].size = size;
    assets[asset_count].is_text = text;
    asset_count++;

    if (text) {
        fprintf(out, "static const char %s[] = \"", name);
        for (size_t i = 0; i < size; i++) {
            unsigned char c = data[i];
            if (c == '\\' || c == '\"')
                fprintf(out, "\\%c", c);
            else if (c == '\n')
                fprintf(out, "\\n");
            else if (c == '\r')
                fprintf(out, "\\r");
            else if (c == '\t')
                fprintf(out, "\\t");
            else if (c < 32 || c > 126)
                fprintf(out, "\\x%02X", c);
            else
                fputc(c, out);
        }
        fprintf(out, "\";\n\n");
    } else {
        fprintf(out, "static const unsigned char %s[] = {", name);
        for (size_t i = 0; i < size; i++)
            fprintf(out, (i % 12) ? "0x%02X," : "\n  0x%02X,", data[i]);
        fprintf(out, "\n};\n\n");
    }

    free(data);
}

void walk(const char *dirpath) {
    DIR *dir = opendir(dirpath);
    if (!dir) return;

    struct dirent *entry;
    char fullpath[1024];

    while ((entry = readdir(dir))) {
        if (entry->d_name[0] == '.') continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, entry->d_name);

        struct stat st;
        stat(fullpath, &st);

        if (S_ISDIR(st.st_mode)) {
            walk(fullpath);
        } else if (S_ISREG(st.st_mode)) {
            char name[256];
            sanitize_name(fullpath, name);
            write_asset(fullpath, name);
        }
    }
    closedir(dir);
}

int main() {
    out = fopen("resources.h", "w");
    if (!out) return 1;

    fprintf(out,
        "#ifndef CHAO_RESOURCES_H\n#define CHAO_RESOURCES_H\n"
        "#include <stddef.h>\n#include <string.h>\n\n"
        "typedef struct {\n"
        "    const unsigned char *data;\n"
        "    const char *text;\n"
        "    const char *path;\n"
        "    size_t size;\n"
        "} AssetResource;\n\n"
    );

    walk("assets");

    fprintf(out, "static AssetResource asset_resources[] = {\n");
    for (int i = 0; i < asset_count; i++) {
        fprintf(out,
            "    { %s, %s, \"%s\", %zu },\n",
            assets[i].is_text ? "NULL" : assets[i].name,
            assets[i].is_text ? assets[i].name : "NULL",
            assets[i].path,
            assets[i].size
        );
    }
    fprintf(out, "};\n\n");

    fprintf(out,
        "static AssetResource* get_asset_resource(const char *path) {\n"
        "    for (int i = 0; i < %d; i++) {\n"
        "        if (strcmp(asset_resources[i].path, path) == 0)\n"
        "            return &asset_resources[i];\n"
        "    }\n"
        "    return NULL;\n"
        "}\n\n", asset_count);

    fprintf(out, "#endif //CHAO_RESOURCES_H\n");

    fclose(out);
    printf("Generated resources.h with %d assets.\n", asset_count);
    return 0;
}
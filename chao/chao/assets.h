#ifndef CHAO_ASSETS_H
#define CHAO_ASSETS_H

#define MAX_BITMAP_ASSETS 128
#define MAX_MODEL3D_ASSETS 128
#define AGB(id) assets_get_bitmap(id)
#define AGM(id) assets_get_model(id)

typedef struct {
    unsigned long id;
    Bitmap* bitmap;
} BitmapAsset;

typedef struct {
	unsigned long id;
	Model3D* model;
} Model3DAsset;

BitmapAsset bitmap_assets[MAX_BITMAP_ASSETS];
Model3DAsset model3d_assets[MAX_MODEL3D_ASSETS];

Bitmap* load_bitmap_from_memory(const unsigned char* mem, size_t size) {
    int width, height, channels;
    unsigned char* data = stbi_load_from_memory(mem, (int)size, &width, &height, &channels, 4);
    if (!data) {
        fprintf(stderr, "Failed to load image from memory\n");
        return NULL;
    }

    Bitmap* bmp = create_bitmap(width, height);

    for (int i = 0; i < width * height; ++i) {
        uint8_t r = data[i * 4];
        uint8_t g = data[i * 4 + 1];
        uint8_t b = data[i * 4 + 2];
        uint8_t a = data[i * 4 + 3];
        bmp->pixels[i] = RGBA(r, g, b, a);
    }

    stbi_image_free(data);
    return bmp;
}

Bitmap* load_bitmap(const char* path) {
    int width, height, channels;
    AssetResource* asset = get_asset_resource(path);
    if (asset == NULL) {
    	printf("No bitmap asset found: %s\n", path);
    	return NULL;
	}
    unsigned char* data = stbi_load_from_memory(asset->data, (int)asset->size, &width, &height, &channels, 4);
    // unsigned char* data = stbi_load(path, &width, &height, &channels, 4); // Force RGBA (4 channels)
    if (!data) {
        fprintf(stderr, "Failed to load image: %s\n", path);
        return NULL;
    }
    Bitmap* bmp = create_bitmap(width, height);

    for (int i = 0; i < width * height; ++i) {
        uint8_t r = data[i * 4];
        uint8_t g = data[i * 4 + 1];
        uint8_t b = data[i * 4 + 2];
        uint8_t a = data[i * 4 + 3];
        bmp->pixels[i] = RGBA(r, g, b, a);
    }
    stbi_image_free(data);
    return bmp;
}

Bitmap* assets_add_bitmap(const char* id, Bitmap* bitmap) {
    int i = 0;
    while (bitmap_assets[i].bitmap != NULL) {
        ++i;
    }
    //printf("Adding bitmap \"%s\" with i=%d\n", id, i);
    bitmap_assets[i].id = hash(id);
    bitmap_assets[i].bitmap = bitmap;
    return bitmap;
}

Bitmap* assets_load_bitmap(const char* id, const char* path) {
    return assets_add_bitmap(id, load_bitmap(path));
}

Bitmap* assets_get_bitmap_from_hash(unsigned long hashed_id) {
    for (int i = 0; i < MAX_BITMAP_ASSETS; ++i) {
        if (bitmap_assets[i].id == hashed_id) {
            return bitmap_assets[i].bitmap;
        }
    }
    printf("No bitmap found! (id: %lu)\n", hashed_id);
    return NULL;
}

Bitmap* assets_get_bitmap(const char* id) {
    return assets_get_bitmap_from_hash(hash(id));
}

void assets_free_bitmaps() {
    for (int i = 0; i < MAX_BITMAP_ASSETS; ++i) {
        if (bitmap_assets[i].bitmap != NULL) {
            bitmap_assets[i].id = -1;
            free_bitmap(bitmap_assets[i].bitmap);
            bitmap_assets[i].bitmap = NULL;
        }
    }
}

Model3D* assets_add_model3d(const char* id, Model3D* model) {
    int i = 0;
    while (model3d_assets[i].model != NULL) {
        ++i;
    }
    model3d_assets[i].id = hash(id);
    model3d_assets[i].model = model;
    return model;
}

Model3D* assets_parse_obj(const char* obj_data);
Model3D* load_model3d(const char* path) {
    AssetResource* asset = get_asset_resource(path);
    return assets_parse_obj(asset->text);
}

Model3D* assets_load_model3d(const char* id, const char* path) {
    return assets_add_model3d(id, load_model3d(path));
}

void assets_free_model3ds() {
	for (int i = 0; i < MAX_MODEL3D_ASSETS; ++i) {
		model3d_free(model3d_assets[i].model);
		model3d_assets[i].id = -1;
	}
}

Model3D* assets_get_model3d_from_hash(unsigned long hashed_id) {
    for (int i = 0; i < MAX_MODEL3D_ASSETS; ++i) {
        if (model3d_assets[i].id == hashed_id) {
            return model3d_assets[i].model;
        }
    }
    return NULL;
}

Model3D* assets_get_model(const char* id) {
	return assets_get_model3d_from_hash(hash(id));
}

Model3D* assets_parse_obj(const char* obj_data) {
    Model3D* model = (Model3D*)malloc(sizeof(Model3D));
    model->texture = NULL;

    int verts_count = 0;
    int uvs_count = 0;
    int normals_count = 0;
    int tris_count = 0;

    const char* p = obj_data;
    while (*p) {
        if (p == obj_data || *(p-1) == '\n') {
            if (strncmp(p, "v ", 2) == 0) verts_count++;
            else if (strncmp(p, "vt ", 3) == 0) uvs_count++;
            else if (strncmp(p, "vn ", 3) == 0) normals_count++;
            else if (strncmp(p, "f ", 2) == 0) tris_count++;
        }
        p++;
    }

    model->vertices = (Vector3*)malloc(sizeof(Vector3) * verts_count);
    model->uvs = (Vector2*)malloc(sizeof(Vector2) * uvs_count);
    model->normals = (Vector3*)malloc(sizeof(Vector3) * normals_count);

    model->tris = (Vector3Int*)malloc(sizeof(Vector3Int) * tris_count);
    model->tri_uvs = (Vector3Int*)malloc(sizeof(Vector3Int) * tris_count);
    model->tri_normals = (Vector3Int*)malloc(sizeof(Vector3Int) * tris_count);

    model->tris_count = tris_count;

    int vi = 0, ti = 0, ui = 0, ni = 0;

    const char* line = obj_data;
    while (*line) {
        const char* next = strchr(line, '\n');
        if (!next) next = line + strlen(line);

        if (strncmp(line, "v ", 2) == 0) {
            sscanf(line, "v %f %f %f", &model->vertices[vi].x, &model->vertices[vi].y, &model->vertices[vi].z);
            vi++;

        } else if (strncmp(line, "vt ", 3) == 0) {
            sscanf(line, "vt %f %f", &model->uvs[ui].x, &model->uvs[ui].y);
			model->uvs[ui].y = 1.0f - model->uvs[ui].y;
            ui++;

        } else if (strncmp(line, "vn ", 3) == 0) {
            sscanf(line, "vn %f %f %f", &model->normals[ni].x, &model->normals[ni].y, &model->normals[ni].z);
            ni++;

        } else if (strncmp(line, "f ", 2) == 0) {
            int v1,v2,v3;
            int t1,t2,t3;
            int n1,n2,n3;

            int parsed = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1,&t1,&n1, &v2,&t2,&n2, &v3,&t3,&n3
            );

            if (parsed == 9) {
                model->tris[ti] = (Vector3Int){ v1-1, v2-1, v3-1 };
                model->tri_uvs[ti] = (Vector3Int){ t1-1, t2-1, t3-1 };
                model->tri_normals[ti] = (Vector3Int){ n1-1, n2-1, n3-1 };
            }
            else {
                parsed = sscanf(line, "f %d %d %d", &v1, &v2, &v3);
                if (parsed == 3) {
                    model->tris[ti] = (Vector3Int){ v1-1, v2-1, v3-1 };
                    model->tri_uvs[ti] = (Vector3Int){ -1, -1, -1 };
                    model->tri_normals[ti] = (Vector3Int){ -1, -1, -1 };
                }
            }
            ti++;
        }

        line = (*next) ? next + 1 : NULL;
    }

    printf("OBJ parsed: %d vertices, %d uvs, %d normals, %d tris\n", verts_count, uvs_count, normals_count, tris_count);

    return model;
}


#endif // CHAO_ASSETS_H

#ifndef LOCATION_OBJECT_H
#define LOCATION_OBJECT_H

#define MAX_LOCATION_OBJECT_DATAS 128

EntityDef location_object_defs[MAX_LOCATION_OBJECT_DATAS];
size_t location_object_defs_count;

void location_objects_init() {
	location_object_defs_count = 0;

	location_object_defs[location_object_defs_count++] = (EntityDef) {
		.id = "mud_school_plaque",
		.type = ENTITY_TYPE_OBJECT,
		.image = AGB("location_object_plaque"),
		.frames_count = 1,
		.screen_pos = (Vector2Int){56,78},
	};
}

EntityDef* location_object_def_get(const char* id) {
	if (id != NULL) {
		for (int i = 0; i < location_object_defs_count; ++i) {
			if (strcmp(location_object_defs[i].id, id) == 0) {
				return &location_object_defs[i];
			}
		}
	}
	return NULL;
}


#endif // LOCATION_OBJECT_H

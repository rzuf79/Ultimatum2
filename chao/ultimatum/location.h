#ifndef LOCATION_H
#define LOCATION_H

#include "../chao/chao.h"

#define MAX_LOCATIONS 512
#define MAX_OBJECTS_IN_LOCATION 4
#define MAX_MOBS_IN_LOCATION 5
#define MAX_LOCATION_ENTITIES 10
#define MAX_DOORS 256
#define MAX_ENTITIES 512

typedef struct {
	Vector3Int pos;
	char* name;
	bool exits[6];
	Bitmap* image;
	bool explored;
	char* objects[MAX_OBJECTS_IN_LOCATION];
	char* mobs[MAX_MOBS_IN_LOCATION];
} Location;

typedef struct {
	char* id;
	Location* location_a;
	Location* location_b;
	char* message;
	char* key;
	char* var;
	bool secret;
	bool opened;
} Door;

typedef struct {
    EntityAnim* anim;
    float anim_timer;
    int anim_frame;
} EntityTimer;

Vector3Int current_world_location;

Location world_locations[MAX_LOCATIONS];
int world_locations_count = 0;
Door world_doors[MAX_DOORS];
int world_doors_count = 0;

Entity world_entities[MAX_ENTITIES];

Bitmap* world_location_canvas;
Vector2Int world_location_canvas_pos;
Vector2Int world_location_canvas_offset;

Location* world_location_current;

Entity* current_location_entities[MAX_LOCATION_ENTITIES];
EntityTimer entities_timers[MAX_LOCATION_ENTITIES];
Entity* hovered_entity = NULL;
float hovered_location_entity_timer = 0.0f;
Vector2Int* world_location_entity_convex = NULL;
int world_location_entity_convex_length = 0;
bool location_update_mouse_hover = false;


void free_world_location_entity_convex() {
    if (world_location_entity_convex != NULL) {
        free(world_location_entity_convex);
        world_location_entity_convex = NULL;
        world_location_entity_convex_length = 0;
    }
}

void world_location_free() {
    free_world_location_entity_convex();
}

Location* location_get(Vector3Int pos) {
	for (int i = 0; i < world_locations_count; ++i) {
		if (world_locations[i].pos.x == pos.x && world_locations[i].pos.y == pos.y && world_locations[i].pos.z == pos.z) {
			return &world_locations[i];
		}
	}
	return NULL;
}

void world_add_entity(EntityDef* def, Vector3Int pos) {
	for (int i = 0; i < MAX_ENTITIES; ++i) {
		Entity* e = &world_entities[i];
		if (e->def == NULL) {
			e->def = def;
			e->pos = pos;
			e->chara = def->chara;
			for (int j = 0; j < ENTITY_MAX_ANIMS; ++j) {
				e->anims[j] = NULL;
				char* anim_id = def->anim_ids[j];
				if (anim_id != NULL) {
					e->anims[j] = entity_anim_get(anim_id);
				}
			}
			entity_init(e);
			return;
		}
	}
	printf("adding entity failed, too many of 'em!\n");
}

void world_reset_entity_anim(Entity* e, EntityTimer* timer) {
    int i = randi_range(0, ENTITY_MAX_ANIMS-1);
    timer->anim = e->anims[i];
    timer->anim_timer = 0.0f;
    timer->anim_frame = 0;
}

void world_remove_entity(Entity* entity) {
	for (int i = 0; i < MAX_ENTITIES; ++i) {
		if (entity == &world_entities[i]) {
			entity->def = NULL;
			for (int j = 0; j < ENTITY_MAX_ANIMS; ++j) {
				entity->anims[j] = NULL;
			}
		}
	}
}

void world_location_init() {
    free_world_location_entity_convex();
	Vector2Int canvas_size = { LOCATION_CANVAS_SIZE, LOCATION_CANVAS_SIZE };
	world_location_canvas = create_bitmap_filled(canvas_size.x, canvas_size.y, COLOR_WHITE);
	assets_add_bitmap("world_location_canvas", world_location_canvas);
}

void world_location_setup(Entity* player) {
    if (player != NULL) {
        current_world_location = player->pos;
    }
    hovered_entity = NULL;
    free_world_location_entity_convex();
	world_location_current = location_get(current_world_location);

	// ADD ENTITIES HERE (objects & mobs)
	for (int i = 0; i < MAX_LOCATION_ENTITIES; ++i) {
		current_location_entities[i] = NULL;
	}
	int entities_i = 0;
	for (int i = 0; i < MAX_ENTITIES; ++i) {
		Entity* e = &world_entities[i];
		if (entity_is_valid(e)) {
			if (vector3_int_compare(&(e->pos), &(current_world_location))) {
				current_location_entities[entities_i] = e;
				world_reset_entity_anim(e, &entities_timers[entities_i]);
				entities_timers[entities_i].anim_timer = rand_range(0.0f, 0.5f);
				entities_i++;
			}
		}
		if (entities_i >= MAX_LOCATION_ENTITIES) break;
	}

	world_location_current->explored = true;
	world_location_canvas_offset = (Vector2Int) {
		(world_location_current->image->size.x - world_location_canvas->size.x) / 2,
		(world_location_current->image->size.y - world_location_canvas->size.y) / 2,
	};
	location_update_mouse_hover = true;
}

void world_location_draw_name_scroll(char* name, int underline_index) {
    Bitmap* frame = AGB("location_scroll");
	Vector2Int pos = { (screen_size.x-frame->size.x)/2, 3 };
	blit(chao_canvas, frame, pos.x, pos.y, COLOR_WHITE);
	const char* txt = (const char*)name;
	char* txt_formatted = ascii_font_break_lines(font, 75, txt);
	Vector2Int txt_size = ascii_font_get_size(font, txt_formatted, -1);
	pos.x = screen_size.x / 2;
	pos.y += (frame->size.y - txt_size.y) / 2 - 2;
	ascii_font_draw_ex(chao_canvas, font, pos.x, pos.y, COLOR_BLACK, TEXT_ALIGN_CENTER, underline_index, txt_formatted);
	free(txt_formatted);
}

void world_location_draw() {
	world_location_canvas_pos = (Vector2Int) {
		(screen_size.x-world_location_canvas->size.x)/2,
		(screen_size.y-world_location_canvas->size.y)/2
	};
	Vector2Int loc_pos = {
		(world_location_canvas->size.x - world_location_current->image->size.x) / 2,
		(world_location_canvas->size.y - world_location_current->image->size.y) / 2
	};

    blit(world_location_canvas, world_location_current->image, loc_pos.x, loc_pos.y, COLOR_WHITE);

	for (int i = 0; i < MAX_LOCATION_ENTITIES; ++i) {
		Entity* e = current_location_entities[i];
		if (e != NULL) {
			Bitmap* image = e->def->image;
			RectInt rect = entity_get_frame_rect(e, entities_timers[i].anim, entities_timers[i].anim_frame);
			Vector2Int pos = entity_get_screen_position(e, current_location_entities, MAX_LOCATION_ENTITIES);
			pos.x -= world_location_canvas_offset.x;
			pos.y -= world_location_canvas_offset.y;
			blit_rect(world_location_canvas, image, rect, pos, COLOR_WHITE);
		}
	}

	if (hovered_entity != NULL && world_location_entity_convex != NULL) {
		Vector2Int entity_pos = entity_get_screen_position(hovered_entity, current_location_entities, MAX_LOCATION_ENTITIES);
		for (int i = 0; i < world_location_entity_convex_length; i++) {
			int ii = (i+((int)hovered_location_entity_timer)) % world_location_entity_convex_length;
			Vector2Int point = world_location_entity_convex[ii];
			int x = point.x + entity_pos.x - world_location_canvas_offset.x;
			int y = point.y + entity_pos.y - world_location_canvas_offset.y;
			put_pixel(world_location_canvas, x, y, input.mouse.pressed ? COLOR_DARKGRAY : COLOR_BLACK);
			if (i%3 == 0) i+= 3;
		}
	}

	blit(chao_canvas, world_location_canvas, world_location_canvas_pos.x, world_location_canvas_pos.y, COLOR_WHITE);

	Vector2Int s = world_location_canvas->size;
	draw_rect(chao_canvas, world_location_canvas_pos.x-1, world_location_canvas_pos.y-1, s.x+2, s.y+2, 2, COLOR_BLACK);


	world_location_draw_name_scroll(world_location_current->name, -1);
}

void update_hovered_entity_outline() {
    free_world_location_entity_convex();
    EntityTimer* timer = NULL;
    for (int i = 0; i < MAX_LOCATION_ENTITIES; ++i) {
        if (hovered_entity == current_location_entities[i]) {
            timer = &entities_timers[i];
        }
    }
    if (timer == NULL) {
        printf("somehow there's no timer for the hovered entity\n");
    }
    RectInt rect = entity_get_frame_rect(hovered_entity, timer->anim, timer->anim_frame);
	world_location_entity_convex = get_outline_moore(hovered_entity->def->image, rect, &world_location_entity_convex_length);
}

void set_location_hovered_entity(Entity* entity) {
	hovered_entity = entity;
	if (hovered_entity != NULL) {
    	update_hovered_entity_outline();
	}
}

Entity* get_next_displayed_entity(Entity* from, Vector2Int dir) {
	bool second_try = false;
	RectInt rect1 = entity_get_frame_rect(from, 0, 0);
	Vector2Int p1 = entity_get_screen_position(from, current_location_entities, MAX_LOCATION_ENTITIES);
	p1.x += rect1.w / 2;
	p1.y += rect1.h / 2;

	begin:;

	Entity* next_entities[MAX_LOCATION_ENTITIES];
	int count = 0;

	for (int i = 0; i < MAX_LOCATION_ENTITIES; ++i) {
		Entity* e = current_location_entities[i];
		if (e == NULL || !entity_is_valid(e)) continue;

		Vector2Int p2 = entity_get_screen_position(e, current_location_entities, MAX_LOCATION_ENTITIES);
    	RectInt rect2 = entity_get_frame_rect(e, 0, 0);
		p2.x += rect2.w / 2;
		p2.y += rect2.h / 2;

		if (dir.x > 0 && p2.x > p1.x) next_entities[count++] = e;
		else if (dir.x < 0 && p2.x < p1.x) next_entities[count++] = e;
		else if (dir.y > 0 && p2.y > p1.y) next_entities[count++] = e;
		else if (dir.y < 0 && p2.y < p1.y) next_entities[count++] = e;
	}

	if (count == 0 && second_try == false) {
		second_try = true;
		dir.x = -dir.x;
		dir.y = -dir.y;
		goto begin;
	}

	int min_length = second_try ? -999999 : 999999;
	int closest_i = -1;
	for (int i = 0; i < count; ++i) {
		Vector2Int p2 = entity_get_screen_position(next_entities[i], current_location_entities, MAX_LOCATION_ENTITIES);
		RectInt rect2 = entity_get_frame_rect(next_entities[i], 0, 0);
		p2.x += rect2.w / 2;
		p2.y += rect2.h / 2;
		Vector2Int vec = { p2.x - p1.x, p2.y - p1.y };
		int len = (vec.x * vec.x) + (vec.y * vec.y);
		if ((second_try && len > min_length) || (!second_try && len < min_length)) {
			closest_i = i;
			min_length = len;
		}
	}
	if (closest_i != -1) {
		return next_entities[closest_i];
	}
	return from;
}

void world_location_update(float dt) {
    // entities interaction
    {
        Entity* new_hovered_entity = NULL;
    	if (input.mouse.moved_last_frame || location_update_mouse_hover) {
        	location_update_mouse_hover = false;
    		for (int i = 0; i < MAX_LOCATION_ENTITIES; ++i) {
    			Entity* entity = current_location_entities[i];
    			if (entity == NULL) {
    				continue;
    			}
    			Vector2Int offset = {
    				world_location_canvas_pos.x - world_location_canvas_offset.x,
    				world_location_canvas_pos.y - world_location_canvas_offset.y,
    			};
        		RectInt rect = entity_get_frame_rect(entity, 0, 0);
    			Vector2Int screen_pos = entity_get_screen_position(entity, current_location_entities, MAX_LOCATION_ENTITIES);
    			screen_pos.x += offset.x;
    			screen_pos.y += offset.y;
    			if (mouse_above(screen_pos.x, screen_pos.y, rect.w, rect.h)) {
    				new_hovered_entity = entity;
    			}
    		}

    		if (new_hovered_entity != hovered_entity) {
                set_location_hovered_entity(new_hovered_entity);
            }
    	}

    	hovered_location_entity_timer += dt * 10.0f;
	}

    // anims
    {
        for (int i = 0; i < MAX_LOCATION_ENTITIES; ++i) {
            Entity* entity = current_location_entities[i];
            if (entity == NULL) {
                continue;
            }
            EntityTimer* timer = &entities_timers[i];
            EntityAnim* anim = timer->anim;
            float delay = anim == NULL ? 0.4f : 0.25f;
            timer->anim_timer += dt;
            if (timer->anim_timer >= delay) {
                timer->anim_frame ++;
                timer->anim_timer = 0.0f;
                if (anim == NULL || anim->seq[timer->anim_frame] == -1) {
                    world_reset_entity_anim(entity, timer);
                }
                if (entity == hovered_entity) {
                    update_hovered_entity_outline();
                }
            }
        }
    }
}

int get_doors_from_location(Location* loc, Door* out_doors[6], Location* out_locations[6], Direction out_dirs[6]) {
	int n = 0;
	Location* other_loc = NULL;
	for (int i = 0; i < world_doors_count; ++i) {
		other_loc = NULL;
		if (world_doors[i].location_a == loc) other_loc = world_doors[i].location_b;
		if (world_doors[i].location_b == loc) other_loc = world_doors[i].location_a;

		if (other_loc != NULL) {
			out_doors[n] = &world_doors[i];
			out_locations[n] = other_loc;

			     if (other_loc->pos.x < loc->pos.x) out_dirs[n] = W;
			else if (other_loc->pos.x > loc->pos.x) out_dirs[n] = E;
			else if (other_loc->pos.y < loc->pos.y) out_dirs[n] = N;
			else if (other_loc->pos.y > loc->pos.y) out_dirs[n] = S;
			else if (other_loc->pos.z < loc->pos.z) out_dirs[n] = U;
			else if (other_loc->pos.z > loc->pos.z) out_dirs[n] = D;

			n++;
		}
	}
	return n;
}

Door* get_door_from_location(Location* loc, Direction dir) {
	Door* out_doors[6];
	Location* out_locations[6];
	Direction out_directions[6];
	int n = get_doors_from_location(loc, out_doors, out_locations, out_directions);
	for (int i = 0; i < n; ++i) {
		if (out_directions[i] == dir) {
			return out_doors[i];
		}
	}
	return NULL;
}



#endif // LOCATION_H

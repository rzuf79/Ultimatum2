#ifndef CHAO_PARTICLES_H
#define CHAO_PARTICLES_H

#define MAX_PARTICLES 512

typedef struct {
	Bitmap* bitmap;
	Vector2 position;
	float lifetime;
	float timer;
	uint32_t color;
	Vector2 speed;
	Vector2 acceleration;
	Vector2 linear_damping;
	float angular_velocity;
	float (*alpha_easing)(float v);
	float alpha;
} Particle;

typedef struct {
	Particle* particles;
	int max;
} ParticleGroup;

ParticleGroup* particles_create_group(int max) {
	ParticleGroup* group = (ParticleGroup*)malloc(sizeof(ParticleGroup));
	group->particles = (Particle*)malloc(sizeof(Particle) * max);
	group->max = max;
	for (int i = 0; i < max; ++i) {
		group->particles[i] = (Particle){
			.bitmap = NULL,
			.position = VECTOR2_ZERO,
			.lifetime = 0.0f,
			.timer = group->particles[i].lifetime = 0.0f,
			.color = COLOR_WHITE,
			.speed = VECTOR2_ZERO,
			.acceleration = VECTOR2_ZERO,
			.linear_damping = VECTOR2_ZERO,
			.angular_velocity = 0.0f,
			.alpha_easing = NULL,
			.alpha = 1.0f,
		};
	}
	return group;
}

void particles_free_group(ParticleGroup* group) {
	if (group != NULL) {
		free(group->particles);
		free(group);
	}
}

void particles_add(ParticleGroup* group, Particle particle) {
	int i = 0;
	while (group->particles[i].timer < group->particles[i].lifetime) {
		++i;
		if (i >= group->max) {
			i = -1;
			break;
		}
	}
	if (i == -1) {
		return;
	}
	group->particles[i] = particle;
	group->particles[i].timer = 0.0f;
}

void particles_draw(Bitmap* dst, ParticleGroup* group, Vector2 offset) {
	for (int i = 0; i < group->max; ++i) {
		Particle* p = &group->particles[i];
		if (p->timer >= p->lifetime || p->lifetime <= 0.0f) {
			continue;
		}
		Vector2Int pos = {
			p->position.x + offset.x,
			p->position.y + offset.y
		};
		blit_ex(dst, p->bitmap, (RectInt){0,0,p->bitmap->size.x,p->bitmap->size.y}, pos, false, false, p->color, p->alpha);
	}
}

void particles_update(ParticleGroup* group, float dt) {
	for (int i = 0; i < group->max; ++i) {
		Particle* p = &group->particles[i];
		if (p->timer >= p->lifetime) {
			continue;
		}
		p->timer += dt;
		if (!fapprox(p->angular_velocity, 0.0f)) {
			vector2_rotate(&p->speed, p->angular_velocity * dt);
		}
		p->position.x += p->speed.x * dt;
		p->position.y += p->speed.y * dt;

		float v = p->timer / p->lifetime;
		if (p->alpha_easing != NULL) {
			p->alpha = p->alpha_easing(v);
		}
	}
}


// EASING FUNCTIONS
float particles_ease_fade_out(float v) {
	return 1.0f - inverse_lerp(0.8f, 1.0f, v);
}

#endif //CHAO_PARTICLES_H

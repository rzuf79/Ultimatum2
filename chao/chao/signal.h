#ifndef CHAO_SIGNAL_H
#define CHAO_SIGNAL_H

typedef void (*SignalCallback)();

typedef struct {
	SignalCallback* callbacks;
	int capacity;
} Signal;

Signal signal_create(int max_callbacks) {
	Signal signal = {
		.callbacks = (SignalCallback*)malloc(sizeof(SignalCallback) * max_callbacks),
		.capacity = max_callbacks,
	};

	for (int i = 0; i < signal.capacity; ++i) {
		signal.callbacks[i] = NULL;
	}

	return signal;
}

void signal_free(Signal* signal) {
	if (signal != NULL && signal->callbacks != NULL) {
		free(signal->callbacks);
	}
	signal->capacity = 0;
}

void signal_invoke(Signal* signal) {
	for (int i = 0; i < signal->capacity; ++i) {
		if (signal->callbacks[i] != NULL) {
			signal->callbacks[i]();
		}
	}
}

void signal_add(Signal* signal, SignalCallback cb) {
	for (int i = 0; i < signal->capacity; ++i) {
		if (signal->callbacks[i] == NULL) {
			signal->callbacks[i] = cb;
			return;
		}
	}
	printf("signal_add: signal at max capacity (%d)\n", signal->capacity);
}

void signal_remove(Signal* signal, SignalCallback cb) {
	for (int i = 0; i < signal->capacity; ++i) {
		if (signal->callbacks[i] == cb) {
			signal->callbacks[i] = NULL;
		}
	}
}

#endif // CHAO_SIGNAL_H

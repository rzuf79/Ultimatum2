#ifndef DIE_H
#define DIE_H

static int roll_die(int count, int sides) {
    int sum = 0;
    for (int i = 0; i < count ; ++i) {
        sum += (rand() % sides) + 1;
    }
    return sum;
}

static int roll_die_max(int count, int sides) {
    int max = 0;
    for (int i = 0; i < count ; ++i) {
        max += sides;
    }
    return max;
}

static int die_parse_number(const char **p) {
    int value = 0;
    while (isdigit(**p)) {
        value = value * 10 + (**p - '0');
        (*p)++;
    }
    return value;
}

static int die_parse_term(const char **p, bool roll_max) {
    int count = 0;
    int sides = 0;

    if (isdigit(**p)) {
        count = die_parse_number(p);
    }

    if (**p == 'd' || **p == 'D') {
        (*p)++;
        if (!isdigit(**p)) {
            return 0;
        }
        sides = die_parse_number(p);

        if (count == 0) {
            count = 1;
        }
        return roll_max ? roll_die_max(count, sides) : roll_die(count, sides);
    }
    return count;
}

int roll_dice(const char *expr) {
    const char *p = expr;
    int result = 0;
    int sign = 1;

    while (*p) {
        while (isspace(*p)) p++;

        if (*p == '+') {
            sign = 1;
            p++;
            continue;
        } else if (*p == '-') {
            sign = -1;
            p++;
            continue;
        }

        int value = die_parse_term(&p, false);
        result += sign * value;
        sign = 1;
    }

    return result;
}

int roll_dice_max(const char* expr) {
    const char *p = expr;
    int result = 0;
    int sign = 1;

    while (*p) {
        while (isspace(*p)) p++;

        if (*p == '+') {
            sign = 1;
            p++;
            continue;
        } else if (*p == '-') {
            sign = -1;
            p++;
            continue;
        }

        int value = die_parse_term(&p, true);
        result += sign * value;
        sign = 1;
    }

    return result;
}

#endif // DIE_H

#include "grib_api_internal.h"

typedef struct grib_concept_index_keys grib_concept_index_keys;
typedef struct grib_concept_index grib_concept_index;
typedef struct grib_conditions_tree grib_conditions_tree;
typedef struct grib_concept_index_entry grib_concept_index_entry;
typedef struct grib_concept_index_keys grib_concept_index_keys;

struct grib_concept_index_entry {
    char* name;
    char* value;
    int type;
    grib_concept_index_entry* next;
};

struct grib_concept_index_keys {
    char* name;
    int type;
    grib_concept_index_keys* next;
};

struct grib_concept_index {
    grib_context* context;
    grib_concept_index_keys* keys;
    grib_conditions_tree* conditions;
};

struct grib_conditions_tree {
    char* value;
    void* object;
    grib_conditions_tree* next;
    grib_conditions_tree* next_key;
};


static grib_concept_index_entry* index_entry_new(grib_context* c, grib_concept_index_keys* keys)
{
    grib_concept_index_entry* entry = NULL;
    grib_concept_index_entry* e;

    Assert(keys);

    if (!c)
        c = grib_context_get_default();

    entry = (grib_concept_index_entry*)grib_context_malloc_clear(c, sizeof(grib_concept_index_entry));
    if (!entry)
        grib_context_log(c, GRIB_LOG_FATAL, "grib_concept_index_entry unable to allocate");
    e = entry;

    while (keys && keys->name) {
        e->name = grib_context_strdup(c, keys->name);
        e->type = keys->type;
        e->next = (grib_concept_index_entry*)grib_context_malloc_clear(c, sizeof(grib_concept_index_entry));
        if (!e->next)
            grib_context_log(c, GRIB_LOG_FATAL, "grib_concept_index_entry unable to allocate");

        e    = e->next;
        keys = keys->next;
    }

    return entry;
}

static void index_entry_delete(grib_context* c, grib_concept_index_entry* entry)
{
    grib_concept_index_entry* e;
    while (entry) {
        e     = entry;
        entry = entry->next;
        grib_context_free(c, e->name);
        grib_context_free(c, e->value);
        grib_context_free(c, e);
    }
}

static int index_insert_entry(grib_concept_index* index, grib_concept_index_entry* entry, void* object)
{
    int err = 0;
    int found;
    grib_conditions_tree* cur     = index->conditions;
    grib_conditions_tree* prev    = index->conditions;
    grib_concept_index_keys* keys = index->keys;

    while (keys->name) {
        if (!cur) {
            cur = (grib_conditions_tree*)grib_context_malloc_clear_persistent(index->context, sizeof(grib_conditions_tree));
            if (!cur)
                grib_context_log(index->context, GRIB_LOG_FATAL, "index_insert_entry unable to allocate");
            prev->next = cur;
        }
        char* value = entry->value ? entry->value : (char*)"*";
        while (cur && (!cur->value || (found = !strcmp(cur->value, value)) == 0))
            cur = cur->next;

        if (!found) {
            cur->next = (grib_conditions_tree*)grib_context_malloc_clear_persistent(index->context, sizeof(grib_conditions_tree));
            Assert(cur->next);
            cur = cur->next;
        }

        cur->value = grib_context_strdup(index->context, value);
        entry      = entry->next;
        keys       = keys->next;
        prev       = cur;
        cur        = cur->next_key;
    }

    while (cur) {
        prev = cur;
        cur  = cur->next_key;
    }

    prev->object = object;

    return err;
}

static void index_add_conditions(grib_concept_index* index, grib_concept_condition* condition)
{
    grib_concept_condition* c = condition;
    char s[512]               = {0,};
    grib_concept_index_entry* e;
    grib_concept_index_entry* entry = index_entry_new(index->context, index->keys);
    long lres;
    double dres;
    int err = 0;

    while (c) {
        size_t size = 512;
        int type;
        e    = entry;
        type = grib_expression_native_type(0, c->expression);
        switch (type) {
            case GRIB_TYPE_LONG:
                grib_expression_evaluate_long(0, c->expression, &lres);
                snprintf(s, sizeof(s), "%ld", lres);
                break;

            case GRIB_TYPE_DOUBLE:
                grib_expression_evaluate_double(0, c->expression, &dres);
                snprintf(s, sizeof(s), "%g", dres);
                break;

            case GRIB_TYPE_STRING:
                grib_expression_evaluate_string(0, c->expression, s, &size, &err);
                break;

            default:
                Assert(0);
                break;
        }

        while (e->name && strcmp(e->name, c->name))
            e = e->next;

        e->type  = type;
        e->value = grib_context_strdup(index->context, s);
        if (!e->name) {
            e->name = grib_context_strdup(index->context, c->name);
            e->next = (grib_concept_index_entry*)grib_context_malloc_clear_persistent(index->context, sizeof(grib_concept_index_entry));
            if (!e->next)
                grib_context_log(index->context, GRIB_LOG_FATAL, "index_add_conditions unable to allocate");
        }

        c = c->next;
    }

    index_insert_entry(index, entry, condition->name);

    index_entry_delete(index->context, entry);
}

grib_concept_index* grib_concept_index_new_from_concept(grib_context* c, grib_concept_value* concept, int* err)
{
    grib_concept_index* index;

    if (!c)
        c = grib_context_get_default();

    index             = (grib_concept_index*)grib_context_malloc_clear_persistent(c, sizeof(grib_concept_index));
    index->keys       = (grib_concept_index_keys*)grib_context_malloc_clear_persistent(c, sizeof(grib_concept_index_keys));
    index->conditions = (grib_conditions_tree*)grib_context_malloc_clear_persistent(c, sizeof(grib_conditions_tree));
    index->context    = c;

    while (concept) {
        index_add_conditions(index, concept->conditions);
        concept = concept->next;
    }

    return index;
}

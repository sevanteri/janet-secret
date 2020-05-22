
#include <glib.h>
#include <libsecret/secret.h>
#include <janet.h>

static const SecretSchema simpleschema = {
    "simple.example.schema", SECRET_SCHEMA_NONE,
    {
        { "NULL", 0 },
    }
};

static GHashTable* schema_to_hashtable(const JanetKV *kv) {
    GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);
    int32_t cap = janet_struct_capacity(kv);
    const JanetKV *cur = NULL;
    while ((cur = janet_dictionary_next(kv, cap, cur))) {
        gchar *name = (gchar*)janet_unwrap_string(cur->key);
        SecretSchemaAttributeType type = janet_unwrap_integer(cur->value);
        g_hash_table_insert(hash, name, GUINT_TO_POINTER(type));
    }
    return hash;
}

static void extract_schema_variables(const JanetStruct schema,
        gchar** name,
        int32_t *flags,
        GHashTable **attributes) {

    Janet schema_name = janet_struct_get(schema, janet_ckeywordv("name"));
    Janet schema_flags = janet_struct_get(schema, janet_ckeywordv("flags"));
    Janet schema_attrs = janet_struct_get(schema, janet_ckeywordv("attributes"));

    if (!janet_checktype(schema_name, JANET_STRING))
        janet_panic("Schema name has to be a String\n");

    if (!janet_checktype(schema_flags, JANET_NUMBER))
        janet_panic("Schema flags has to be either schema/NONE or schema/DONT-MATCH-NAME\n");

    if (!janet_checktype(schema_attrs, JANET_STRUCT))
        janet_panic("Schema attributes has to be a Struct\n");

    JanetStruct jattrs = janet_unwrap_struct(schema_attrs);

    *attributes = schema_to_hashtable(jattrs);
    *name = (gchar*)janet_unwrap_string(schema_name);
    *flags = janet_unwrap_integer(schema_flags);
}

static GHashTable* attributes_to_hashtable(const JanetTable *table) {
    GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);
    int32_t cap = table->capacity;
    const JanetKV *cur = NULL;
    while ((cur = janet_dictionary_next(table->data, cap, cur))) {
        gchar *name = (gchar*)janet_unwrap_string(cur->key);
        gchar *value = (gchar*)janet_unwrap_string(cur->value);
        g_hash_table_insert(hash, name, value);
    }
    return hash;
}

static Janet save_password(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 5);
    GError *error = NULL;

    if (!janet_checktype(argv[0], JANET_STRUCT))
        janet_panic("Schema has to be a Struct\n");
    JanetStruct schema = janet_getstruct(argv, 0);

    if (!janet_checktype(argv[1], JANET_TABLE))
        janet_panic("Attributes has to be a Table\n");
    JanetTable *attributes = janet_gettable(argv, 1);

    if (!janet_checktype(argv[2], JANET_STRING))
        janet_panic("Collection has to be a String\n");
    const gchar *collection = janet_getcstring(argv, 2);

    if (!janet_checktype(argv[3], JANET_STRING))
        janet_panic("Label has to be a String\n");
    const gchar *label = janet_getcstring(argv, 3);

    if (!janet_checktype(argv[4], JANET_STRING))
        janet_panic("Password has to be a String\n");
    const gchar *password = janet_getcstring(argv, 4);

    gchar *name = NULL;
    int32_t flags = -1;
    GHashTable *schema_attributes = NULL;
    extract_schema_variables(schema, &name, &flags, &schema_attributes);

    SecretSchema *the_schema = secret_schema_newv(
        name,
        flags,
        schema_attributes
    );

    GHashTable *pw_attrs = attributes_to_hashtable(attributes);

    gboolean ret = secret_password_storev_sync(
        the_schema,
        pw_attrs,
        collection,
        label,
        password,
        NULL, &error
    );

    g_hash_table_destroy(schema_attributes);
    g_hash_table_destroy(pw_attrs);
    if (error != NULL) {
        g_error_free(error);
        return janet_wrap_false();
    }

    if (ret == TRUE)
        return janet_wrap_true();
    else
        return janet_wrap_false();
}

static Janet lookup_password(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);
    GError *error = NULL;

    if (!janet_checktype(argv[0], JANET_STRUCT))
        janet_panic("Schema has to be a Struct\n");
    JanetStruct schema = janet_getstruct(argv, 0);

    if (!janet_checktype(argv[1], JANET_TABLE))
        janet_panic("Attributes has to be a Table\n");
    JanetTable *attributes = janet_gettable(argv, 1);

    gchar *name = NULL;
    int32_t flags = -1;
    GHashTable *schema_attributes = NULL;
    extract_schema_variables(schema, &name, &flags, &schema_attributes);

    SecretSchema *the_schema = secret_schema_newv(
        name,
        flags,
        schema_attributes
    );

    GHashTable *pw_attributes = attributes_to_hashtable(attributes);

    gchar *password = secret_password_lookupv_sync(
        the_schema,
        pw_attributes,
        NULL,
        &error
    );

    if (error != NULL) {
        g_error_free(error);
        return janet_wrap_nil();
    } else if (password == NULL) {
        return janet_wrap_nil();
    } else {
        JanetString pws = janet_string((uint8_t*) password, strlen(password));
        Janet pw = janet_wrap_string(pws);
        secret_password_free(password);
        return pw;
    }
}

static const JanetReg funs[] = {
    {"save-password", save_password,
        "(secret/save-password)\n\n"
        "A fine documentation."
    },
    {"lookup-password", lookup_password,
        "(secret/lookup-password)\n\n"
        "A fine documentation."
    },
    {NULL, NULL, NULL}
};

JANET_MODULE_ENTRY(JanetTable *env) {
    janet_cfuns(env, "secret", funs);

    janet_def(env, "attribute/INTEGER", janet_wrap_integer(SECRET_SCHEMA_ATTRIBUTE_INTEGER), NULL);
    janet_def(env, "attribute/STRING", janet_wrap_integer(SECRET_SCHEMA_ATTRIBUTE_STRING), NULL);
    janet_def(env, "attribute/BOOLEAN", janet_wrap_integer(SECRET_SCHEMA_ATTRIBUTE_BOOLEAN), NULL);

    janet_def(
        env, "collection/DEFAULT",
        janet_wrap_string(
            janet_string(SECRET_COLLECTION_DEFAULT, strlen(SECRET_COLLECTION_DEFAULT))
        ),
        NULL
    );
    janet_def(
        env, "collection/SESSION",
        janet_wrap_string(
            janet_string(SECRET_COLLECTION_SESSION, strlen(SECRET_COLLECTION_SESSION))
        ),
        NULL
    );

    janet_def(env, "schema/NONE", janet_wrap_integer(SECRET_SCHEMA_NONE), NULL);
    janet_def(env, "schema/DONT-MATCH-NAME", janet_wrap_integer(SECRET_SCHEMA_DONT_MATCH_NAME), NULL);
}

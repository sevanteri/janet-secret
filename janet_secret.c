
#include <glib.h>
#include <libsecret/secret.h>
#include <janet.h>

static const SecretSchema simpleschema = {
    "simple.example.schema", SECRET_SCHEMA_NONE,
    {
        { "NULL", 0 },
    }
};

static Janet save_password(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);
    GError *error = NULL;
    gboolean ret = secret_password_store_sync(
            &simpleschema,
            SECRET_COLLECTION_DEFAULT,
            janet_getcstring(argv, 0), /* label */
            janet_getcstring(argv, 1),
            NULL, &error, NULL);

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
    janet_fixarity(argc, 0);

    GError *error = NULL;

    gchar *password = secret_password_lookup_sync(
            &simpleschema,
            NULL,
            &error,
            NULL
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
}

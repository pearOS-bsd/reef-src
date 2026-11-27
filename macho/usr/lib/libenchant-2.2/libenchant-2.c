// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libenchant: every function looks the host's function of the same name up the first time it is called and jumps to it.

#include <stddef.h>

extern void *reef_host_symbol(const char *library, const char *name);

typedef struct EnchantBroker EnchantBroker;
typedef struct EnchantDict EnchantDict;

#define HOST_LIBRARY "libenchant-2.so.2"

#define FORWARD(result, name, parameters, arguments)                       \
    result name parameters                                                 \
    {                                                                      \
        static result (*hostFunction) parameters;                          \
        if (!hostFunction)                                                 \
            hostFunction = reef_host_symbol(HOST_LIBRARY, #name);          \
        return hostFunction arguments;                                     \
    }

EnchantBroker *enchant_broker_init(void)
{
    static EnchantBroker *(*function)(void);
    if (!function) {
        function = reef_host_symbol(HOST_LIBRARY, "enchant_broker_init");
        if (!function)
            return NULL;
    }
    return function();
}

FORWARD(void, enchant_broker_free, (EnchantBroker *broker), (broker))
FORWARD(EnchantDict *, enchant_broker_request_dict, (EnchantBroker *broker, const char *tag), (broker, tag))
FORWARD(EnchantDict *, enchant_broker_request_pwl_dict, (EnchantBroker *broker, const char *pwl), (broker, pwl))
FORWARD(void, enchant_broker_free_dict, (EnchantBroker *broker, EnchantDict *dict), (broker, dict))
FORWARD(int, enchant_broker_dict_exists, (EnchantBroker *broker, const char *tag), (broker, tag))
FORWARD(void, enchant_broker_list_dicts, (EnchantBroker *broker, void (*callback)(const char *, const char *, const char *, const char *, void *), void *data), (broker, callback, data))
FORWARD(const char *, enchant_broker_get_error, (EnchantBroker *broker), (broker))
FORWARD(int, enchant_dict_check, (EnchantDict *dict, const char *word, long length), (dict, word, length))
FORWARD(char **, enchant_dict_suggest, (EnchantDict *dict, const char *word, long length, size_t *count), (dict, word, length, count))
FORWARD(void, enchant_dict_free_string_list, (EnchantDict *dict, char **list), (dict, list))
FORWARD(void, enchant_dict_add, (EnchantDict *dict, const char *word, long length), (dict, word, length))
FORWARD(void, enchant_dict_add_to_session, (EnchantDict *dict, const char *word, long length), (dict, word, length))
FORWARD(void, enchant_dict_remove, (EnchantDict *dict, const char *word, long length), (dict, word, length))
FORWARD(void, enchant_dict_remove_from_session, (EnchantDict *dict, const char *word, long length), (dict, word, length))
FORWARD(int, enchant_dict_is_added, (EnchantDict *dict, const char *word, long length), (dict, word, length))
FORWARD(int, enchant_dict_is_removed, (EnchantDict *dict, const char *word, long length), (dict, word, length))
FORWARD(const char *, enchant_dict_get_error, (EnchantDict *dict), (dict))

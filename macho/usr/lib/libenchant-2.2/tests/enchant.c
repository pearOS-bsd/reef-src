// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libenchant: the host's library is looked up on first use; without it enchant_broker_init returns NULL (the other functions
// are then not called, they would jump to a missing function), with it a broker is created and asked about the dictionaries.
#include <stdio.h>

typedef struct EnchantBroker EnchantBroker;
EnchantBroker *enchant_broker_init(void);
void enchant_broker_free(EnchantBroker *);
int enchant_broker_dict_exists(EnchantBroker *, const char *);
const char *enchant_broker_get_error(EnchantBroker *);

int main(void)
{
    EnchantBroker *broker = enchant_broker_init();
    printf("broker %s\n", broker ? "created" : "none");
    if (broker) {
        printf("en_US %d\n", enchant_broker_dict_exists(broker, "en_US"));
        printf("no error %d\n", enchant_broker_get_error(broker) == 0);
        enchant_broker_free(broker);
    }
    EnchantBroker *again = enchant_broker_init();
    printf("again %d\n", (again != 0) == (broker != 0));
    return 0;
}

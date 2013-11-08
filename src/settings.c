//add dependency on libuci
//- create settings.c and have it conditionally include uci.h
//- when uci.h is not included, return hardcoded fake values

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "settings.h"

#ifdef USE_LIB_UCI
static struct uci_context *ctx = NULL;
#else
//static const char *dummy_response = "ultimaker";
static const char *dummy_response = "makerbot_generic";
#endif


int settings_init() {
#ifdef USE_LIB_UCI
	if (ctx == NULL) ctx = uci_alloc_context();
	return (ctx != NULL) ? 1 : 0;
#else
	return 1;
#endif
}

int settings_deinit() {
#ifdef USE_LIB_UCI
	if (ctx != NULL) uci_free_context(ctx);
	return 1;
#else
	return 1;
#endif
}

const char *settings_get(const char *uci_spec) {
#ifdef USE_LIB_UCI
	struct uci_ptr ptr;

	//NOTE: make a writable copy (necessary for strdup called inside uci_lookup_ptr)
	char *uci_spec_copy = strdup(uci_spec);

	if (uci_lookup_ptr(ctx, &ptr, uci_spec_copy, true) != UCI_OK) {
		free(uci_spec_copy);
		return NULL;
	}
	free(uci_spec_copy);

	if (!(ptr.flags & UCI_LOOKUP_COMPLETE)) {
		ctx->err = UCI_ERR_NOTFOUND;
		return NULL;
	}

	if (ptr.last->type == UCI_TYPE_OPTION) {
		if (ptr.o->type != UCI_TYPE_STRING) return NULL;
		else return ptr.o->v.string;
	} else {
		return NULL;
	}

#else
	if (strcmp(uci_spec, "wifibox.general.printer_type") == 0) {
		return dummy_response;
	} else {
		return NULL;
	}
#endif
}
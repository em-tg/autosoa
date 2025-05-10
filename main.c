#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include "instantiations.h"

#define arrlen(a) (sizeof(a) / sizeof(a)[0])

int test_soa_vec(struct my_soa_vec *v){
	struct my_soa_vec_entry arr[] = {{
		.a = 12,
		.b = 13,
		.whatever = &v
	},{
		.a = 14,
		.b = 15,
		.whatever = 0
	},{
		.a = 16,
		.b = 17,
		.whatever = "Hello!"
	}};

	for(size_t i = 0; i < arrlen(arr); i++){
		if(!my_soa_vec_append(v, &arr[i])) return 1;
	}

	assert((size_t)v->len == arrlen(arr));
	assert(v->cap >= v->len);
	for(int i = 0; i < v->len; i++){
		struct my_soa_vec_entry_ref item = my_soa_vec_get(v, i);
		assert(arr[i].a == *item.a);
		assert(arr[i].b == *item.b);
		assert(arr[i].whatever == *item.whatever);
	}

	return 0;
}

int main(void){
	struct my_soa_vec v = {0};
	int ret = test_soa_vec(&v);
	my_soa_vec_dtor(&v);

	printf("buffers: %zd\n", offsetof(struct my_soa_vec, len) / sizeof(void *));

	assert(ret == 0);
	return ret;
}




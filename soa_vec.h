/*
A C template library that allows you to easily switch between
array-of-structs and struct-of-arrays.  To use, define your
struct as an x-macro called "FIELDS":

#define FIELDS \
	x(type1, field1) \
	x(type2, field2) \
	x(type3, field3)

Then define the `id` macro to set a namespace:

#define id(n) namespace_ ## n

And include the header.  Define `SOA_VEC_IMPL` to generate function
implementations.  Define `USE_AOS` to switch from struct-of-arrays
to array-of-structs.
*/

#ifndef FIELDS
#error "need FIELDS"
#endif

#ifndef id
#error "need id"
#endif

struct id(soa_vec);

struct id(soa_vec_entry) {
	#define x(t, nm) t nm;
	FIELDS
	#undef x
};

// Append an entry to the vec
int id(soa_vec_append)(struct id(soa_vec) *v, struct id(soa_vec_entry) *src);

// Retrieve an entry from the vec (returned as a struct of pointers to fields of the entry)
struct id(soa_vec_entry_ref) {
	#define x(t, nm) t *nm;
	FIELDS
	#undef x
} id(soa_vec_get)(struct id(soa_vec) *v, int i);

// Destroy the vec
void id(soa_vec_dtor)(struct id(soa_vec) *v);

#ifdef SOA_VEC_IMPL
	#include <stdlib.h>
	#include <stdint.h>
	#include <limits.h>
#endif

#ifdef USE_AOS
	struct id(soa_vec) {
		struct id(soa_vec_entry) *buf;
		int len;
		int cap;
	};

	#ifdef SOA_VEC_IMPL
		static int id(ensure_cap)(struct id(soa_vec) *v, int req_cap){
			if(v->cap >= req_cap) return 1;

			int new_cap = v->cap ? v->cap : 1;
			while(new_cap < req_cap){
				if(INT_MAX / 2 < new_cap) return 0;
				new_cap *= 2;
			}

			if(SIZE_MAX / sizeof v->buf[0] < (size_t)new_cap) return 0;
			size_t bytes = (size_t)new_cap * sizeof v->buf[0];
			void *new_buf = realloc(v->buf, bytes);
			if(!new_buf) return 0;
			v->buf = new_buf;
			v->cap = new_cap;
			return 1;
		}

		int id(soa_vec_append)(struct id(soa_vec) *v, struct id(soa_vec_entry) *src){
			if(v->len == INT_MAX) return 0;
			if(!id(ensure_cap)(v, v->len + 1)) return 0;

			v->buf[v->len++] = *src;
			return 1;
		}

		struct id(soa_vec_entry_ref) id(soa_vec_get)(struct id(soa_vec) *v, int i){
			return (struct id(soa_vec_entry_ref)){
				#define x(t, nm) .nm = &v->buf[i].nm,
				FIELDS
				#undef x
			};
		}

		void id(soa_vec_dtor)(struct id(soa_vec) *v){
			free(v->buf);
			*v = (struct id(soa_vec)){0};
		}
	#endif
#else
	struct id(soa_vec) {
		struct id(soa_vec_inner) {
			#define x(t, nm) t *nm;
			FIELDS
			#undef x
		} inner;
		int len;
		int cap;
	};

	#ifdef SOA_VEC_IMPL
		static int id(ensure_cap_buf)(void **buf, size_t req_cap, size_t elem_size){
			if(SIZE_MAX / elem_size < req_cap) return 0;
			size_t bytes = req_cap * elem_size;
			void *new_buf = realloc(*buf, bytes);
			if(!new_buf) return 0;
			*buf = new_buf;
			return 1;
		}

		static int id(ensure_cap)(struct id(soa_vec) *v, int req_cap){
			if(req_cap <= v->cap) return 1;
			int new_cap = v->cap ? v->cap : 1;
			while(new_cap < req_cap){
				if(INT_MAX / 2 < new_cap) return 0;
				new_cap *= 2;
			}

			#define x(t, nm) if(!id(ensure_cap_buf)((void **)&v->inner.nm, (size_t)new_cap, sizeof(t))) return 0;
			FIELDS
			#undef x

			v->cap = new_cap;
			return 1;
		}

		int id(soa_vec_append)(struct id(soa_vec) *v, struct id(soa_vec_entry) *src){
			if(v->len == INT_MAX) return 0;
			if(!id(ensure_cap)(v, v->len + 1)) return 0;

			#define x(t, nm) v->inner.nm[v->len] = src->nm;
			FIELDS
			#undef x
			v->len += 1;
			return 1;
		}

		struct id(soa_vec_entry_ref) id(soa_vec_get)(struct id(soa_vec) *v, int i){
			return (struct id(soa_vec_entry_ref)){
				#define x(t, nm) .nm = &v->inner.nm[i],
				FIELDS
				#undef x
			};
		}

		void id(soa_vec_dtor)(struct id(soa_vec) *v){
			#define x(t, nm) free(v->inner.nm);
			FIELDS
			#undef x
			*v = (struct id(soa_vec)){0};
		}
	#endif
#endif

#undef FIELDS
#undef id

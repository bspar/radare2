#ifndef R2_REG_H
#define R2_REG_H

#include <r_types.h>
#include <r_util.h>
#include <list.h>

R_LIB_VERSION_HEADER (r_reg);

enum {
	R_REG_TYPE_GPR,
	R_REG_TYPE_DRX,
	R_REG_TYPE_FPU,
	R_REG_TYPE_MMX,
	R_REG_TYPE_XMM,
	R_REG_TYPE_FLG,
	R_REG_TYPE_SEG,
	R_REG_TYPE_LAST,
	R_REG_TYPE_ALL = -1, // TODO; rename to ANY
};

enum {
	R_REG_NAME_PC, // program counter
	R_REG_NAME_SP, // stack pointer
	R_REG_NAME_SR, // status register
	R_REG_NAME_BP, // base pointer
	/* args */
	R_REG_NAME_A0, // arguments
	R_REG_NAME_A1,
	R_REG_NAME_A2,
	R_REG_NAME_A3,
	R_REG_NAME_A4,
	R_REG_NAME_A5,
	R_REG_NAME_A6,
	/* retval */
	R_REG_NAME_R0, // arguments
	R_REG_NAME_R1,
	R_REG_NAME_R2,
	R_REG_NAME_R3,
	/* flags */
	R_REG_NAME_ZF,
	R_REG_NAME_SF,
	R_REG_NAME_CF,
	R_REG_NAME_OF,
	/* syscall number (orig_eax,rax,r0,x0) */
	R_REG_NAME_SN,
	R_REG_NAME_LAST,
};

// TODO: use enum here?
#define R_REG_COND_EQ 0
#define R_REG_COND_NE 1
#define R_REG_COND_CF 2
#define R_REG_COND_CARRY 2
#define R_REG_COND_NEG 3
#define R_REG_COND_NEGATIVE 3
#define R_REG_COND_OF 4
#define R_REG_COND_OVERFLOW 4
// unsigned
#define R_REG_COND_HI 5
#define R_REG_COND_HE 6
#define R_REG_COND_LO 7
#define R_REG_COND_LOE 8
// signed
#define R_REG_COND_GE 9
#define R_REG_COND_GT 10
#define R_REG_COND_LT 11
#define R_REG_COND_LE 12
#define R_REG_COND_LAST 13

typedef struct r_reg_item_t {
	char *name;
	int type;
	int size;	/* 8,16,32,64 ... 128/256 ??? */
	int offset;      // offset in data structure
	int packed_size; /* 0 means no packed register, 1byte pack, 2b pack... */
	bool is_float;
	char *flags;
} RRegItem;

typedef struct r_reg_arena_t {
	ut8 *bytes;
	int size;
} RRegArena;

typedef struct r_reg_set_t {
	RRegArena *arena;
	RList *pool; /* RRegArena */
	RList *regs; /* RRegItem */
} RRegSet;

typedef struct r_reg_t {
	char *profile;
	char *reg_profile_str;
	char *name[R_REG_NAME_LAST];
	RRegSet regset[R_REG_TYPE_LAST];
	int iters;
	int arch;
	int bits;
	int size;
	bool is_thumb;
	bool big_endian;
} RReg;

typedef struct r_reg_flags_t {
	bool s; // sign, negative number (msb)
	bool z; // zero
	bool a; // half-carry adjust (if carry happens at nibble level)
	bool c; // carry
	bool o; // overflow
	bool p; // parity (lsb)
} RRegFlags;

#ifdef R_API
R_API void r_reg_free(RReg *reg);
R_API void r_reg_free_internal(RReg *reg);
R_API RReg *r_reg_new(void);
R_API int r_reg_set_name(RReg *reg, int role, const char *name);
R_API int r_reg_set_profile_string(RReg *reg, const char *profile);
R_API int r_reg_set_profile(RReg *reg, const char *profile);

R_API RRegSet *r_reg_regset_get(RReg *r, int type);
R_API ut64 r_reg_getv(RReg *reg, const char *name);
R_API ut64 r_reg_setv(RReg *reg, const char *name, ut64 val);
R_API const char *r_reg_get_type(int idx);
R_API const char *r_reg_get_name(RReg *reg, int kind);
R_API const char *r_reg_get_role(int role);
R_API RRegItem *r_reg_get(RReg *reg, const char *name, int type);
R_API RList *r_reg_get_list(RReg *reg, int type);
R_API RRegItem *r_reg_get_at(RReg *reg, int type, int regsize, int delta);
R_API RRegItem *r_reg_next_diff(RReg *reg, int type, const ut8 *buf, int buflen, RRegItem *prev_ri, int regsize);

/* Item */
R_API void r_reg_item_free(RRegItem *item);

/* XXX: dupped ?? */
R_API int r_reg_type_by_name(const char *str);
R_API int r_reg_get_name_idx(const char *type);

R_API RRegItem *r_reg_cond_get(RReg *reg, const char *name);
R_API int r_reg_cond_get_value(RReg *r, const char *name);
R_API int r_reg_cond_bits(RReg *r, int type, RRegFlags *f);
R_API RRegFlags *r_reg_cond_retrieve(RReg *r, RRegFlags *);
R_API int r_reg_cond(RReg *r, int type);

/* integer value 8-64 bits */
R_API ut64 r_reg_get_value(RReg *reg, RRegItem *item);
R_API bool r_reg_set_value(RReg *reg, RRegItem *item, ut64 value);

/* float */
R_API float r_reg_get_float(RReg *reg, RRegItem *item);
R_API bool r_reg_set_float(RReg *reg, RRegItem *item, float value);

/* double */
R_API double r_reg_get_double(RReg *reg, RRegItem *item);
R_API bool r_reg_set_double(RReg *reg, RRegItem *item, double value);

/* long double */
R_API long double r_reg_get_longdouble(RReg *reg, RRegItem *item);
R_API bool r_reg_set_longdouble(RReg *reg, RRegItem *item, long double value);

/* boolean */
R_API char *r_reg_get_bvalue(RReg *reg, RRegItem *item);
R_API ut64 r_reg_set_bvalue(RReg *reg, RRegItem *item, const char *str);

/* packed registers */
R_API int r_reg_set_pack(RReg *reg, RRegItem *item, int packidx, int packbits, ut64 val);
R_API ut64 r_reg_get_pack(RReg *reg, RRegItem *item, int packidx, int packbits);

/* byte arena */
R_API ut8 *r_reg_get_bytes(RReg *reg, int type, int *size);
R_API bool r_reg_set_bytes(RReg *reg, int type, const ut8 *buf, const int len);
R_API RRegArena *r_reg_arena_new(int size);
R_API void r_reg_arena_free(RRegArena *ra);
R_API int r_reg_fit_arena(RReg *reg);
R_API int r_reg_arena_set(RReg *reg, int n, int copy);
R_API void r_reg_arena_swap(RReg *reg, int copy);
R_API int r_reg_arena_push(RReg *reg);
R_API void r_reg_arena_pop(RReg *reg);
R_API void r_reg_arena_zero(RReg *reg);

R_API ut8 *r_reg_arena_peek(RReg *reg);
R_API void r_reg_arena_poke(RReg *reg, const ut8 *buf);
R_API ut64 r_reg_cmp(RReg *reg, RRegItem *item);
R_API const char *r_reg_cond_to_string(int n);
R_API int r_reg_cond_from_string(const char *str);
#endif

#endif

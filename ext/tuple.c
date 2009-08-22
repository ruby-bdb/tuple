#include "ruby.h"

VALUE mTuple;

#define TRUE_SORT  255 // TrueClass
#define TIME_SORT  128 // Time
#define SYM_SORT    64 // Symbol
#define STR_SORT    32 // String
#define INTP_SORT   16 // Integer (Positive)
#define INTN_SORT    8 // Integer (Negative)
#define FALSE_SORT   1 // FalseClass
#define NIL_SORT     0 // NilClass

#define MAX_INT 0xffffffff

#define BDIGITS(x) ((BDIGIT*)RBIGNUM(x)->digits)

static void null_pad(VALUE data, int len) {
  static u_int8_t null = 0;

  // Pad with null bytes so subsequent fields will be aligned.
  while (len % 4 != 0) {
    rb_str_cat(data, (char*)&null, 1);
    len++;
  }
}

/*
 * call-seq:
 * Tuple.dump(tuple) -> string
 *
 * Dumps an array of simple Ruby types into a string of binary data.
 *
 */
static VALUE tuple_dump(VALUE self, VALUE tuple) {
  VALUE data = rb_str_new2("");
  VALUE item;
  int i, j, len, sign;
  u_int8_t header[4];
  u_int32_t digit;
  BDIGIT *digits;

  if (TYPE(tuple) != T_ARRAY) tuple = rb_ary_new4(1, &tuple);

  for (i = 0; i < RARRAY(tuple)->len; i++) {
    item = RARRAY(tuple)->ptr[i];
    header[0] = header[1] = header[2] = header[3] = 0;
    if (FIXNUM_P(item)) {
      sign = (digit >= 0);
      header[2] = (sign ? INTP_SORT : INTN_SORT);
      header[3] = 0;
      rb_str_cat(data, (char*)&header, sizeof(header));

      digit = htonl(FIX2INT(item) + (sign ? 0 : MAX_INT));
      rb_str_cat(data, (char*)&digit,  sizeof(digit));
    } else if (TYPE(item) == T_BIGNUM) {
      sign = RBIGNUM(item)->sign;
      header[2] = sign ? INTP_SORT : INTN_SORT;
      header[3] = RBIGNUM(item)->len;
      rb_str_cat(data, (char*)&header, sizeof(header));

      digits = BDIGITS(item);
      for (j = header[3]-1; j >= 0; j--) {
        digit = htonl(sign ? digits[j] : (MAX_INT - digits[j]));
        rb_str_cat(data, (char*)&digit, sizeof(digit));
      }
    } else if (SYMBOL_P(item) || TYPE(item) == T_STRING) {
      if (SYMBOL_P(item)) {
        header[2] = SYM_SORT;
        item = rb_funcall(item, rb_intern("to_s"), 0);
      } else {
        header[2] = STR_SORT;
      }
      rb_str_cat(data, (char*)&header, sizeof(header));
      len = RSTRING_LEN(item);
      rb_str_cat(data, RSTRING_PTR(item), len);
      
      null_pad(data, len);
    } else if (rb_obj_class(item) == rb_cTime) {
      header[2] = TIME_SORT;
      rb_str_cat(data, (char*)&header, sizeof(header));

      item = rb_funcall(item, rb_intern("getgm"), 0);
      item = rb_funcall(item, rb_intern("strftime"), 1, rb_str_new2("%Y/%m/%d %H:%M:%S +0000"));
      len  = RSTRING_LEN(item);
      rb_str_cat(data, RSTRING_PTR(item), len);

      null_pad(data, len);
    } else {        
      if      (item == Qnil)   header[2] = NIL_SORT;
      else if (item == Qtrue)  header[2] = TRUE_SORT;
      else if (item == Qfalse) header[2] = FALSE_SORT;
      else    rb_raise(rb_eTypeError, "invalid type %s in tuple", rb_obj_classname(item));
      
      rb_str_cat(data, (char*)&header, sizeof(header));      
    }
  }
  return data;
}

/*
 * call-seq:
 * Tuple.load(string) -> tuple
 *
 * Reads in a previously dumped tuple from a string of binary data.
 *
 */
static VALUE tuple_load(VALUE self, VALUE data) {
  VALUE tuple = rb_ary_new();
  VALUE item;
  data = StringValue(data);
  void* ptr = RSTRING_PTR(data); 
  void* end = ptr + RSTRING_LEN(data);
  int i, len, sign;
  u_int8_t header[4];
  u_int32_t digit;
  BDIGIT *digits;

  while (ptr < end) {
    memcpy(header, ptr, 4);
    ptr += 4;

    switch(header[2]) {
    case TRUE_SORT:  rb_ary_push(tuple, Qtrue);  break;
    case FALSE_SORT: rb_ary_push(tuple, Qfalse); break;
    case NIL_SORT:   rb_ary_push(tuple, Qnil);   break;
    case INTP_SORT:
    case INTN_SORT:
      sign = (header[2] == INTP_SORT);
      if (header[3] == 0) {
        digit = ntohl(*(u_int32_t*)ptr) - (sign ? 0 : MAX_INT);
        ptr += 4;
        rb_ary_push(tuple, INT2NUM(digit));
      } else {
        // Create an empty bignum with the right number of digits.
        NEWOBJ(item, struct RBignum);
        OBJSETUP(item, rb_cBignum, T_BIGNUM);        
        item->sign = sign ? 1 : 0;
        item->len = header[3];
        item->digits = ALLOC_N(BDIGIT, header[3]);

        digits = BDIGITS(item);
        for (i = header[3]-1; i >= 0; i--) {
          digit = ntohl(*(u_int32_t*)ptr);
          digits[i] = sign ? digit : MAX_INT - digit;
          ptr += 4;
        }
        rb_ary_push(tuple, (VALUE)item);
      }
      break;
    case STR_SORT:
    case SYM_SORT:
      item = rb_str_new2(ptr);
      len  = RSTRING_LEN(item);
      while (len % 4 != 0) len++;
      ptr += len;
      if (header[2] == SYM_SORT) item = rb_funcall(item, rb_intern("to_sym"), 0);
      rb_ary_push(tuple, item);
      break;
    case TIME_SORT:
      item = rb_str_new2(ptr);
      len  = RSTRING_LEN(item);
      while (len % 4 != 0) len++;
      ptr += len;
      item = rb_funcall(rb_cTime, rb_intern("parse"), 1, item);
      rb_ary_push(tuple, item);
      break;
    default:
      rb_raise(rb_eTypeError, "invalid type code %d in tuple", header[2]);
      break;
    }
  }  
  return tuple;
}

VALUE mTuple;
void Init_tuple() {
  rb_require("time");

  mTuple = rb_define_module("Tuple");
  rb_define_module_function(mTuple, "dump", tuple_dump, 1);
  rb_define_module_function(mTuple, "load", tuple_load, 1);
}

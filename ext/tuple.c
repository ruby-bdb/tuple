#include "ruby.h"

VALUE mTuple;

#define TRUE_SORT  128 // TrueClass
#define SYM_SORT    64 // Symbol
#define STR_SORT    32 // String
#define INTP_SORT   16 // Integer (Positive)
#define INTN_SORT    8 // Integer (Negative)
#define FALSE_SORT   1 // FalseClass
#define NIL_SORT     0 // NilClass

#define MAX_INT 0xffffffff

#define BDIGITS(x) ((BDIGIT*)RBIGNUM(x)->digits)

static VALUE tuple_dump(VALUE self, VALUE tuple) {
  tuple = rb_check_array_type(tuple);
  VALUE data = rb_str_new2("");
  VALUE item;
  int i, j, len, sign;
  u_int8_t header[4];
  u_int32_t digit;
  BDIGIT *digits;

  for (i = 0; i < RARRAY(tuple)->len; i++) {
    item = RARRAY(tuple)->ptr[i];
    header[0] = header[1] = header[2] = header[3] = 0;
    if (FIXNUM_P(item)) {
      sign = (digit >= 0);
      header[2] = (sign ? INTP_SORT : INTN_SORT);
      header[3] = 1;
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
      header[2] = SYMBOL_P(item) ? SYM_SORT : STR_SORT;

      rb_str_cat(data, (char*)&header, sizeof(header));
      len = RSTRING_LEN(item);
      rb_str_cat(data, RSTRING_PTR(item), len);
      
      // Pad with null bytes so subsequent fields will be aligned.
      while (len % 4 != 0) {
        rb_str_cat(data, (char*)&header, 1);
        len++;
      }
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
      if (header[3] == 1) {
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
      len = RSTRING_LEN(item);
      while (len % 4 != 0) len++;
      item = header[2] == STR_SORT ? item : rb_funcall(item, rb_intern("to_sym"), 0);
      rb_ary_push(tuple, item);
      ptr += len;
      break;
    }
  }  
  return tuple;
}

VALUE mTuple;
void Init_tuple() {
  mTuple = rb_define_module("Tuple");
  rb_define_module_function(mTuple, "dump", tuple_dump, 1);
  rb_define_module_function(mTuple, "load", tuple_load, 1);
}

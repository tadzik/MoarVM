#define MVM_encoding_type_MIN       1
#define MVM_encoding_type_utf8      1
#define MVM_encoding_type_ascii     2
#define MVM_encoding_type_latin1    3
#define MVM_encoding_type_utf16     4
#define MVM_encoding_type_MAX       4
#define ENCODING_VALID(enc) \
    (((enc) >= MVM_encoding_type_MIN && (enc) <= MVM_encoding_type_MAX) \
    || (MVM_exception_throw_adhoc(tc, "invalid encoding type flag: %d", (enc)),1))

/* substring consumer functions accept a state object in *data and
    consume a substring portion. Utilized by many of the string ops
    so traversal state can be maintained while applying a function to
    each subsection of a string. Accepts physical strings only. (non-
    ropes). Each function should branch for the ascii and wide modes
    so there doesn't have to be a function call on every codepoint.
    Returns nonzero if the traverser is supposed to stop traversal
    early. See how it's used in ops.c */
#define MVM_SUBSTRING_CONSUMER(name) MVMuint8 name(MVMThreadContext *tc, \
    MVMString *string, MVMStringIndex start, MVMStringIndex length, MVMStringIndex top_index, void *data)
typedef MVM_SUBSTRING_CONSUMER((*MVMSubstringConsumer));

/* number of grahemes in the string */
#define NUM_ROPE_GRAPHS(str) ((str)->body.num_strands ? (str)->body.strands[(str)->body.num_strands].graphs : 0)
#define NUM_GRAPHS(str) (IS_ROPE((str)) ? NUM_ROPE_GRAPHS((str)) : (str)->body.graphs)
/* gets the code that defines the type of string. More things could be
    stored in flags later. */
#define STR_FLAGS(str) (((MVMString *)(str))->body.flags & MVM_STRING_TYPE_MASK)
/* whether it's a string of full-blown 4-byte (positive and/or negative)
    codepoints. */
#define IS_WIDE(str) (STR_FLAGS((str)) == MVM_STRING_TYPE_INT32)
/* whether it's a string of only codepoints that fit in 8 bits, so
    are stored compactly in a byte array. */
#define IS_ASCII(str) (STR_FLAGS((str)) == MVM_STRING_TYPE_UINT8)
/* whether it's a composite of strand segments */
#define IS_ROPE(str) (STR_FLAGS((str)) == MVM_STRING_TYPE_ROPE)
/* potentially lvalue version of the below */
#define _STRAND_DEPTH(str) ((str)->body.strands[(str)->body.num_strands].strand_depth)
/* the max number of levels deep the rope tree goes */
#define STRAND_DEPTH(str) ((IS_ROPE((str)) && NUM_ROPE_GRAPHS(str)) ? _STRAND_DEPTH((str)) : 0)
/* whether the rope is composed of only one segment of another string */
#define IS_ONE_STRING_ROPE(str) (IS_ROPE((str)) && (str)->body.num_strands == 1)

struct MVMConcatState {
    MVMuint32 some_state;
};

/* Character class constants (map to nqp::const::CCLASS_* values). */
#define MVM_CCLASS_ANY          65535
#define MVM_CCLASS_UPPERCASE    1
#define MVM_CCLASS_LOWERCASE    2
#define MVM_CCLASS_ALPHABETIC   4
#define MVM_CCLASS_NUMERIC      8
#define MVM_CCLASS_HEXADECIMAL  16
#define MVM_CCLASS_WHITESPACE   32
#define MVM_CCLASS_PRINTING     64
#define MVM_CCLASS_BLANK        256
#define MVM_CCLASS_CONTROL      512
#define MVM_CCLASS_PUNCTUATION  1024
#define MVM_CCLASS_ALPHANUMERIC 2048
#define MVM_CCLASS_NEWLINE      4096
#define MVM_CCLASS_WORD         8192

MVMCodepoint32 MVM_string_get_codepoint_at_nocheck(MVMThreadContext *tc, MVMString *a, MVMint64 index);
MVMint64 MVM_string_equal(MVMThreadContext *tc, MVMString *a, MVMString *b);
MVMint64 MVM_string_index(MVMThreadContext *tc, MVMString *haystack, MVMString *needle, MVMint64 start);
MVMint64 MVM_string_index_from_end(MVMThreadContext *tc, MVMString *haystack, MVMString *needle, MVMint64 start);
MVMString * MVM_string_concatenate(MVMThreadContext *tc, MVMString *a, MVMString *b);
MVMString * MVM_string_repeat(MVMThreadContext *tc, MVMString *a, MVMint64 count);
MVMString * MVM_string_substring(MVMThreadContext *tc, MVMString *a, MVMint64 start, MVMint64 length);
MVMString * MVM_string_replace(MVMThreadContext *tc, MVMString *a, MVMint64 start, MVMint64 length, MVMString *replacement);
void MVM_string_say(MVMThreadContext *tc, MVMString *a);
void MVM_string_print(MVMThreadContext *tc, MVMString *a);
MVMint64 MVM_string_equal_at(MVMThreadContext *tc, MVMString *a, MVMString *b, MVMint64 offset);
MVMint64 MVM_string_equal_at_ignore_case(MVMThreadContext *tc, MVMString *a, MVMString *b, MVMint64 offset);
MVMint64 MVM_string_have_at(MVMThreadContext *tc, MVMString *a, MVMint64 starta, MVMint64 length, MVMString *b, MVMint64 startb);
MVMint64 MVM_string_get_codepoint_at(MVMThreadContext *tc, MVMString *a, MVMint64 index);
MVMint64 MVM_string_index_of_codepoint(MVMThreadContext *tc, MVMString *a, MVMint64 codepoint);
MVMString * MVM_string_uc(MVMThreadContext *tc, MVMString *s);
MVMString * MVM_string_lc(MVMThreadContext *tc, MVMString *s);
MVMString * MVM_string_tc(MVMThreadContext *tc, MVMString *s);
MVMString * MVM_string_decode(MVMThreadContext *tc, MVMObject *type_object, char *Cbuf, MVMint64 byte_length, MVMint64 encoding_flag);
MVMuint8 * MVM_string_encode(MVMThreadContext *tc, MVMString *s, MVMint64 start, MVMint64 length, MVMuint64 *output_size, MVMint64 encoding_flag);
void MVM_string_encode_to_buf(MVMThreadContext *tc, MVMString *s, MVMString *enc_name, MVMObject *buf);
MVMString * MVM_string_decode_from_buf(MVMThreadContext *tc, MVMObject *buf, MVMString *enc_name);
MVMObject * MVM_string_split(MVMThreadContext *tc, MVMString *separator, MVMString *input);
MVMString * MVM_string_join(MVMThreadContext *tc, MVMString *separator, MVMObject *input);
MVMint64 MVM_string_char_at_in_string(MVMThreadContext *tc, MVMString *a, MVMint64 offset, MVMString *b);
MVMint64 MVM_string_offset_has_unicode_property_value(MVMThreadContext *tc, MVMString *s, MVMint64 offset, MVMint64 property_code, MVMint64 property_value_code);
void MVM_string_flatten(MVMThreadContext *tc, MVMString *s);
MVMString * MVM_string_escape(MVMThreadContext *tc, MVMString *s);
MVMString * MVM_string_flip(MVMThreadContext *tc, MVMString *s);
MVMint64 MVM_string_compare(MVMThreadContext *tc, MVMString *a, MVMString *b);
MVMString * MVM_string_bitand(MVMThreadContext *tc, MVMString *a, MVMString *b);
MVMString * MVM_string_bitor(MVMThreadContext *tc, MVMString *a, MVMString *b);
MVMString * MVM_string_bitxor(MVMThreadContext *tc, MVMString *a, MVMString *b);
void MVM_string_cclass_init(MVMThreadContext *tc);
MVMint64 MVM_string_is_cclass(MVMThreadContext *tc, MVMint64 cclass, MVMString *s, MVMint64 offset);
MVMint64 MVM_string_find_cclass(MVMThreadContext *tc, MVMint64 cclass, MVMString *s, MVMint64 offset, MVMint64 count);
MVMint64 MVM_string_find_not_cclass(MVMThreadContext *tc, MVMint64 cclass, MVMString *s, MVMint64 offset, MVMint64 count);
MVMuint8 MVM_string_find_encoding(MVMThreadContext *tc, MVMString *name);

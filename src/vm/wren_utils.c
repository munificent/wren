#include <string.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>

#include "wren_utils.h"
#include "wren_vm.h"

DEFINE_BUFFER(Byte, uint8_t);
DEFINE_BUFFER(Int, int);
DEFINE_BUFFER(String, ObjString*);

void wrenSymbolTableInit(SymbolTable* symbols)
{
  wrenStringBufferInit(symbols);
}

void wrenSymbolTableClear(WrenVM* vm, SymbolTable* symbols)
{
  wrenStringBufferClear(vm, symbols);
}

int wrenSymbolTableAdd(WrenVM* vm, SymbolTable* symbols,
                       const char* name, size_t length)
{
  ObjString* symbol = AS_STRING(wrenNewStringLength(vm, name, length));
  
  wrenPushRoot(vm, &symbol->obj);
  wrenStringBufferWrite(vm, symbols, symbol);
  wrenPopRoot(vm);
  
  return symbols->count - 1;
}

int wrenSymbolTableEnsure(WrenVM* vm, SymbolTable* symbols,
                          const char* name, size_t length)
{
  // See if the symbol is already defined.
  int existing = wrenSymbolTableFind(symbols, name, length);
  if (existing != -1) return existing;

  // New symbol, so add it.
  return wrenSymbolTableAdd(vm, symbols, name, length);
}

int wrenSymbolTableFind(const SymbolTable* symbols,
                        const char* name, size_t length)
{
  // See if the symbol is already defined.
  // TODO: O(n). Do something better.
  for (int i = 0; i < symbols->count; i++)
  {
    if (wrenStringEqualsCString(symbols->data[i], name, length)) return i;
  }

  return -1;
}

void wrenBlackenSymbolTable(WrenVM* vm, SymbolTable* symbolTable)
{
  for (int i = 0; i < symbolTable->count; i++)
  {
    wrenGrayObj(vm, &symbolTable->data[i]->obj);
  }
  
  // Keep track of how much memory is still in use.
  vm->bytesAllocated += symbolTable->capacity * sizeof(*symbolTable->data);
}

int wrenUtf8EncodeNumBytes(int value)
{
  ASSERT(value >= 0, "Cannot encode a negative value.");
  
  if (value <= 0x7f) return 1;
  if (value <= 0x7ff) return 2;
  if (value <= 0xffff) return 3;
  if (value <= 0x10ffff) return 4;
  return 0;
}

int wrenUtf8Encode(int value, uint8_t* bytes)
{
  if (value <= 0x7f)
  {
    // Single byte (i.e. fits in ASCII).
    *bytes = value & 0x7f;
    return 1;
  }
  else if (value <= 0x7ff)
  {
    // Two byte sequence: 110xxxxx 10xxxxxx.
    *bytes = 0xc0 | ((value & 0x7c0) >> 6);
    bytes++;
    *bytes = 0x80 | (value & 0x3f);
    return 2;
  }
  else if (value <= 0xffff)
  {
    // Three byte sequence: 1110xxxx 10xxxxxx 10xxxxxx.
    *bytes = 0xe0 | ((value & 0xf000) >> 12);
    bytes++;
    *bytes = 0x80 | ((value & 0xfc0) >> 6);
    bytes++;
    *bytes = 0x80 | (value & 0x3f);
    return 3;
  }
  else if (value <= 0x10ffff)
  {
    // Four byte sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx.
    *bytes = 0xf0 | ((value & 0x1c0000) >> 18);
    bytes++;
    *bytes = 0x80 | ((value & 0x3f000) >> 12);
    bytes++;
    *bytes = 0x80 | ((value & 0xfc0) >> 6);
    bytes++;
    *bytes = 0x80 | (value & 0x3f);
    return 4;
  }

  // Invalid Unicode value. See: http://tools.ietf.org/html/rfc3629
  UNREACHABLE();
  return 0;
}

int wrenUtf8Decode(const uint8_t* bytes, uint32_t length)
{
  // Single byte (i.e. fits in ASCII).
  if (*bytes <= 0x7f) return *bytes;

  int value;
  uint32_t remainingBytes;
  if ((*bytes & 0xe0) == 0xc0)
  {
    // Two byte sequence: 110xxxxx 10xxxxxx.
    value = *bytes & 0x1f;
    remainingBytes = 1;
  }
  else if ((*bytes & 0xf0) == 0xe0)
  {
    // Three byte sequence: 1110xxxx	 10xxxxxx 10xxxxxx.
    value = *bytes & 0x0f;
    remainingBytes = 2;
  }
  else if ((*bytes & 0xf8) == 0xf0)
  {
    // Four byte sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx.
    value = *bytes & 0x07;
    remainingBytes = 3;
  }
  else
  {
    // Invalid UTF-8 sequence.
    return -1;
  }

  // Don't read past the end of the buffer on truncated UTF-8.
  if (remainingBytes > length - 1) return -1;

  while (remainingBytes > 0)
  {
    bytes++;
    remainingBytes--;

    // Remaining bytes must be of form 10xxxxxx.
    if ((*bytes & 0xc0) != 0x80) return -1;

    value = value << 6 | (*bytes & 0x3f);
  }

  return value;
}

int wrenUtf8DecodeNumBytes(uint8_t byte)
{
  // If the byte starts with 10xxxxx, it's the middle of a UTF-8 sequence, so
  // don't count it at all.
  if ((byte & 0xc0) == 0x80) return 0;
  
  // The first byte's high bits tell us how many bytes are in the UTF-8
  // sequence.
  if ((byte & 0xf8) == 0xf0) return 4;
  if ((byte & 0xf0) == 0xe0) return 3;
  if ((byte & 0xe0) == 0xc0) return 2;
  return 1;
}

// From: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2Float
int wrenPowerOf2Ceil(int n)
{
  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n++;
  
  return n;
}

uint32_t wrenValidateIndex(uint32_t count, int64_t value)
{
  // Negative indices count from the end.
  if (value < 0) value = count + value;

  // Check bounds.
  if (value >= 0 && value < count) return (uint32_t)value;

  return UINT32_MAX;
}

// MaxMantissaDigits is the maximum amount of digits that can be preserved in
// the mantissa of a float64 per base. Bases that are greater than 36 or less
// than 2 are invalid. The values are found by `ceil(log[base](2^53 - 1))`.
int static const maxMantissaDigits[] = {
    /* 2*/ 53, /* 3*/ 34, /* 4*/ 27, /* 5*/ 23, /* 6*/ 21, /* 7*/ 19, /* 8*/ 18,
    /* 9*/ 17, /*10*/ 16, /*11*/ 16, /*12*/ 15, /*13*/ 15, /*14*/ 14, /*15*/ 14,
    /*16*/ 14, /*17*/ 13, /*18*/ 13, /*19*/ 13, /*20*/ 13, /*21*/ 13, /*22*/ 12,
    /*23*/ 12, /*24*/ 12, /*25*/ 12, /*26*/ 12, /*27*/ 12, /*28*/ 12, /*29*/ 11,
    /*30*/ 11, /*31*/ 11, /*32*/ 11, /*33*/ 11, /*34*/ 11, /*35*/ 11, /*36*/ 11,
};

void static wrenParseNumError(int count, const char* err,
                                 wrenParseNumResults* results)
{
  results->consumed = count;
  results->errorMessage = err;
  results->value = 0;
}

void wrenParseNum(const char* str, int base, wrenParseNumResults* results)
{
  int i = 0;
  bool hasDigits = false;
  bool neg = false;
  char c = str[i];
  // A double is limited in the amount of digits it can represent exactly. We
  // can keep a count of how many digits we have and ignore the rest when we
  // reach our limit
  int maxMantissa;

  // Check sign.
  if (c == '-')
  {
    neg = true;
    c = str[++i];
  }
  else if (c == '+') c = str[++i];
  if (base == 0)
  {
    // Base unset. Check for prefix or default to base 10.
    base = 10;
    maxMantissa = maxMantissaDigits[10 - 2];
    if (c == '0')
    {
      switch (c = str[++i])
      {
        case 'x':
          base = 16;
          c = str[++i];
          maxMantissa = maxMantissaDigits[16 - 2];
          break;
        case 'o':
          base = 8;
          c = str[++i];
          maxMantissa = maxMantissaDigits[8 - 2];
          break;
        case 'b':
          base = 2;
          c = str[++i];
          maxMantissa = maxMantissaDigits[2 - 2];
          break;
        default:
          // this number is base 10 so treat the '0' as a digit.
          hasDigits = true;
      }
    }
  }
  else
  {
    // If base is set, make sure that it is valid.
    if (base > 36) return wrenParseNumError(i, "Base is to high.", results);
    else if (base < 2) return wrenParseNumError(i, "Base is to low.", results);
    // if the base may have a prefix, skip past the prefix.
    if (c == '0')
    {
      c = str[++i];
      switch (base)
      {
        case 16:
          if (c == 'x') c = str[++i];
          break;
        case 8:
          if (c == 'o') c = str[++i];
          break;
        case 2:
          if (c == 'b') c = str[++i];
          break;
        default:
          hasDigits = true;
      }
    }
    maxMantissa = maxMantissaDigits[base - 2];
  }

  int e = 0;
  long long num = 0;
  int mantissaDigits = 0;
  // Parse the integer part of the number.
  for (;;)
  {
    int t;
    if (isdigit(c)) t = c - '0';
    else if (islower(c)) t = c - ('a' - 10);
    else if (isupper(c)) t = c - ('A' - 10);
    else if (c == '_')
    {
      c = str[++i];
      continue;
    }
    else break;
    if (t >= base) break;

    hasDigits = true;
    // If we have reached our quota of digits, we can ignore the rest and
    // increment our exponent.
    if (mantissaDigits < maxMantissa)
    {
      num = num * base + t;
      if (num > 0) mantissaDigits++;
    }
    else if (e < INT_MAX) e++;
    else return wrenParseNumError(i, "Too many digits.", results);
    c = str[++i];
  }

  // Only parse the fractional and the exponential part of the number if the
  // base is 10.
  if (base == 10)
  {
    // Parse the decimal part of the number. The decimal point must be followed
    // by a digit or else the decimal point may actually be a fuction call.
    if (c == '.' && isdigit(str[i + 1]))
    {
      c = str[++i];
      for (;;)
      {
        if (isdigit(c))
        {
          // If we have reached our quota of digits, we can ignore the rest.
          // This time we don't need to worry about the exponent as the decimal
          // value is too insignificant.
          if (mantissaDigits < maxMantissa)
          {
            num = num * 10 + (c - '0');
            hasDigits = true;
            if (num > 0) mantissaDigits++;
            if (e > INT_MIN) e--;
            else return wrenParseNumError(i, "Too many digits.", results);
          }
        }
        else if (c != '_')
          break;
        c = str[++i];
      }
    }

    // We must have parsed digits from here or else this number is invalid
    if (!hasDigits) return wrenParseNumError(i, "Number has no digits.",
                                             results);

    // Parse the exponential part of the number.
    if (c == 'e' || c == 'E')
    {
      c = str[++i];
      int expNum = 0;
      bool expHasDigits = false;
      bool expNeg = false;
      // Parse exponent sign.
      if (c == '-')
      {
        expNeg = true;
        c = str[++i];
      }
      else if (c == '+') c = str[++i];
      // Parse the actual exponent.
      for (;;)
      {
        if (isdigit(c))
        {
          int t = c - '0';
          if (expNum < INT_MAX / 10 ||
              (expNum == INT_MAX / 10 && t <= INT_MAX % 10))
          {
            expNum = expNum * 10 + t;
          }
          else
          {
            if (expNeg) return wrenParseNumError(i, "Exponent is too small.",
                                                 results);
            return wrenParseNumError(i, "Exponent is too large.", results);
          }
          expHasDigits = true;
        }
        else if (c != '_') break;
        c = str[++i];
      }
      if (!expHasDigits)
          return wrenParseNumError(i, "Unterminated scientific literal.",
                                   results);
      // Before changing "e", ensure that it will not overflow.
      if (expNeg)
      {
        if (e >= INT_MIN + expNum) e -= expNum;
        else return wrenParseNumError(i, "Exponent is too small.", results);
      }
      else
      {
        if (e <= INT_MAX - expNum) e += expNum;
        else return wrenParseNumError(i, "Exponent is too large.", results);
      }
    }
  } else {
    if (!hasDigits) return wrenParseNumError(i, "Number has no digits.",
                                             results);
  }

  // Floating point math is often inaccurate. To get around this issue, we
  // calculate using long doubles to preserve as much data as possible.
  long double power = powl((long double)base, (long double)e);
  long double result = (long double)num * power;
  double f = (double)result;

  // Check whether the number became infinity or 0 from being too big or too
  // small.
  if (isinf(f)) return wrenParseNumError(i, "Number is too large.", results);
  else if (f == 0 && num != 0)
           return wrenParseNumError(i, "Number is too small.", results);
  results->errorMessage = NULL;
  results->consumed = i;
  results->value = neg ? f * -1 : f;
}
